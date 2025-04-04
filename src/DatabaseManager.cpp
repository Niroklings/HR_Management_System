#include "DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& host, const std::string& user,
                               const std::string& password, const std::string& database) {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(host, user, password);
        con->setSchema(database);
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

DatabaseManager::~DatabaseManager() {
    delete con;
}

sql::Connection* DatabaseManager::getConnection() { return con; }

void DatabaseManager::initializeDatabase() {
    try {
        sql::Statement* stmt = con->createStatement();

        stmt->execute("CREATE TABLE IF NOT EXISTS directors ("
                      "id INT PRIMARY KEY AUTO_INCREMENT,"
                      "name VARCHAR(50),"
                      "surname VARCHAR(50),"
                      "patronymic VARCHAR(50),"
                      "salary DOUBLE)");

        stmt->execute("CREATE TABLE IF NOT EXISTS employees ("
                      "id INT PRIMARY KEY AUTO_INCREMENT,"
                      "type ENUM('accountant', 'secretary', 'programmer', 'driver'),"
                      "name VARCHAR(50),"
                      "surname VARCHAR(50),"
                      "patronymic VARCHAR(50),"
                      "salary DOUBLE,"
                      "director_id INT,"
                      "rate DOUBLE,"
                      "FOREIGN KEY (director_id) REFERENCES directors(id))");

        stmt->execute("CREATE TABLE IF NOT EXISTS driver_details ("
                      "employee_id INT PRIMARY KEY,"
                      "license_categories TEXT,"
                      "vehicles TEXT,"
                      "FOREIGN KEY (employee_id) REFERENCES employees(id))");

        stmt->execute("CREATE TABLE IF NOT EXISTS programmer_details ("
                      "employee_id INT PRIMARY KEY,"
                      "level VARCHAR(20),"
                      "technologies TEXT,"
                      "FOREIGN KEY (employee_id) REFERENCES employees(id))");

        delete stmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

bool DatabaseManager::existsInDatabase(const std::string& table, int id) {
    try {
        sql::Connection* con = getConnection();
        sql::PreparedStatement* pstmt = con->prepareStatement(
            "SELECT id FROM " + table + " WHERE id = ?"
        );
        pstmt->setInt(1, id);
        sql::ResultSet* res = pstmt->executeQuery();
        bool exists = res->next();
        delete res;
        delete pstmt;
        return exists;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL Error in existsInDatabase: " << e.what() << std::endl;
        return false;
    }
}
