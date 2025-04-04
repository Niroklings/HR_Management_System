#include "Driver.h"
#include "IdentifierGenerator.h"
#include <iostream>
#include <sstream>

Driver::Driver(std::string tname, std::string tsurname, 
              std::string tpatronymic, double tsalary,
              std::vector<std::string> lic, std::vector<std::string> veh) : 
    inNum(IdentifierGenerator::getNextId()), name(tname), surname(tsurname), 
    patronymic(tpatronymic), salary(tsalary), licenseCategories(lic), vehicles(veh) {}

void Driver::setName(const std::string& value) { name = value; }
const std::string& Driver::getName() const { return name; }

void Driver::setSurname(const std::string& value) { surname = value; }
const std::string& Driver::getSurname() const { return surname; }

void Driver::setPatronymic(const std::string& value) { patronymic = value; }
const std::string& Driver::getPatronymic() const { return patronymic; }

void Driver::setSalary(double value) { salary = value; }
double Driver::getSalary() const { return salary; }

int Driver::getInNum() const { return inNum; }

void Driver::addLicenseCategory(const std::string& category) {
    licenseCategories.push_back(category);
}

const std::vector<std::string>& Driver::getLicenseCategories() const {
    return licenseCategories;
}

void Driver::addVehicle(const std::string& vehicle) {
    vehicles.push_back(vehicle);
}

const std::vector<std::string>& Driver::getVehicles() const {
    return vehicles;
}

void Driver::getInfo() const {
    std::cout << "ID: " << inNum << "\n"
              << "Имя: " << name << "\n"
              << "Фамилия: " << surname << "\n"
              << "Отчество: " << patronymic << "\n"
              << "Оклад: " << salary << "\n"
              << "Категории прав: ";

    for (int i = 0; i < licenseCategories.size(); ++i) {
        std::cout << licenseCategories[i] << " ";
    }
    std::cout << "\nТранспортные средства: ";

    for (int i = 0; i < vehicles.size(); ++i) {
        std::cout << vehicles[i] << " ";
    }
    std::cout << std::endl;
}

bool Driver::saveToDatabase(sql::Connection* con) {
    try {
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "INSERT INTO employees (type, name, surname, patronymic, salary) "
            "VALUES ('driver', ?, ?, ?, ?)");
        pstmt->setString(1, name);
        pstmt->setString(2, surname);
        pstmt->setString(3, patronymic);
        pstmt->setDouble(4, salary);
        pstmt->execute();

        delete pstmt;

        pstmt = con->prepareStatement("SELECT LAST_INSERT_ID()");
        sql::ResultSet* res = pstmt->executeQuery();
        if (res->next()) {
            inNum = res->getInt(1);
        }
        delete res;
        delete pstmt;

        std::string licenses;
        for (const auto& lic : licenseCategories) {
            licenses += lic + ",";
        }

        std::string vehs;
        for (const auto& veh : vehicles) {
            vehs += veh + ",";
        }

        pstmt = con->prepareStatement(
            "INSERT INTO driver_details (employee_id, license_categories, vehicles) "
            "VALUES (?, ?, ?)");
        pstmt->setInt(1, inNum);
        pstmt->setString(2, licenses);
        pstmt->setString(3, vehs);
        pstmt->execute();

        delete pstmt;
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        return false;
    }
}

void Driver::loadFromDatabase(sql::Connection* con, int id) {
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
            "SELECT * FROM driver_details WHERE employee_id = ?");
        pstmt->setInt(1, id);
        res = pstmt->executeQuery();

        if (res->next()) {
            std::string licenses = res->getString("license_categories");
            std::stringstream ss_lic(licenses);
            std::string lic;
            while (std::getline(ss_lic, lic, ',')) {
                if (!lic.empty()) licenseCategories.push_back(lic);
            }

            std::string vehs = res->getString("vehicles");
            std::stringstream ss_veh(vehs);
            std::string veh;
            while (std::getline(ss_veh, veh, ',')) {
                if (!veh.empty()) vehicles.push_back(veh);
            }
        }

        delete res;
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

Human* Driver::createFromDatabase(sql::Connection* con, int id) {
    Driver* driver = new Driver();
    driver->loadFromDatabase(con, id);
    return driver;
}

Driver::~Driver() {}