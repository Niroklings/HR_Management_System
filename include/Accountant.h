#ifndef ACCOUNTANT_H
#define ACCOUNTANT_H

#include "Human.h"
#include "Director.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <unordered_map>

class Accountant : public Human {
private:
    int inNum;
    std::string name;
    std::string surname;
    std::string patronymic;
    double salary;
    std::unordered_map<int, double> rates;

public:
    Accountant(std::string tname = "", std::string tsurname = "", 
               std::string tpatronymic = "", double tsalary = 0);
    
    void printInNumOfDir(Director& director);
    void addRate(int employeeId, double rate);
    void removeRate(int employeeId);  
    double getRate(int employeeId);
    
    void setName(const std::string& value) override;
    const std::string& getName() const override;
    void setSurname(const std::string& value) override;
    const std::string& getSurname() const override;
    void setPatronymic(const std::string& value) override;
    const std::string& getPatronymic() const override;
    void setSalary(double value) override;
    double getSalary() const override;
    int getInNum() const override;
    
    double countSalary(Human* employee);
    void boostSalary(Human* employee, double salary);
    
    void getInfo() const override;
    bool saveToDatabase(sql::Connection* con) override;
    void loadFromDatabase(sql::Connection* con, int id) override;
    
    static Human* createFromDatabase(sql::Connection* con, int id);
    
    ~Accountant();
};

#endif // ACCOUNTANT_H