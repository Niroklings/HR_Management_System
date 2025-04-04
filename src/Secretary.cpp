#include "Secretary.h"
#include "Director.h"
#include "Programmer.h"
#include "Driver.h"
#include "Accountant.h"
#include "IdentifierGenerator.h"
#include <iostream>
#include <iomanip>

Secretary::Secretary(std::string tname, std::string tsurname, 
                    std::string tpatronymic, double tsalary) : 
    inNum(IdentifierGenerator::getNextId()), name(tname), surname(tsurname), 
    patronymic(tpatronymic), salary(tsalary) {}

void Secretary::setName(const std::string& value) { name = value; }
const std::string& Secretary::getName() const { return name; }

void Secretary::setSurname(const std::string& value) { surname = value; }
const std::string& Secretary::getSurname() const { return surname; }

void Secretary::setPatronymic(const std::string& value) { patronymic = value; }
const std::string& Secretary::getPatronymic() const { return patronymic; }

void Secretary::setSalary(double value) { salary = value; }
double Secretary::getSalary() const { return salary; }

int Secretary::getInNum() const { return inNum; }

void Secretary::getEmployeesOfDirector(Director& director) {
    for (const auto& employee : director.getEmployees()) {
        if (dynamic_cast<class Secretary*>(employee)) {
            std::cout << std::left << std::setw(15) << "Секретарь";
        }
        else if (dynamic_cast<class Accountant*>(employee)) {
            std::cout << std::left << std::setw(15) << "Бухгалтер";
        }
        else if (dynamic_cast<class Programmer*>(employee)) {
            std::cout << std::left << std::setw(15) << "Программист";
        }
        else if (dynamic_cast<class Driver*>(employee)) {
            std::cout << std::left << std::setw(15) << "Водитель";
        }
        
        std::cout << "| " << std::left << std::setw(15) << employee->getName() 
                  << " | " << std::left << std::setw(15) << employee->getSurname()
                  << " | " << std::left << std::setw(15) << employee->getPatronymic() 
                  << " | " << std::left << std::setw(10) << employee->getSalary() 
                  << " |" << std::endl;
    }
}

void Secretary::printDrivers(Director& director) {
    const auto& humans = director.getDrivers();
    std::vector<Driver*> drivers;
    for (Human* human : humans) {
        Driver* driver = dynamic_cast<Driver*>(human);
        drivers.push_back(driver);
    }
    
    if (drivers.empty()) {
        std::cout << "У директора нет водителей.\n";
        return;
    }
    
    for (const auto& driver : drivers) {
        std::cout << "| " << std::left << std::setw(15) << driver->getName() 
                  << " | " << std::left << std::setw(15) << driver->getSurname() 
                  << " | " << std::left << std::setw(15) << driver->getPatronymic()
                  << " | " << std::left << std::setw(10) << driver->getSalary() << " | ";

        const auto& licenseCategories = driver->getLicenseCategories();
        for (size_t i = 0; i < licenseCategories.size(); ++i) {
            std::cout << licenseCategories[i];
            if (i < licenseCategories.size() - 1) {
                std::cout << ", ";
            }
        }

        std::cout << " | ";

        const auto& vehicles = driver->getVehicles();
        for (size_t i = 0; i < vehicles.size(); ++i) {
            std::cout << vehicles[i];
            if (i < vehicles.size() - 1) {
                std::cout << ", ";
            }
        }

        std::cout << " |" << std::endl;
    }
}

void Secretary::printProgrammers(Director& director) {
    const auto& humans = director.getProgrammers();
    std::vector<Programmer*> programmers;
    for (Human* human : humans) {
        Programmer* programmer = dynamic_cast<Programmer*>(human);
        programmers.push_back(programmer);
    }
    
    if (programmers.empty()) {
        std::cout << "У директора нет программистов.\n";
        return;
    }
    
    for (const auto& programmer : programmers) {
        std::cout << "| " << std::left << std::setw(15) << programmer->getName() 
                  << " | " << std::left << std::setw(15) << programmer->getSurname() 
                  << " | " << std::left << std::setw(15) << programmer->getPatronymic()
                  << " | " << std::left << std::setw(10) << programmer->getSalary() 
                  << " | " << std::left << std::setw(5) << programmer->getLevel() << " | ";

        const auto& techs = programmer->getTechnologies();
        for (size_t i = 0; i < techs.size(); ++i) {
            std::cout << techs[i];
            if (i < techs.size() - 1) {
                std::cout << ", ";
            }
        }

        std::cout << " |" << std::endl;
    }
}

void Secretary::getInfo() const {
    std::cout << "ID: " << inNum << "\n"
              << "Имя: " << name << "\n"
              << "фамилия: " << surname << "\n"
              << "Отчество: " << patronymic << "\n"
              << "Оклад: " << salary << "\n";
}

bool Secretary::saveToDatabase(sql::Connection* con) {
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO employees (type, name, surname, patronymic, salary) "
            "VALUES ('secretary', ?, ?, ?, ?)");
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

void Secretary::loadFromDatabase(sql::Connection* con, int id) {
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

Human* Secretary::createFromDatabase(sql::Connection* con, int id) {
    Secretary* sec = new Secretary();
    sec->loadFromDatabase(con, id);
    return sec;
}

Secretary::~Secretary() {}