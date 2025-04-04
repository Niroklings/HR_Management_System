#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "Human.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <vector>
#include <string>

class Programmer : public Human {
private:
    int inNum;
    std::string name;
    std::string surname;
    std::string patronymic;
    double salary;
    std::string level;
    std::vector<std::string> technologies;

public:
    Programmer(std::string tname = "", std::string tsurname = "", 
              std::string tpatronymic = "", double tsalary = 0,
              std::string tlevel = "", std::vector<std::string> tech = {});
    
    void setName(const std::string& value) override;
    const std::string& getName() const override;
    void setSurname(const std::string& value) override;
    const std::string& getSurname() const override;
    void setPatronymic(const std::string& value) override;
    const std::string& getPatronymic() const override;
    void setSalary(double value) override;
    double getSalary() const override;
    int getInNum() const override;
    
    void setLevel(const std::string& value);
    const std::string& getLevel() const;
    void addTechnology(const std::string& tech);
    const std::vector<std::string>& getTechnologies() const;
    
    void getInfo() const override;
    bool saveToDatabase(sql::Connection* con) override;
    void loadFromDatabase(sql::Connection* con, int id) override;
    
    static Human* createFromDatabase(sql::Connection* con, int id);
    
    ~Programmer();
};

#endif // PROGRAMMER_H