#ifndef DRIVER_H
#define DRIVER_H

#include "Human.h"
#include <vector>
#include <string>

class Driver : public Human {
private:
    int inNum;
    std::string name;
    std::string surname;
    std::string patronymic;
    double salary;
    std::vector<std::string> licenseCategories;
    std::vector<std::string> vehicles;

public:
    Driver(std::string tname = "", std::string tsurname = "", 
          std::string tpatronymic = "", double tsalary = 0,
          std::vector<std::string> lic = {}, std::vector<std::string> veh = {});
    
    void setName(const std::string& value) override;
    const std::string& getName() const override;
    void setSurname(const std::string& value) override;
    const std::string& getSurname() const override;
    void setPatronymic(const std::string& value) override;
    const std::string& getPatronymic() const override;
    void setSalary(double value) override;
    double getSalary() const override;
    int getInNum() const override;
    
    void addLicenseCategory(const std::string& category);
    const std::vector<std::string>& getLicenseCategories() const;
    void addVehicle(const std::string& vehicle);
    const std::vector<std::string>& getVehicles() const;
    
    void getInfo() const override;
    bool saveToDatabase(sql::Connection* con) override;
    void loadFromDatabase(sql::Connection* con, int id) override;
    
    static Human* createFromDatabase(sql::Connection* con, int id);
    
    ~Driver();
};

#endif // DRIVER_H