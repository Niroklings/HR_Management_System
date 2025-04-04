#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>

class Director;

class DatabaseManager {
private:
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* con;

public:
    DatabaseManager(const std::string& host, const std::string& user,
                   const std::string& password, const std::string& database);
    ~DatabaseManager();

    sql::Connection* getConnection();
    void initializeDatabase();
    void updateDirector(Director* director);
    bool existsInDatabase(const std::string& table, int id);
};

#endif // DATABASEMANAGER_H