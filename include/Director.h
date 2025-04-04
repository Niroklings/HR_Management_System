#ifndef DIRECTOR_H
#define DIRECTOR_H

#include "Human.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <vector>
#include <unordered_map>
#include <random>

class DatabaseManager;

class Director : public Human {
private:
    DatabaseManager& dbManager; 
    int inNum;
    std::string name;
    std::string surname;
    std::string patronymic;
    double salary;
    Human* secretary = nullptr;
    Human* accountant = nullptr;
    std::vector<Human*> programmers;
    std::vector<Human*> drivers;

public:
    Director(DatabaseManager& db, std::string tname = "", std::string tsurname = "", std::string tpatronymic = "", double tsalary = 0);
    
    const std::vector<Human*>& getDrivers();
    const std::vector<Human*>& getProgrammers();

    void hireEmployee(Human* employee, double rate);
    bool isSecretary(Director& director);
    bool isAccountant(Director& director);
    void print(const Human* employee);
    double countSalary(Human* employee);
    void getEmployees(Director& director);
    void printProgrammers(Director& director);
    void printDrivers(Director& director);
    void boostSalary(Human* employee, double salary);
    void printInNum(Director& director);

    std::vector<Human*> getEmployees() const;
    void fireEmployee(Human* employee);

    void setName(const std::string& value) override;
    const std::string& getName() const override;
    void setSurname(const std::string& value) override;
    const std::string& getSurname() const override;
    void setPatronymic(const std::string& value) override;
    const std::string& getPatronymic() const override;
    void setSalary(double value) override;
    double getSalary() const override;
    int getInNum() const override;
    void getInfo() const override;
    void setInNum(int value);

    bool otvet();
    bool saveToDatabase(sql::Connection* con) override; 
    void loadFromDatabase(sql::Connection* con, int id) override;

    static Director* createFromDatabase(DatabaseManager& dbManager, sql::Connection* con, int id);

    ~Director();
};

#endif // DIRECTOR_H