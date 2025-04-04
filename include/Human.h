#ifndef HUMAN_H
#define HUMAN_H

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <string>

class Human {
public:
    virtual void getInfo() const = 0;
    virtual int getInNum() const = 0;

    virtual void setName(const std::string& name) = 0;
    virtual const std::string& getName() const = 0;

    virtual void setSurname(const std::string& surname) = 0;
    virtual const std::string& getSurname() const = 0;

    virtual void setPatronymic(const std::string& patronymic) = 0;
    virtual const std::string& getPatronymic() const = 0;

    virtual void setSalary(double value) = 0;
    virtual double getSalary() const = 0;

    virtual bool saveToDatabase(sql::Connection* con) = 0;
    virtual void loadFromDatabase(sql::Connection* con, int id) = 0;

    static Human* createFromDatabase(sql::Connection* con, int id);

    virtual ~Human() {}
};

#endif // HUMAN_H