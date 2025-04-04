#include "Director.h"
#include "Accountant.h"
#include "Secretary.h"
#include "Programmer.h"
#include "Driver.h"
#include "IdentifierGenerator.h"
#include "DatabaseManager.h"

#include <algorithm>
#include <iostream>

Director::Director(DatabaseManager& db, std::string tname, std::string tsurname, 
    std::string tpatronymic, double tsalary) 
: dbManager(db), inNum(IdentifierGenerator::getNextId()), 
name(tname), surname(tsurname), patronymic(tpatronymic), salary(tsalary) {
}
const std::vector<Human*>& Director::getDrivers() {
    return drivers;
}

const std::vector<Human*>& Director::getProgrammers() {
    return programmers;
}

bool Director::isSecretary(Director& director) {
    return secretary != nullptr;
}

bool Director::isAccountant(Director& director) {
    return accountant != nullptr;
}

double Director::countSalary(Human* employee) {
    Accountant* acc = dynamic_cast<Accountant*>(accountant);
    return acc ? acc->countSalary(employee) : 0.0;
}

void Director::hireEmployee(Human* employee, double rate) {
    if (!employee) {
        std::cerr << "Ошибка: передан nullptr в hireEmployee!" << std::endl;
        return;
    }
    if (dynamic_cast<Secretary*>(employee)) {
        if (secretary) {
            std::cerr << "Предупреждение: у директора уже есть секретарь!" << std::endl;
            return;
        }
        secretary = employee;
    } 
    else if (dynamic_cast<Accountant*>(employee)) {
        if (accountant) {
            std::cerr << "Предупреждение: у директора уже есть бухгалтер!" << std::endl;
            return;
        }
        accountant = employee;
    }
    else if (dynamic_cast<Programmer*>(employee)) {
        programmers.push_back(employee);
    }
    else if (dynamic_cast<Driver*>(employee)) {
        drivers.push_back(employee);
    }
    else {
        std::cerr << "Ошибка: неизвестный тип сотрудника!" << std::endl;
        return;
    }
    if (accountant) {
        Accountant* acc = dynamic_cast<Accountant*>(accountant);
        if (acc) {
            acc->addRate(employee->getInNum(), rate);
        }
    }
}

std::vector<Human*> Director::getEmployees() const {
    std::vector<Human*> staff;
    if (secretary) staff.push_back(secretary);
    if (accountant) staff.push_back(accountant);
    staff.insert(staff.end(), programmers.begin(), programmers.end());
    staff.insert(staff.end(), drivers.begin(), drivers.end());
    return staff;
}

void Director::fireEmployee(Human* employee) {
    if (secretary == employee) {
        secretary = nullptr;
    } 
    else if (accountant == employee) {
        accountant = nullptr;
    }
    else {
        auto it = std::find(programmers.begin(), programmers.end(), employee);
        if (it != programmers.end()) {
            programmers.erase(it);
        }
        
        it = std::find(drivers.begin(), drivers.end(), employee);
        if (it != drivers.end()) {
            drivers.erase(it);
        }
    }

    if (accountant) {
        Accountant* acc = dynamic_cast<Accountant*>(accountant);
        acc->removeRate(employee->getInNum()); 
    }

    try {
        sql::Connection* con = dbManager.getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "UPDATE employees SET director_id = NULL, rate = 0 WHERE in_num = ?");
        pstmt->setInt(1, employee->getInNum());
        pstmt->executeUpdate();
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error in fireEmployee: " << e.what() << std::endl;
    }
}

void Director::setName(const std::string& value) { name = value; }
const std::string& Director::getName() const { return name; }

void Director::setSurname(const std::string& value) { surname = value; }
const std::string& Director::getSurname() const { return surname; }

void Director::setPatronymic(const std::string& value) { patronymic = value; }
const std::string& Director::getPatronymic() const { return patronymic; }

void Director::setSalary(double value) { salary = value; }
double Director::getSalary() const { return salary; }

int Director::getInNum() const { return inNum; }
void Director::setInNum(int value) { inNum = value; }

void Director::getInfo() const {
    if (this == nullptr) {
        std::cerr << "Ошибка: попытка вызова метода для nullptr!" << std::endl;
        return;
    }
    std::cout << "ID: " << inNum << "\n"
              << "Имя: " << name << "\n"
              << "Фамилия: " << surname << "\n"
              << "Отчество: " << patronymic << "\n"
              << "оклад: " << salary << "\n";
}

bool Director::otvet() {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> dis(0, 1);
    return dis(gen) == 1;
}

bool Director::saveToDatabase(sql::Connection* con) {
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT id FROM directors WHERE id = ?");
        pstmt->setInt(1, inNum);
        sql::ResultSet* res = pstmt->executeQuery();

        if (res->next()) {
            pstmt = con->prepareStatement(
                "UPDATE directors SET name = ?, surname = ?, patronymic = ?, salary = ? WHERE id = ?");
            pstmt->setString(1, name);
            pstmt->setString(2, surname);
            pstmt->setString(3, patronymic);
            pstmt->setDouble(4, salary);
            pstmt->setInt(5, inNum);
            pstmt->execute();
        } else {
            pstmt = con->prepareStatement(
                "INSERT INTO directors (name, surname, patronymic, salary) VALUES (?, ?, ?, ?)");
            pstmt->setString(1, name);
            pstmt->setString(2, surname);
            pstmt->setString(3, patronymic);
            pstmt->setDouble(4, salary);
            pstmt->execute();

            pstmt = con->prepareStatement("SELECT LAST_INSERT_ID()");
            res = pstmt->executeQuery();
            if (res->next()) {
                inNum = res->getInt(1);
            }
        }
        for (auto emp : getEmployees()) {
            pstmt = con->prepareStatement(
                "UPDATE employees SET director_id = ? WHERE id = ?");
            pstmt->setInt(1, inNum);
            pstmt->setInt(2, emp->getInNum());
            pstmt->execute();
            delete pstmt;
        }

        delete res;
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        return false;
    }
}

void Director::loadFromDatabase(sql::Connection* con, int id) {
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM directors WHERE id = ?");
        pstmt->setInt(1, id);
        sql::ResultSet* res = pstmt->executeQuery();

        if (res->next()) {
            inNum = id;
            name = res->getString("name");
            surname = res->getString("surname");
            patronymic = res->getString("patronymic");
            salary = res->getDouble("salary");
        }

        delete res;
        delete pstmt;

        pstmt = con->prepareStatement(
            "SELECT id, type FROM employees WHERE director_id = ?");
        pstmt->setInt(1, id);
        res = pstmt->executeQuery();

        while (res->next()) {
            int empId = res->getInt("id");
            std::string type = res->getString("type");

            Human* employee = nullptr;
            if (type == "accountant") {
                employee = Accountant::createFromDatabase(con, empId);
            }
            else if (type == "secretary") {
                employee = Secretary::createFromDatabase(con, empId);
            }
            else if (type == "programmer") {
                employee = Programmer::createFromDatabase(con, empId);
            }
            else if (type == "driver") {
                employee = Driver::createFromDatabase(con, empId);
            }

            if (employee) {
                hireEmployee(employee, 1.0);
            }
        }

        delete res;
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

Director* Director::createFromDatabase(DatabaseManager& dbManager, sql::Connection* con, int id) {
    sql::PreparedStatement* pstmt = con->prepareStatement(
        "SELECT name, surname, patronymic, salary FROM directors WHERE id = ?"
    );
    pstmt->setInt(1, id);
    sql::ResultSet* res = pstmt->executeQuery();

    if (res->next()) {
        std::string name = res->getString("name");
        std::string surname = res->getString("surname");
        std::string patronymic = res->getString("patronymic");
        double salary = res->getDouble("salary");

        delete res;
        delete pstmt;
        Director* dir = new Director(dbManager, name, surname, patronymic, salary);
        dir->setInNum(id);  
        return dir;
    }

    delete res;
    delete pstmt;
    return nullptr;
}


void Director::print(const Human* employee) {
    employee->getInfo();
}

void Director::getEmployees(Director& director) {
    Secretary* sec = dynamic_cast<Secretary*>(secretary);
    sec->getEmployeesOfDirector(director);
}
void Director::printDrivers(Director& director) {
    Secretary* sec = dynamic_cast<Secretary*>(secretary);
    sec->printDrivers(director);
}
void Director::printProgrammers(Director& director) {
    Secretary* sec = dynamic_cast<Secretary*>(secretary);
    sec->printProgrammers(director);
}

void Director::boostSalary(Human* employee, double salary) {
    Accountant* acc = dynamic_cast<Accountant*>(accountant);
    acc->boostSalary(employee, salary);
}
void Director::printInNum(Director& director) {
    Accountant* acc = dynamic_cast<Accountant*>(accountant);
    acc->printInNumOfDir(director);
}

Director::~Director() {}