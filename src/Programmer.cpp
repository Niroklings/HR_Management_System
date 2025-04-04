#include "Programmer.h"
#include "IdentifierGenerator.h"
#include <iostream>
#include <sstream>

Programmer::Programmer(std::string tname, std::string tsurname, 
                      std::string tpatronymic, double tsalary,
                      std::string tlevel, std::vector<std::string> tech) : 
    inNum(IdentifierGenerator::getNextId()), name(tname), surname(tsurname), 
    patronymic(tpatronymic), salary(tsalary), level(tlevel), technologies(tech) {}

void Programmer::setName(const std::string& value) { name = value; }
const std::string& Programmer::getName() const { return name; }

void Programmer::setSurname(const std::string& value) { surname = value; }
const std::string& Programmer::getSurname() const { return surname; }

void Programmer::setPatronymic(const std::string& value) { patronymic = value; }
const std::string& Programmer::getPatronymic() const { return patronymic; }

void Programmer::setSalary(double value) { salary = value; }
double Programmer::getSalary() const { return salary; }

int Programmer::getInNum() const { return inNum; }

void Programmer::setLevel(const std::string& value) { level = value; }
const std::string& Programmer::getLevel() const { return level; }

void Programmer::addTechnology(const std::string& tech) {
    technologies.push_back(tech);
}

const std::vector<std::string>& Programmer::getTechnologies() const {
    return technologies;
}

void Programmer::getInfo() const {
    std::cout << "ID: " << inNum << "\n"
              << "Имя: " << name << "\n"
              << "Фамилия: " << surname << "\n"
              << "Отчество: " << patronymic << "\n"
              << "Оклад: " << salary << "\n"
              << "Уровень: " << level << "\n"
              << "Навыки: ";
    for (int i = 0; i < technologies.size(); ++i) {
        std::cout << technologies[i] << " ";
    }
    std::cout << std::endl;
}

bool Programmer::saveToDatabase(sql::Connection* con) {
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO employees (type, name, surname, patronymic, salary) "
            "VALUES ('programmer', ?, ?, ?, ?)");
        pstmt->setString(1, name);
        pstmt->setString(2, surname);
        pstmt->setString(3, patronymic);
        pstmt->setDouble(4, salary);
        pstmt->execute();
    
        pstmt = con->prepareStatement("SELECT LAST_INSERT_ID()");
        sql::ResultSet* res = pstmt->executeQuery();
        if (res->next()) {
            inNum = res->getInt(1);
        }
        delete res;
        delete pstmt;
    
        std::string techs;
        for (const auto& tech : technologies) {
            techs += tech + ",";
        }
    
        pstmt = con->prepareStatement(
            "INSERT INTO programmer_details (employee_id, level, technologies) "
            "VALUES (?, ?, ?)");
        pstmt->setInt(1, inNum);
        pstmt->setString(2, level);
        pstmt->setString(3, techs);
        pstmt->execute();
        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        return false;
    }
}

void Programmer::loadFromDatabase(sql::Connection* con, int id) {
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
    
        pstmt = con->prepareStatement(
            "SELECT * FROM programmer_details WHERE employee_id = ?");
        pstmt->setInt(1, id);
        res = pstmt->executeQuery();
    
        if (res->next()) {
            level = res->getString("level");
            std::string techs = res->getString("technologies");
            std::stringstream ss(techs);
            std::string tech;
            while (std::getline(ss, tech, ',')) {
                if (!tech.empty()) technologies.push_back(tech);
            }
        }
    
        delete res;
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

Human* Programmer::createFromDatabase(sql::Connection* con, int id) {
    Programmer* prog = new Programmer();
    prog->loadFromDatabase(con, id);
    return prog;
}

Programmer::~Programmer() {}