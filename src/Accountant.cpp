#include "Accountant.h"
#include "IdentifierGenerator.h"
#include <iostream>
#include <iomanip>

Accountant::Accountant(std::string tname, std::string tsurname, 
                      std::string tpatronymic, double tsalary) : 
    inNum(IdentifierGenerator::getNextId()), name(tname), surname(tsurname), 
    patronymic(tpatronymic), salary(tsalary) {}

void Accountant::printInNumOfDir(Director& director) {
    for (const auto& employee : director.getEmployees()) {
        std::cout << "| " << std::left << std::setw(10) << employee->getInNum() 
                  << " | " << std::left << std::setw(15) << employee->getName() 
                  << " | " << std::left << std::setw(15) << employee->getSurname()
                  << " | " << std::left << std::setw(15) << employee->getPatronymic() 
                  << " |" << std::endl;
    }
}

void Accountant::addRate(int employeeId, double rate) {
    rates[employeeId] = rate;
}

void Accountant::removeRate(int employeeId) {
    rates.erase(employeeId);
}

double Accountant::getRate(int employeeId){
    auto it = rates.find(employeeId);
    return (it != rates.end()) ? it->second : 1.0;  // Возвращаем 1.0, если ставка не найдена
}

void Accountant::setName(const std::string& value) { name = value; }
const std::string& Accountant::getName() const { return name; }

void Accountant::setSurname(const std::string& value) { surname = value; }
const std::string& Accountant::getSurname() const { return surname; }

void Accountant::setPatronymic(const std::string& value) { patronymic = value; }
const std::string& Accountant::getPatronymic() const { return patronymic; }

void Accountant::setSalary(double value) { salary = value; }
double Accountant::getSalary() const { return salary; }

int Accountant::getInNum() const { return inNum; }

double Accountant::countSalary(Human* employee) {
    return employee->getSalary() * rates.at(employee->getInNum());
}

void Accountant::boostSalary(Human* employee, double salary) {
    employee->setSalary(salary);
}

void Accountant::getInfo() const {
    std::cout << "ID: " << inNum << "\n"
              << "Name: " << name << "\n"
              << "Surname: " << surname << "\n"
              << "Patronymic: " << patronymic << "\n"
              << "Salary: " << salary << "\n";
}

bool Accountant::saveToDatabase(sql::Connection* con) {
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO employees (type, name, surname, patronymic, salary) "
            "VALUES ('accountant', ?, ?, ?, ?)");
        pstmt->setString(1, name);
        pstmt->setString(2, surname);
        pstmt->setString(3, patronymic);
        pstmt->setDouble(4, salary);
        pstmt->execute();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        return false;
    }
}

void Accountant::loadFromDatabase(sql::Connection* con, int id) {
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT * FROM employees WHERE id = ?");
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
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

Human* Accountant::createFromDatabase(sql::Connection* con, int id) {
    Accountant* acc = new Accountant();
    acc->loadFromDatabase(con, id);
    return acc;
}

Accountant::~Accountant() {}