#ifndef SECRETARY_H
#define SECRETARY_H

#include "Human.h"
#include "Director.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <vector>
#include <iomanip>

class Secretary : public Human {
private:
    int inNum;
    std::string name;
    std::string surname;
    std::string patronymic;
    double salary;

public:
    Secretary(std::string tname = "", std::string tsurname = "", 
              std::string tpatronymic = "", double tsalary = 0);
    
    void setName(const std::string& value) override;
    const std::string& getName() const override;
    void setSurname(const std::string& value) override;
    const std::string& getSurname() const override;
    void setPatronymic(const std::string& value) override;
    const std::string& getPatronymic() const override;
    void setSalary(double value) override;
    double getSalary() const override;
    int getInNum() const override;
    
    void getEmployeesOfDirector(Director& director);
    void printDrivers(Director& director);
    void printProgrammers(Director& director);
    
    void getInfo() const override;
    bool saveToDatabase(sql::Connection* con) override;
    void loadFromDatabase(sql::Connection* con, int id) override;
    
    static Human* createFromDatabase(sql::Connection* con, int id);
    
    ~Secretary();
};

#endif // SECRETARY_H