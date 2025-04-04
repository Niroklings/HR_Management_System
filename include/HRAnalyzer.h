#ifndef HRANALYZER_H
#define HRANALYZER_H

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_driver.h>
#include <mysql_connection.h>

class HRAnalyzer {
private:
    sql::Connection* con;

public:
    HRAnalyzer(sql::Connection* connection);
    void printSalaryStatistics();
    void printEmployeeDistribution();
};

#endif // HRANALYZER_H