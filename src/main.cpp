#include "DatabaseManager.h"
#include "HRAnalyzer.h"
#include "Human.h"
#include "Director.h"
#include "Accountant.h"
#include "Secretary.h"
#include "Programmer.h"
#include "Driver.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <iterator>
#include <cstdlib>
#include <random>
#include <cctype>
#include <locale>

void searchEmployees(sql::Connection* con) {
    try {
        std::cout << "Критерии поиска:\n"
                  << "1. По должности\n"
                  << "2. По уровню зарплаты\n"
                  << "3. По навыкам/категориям\n"
                  << "Ваш выбор: ";

        int choice;
        std::cin >> choice;

        sql::PreparedStatement* pstmt = nullptr;

        switch (choice) {
        case 1: {
            std::cout << "Введите должность (accountant, secretary, programmer, driver): ";
            std::string type;
            std::cin >> type;

            pstmt = con->prepareStatement(
                "SELECT * FROM employees WHERE type = ?");
            pstmt->setString(1, type);
            break;
        }
        case 2: {
            std::cout << "Введите минимальную зарплату: ";
            double minSalary;
            std::cin >> minSalary;

            std::cout << "Введите максимальную зарплату: ";
            double maxSalary;
            std::cin >> maxSalary;

            pstmt = con->prepareStatement(
                "SELECT * FROM employees WHERE salary BETWEEN ? AND ?");
            pstmt->setDouble(1, minSalary);
            pstmt->setDouble(2, maxSalary);
            break;
        }
        case 3: {
            std::cout << "Введите навык/категорию для поиска: ";
            std::string skill;
            std::cin >> skill;

            pstmt = con->prepareStatement(
                "SELECT e.* FROM employees e "
                "LEFT JOIN programmer_details pd ON e.id = pd.employee_id "
                "LEFT JOIN driver_details dd ON e.id = dd.employee_id "
                "WHERE e.type = 'programmer' AND pd.technologies LIKE ? "
                "OR e.type = 'driver' AND dd.license_categories LIKE ?");
            pstmt->setString(1, "%" + skill + "%");
            pstmt->setString(2, "%" + skill + "%");
            break;
        }
        default:
            std::cout << "Неверный выбор.\n";
            return;
        }

        sql::ResultSet* res = pstmt->executeQuery();

        std::cout << "Результаты поиска:\n";
        std::cout << std::left << std::setw(10) << "ID"
                  << std::setw(15) << "Тип"
                  << std::setw(20) << "Имя"
                  << std::setw(20) << "Фамилия"
                  << std::setw(15) << "Зарплата" << "\n";

        while (res->next()) {
            std::cout << std::left << std::setw(10) << res->getInt("id")
                      << std::setw(15) << res->getString("type")
                      << std::setw(20) << res->getString("name")
                      << std::setw(20) << res->getString("surname")
                      << std::setw(15) << res->getDouble("salary") << "\n";
        }

        delete res;
        delete pstmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

    void initializeData(std::vector<std::unique_ptr<Director>>& directors, std::vector<Human*>& employees, DatabaseManager& dbManager) {
    directors.push_back(std::make_unique<Director>(dbManager, "Иван", "Иванов", "Иванович", 100000));
    directors.push_back(std::make_unique<Director>(dbManager, "Пётр", "Петров", "Петрович", 120000));

    employees.push_back(new Accountant("Maxim", "Balashov", "Fedorovich", 50000));
    employees.push_back(new Accountant("Vladimir", "Ermolov", "Demidovich", 55000)); 
    employees.push_back(new Accountant("Daniil", "Alekseev", "Nikolaevich", 52000));
    
    employees.push_back(new Secretary("Polina", "Gushchina", "Timurovna", 30000));
    employees.push_back(new Secretary("Danila", "Maximov", "Pavlovich", 40000));
    employees.push_back(new Secretary("Ilya", "Vinogradov", "Lukich", 37000));

    employees.push_back(new Driver("Denis", "Saveliev", "Sergeevich", 46000, { "A","B","C"}, { "Tayota","Subaru"}));
    employees.push_back(new Driver("Anastasia", "Ivanova", "Yaroslavovna", 58000, { "B","B1","C"}, { "Mercedes-Benz"}));
    employees.push_back(new Driver("Vladimir", "Kuznetsov", "Daniilovich", 51000, { "A","B","C" }, { "Kia","Audi"}));
    employees.push_back(new Driver("Timofey", "Smirnov", "Ivanovich", 70000, { "A","B","C" }, { "BMW","Lexus", "Bentley"}));

    employees.push_back(new Programmer("Robert", "Moiseev", "Leonidovich", 80000, "Middle", { "C++" }));
    employees.push_back(new Programmer("Miroslav", "Chernyshev", "Vladimirovich", 80000, "Middle", { "C++" }));
    employees.push_back(new Programmer("Julia", "Ryabova", "Damirovna", 80000, "Junior", { "C++" }));
    employees.push_back(new Programmer("Andrey", "Ermolaev", "Olegovich", 80000, "Middle", { "C++" }));
    
    directors[0].get()->hireEmployee(employees[0], 1);
    employees.erase(employees.begin() + 0);
    directors[0].get()->hireEmployee(employees[3], 1);
    employees.erase(employees.begin() + 3);
    directors[0].get()->hireEmployee(employees[6], 0.5);
    employees.erase(employees.begin() + 6);
    directors[0].get()->hireEmployee(employees[9], 0.8);
    employees.erase(employees.begin() + 9);
    system("clear");
}

int inputPosNumber(std::string& prompt, int max) {
    bool isNumber = true;
    std::string temp = prompt;
    while (true) {
        for (char c : temp) {
            if (!std::isdigit(c)) {
                isNumber = false;
                continue;
            }
        }
        if (!isNumber) {
            std::cout << "Ошибка! Введите целое положительное число: ";
            std::cin >> temp;
            isNumber = true;
        }
        else if (std::stoi(temp) > max or std::stoi(temp)==0) {
            std::cout << "Варианта ответа не существует. Попробуйте снова: ";
            std::cin >> temp;
            isNumber = true;
        }
        else {
            return  std::stoi(temp);
        }
    }
}

double inputNumber(std::string& str) {
    while (true) {
        bool isNumber = true;
        int countPoint = 0;
        int countAlpha = 0;
        for (char c : str) {
            if (!std::isdigit(c)) {
                if (countPoint < 1 and c == ',' and countAlpha > 0) {
                    countPoint++;
                }
                else {
                    isNumber = false;
                    break;
                }
            }
            else if (std::isdigit(c)) {
                countAlpha++;
            }
        }
        if (isNumber) {
            try {
                return std::stod(str); 
            }
            catch (const std::invalid_argument& ) {
                std::cout << "Неверный аргумент\n";
            }
            catch (const std::out_of_range& ) {
                std::cout << "Число выходит за пределы\n";
            }
        }
       
        std::cout << "Ошибка! Введите положительное число: ";
        std::cin >> str;
    }    
}

std::string inputString(std::string& prompt) {
    std::string value = prompt;
    while (true) {
        if (!std::all_of(value.begin(), value.end(), [](char c) { return std::isalpha(c) || c == ' '; })) {
            std::cout << "Введите только буквы: ";
            std::cin >> value;
        }
        else {
            return value;
        }
    }
}

int main() {
    DatabaseManager dbManager("tcp://127.0.0.1:3306", "username", "password", "hr_database");
    dbManager.initializeDatabase();
    

    std::vector<Human*> employees;
    std::vector<std::unique_ptr<Director>> directors;
    
    initializeData(directors, employees, dbManager);
    int dirInd = -1;
    
    try {
        sql::Connection* con = dbManager.getConnection();
        sql::Statement* stmt = con->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT id FROM directors");

        while (res->next()) {
            auto dirPtr = Director::createFromDatabase(dbManager, con, res->getInt("id"));
if (!dirPtr) {
    std::cerr << "Ошибка создания директора из БД" << std::endl;
    continue;
}

Director* dir = dynamic_cast<Director*>(dirPtr);
if (!dir) {
    std::cerr << "Ошибка: созданный объект не является Director!" << std::endl;
    continue;
}
std::unique_ptr<Director> uniqueDir(dirPtr);
directors.push_back(std::move(uniqueDir));  // Добавляем в вектор
        }

        delete res;
        delete stmt;

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT id, type FROM employees WHERE director_id IS NULL");

        while (res->next()) {
            int empId = res->getInt("id");
            std::string type = res->getString("type");

            Human* employee = nullptr;
            if (type == "accountant") {
                employee = Accountant::createFromDatabase(con, empId);
            }
            else if (type == "secretary") {
                employee = Secretary::createFromDatabase(con, empId);
            }
            else if (type == "programmer") {
                employee = Programmer::createFromDatabase(con, empId);
            }
            else if (type == "driver") {
                employee = Driver::createFromDatabase(con, empId);
            }

            if (employee) {
                employees.push_back(employee);
            }
        }

        delete res;
        delete stmt;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }

    std::cout << "Выберите директора, с которым будете работать: \n";
for (size_t i = 0; i < directors.size(); ++i) {
    if (!directors[i]) {
        std::cerr << "Ошибка: директор " << i+1 << " не инициализирован!" << std::endl;
        continue;
    }
    std::cout << i+1 << ". " 
              << directors[i].get()->getName() << " " 
              << directors[i].get()->getSurname() << std::endl;
}

if (directors.empty()) {
    std::cerr << "Нет доступных директоров!" << std::endl;
    return 1;
}

std::string tempDirInd;
std::cin >> tempDirInd;
dirInd = inputPosNumber(tempDirInd, directors.size()) - 1;
if (dirInd < 0 || dirInd >= directors.size() || !directors[dirInd]) {
    std::cerr << "Ошибка: выбран недопустимый директор!" << std::endl;
    return 1;
}

    std::cout << "Выбран директор " << ++dirInd << std::endl;
    dirInd--;
    
    std::string tempChoice;
    int choice;
    do {
        std::cout << "--------------------------------------------\n"
                  << "Меню:\n"
                  << "1. Добавить нового человека\n"
                  << "2. Принять сотрудника на работу\n"
                  << "3. Уволить сотрудника\n"
                  << "4. Вывести подчиненных директора\n"
                  << "5. Рассчитать заработную плату сотрудника\n"
                  << "6. Вывести водителей\n"
                  << "7. Вывести программистов\n"
                  << "8. Добавить директора\n"
                  << "9. Вывести данные о сотруднике\n"
                  << "10.Сменить директора\n"
                  << "11.Повышение квалификации программиста\n"
                  << "12.Добавление новой категории прав водителю\n"
                  << "13.Удалить директора\n"
                  << "14.Вывести внутренние номера сотрудников\n"
                  << "15. Показать статистику зарплат\n"
                  << "16. Показать распределение сотрудников\n"
                  << "17. Поиск сотрудников по критериям\n"
                  << "18.Выйти\n"
                  << "Введите номер действия: ";
        std::cin >> tempChoice;
        choice = inputPosNumber(tempChoice, 18);

        std::cout << "--------------------------------------------\n";

        switch (choice) {
            case 1: {
                std::string tempRoleChoice;
                int roleChoice = 0;
                std::cout << "Выберите должность:\n";
                std::cout << "1. Бухгалтер\n";
                std::cout << "2. Водитель\n";
                std::cout << "3. Программист\n";
                std::cout << "4. Секретарь\n";
                std::cout << "Введите номер должности: ";
                std::cin >> tempRoleChoice;
                roleChoice = inputPosNumber(tempRoleChoice, 4);
    
                std::string name, surname, patronymic;
                double salary = 0.0;
    
                std::cout << "Введите имя: ";
                std::cin >> name;
                name = inputString(name);
                std::cout << "Введите фамилию: ";
                std::cin >> surname;
                surname = inputString(surname);
                std::cout << "Введите отчество: ";
                std::cin >> patronymic;
                patronymic = inputString(patronymic);
                std::cout << "Введите запрашиваемый оклад: ";
                std::string tempSalary;
                std::cin >> tempSalary;
                salary = inputNumber(tempSalary);
    
                switch (roleChoice) {
                case 1: {
                    auto* accountant = new Accountant(name, surname, patronymic, salary);
                    employees.push_back(accountant);
                    std::cout << "Бухгалтер добавлен.\n";
                    break;
                }
                      
                case 2: {
                    auto* driver = new Driver(name, surname, patronymic, salary);
    
                    std::cout << "Введите категории прав  водителя через пробел: ";
                    std::cin.ignore();
                    std::string inputCat;
                    std::getline(std::cin, inputCat);
    
                    std::stringstream sss(inputCat);
                    std::string category;
    
                    while (sss >> category) {
                        driver->addLicenseCategory(category);
                    }
    
                    std::cout << "Введите транспортные стредства водителя через пробел: ";
                    std::string inputVeh;
                    std::getline(std::cin, inputVeh);
    
                    std::stringstream ss(inputVeh);
                    std::string vehicle;
    
                    while (ss >> vehicle) {
                        driver->addVehicle(vehicle);
                    }
    
                    employees.push_back(driver);
                    std::cout << "Водитель добавлен.\n";
                    break;
                }
                case 3: {
                    auto* programmer = new Programmer(name, surname, patronymic, salary);
                    std::cout << "Введите уровень программиста\n"
                        << "1. Junior\n"
                        << "2. Middle\n"
                        << "3. Senior\n"
                        << "Ваш выбор: \n";
                    int level = 0;
                    std::string tempLevel;
                    std::cin >> tempLevel;
                    level = inputPosNumber(tempLevel, 3);
    
                    switch (level) {
                    case 1:
                        programmer->setLevel("Junior");
                        break;
                    case 2:
                        programmer->setLevel("Middle");
                        break;
                    case 3:
                        programmer->setLevel("Senior");
                        break;
                    }
    
                    std::cout << "Введите технологии, которыми владеет программист, через пробел: ";
    
                    std::cin.ignore();
                    std::string inputTech;
                    std::getline(std::cin, inputTech);
    
                    std::stringstream ss(inputTech);
                    std::string tech;
    
                    while (ss >> tech) {
                        programmer->addTechnology(tech);
                    }
    
                    employees.push_back(programmer);
                    std::cout << "Программист добавлен.\n";
                    break;
                }
                case 4: {
                    auto* secretary = new Secretary(name, surname, patronymic, salary);
                    employees.push_back(secretary);
                    std::cout << "Секретарь добавлен.\n";
                    break;
                }
                default:
                    std::cout << "Некорректный выбор должности.\n";
                }
                break;
            }
            
            case 2: {
    
                if (dirInd == -1) {
                    std::cout << "Сначала выберите директора\n";
                    break;
                }
                std::cout << "Кого необходимо нанять?\n";
                if (!(directors[dirInd]->isAccountant(*directors[dirInd]))) {
                    std::cout << "Примечание: сначала необходимо нанять бухгалтера\n";
    
                }
                else if (!directors[dirInd]->isSecretary(*directors[dirInd])) {
                    std::cout << "Примечание: необходимо нанять секретаря\n";
    
                }
                std::cout << "1. Секретарь\n"
                    << "2. Бухгалтер\n"
                    << "3. Водитель\n"
                    << "4. Программист\n"
                    << "Ваш выбор: ";
                int v = 0;
                std::string tempV;
                std::cin >> tempV;
                v = inputPosNumber(tempV, 4);
    
                int i = 1;
                int tempempIndex = -1;
                int empIndex = -1;
                std::vector<int> secretary;
                std::vector<int> accountant;
                std::vector<int> programmer;
                std::vector<int> driver;
                bool ans = true;
                switch (v) {
                case 1: {
                    if (!(directors[dirInd]->isAccountant(*directors[dirInd]))) {
                        std::cout << "Сначала наймите бухгалтера!\n";
                        ans = false;
                        break;
                        
                    }
                    if (directors[dirInd]->isSecretary(*directors[dirInd])) {
                        std::cout << "У текущего директора уже есть секретарь!\n";
                        ans = false;
                        break;
                        
                    }
                    i = 0;
    
                    std::cout << "Список доступных для найма секретарей:\n\n"
                        << std::left << std::setw(15) << "Имя"
                        << std::left << std::setw(15) << "Фамилия"
                        << std::left << std::setw(10) << "Запрашиваемый оклад\n\n";
    
                    for (int j = 0; j < employees.size(); ++j) {
    
                        if (dynamic_cast<class Secretary*>(employees[j])) {
                            secretary.push_back(j);
                            std::cout << ++i << ". ";
                            std::cout << std::left << std::setw(15) << employees[j]->getName()
                                << std::left << std::setw(15) << employees[j]->getSurname()
                                << std::left << std::setw(10) << employees[j]->getSalary() << "\n";
                        }
                    }
                    std::cout << "Ваш выбор: ";
                    std::string temp;
                    std::cin >> temp;
                    tempempIndex = inputPosNumber(temp, secretary.size());
                    --tempempIndex;
                    empIndex = std::distance(employees.begin(), find(employees.begin(), employees.end(), employees[secretary[tempempIndex]]));
    
                    break;
                }
                case 2: {
                    
                    if ((directors[dirInd]->isAccountant(*directors[dirInd]))) {
                        std::cout << "У текущего директора уже есть бухгалтер!\n";
                        ans = false;
                        break;
                    }
    
                    i = 0;
                    std::cout << "Список доступных бухгалтеров:\n\n"
                        << std::left << std::setw(15) << "Имя"
                        << std::left << std::setw(15) << "Фамилия"
                        << std::left << std::setw(10) << "Запрашиваемый оклад\n\n";
    
                    for (int j = 0; j < employees.size(); ++j) {
    
                        if (dynamic_cast<class Accountant*>(employees[j])) {
                            accountant.push_back(j);
                            std::cout << ++i << ". "
                                << std::left << std::setw(15) << employees[j]->getName()
                                << std::left << std::setw(15) << employees[j]->getSurname()
                                << std::left << std::setw(10) << employees[j]->getSalary() << "\n";
                        }
                    }
                    std::cout << "Ваш выбор: ";
                    int tempempIndex;
                    std::string temp;
                    std::cin >> temp;
                    tempempIndex = inputPosNumber(temp, accountant.size()+1);
                    --tempempIndex;
                    empIndex = std::distance(employees.begin(), find(employees.begin(), employees.end(), employees[accountant[tempempIndex]]));
                    break;
                    
                }
                case 3: {
                    if (!(directors[dirInd]->isAccountant(*directors[dirInd]))) {
                        std::cout << "Сначала наймите бухгалтера!\n";
                        ans = false;
                        break;
                    }
                    if (!directors[dirInd]->isSecretary(*directors[dirInd])) {
                        std::cout << "Сначала наймите секретаря!\n";
                        ans = false;
                        break;
                    }
                    i = 0;
    
                    std::cout << "Список доступных водетелей:\n\n"
                        << std::left << std::setw(15) << "    Имя"
                        << std::left << std::setw(15) << "Фамилия"
                        << std::left << std::setw(20) << "Категории прав"
                        << std::left << std::setw(25) << "Транспортные средства "
                        << std::left << std::setw(10) << "Запрашиваемый оклад\n\n";
                    for (int j = 0; j < employees.size(); ++j) {
                        if (Driver* drv = dynamic_cast<Driver*>(employees[j])) {
                            driver.push_back(j);
                            std::vector<std::string> licenseCategories = drv->getLicenseCategories();
                            std::vector<std::string> vehicles = drv->getVehicles();
                            std::cout << ++i << ". ";
                            std::cout << std::left << std::setw(15) << drv->getName() << std::left << std::setw(15) << drv->getSurname();
    
                            for (int i = 0; i < licenseCategories.size(); ++i) {
                                if (i != licenseCategories.size() - 1) {
                                    std::cout << std::left << licenseCategories[i] << ", ";
                                }
                                else {
                                    std::cout << std::left << std::setw(20) << licenseCategories[i];
                                }
                            }
                            for (int i = 0; i < vehicles.size(); ++i) {
                                if (i != vehicles.size() - 1) {
                                    std::cout << std::left << vehicles[i] << ", ";
                                }
                                else {
                                    std::cout << std::left << std::setw(25) << vehicles[i];
                                }
                            }
    
                            std::cout << std::left << std::setw(10) << drv->getSalary() << "\n";
                        }
                    }
    
                    std::cout << "Ваш выбор: ";
    
                    std::string temp;
                    std::cin >> temp;
                    tempempIndex = inputPosNumber(temp, driver.size());
                    --tempempIndex;
                    empIndex = std::distance(employees.begin(), find(employees.begin(), employees.end(), employees[driver[tempempIndex]]));
                    break;
                }
                case 4: {
                    if (!(directors[dirInd]->isAccountant(*directors[dirInd]))) {
                        std::cout << "Сначала наймите бухгалтера!\n";
                        ans = false;
                        break;
                    }
                    if (!directors[dirInd]->isSecretary(*directors[dirInd])) {
                        std::cout << "Сначала наймите секретаря!\n";
                        ans = false;
                        break;
                    }
                    i = 0;
    
                    std::cout << "Список доступных программистов:\n\n"
                        << std::left << std::setw(15) << "    Имя"
                        << std::left << std::setw(15) << "Фамилия"
                        << std::left << std::setw(15) << "Уровень"
                        << std::left << std::setw(15) << "Технологии"
                        << std::left << std::setw(10) << "Запрашиваемый оклад\n\n";
                    for (int j = 0; j < employees.size(); ++j) {
                        if (Programmer* prog = dynamic_cast<Programmer*>(employees[j])) {
                            programmer.push_back(j);
                            std::vector<std::string> technologies = prog->getTechnologies();
                            std::cout << ++i << ". ";
                            std::cout << std::left << std::setw(15) << prog->getName()
                                << std::left << std::setw(15) << prog->getSurname()
                                << std::left << std::setw(15) << prog->getLevel();
    
                            for (int i = 0; i < technologies.size(); ++i) {
                                if (i != technologies.size() - 1) {
                                    std::cout << std::left << technologies[i] << ", ";
                                }
                                else {
                                    std::cout << std::left << std::setw(15) << technologies[i];
                                }
                            }
    
                            std::cout << std::left << std::setw(10) << prog->getSalary() << "\n";
                        }
    
    
                    }
    
                    std::cout << "Ваш выбор: ";
                    std::string temp;
                    std::cin >> temp;
                    tempempIndex = inputPosNumber(temp, programmer.size());
                    --tempempIndex;
                    empIndex = std::distance(employees.begin(), find(employees.begin(), employees.end(), employees[programmer[tempempIndex]]));
                    break;
                    }
                }
                if (ans) {
                    std::cout << "На какую ставку нанимается сотрудник? (0;1]\n";
                    double rate;
                    std::string tempRate;
                    std::cin >> tempRate;
                    rate = inputNumber(tempRate);
                    rate = std::stod(tempRate);
                    while (!(rate > 0 and rate <= 1)) {
                        std::cout << "Введите число от 0 до 1: ";
                        std::cin >> tempRate;
                        rate = inputNumber(tempRate);
                    }
    
                    directors[dirInd]->hireEmployee(employees[empIndex], rate);
    
                    employees.erase(employees.begin() + empIndex);
                }
                
                break;
            }
            
                case 3: {
    
                    if (dirInd == -1) {
                        std::cout << "Сначала выберите директора\n";
                        break;
                    }
    
                    auto staff = directors[dirInd]->getEmployees();
                    if (staff.empty()) {
                        std::cout << "У директора нет сотрудников.\n";
                        break;
                    }
    
                    std::cout << "Выберите сотрудника для увольнения:\n";
                    for (int i = 0; i < staff.size(); ++i) {
                        std::cout << i + 1 << ". " << staff[i]->getName() << " " << staff[i]->getSurname() << "\n";
                    }
                    int empIndex;
                    std::string tempEmpIndex;
                    std::cin >> tempEmpIndex;
    
                    empIndex = inputPosNumber(tempEmpIndex, staff.size());
                    --empIndex;
    
                    employees.push_back(staff[empIndex]);
                    directors[dirInd]->fireEmployee(staff[empIndex]);
                    std::cout << "Сотрудник уволен.\n";
                    break;
                }
    
                case 4: {
    
                    if (dirInd == -1) {
                        std::cout << "Сначала выберите директора\n";
                        break;
                    }
                    if (!directors[dirInd]->isSecretary(*directors[dirInd])) {
                        std::cout << "Сначала наймите секретаря" << std::endl;
                        break;
                    }
    
                    directors[dirInd]->getEmployees(*directors[dirInd]);
                    break;
                }
    
                case 5: {
    
                    if (dirInd == -1) {
                        std::cout << "Сначала выберите директора\n";
                        break;
                    }
                    if (!directors[dirInd]->isAccountant(*directors[dirInd])) {
                        std::cout << "Сначала наймите бухгалтера" << std::endl;
                        break;
                    }
                    if (!directors[dirInd]->isSecretary(*directors[dirInd])) {
                        std::cout << "Сначала наймите секретаря" << std::endl;
                        break;
                    }
                    const auto& staff = directors[dirInd]->getEmployees();
    
                    std::cout << "Выберите сотрудника:\n";
                    directors[dirInd]->getEmployees(*directors[dirInd]);
                    int empIndex;
                    std::string tempEmpInd;
                    std::cin >> tempEmpInd;
                    empIndex = inputPosNumber(tempEmpInd, staff.size());
                    --empIndex;
    
                     std::cout << "Зарплата сотрудника: " << directors[dirInd]->countSalary(staff[empIndex]) << "\n";
                    break;
                }
    
                case 6: {
    
                    if (dirInd == -1) {
                        std::cout << "Сначала выберите директора\n";
                        break;
                    }
                    if (!directors[dirInd]->isSecretary(*directors[dirInd])) {
                        std::cout << "Сначала наймите секретаря" << std::endl;
                        break;
                    }
                    directors[dirInd]->printDrivers(*directors[dirInd]);
    
                    break;
                }
    
                case 7: {
    
                    if (dirInd == -1) {
                        std::cout << "Сначала выберите директора\n";
                        break;
                    }
                    if (!directors[dirInd]->isSecretary(*directors[dirInd])) {
                        std::cout << "Сначала наймите секретаря" << std::endl;
                        break;
                    }
                    directors[dirInd]->printProgrammers(*directors[dirInd]);
                    break;
                }
    
                case 8: {
                    std::string name, surname, patronymic;
                    double salary;
                    std::cout << "Введите имя: ";
                    std::cin >> name;
                    name = inputString(name);
                    std::cout << "Введите фамилию: ";
                    std::cin >> surname;
                    surname = inputString(surname);
                    std::cout << "Введите отчество: ";
                    std::cin >> patronymic;
                    patronymic = inputString(patronymic);
                    std::cout << "Введите оклад: ";
                    std::string tempSalary;
                    std::cin >> tempSalary;
                    salary = inputNumber(tempSalary);
                    directors.push_back(std::make_unique<Director>(dbManager, name, surname, patronymic, salary));
                    std::cout << "Директор добавлен.\n";
                    break;
                }
                case 9: {
                    if (dirInd == -1) {
                        std::cout << "Сначала выберите директора\n";
                        break;
                    }
                    const auto& staff = directors[dirInd]->getEmployees();
                    if (staff.empty()) {
                        std::cout << "У директора нет сотрудников.\n";
                        break;
                    }
    
                    std::cout << "Выберите сотрудника:\n";
                    directors[dirInd]->getEmployees(*directors[dirInd]);
                    int empIndex;
                    std::string tempEmpInd;
                    std::cin >> tempEmpInd;
                    empIndex = inputPosNumber(tempEmpInd, staff.size());
                   
                    --empIndex;
    
                    directors[dirInd]->print(staff[empIndex]);
                    break;
                }
                case 10: {
                    if (directors.empty()) {
                        std::cout << "Нет доступных директоров.\n";
                        break;
                    }
                    std::cout << "Выберите директора:\n";
                    for (int i = 0; i < directors.size(); ++i) {
                        std::cout << i + 1 << ". " << directors[i]->getName() << " " << directors[i]->getSurname() << "\n";
                    }
                    std::string tempDirInd;
                    std::cin >> tempDirInd;
                    dirInd = inputPosNumber(tempDirInd, directors.size());
    
                    std::cout << "Выбран директор " << dirInd << std::endl;
                    --dirInd;
                    break;
                }
                case 11: {
    
                    if (dirInd == -1) {
                        std::cout << "Сначала выберите директора\n";
                        break;
                    }
                    if (directors.empty()) {
                        std::cout << "Нет доступных директоров.\n";
                        break;
                    }
                    
                    std::vector<int> programmer;
                    std::vector<Human*> staff = directors[dirInd]->getEmployees();
                    
                    
                    int j = 0;
                    int countProg = 0;
                    for (int i = 0; i < directors[dirInd]->getEmployees().size(); ++i) {
                        if (dynamic_cast<Programmer*>(directors[dirInd]->getEmployees()[i])) {
                            countProg++;
                            if (countProg < 1) {
                                std::cout << "Выберите программиста: " << std::endl;
                            }
                            Programmer* prog = dynamic_cast<Programmer*>(directors[dirInd]->getEmployees()[i]);
                            programmer.push_back(i);
                            std::cout << ++j << ". " << prog->getName() << " " << prog->getSurname() << std::endl;
                        }
                    }
                    if (programmer.size() == 0) {
                        std::cout << "У директора нет программистов\n";
                        break;
                    }
                    std::string temp;
                    std::cin >> temp;
                    int tempprogInd = -1;
                    int progInd = -1;
    
                    tempprogInd = inputPosNumber(temp, programmer.size());
                    --tempprogInd;
    
                    progInd = std::distance(staff.begin(), staff.begin() + programmer[tempprogInd]);
    
                    Programmer* prog = dynamic_cast<Programmer*>(directors[dirInd]->getEmployees()[progInd]);
    
                    if (prog->getLevel() == "Senior") {
                        std::cout << "У выбранного программиста уже наивысший уровень\n";
                        break;
                    }
                    else if (prog->getLevel() == "Junior") {
                        prog->setLevel("Middle");
    
                    }
                    else if (prog->getLevel() == "Middle") {
                        prog->setLevel("Senior");
    
                    }
                    std::cout << "Поднять ли этому программисту оклад?\n"
                        << "1. Да\n"
                        << "2. Нет\n"
                        << "Ваш выбор: ";
                    int v = 0;
                    std::string tempV;
                    std::cin >> tempV;
                    v = inputPosNumber(tempV, 2);
    
                    if(v == 1) {
                        if (!(directors[dirInd]->otvet())) {
                            std::cout << "Директор отказал в повышении оклада\n";
                            break;
                        }
                        else {
                            std::cout << "Директор одобрил повышение оклада\n";
                            std::cout << "Введите новый оклад: ";
                            double newSalary = 0;
                            std::string tempS;
                            std::cin >> tempS;
                            newSalary = inputNumber(tempS);
                            directors[dirInd]->boostSalary(prog, newSalary);
    
                        }
                    }
                    
    
                    break;
                }
                       
                case 12: {
                    if (dirInd == -1) {
                        std::cout << "Сначала выберите директора\n";
                        break;
                    }
                    if (directors.empty()) {
                        std::cout << "Нет доступных директоров.\n";
                    }
    
    
                    std::vector<int> driver;
                    std::vector<Human*> staff = directors[dirInd]->getEmployees();
                    int j = 0;
                    int countDrv = 0;
                    for (int i = 0; i < directors[dirInd]->getEmployees().size(); ++i) {
                        if (dynamic_cast<Driver*>(directors[dirInd]->getEmployees()[i])) {
                            countDrv++;
                            if (countDrv < 1) {
                                std::cout << "Выберите водителя: " << std::endl;
                            }
                            Driver* drv = dynamic_cast<Driver*>(directors[dirInd]->getEmployees()[i]);
                            driver.push_back(i);
                            std::cout << ++j << ". " << drv->getName() << " " << drv->getSurname() << std::endl;
                        }
                    }
                    if (driver.size() == 0) {
                        std::cout << "У директора нет программистов\n";
                        break;
                    }
                    int tempprogInd = -1;
                    int progInd = -1;
                    std::string temp;
                    std::cin >> temp;
                    tempprogInd = inputPosNumber(temp, driver.size());
                    --tempprogInd;
    
                    progInd = std::distance(staff.begin(), staff.begin() + driver[tempprogInd]);
    
                    Driver* drv = dynamic_cast<Driver*>(directors[dirInd]->getEmployees()[progInd]);
                    std::cout << "Предыдущие категории прав: ";
                    std::vector<std::string> licnseCategories = drv->getLicenseCategories();
                    for (const auto& lic : licnseCategories) {
                        std::cout << lic << " ";
                    }
                    std::cout << "\nНа какую категорию сдал водитель?\n";
                    std::string newLic;
                    std::cin >> newLic;
                    drv->addLicenseCategory(newLic);
                    std::cout << "Поднять ли этому водителю оклад?\n"
                        << "1. Да\n"
                        << "2. Нет\n"
                        << "Ваш выбор: ";
                    int v = 0;
                    std::string tempV;
                    std::cin >> tempV;
                    v = inputPosNumber(tempV, 2);
    
                    if (v == 1) {
                        if (!(directors[dirInd]->otvet())) {
                            std::cout << "Директор отказал в повышении оклада\n";
                            break;
                        }
                        else {
                            std::cout << "Директор одобрил повышение оклада\n";
                            std::cout << "Введите новый оклад: ";
                            double newSalary = 0;
                            std::string tempS;
                            std::cin >> tempS;
                            newSalary = inputNumber(tempS);
                            directors[dirInd]->boostSalary(drv, newSalary);
    
                        }
                    }
                    break;
                }
                case 13: {
                    if (directors.empty()) {
                        std::cout << "Нет доступных директоров.\n";
                        break;
                    }
                    std::cout << "Выберите директора:\n";
                    for (int i = 0; i < directors.size(); ++i) {
                        std::cout << i + 1 << ". " << directors[i]->getName() << " " << directors[i]->getSurname() << "\n";
                    }
                    std::string tempDirInd;
                    std::cin >> tempDirInd;
                    dirInd = inputPosNumber(tempDirInd, directors.size());
                    dirInd--;
                    auto staff = directors[dirInd]->getEmployees();
                    for (const auto c : staff) {
                        employees.push_back(c);
                        directors[dirInd]->fireEmployee(c);
                    }
                    auto it = std::find(directors.begin(), directors.end(), directors[dirInd]);
                    directors.erase(it);
                    std::cout << "Директор " << dirInd + 1 << " удален" << std::endl;
                    dirInd = -1;
                    break;
                }
                case 14: {
                    if (dirInd == -1) {
                        std::cout << "Сначала выберите директора\n";
                        break;
                    }
                    if (directors.empty()) {
                        std::cout << "Нет доступных директоров.\n";
                    }
                    if (!directors[dirInd]->isAccountant(*directors[dirInd])) {
                        std::cout << "Сначала наймите бухгалтера" << std::endl;
                        break;
                    }
                    if (!directors[dirInd]->isSecretary(*directors[dirInd])) {
                        std::cout << "Сначала наймите секретаря" << std::endl;
                        break;
                    }
                    directors[dirInd]->printInNum(*directors[dirInd]);
                    break;
                }
                case 15: {
                    HRAnalyzer analyzer(dbManager.getConnection());
                    analyzer.printSalaryStatistics();
                    break;
                }
                case 16: {
                    HRAnalyzer analyzer(dbManager.getConnection());
                    analyzer.printEmployeeDistribution();
                    break;
                }
                case 17: {
                    searchEmployees(dbManager.getConnection());
                    break;
                }
            case 18:
                std::cout << "Выход из программы.\n";
                break;
            default:
                std::cout << "Ошибка: введите число от 0 до 18.\n";
        }
    } while (choice != 18);

    try {
        sql::Connection* con = dbManager.getConnection();
        if (!con || con->isClosed()) {
            std::cerr << "Database connection error!" << std::endl;
            return 1;
        }
    
        for (auto& dir : directors) {
            if (!dir->saveToDatabase(con)) {
                std::cerr << "Failed to save director (ID: " << dir->getInNum() << ")" << std::endl;
            }
        }

        for (auto emp : employees) {
            if (!emp->saveToDatabase(con)) {
                std::cerr << "Failed to save employee (ID: " << emp->getInNum() << ")" << std::endl;
            }
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
    for (auto emp : employees) {
        delete emp;
    }
    employees.clear();
    directors.clear();

    return 0;
}