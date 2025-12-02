#include "contracts.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip> 

using namespace std;

Entity::Entity(int id) : id(id) {}

int Entity::getId() const {
    return id;
}

void Entity::setId(int newId) {
    id = newId;
}

// Реализация сеттеров для Contract
void Contract::setClientId(int id) { clientId = id; }
void Contract::setObjectId(int id) { objectId = id; }
void Contract::setStartDate(const Date& date) { startDate = date; }
void Contract::setDuration(int duration) { this->duration = duration; }
void Contract::setAmount(double amount) { contractAmount = amount; }
void Contract::setWorkType(const std::string& type) { workType = type; }
void Contract::setStatus(const std::string& status) { this->status = status; }
void Contract::setManager(const std::string& manager) { this->manager = manager; }

// Реализация сеттеров для Client
void Client::setCompanyName(const std::string& name) { companyName = name; }
void Client::setContactPerson(const std::string& person) { contactPerson = person; }
void Client::setPhone(const std::string& phone) { this->phone = phone; }
void Client::setEmail(const std::string& email) { this->email = email; }
void Client::setAddress(const std::string& address) { this->address = address; }

// Реализация сеттеров для ConstructionObject
void ConstructionObject::setName(const std::string& name) { objectName = name; }
void ConstructionObject::setAddress(const std::string& address) { this->address = address; }
void ConstructionObject::setType(const std::string& type) { objectType = type; }
void ConstructionObject::setArea(double area) { this->area = area; }


string Encryption::encrypt(const string& data) {
    string result = data;
    for (char& c : result) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = base + (c - base + KEY) % 26;
        }
    }
    return result;
}

string Encryption::decrypt(const string& data) {
    string result = data;
    for (char& c : result) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            c = base + (c - base - KEY + 26) % 26;
        }
    }
    return result;
}

Date::Date(int d, int m, int y) : day(d), month(m), year(y) {}

bool Date::operator<(const Date& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

bool Date::operator==(const Date& other) const {
    return day == other.day && month == other.month && year == other.year;
}

string Date::toString() const {
    stringstream ss;
    ss << setfill('0') << setw(2) << day << "."
        << setfill('0') << setw(2) << month << "."
        << year;
    return ss.str();
}

ostream& operator<<(ostream& os, const Date& date) {
    os << date.day << " " << date.month << " " << date.year;
    return os;
}

istream& operator>>(istream& is, Date& date) {
    is >> date.day >> date.month >> date.year;
    return is;
}

User::User(int id, const string& login, const string& password, bool isAdmin)
    : Entity(id), login(login), isAdmin(isAdmin) {
    setPassword(password);
}

void User::setPassword(const string& pwd) {
    password = Encryption::encrypt(pwd);
}

bool User::checkPassword(const string& pwd) const {
    return Encryption::encrypt(pwd) == password;
}

bool User::getIsAdmin() const {
    return isAdmin;
}

string User::getLogin() const {
    return login;
}

void User::display() const {
    cout << "ID: " << id << ", Логин: " << login << ", Админ: " << (isAdmin ? "Да" : "Нет") << endl;
}

void User::saveToFile(ofstream& file) const {
    file << id << " " << login << " " << password << " " << isAdmin << endl;
}

void User::loadFromFile(ifstream& file) {
    file >> id >> login >> password >> isAdmin;
}

Client::Client(int id, const string& company, const string& contact,
    const string& phone, const string& email, const string& address)
    : Entity(id), companyName(company), contactPerson(contact),
    phone(phone), email(email), address(address) {
}

void Client::display() const {
    cout << "ID: " << id << ", Компания: " << companyName
        << ", Контактное лицо: " << contactPerson << ", Телефон: " << phone << endl;
}

string Client::getCompanyName() const {
    return companyName;
}

string Client::getContactPerson() const {
    return contactPerson;
}

string Client::getPhone() const {
    return phone;
}

string Client::getEmail() const {
    return email;
}

string Client::getAddress() const {
    return address;
}

void Client::saveToFile(ofstream& file) const {
    file << id << "|" << companyName << "|" << contactPerson << "|"
        << phone << "|" << email << "|" << address << endl;
}

void Client::loadFromFile(ifstream& file) {
    string line;
    if (getline(file, line)) {
        stringstream ss(line);
        string temp;

        if (!getline(ss, temp, '|')) return;
        try {
            id = stoi(temp);
        }
        catch (...) {
            return;
        }

        if (!getline(ss, companyName, '|')) return;
        if (!getline(ss, contactPerson, '|')) return;
        if (!getline(ss, phone, '|')) return;
        if (!getline(ss, email, '|')) return;
        if (!getline(ss, address)) return;
    }
}

ConstructionObject::ConstructionObject(int id, const string& name, const string& addr,
    const string& type, double area)
    : Entity(id), objectName(name), address(addr), objectType(type), area(area) {
}

void ConstructionObject::display() const {
    cout << "ID: " << id << ", Название: " << objectName << ", Адрес: " << address
        << ", Тип: " << objectType << ", Площадь: " << area << endl;
}

string ConstructionObject::getName() const {
    return objectName;
}

string ConstructionObject::getAddress() const {
    return address;
}

string ConstructionObject::getType() const {
    return objectType;
}

double ConstructionObject::getArea() const {
    return area;
}

void ConstructionObject::saveToFile(ofstream& file) const {
    file << id << "|" << objectName << "|" << address << "|"
        << objectType << "|" << area << endl;
}

void ConstructionObject::loadFromFile(ifstream& file) {
    string line;
    if (getline(file, line)) {
        stringstream ss(line);
        string temp;

        if (!getline(ss, temp, '|')) return;
        try {
            id = stoi(temp);
        }
        catch (...) {
            return;
        }

        if (!getline(ss, objectName, '|')) return;
        if (!getline(ss, address, '|')) return;
        if (!getline(ss, objectType, '|')) return;

        if (!getline(ss, temp)) return;
        try {
            area = stod(temp);
        }
        catch (...) {
            return;
        }
    }
}

Contract::Contract(int id, int clientId, int objectId, const Date& date,
    int duration, double amount, const string& workType,
    const string& status, const string& manager)
    : Entity(id), clientId(clientId), objectId(objectId), startDate(date),
    duration(duration), contractAmount(amount), workType(workType),
    status(status), manager(manager) {
}

bool Contract::operator<(const Contract& other) const {
    return contractAmount < other.contractAmount;
}

void Contract::display() const {
    cout.unsetf(ios_base::floatfield);
    cout << setprecision(6);

    cout << "Номер контракта: " << id
        << ", ID клиента: " << clientId
        << ", ID объекта: " << objectId
        << ", Дата начала: " << startDate.toString()
        << ", Срок: " << duration << " дн."
        << ", Сумма: " << fixed << setprecision(0) << contractAmount
        << ", Тип работ: " << workType
        << ", Статус: " << status
        << ", Менеджер: " << manager
        << endl;
    cout << endl; // Добавляем пустую строку между контрактами
}

void Contract::saveToFile(ofstream& file) const {
    file << id << " " << clientId << " " << objectId << " "
        << startDate << " " << duration << " " << contractAmount << "|"
        << workType << "|" << status << "|" << manager << endl;
}

void Contract::loadFromFile(ifstream& file) {
    string line;
    if (getline(file, line)) {
        stringstream ss(line);

        ss >> id >> clientId >> objectId >> startDate >> duration >> contractAmount;

        if (ss.fail()) return;

        size_t pipePos = line.find('|', ss.tellg());
        if (pipePos == string::npos) return;

        ss.seekg(pipePos + 1);

        if (!getline(ss, workType, '|')) return;
        if (!getline(ss, status, '|')) return;
        if (!getline(ss, manager)) return;
    }
}

double Contract::getAmount() const {
    return contractAmount;
}

string Contract::getStatus() const {
    return status;
}

string Contract::getWorkType() const {
    return workType;
}

string Contract::getManager() const {
    return manager;
}

Date Contract::getStartDate() const {
    return startDate;
}

int Contract::getClientId() const {
    return clientId;
}

int Contract::getObjectId() const {
    return objectId;
}

int Contract::getDuration() const {
    return duration;
}

void ReportGenerator::generateContractsReport(const Repository<Contract>& contracts,
    const Repository<Client>& clients,
    const Repository<ConstructionObject>& objects) {
    cout << "\n========== ОТЧЕТ ПО КОНТРАКТАМ ==========\n";
    auto allContracts = contracts.findAll();

    double totalAmount = 0;
    int activeCount = 0;

    cout.unsetf(ios_base::floatfield);
    cout << setprecision(6);

    for (const auto& contract : allContracts) {
        totalAmount += contract->getAmount();
        if (contract->getStatus() == "в работе") activeCount++;

        auto client = clients.find(contract->getClientId());
        auto object = objects.find(contract->getObjectId());

        cout << "Контракт " << contract->getId() << ": "
            << (client ? client->getCompanyName() : "N/A") << " - "
            << (object ? object->getName() : "N/A") << " - "
            << fixed << setprecision(0) << contract->getAmount() << " руб. - "
            << contract->getStatus() << endl;
    }

    cout << "\nИтого:\n";
    cout << "Всего контрактов: " << allContracts.size() << endl;
    cout << "Контрактов в работе: " << activeCount << endl;
    cout << "Общая сумма: " << fixed << setprecision(0) << totalAmount << " руб.\n";
    cout << "==========================================\n";

    cout.unsetf(ios_base::floatfield);
    cout << setprecision(6);
}