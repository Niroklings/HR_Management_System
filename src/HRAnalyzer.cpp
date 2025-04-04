#include "HRAnalyzer.h"
#include <iostream>
#include <iomanip>

HRAnalyzer::HRAnalyzer(sql::Connection* connection) : con(connection) {}

void HRAnalyzer::printSalaryStatistics() {
    try {
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT type, AVG(salary) as avg_salary, MAX(salary) as max_salary, "
            "MIN(salary) as min_salary FROM employees GROUP BY type");

        std::cout << "Статистика по зарплатам:\n";
        std::cout << std::left << std::setw(15) << "Должность"
                  << std::setw(15) << "Средняя"
                  << std::setw(15) << "Максимальная"
                  << std::setw(15) << "Минимальная" << "\n";

        while (res->next()) {
            std::cout << std::left << std::setw(15) << res->getString("type")
                      << std::setw(15) << res->getDouble("avg_salary")
                      << std::setw(15) << res->getDouble("max_salary")
                      << std::setw(15) << res->getDouble("min_salary") << "\n";
        }

        delete res;
        delete stmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

void HRAnalyzer::printEmployeeDistribution() {
    try {
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery(
            "SELECT d.name, d.surname, COUNT(e.id) as employee_count "
            "FROM directors d LEFT JOIN employees e ON d.id = e.director_id "
            "GROUP BY d.id, d.name, d.surname");

        std::cout << "Распределение сотрудников по директорам:\n";
        std::cout << std::left << std::setw(20) << "Директор"
                  << std::setw(10) << "Сотрудников" << "\n";

        while (res->next()) {
            std::cout << std::left << std::setw(20)
                      << res->getString("name") + " " + res->getString("surname")
                      << std::setw(10) << res->getInt("employee_count") << "\n";
        }

        delete res;
        delete stmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}