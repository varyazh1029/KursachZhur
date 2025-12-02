#ifndef CONTRACTS_H
#define CONTRACTS_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <functional>

// Макрос для ключа шифрования (если используется)
#define KEY 3

class Encryption {
public:
    static std::string encrypt(const std::string& data);
    static std::string decrypt(const std::string& data);
};

class Entity {
protected:
    int id;
public:
    Entity(int id = 0);
    int getId() const;
    void setId(int newId);
    virtual void display() const = 0;
    virtual void saveToFile(std::ofstream& file) const = 0;
    virtual void loadFromFile(std::ifstream& file) = 0;
    virtual ~Entity() = default;
};

class Date {
private:
    int day;
    int month;
    int year;
public:
    Date(int d = 1, int m = 1, int y = 2000);
    bool operator<(const Date& other) const;
    bool operator==(const Date& other) const;
    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const Date& date);
    friend std::istream& operator>>(std::istream& is, Date& date);
};

class User : public Entity {
private:
    std::string login;
    std::string password; // Зашифрованный
    bool isAdmin;
public:
    User(int id = 0, const std::string& login = "", const std::string& password = "", bool isAdmin = false);
    void setPassword(const std::string& pwd);
    bool checkPassword(const std::string& pwd) const;
    bool getIsAdmin() const;
    std::string getLogin() const;
    void display() const override;
    void saveToFile(std::ofstream& file) const override;
    void loadFromFile(std::ifstream& file) override;
};

class Client : public Entity {
private:
    std::string companyName;
    std::string contactPerson;
    std::string phone;
    std::string email;
    std::string address;
public:
    Client(int id = 0, const std::string& company = "", const std::string& contact = "",
        const std::string& phone = "", const std::string& email = "", const std::string& address = "");

    // Геттеры
    std::string getCompanyName() const;
    std::string getContactPerson() const;
    std::string getPhone() const;
    std::string getEmail() const;
    std::string getAddress() const;

    // Сеттеры для редактирования:
    void setCompanyName(const std::string& name);
    void setContactPerson(const std::string& person);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);
    void setAddress(const std::string& address);

    void display() const override;
    void saveToFile(std::ofstream& file) const override;
    void loadFromFile(std::ifstream& file) override;
};

class ConstructionObject : public Entity {
private:
    std::string objectName;
    std::string address;
    std::string objectType;
    double area;
public:
    ConstructionObject(int id = 0, const std::string& name = "", const std::string& addr = "",
        const std::string& type = "", double area = 0.0);

    // Геттеры
    std::string getName() const;
    std::string getAddress() const;
    std::string getType() const;
    double getArea() const;

    // Сеттеры для редактирования:
    void setName(const std::string& name);
    void setAddress(const std::string& address);
    void setType(const std::string& type);
    void setArea(double area);

    void display() const override;
    void saveToFile(std::ofstream& file) const override;
    void loadFromFile(std::ifstream& file) override;
};

class Contract : public Entity {
private:
    int clientId;
    int objectId;
    Date startDate;
    int duration;
    double contractAmount;
    std::string workType;
    std::string status;
    std::string manager;
public:
    Contract(int id = 0, int clientId = 0, int objectId = 0, const Date& date = Date(),
        int duration = 0, double amount = 0.0, const std::string& workType = "",
        const std::string& status = "", const std::string& manager = "");

    bool operator<(const Contract& other) const;

    // Геттеры
    double getAmount() const;
    std::string getStatus() const;
    std::string getWorkType() const;
    std::string getManager() const;
    Date getStartDate() const;
    int getClientId() const;
    int getObjectId() const;
    int getDuration() const;

    // Сеттеры для редактирования:
    void setClientId(int id);
    void setObjectId(int id);
    void setStartDate(const Date& date);
    void setDuration(int duration);
    void setAmount(double amount);
    void setWorkType(const std::string& type);
    void setStatus(const std::string& status);
    void setManager(const std::string& manager);

    void display() const override;
    void saveToFile(std::ofstream& file) const override;
    void loadFromFile(std::ifstream& file) override;
};

template<typename T>
class Repository {
private:
    std::vector<std::shared_ptr<T>> data;
    std::string filename;

public:
    Repository(const std::string& fname) : filename(fname) {}

    void add(std::shared_ptr<T> item) {
        data.push_back(item);
    }

    bool remove(int id) {
        auto it = std::remove_if(data.begin(), data.end(), [id](const std::shared_ptr<T>& item) {
            return item->getId() == id;
            });
        if (it == data.end()) {
            return false;
        }
        data.erase(it, data.end());
        return true;
    }

    std::shared_ptr<T> find(int id) const {
        for (const auto& item : data) {
            if (item->getId() == id) {
                return item;
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<T>> findAll() const {
        return data;
    }

    size_t size() const {
        return data.size();
    }

    // Поиск с использованием лямбда-функции
    std::vector<std::shared_ptr<T>> search(std::function<bool(const std::shared_ptr<T>&)> predicate) const {
        std::vector<std::shared_ptr<T>> results;
        for (const auto& item : data) {
            if (predicate(item)) {
                results.push_back(item);
            }
        }
        return results;
    }

    // Сортировка с использованием лямбда-функции
    std::vector<std::shared_ptr<T>> sort(std::function<bool(const std::shared_ptr<T>&, const std::shared_ptr<T>&)> comparator) const {
        std::vector<std::shared_ptr<T>> sortedData = data;
        std::sort(sortedData.begin(), sortedData.end(), comparator);
        return sortedData;
    }

    void saveToFile() const {
        std::ofstream file(filename, std::ios::binary | std::ios::out | std::ios::trunc);
        if (file.is_open()) {
            for (const auto& item : data) {
                item->saveToFile(file);
            }
            file.close();
        }
        else {
            throw std::runtime_error("Невозможно открыть файл для записи: " + filename);
        }
    }

    void loadFromFile() {
        std::ifstream file(filename, std::ios::binary | std::ios::in);
        if (file.is_open()) {
            data.clear();
            while (!file.eof()) {
                std::shared_ptr<T> item = std::make_shared<T>();
                item->loadFromFile(file);
                if (file.good()) {
                    data.push_back(item);
                }
            }
            file.close();
        }
    }
};

class ReportGenerator {
public:
    static void generateContractsReport(const Repository<Contract>& contracts,
        const Repository<Client>& clients,
        const Repository<ConstructionObject>& objects);
};

#endif // CONTRACTS_H