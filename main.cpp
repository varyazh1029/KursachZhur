#include <Windows.h>
#include <iostream>
#include <memory>
#include <limits> 
#include <string>
#include <iomanip>
#include "contracts.h"
#include "input_validation.h"

using namespace std;

// Глобальные репозитории
Repository<User> userRepo("users.dat");
Repository<Client> clientRepo("clients.dat");
Repository<ConstructionObject> objectRepo("objects.dat");
Repository<Contract> contractRepo("contracts.dat");

void initData();
void menu();
void signIn();
void userMenu(shared_ptr<User> user);
void adminMenu(shared_ptr<User> user);
void handleDataMenu();
void printDataMenu();
void addDataMenu();
void deleteDataMenu();
void changeDataMenu();
void editClient();
void editObject();
void editContract();
void searchDataMenu();
void sortDataMenu();
void generateReport();
void handleAccountsMenu();
void showMostProfitableContract();

// Функция для получения следующего доступного ID
template<typename T>
int getNextId(const Repository<T>& repo) {
    int maxId = 0;
    auto allItems = repo.findAll();
    for (const auto& item : allItems) {
        if (item->getId() > maxId) {
            maxId = item->getId();
        }
    }
    return maxId + 1;
}

// Функция для показа самого прибыльного контракта
void showMostProfitableContract() {
    auto contracts = contractRepo.findAll();
    if (contracts.empty()) {
        cout << "Нет контрактов для отображения." << endl;
        return;
    }

    shared_ptr<Contract> mostProfitable = contracts[0];
    for (const auto& contract : contracts) {
        if (contract->getAmount() > mostProfitable->getAmount()) {
            mostProfitable = contract;
        }
    }

    cout << "\n====== САМЫЙ ПРИБЫЛЬНЫЙ КОНТРАКТ ======" << endl;
    mostProfitable->display();
    cout << "=======================================" << endl;
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    try {
        initData();
        menu();
    }
    catch (const exception& e) {
        cerr << "Critical error: " << e.what() << endl;
        return 1;
    }

    return 0;
}

void initData() {
    userRepo.loadFromFile();
    clientRepo.loadFromFile();
    objectRepo.loadFromFile();
    contractRepo.loadFromFile();

    if (userRepo.size() == 0) {
        userRepo.add(make_shared<User>(1, "admin", "admin123", true));
        userRepo.add(make_shared<User>(2, "user", "user123", false));
        userRepo.saveToFile();
    }

    if (clientRepo.size() == 0) {
        clientRepo.add(make_shared<Client>(1, "СтройГарант", "Иванов Петр", "+375291234567", "ivanov@stroygarant.by", "Минск, ул. Строителей 15"));
        clientRepo.add(make_shared<Client>(2, "МонтажСервис", "Сидорова Анна", "+375297654321", "sidorova@montag.by", "Гродно, пр-т Космонавтов 25"));
        clientRepo.saveToFile();
    }

    if (objectRepo.size() == 0) {
        objectRepo.add(make_shared<ConstructionObject>(1, "Жилой комплекс Северный", "Минск, Северная 45", "Многоквартирный дом", 2500.5));
        objectRepo.add(make_shared<ConstructionObject>(2, "Торговый центр Восток", "Гродно, Восток 33", "Торговый центр", 1800.0));
        objectRepo.saveToFile();
    }

    if (contractRepo.size() == 0) {
        contractRepo.add(make_shared<Contract>(1, 1, 1, Date(15, 3, 2024), 180, 250000.0, "Строительство жилого дома", "в работе", "Смирнов А.В."));
        contractRepo.add(make_shared<Contract>(2, 2, 2, Date(1, 4, 2024), 120, 180000.0, "Отделочные работы", "планируется", "Ковалева И.С."));
        contractRepo.saveToFile();
    }
}

void menu() {
    int choice;

    do {
        cout << "\n__________СИСТЕМА УЧЕТА КОНТРАКТОВ__________" << endl;
        cout << "1. Войти в систему" << endl;
        cout << "2. Зарегистрироваться" << endl;
        cout << "0. Выход" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 2);

        switch (choice) {
        case 1:
            signIn();
            break;
        case 2: {
            string login = safeInputLogin("Логин: ");
            string password = safeInputString("Пароль: ");
            int newId = getNextId(userRepo);
            userRepo.add(make_shared<User>(newId, login, password, false));
            userRepo.saveToFile();
            cout << "Регистрация успешна!" << endl;
            break;
        }
        case 0:
            cout << "Выход из программы..." << endl;
            return;
        }
    } while (choice != 0);
}

void signIn() {
    string login = safeInputLogin("Логин: ");
    string password = safeInputString("Пароль: ");

    auto users = userRepo.findAll();
    for (const auto& user : users) {
        if (user->getLogin() == login && user->checkPassword(password)) {
            cout << "Добро пожаловать, " << login << "!" << endl;
            if (user->getIsAdmin()) {
                adminMenu(user);
            }
            else {
                userMenu(user);
            }
            return;
        }
    }
    cout << "Неверный логин или пароль!" << endl;
}

void userMenu(shared_ptr<User> user) {
    int choice;
    do {
        cout << "\n__________МЕНЮ ПОЛЬЗОВАТЕЛЯ__________" << endl;
        cout << "1. Просмотр контрактов" << endl;
        cout << "2. Поиск контрактов" << endl;
        cout << "3. Сортировка контрактов" << endl;
        cout << "4. Самый прибыльный контракт" << endl;
        cout << "0. Выход" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 4);

        auto contracts = contractRepo.findAll();

        switch (choice) {
        case 1:
            cout << "\nВсе контракты:\n";
            for (const auto& contract : contracts) {
                contract->display();
            }
            break;
        case 2: {
            string status = selectStatusForSearch();
            auto results = contractRepo.search([status](const shared_ptr<Contract>& c) {
                return c->getStatus() == status;
                });
            cout << "Найдено контрактов: " << results.size() << endl;
            for (const auto& contract : results) {
                contract->display();
            }
            break;
        }
        case 3: {
            auto sorted = contractRepo.sort([](const shared_ptr<Contract>& a, const shared_ptr<Contract>& b) {
                return a->getAmount() < b->getAmount();
                });
            cout << "Контракты отсортированы по сумме (по возрастанию):\n";
            for (const auto& contract : sorted) {
                contract->display();
            }
            break;
        }
        case 4:
            showMostProfitableContract();
            break;
        case 0: return;
        }
    } while (choice != 0);
}

void adminMenu(shared_ptr<User> user) {
    int choice;
    do {
        cout << "\n__________МЕНЮ АДМИНИСТРАТОРА__________" << endl;
        cout << "1. Управление данными" << endl;
        cout << "2. Управление учетными записями" << endl;
        cout << "3. Генерация отчета" << endl;
        cout << "4. Самый прибыльный контракт" << endl;
        cout << "0. Выход" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 4);

        switch (choice) {
        case 1: handleDataMenu(); break;
        case 2: handleAccountsMenu(); break;
        case 3: generateReport(); break;
        case 4: showMostProfitableContract(); break;
        case 0: return;
        }
    } while (choice != 0);
}

void handleDataMenu() {
    int choice;
    do {
        cout << "\n__________УПРАВЛЕНИЕ ДАННЫМИ__________" << endl;
        cout << "1. Просмотр данных" << endl;
        cout << "2. Добавление данных" << endl;
        cout << "3. Удаление данных" << endl;
        cout << "4. Редактирование данных" << endl;
        cout << "5. Поиск данных" << endl;
        cout << "6. Сортировка данных" << endl;
        cout << "0. Назад" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 6);

        switch (choice) {
        case 1: printDataMenu(); break;
        case 2: addDataMenu(); break;
        case 3: deleteDataMenu(); break;
        case 4: changeDataMenu(); break;
        case 5: searchDataMenu(); break;
        case 6: sortDataMenu(); break;
        case 0: return;
        }
    } while (choice != 0);
}

void printDataMenu() {
    int choice;
    do {
        cout << "\n__________ПРОСМОТР ДАННЫХ__________" << endl;
        cout << "1. Клиенты" << endl;
        cout << "2. Объекты" << endl;
        cout << "3. Контракты" << endl;
        cout << "0. Назад" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 3);

        switch (choice) {
        case 1:
            cout << "\nКлиенты:\n";
            for (const auto& client : clientRepo.findAll()) {
                client->display();
            }
            break;
        case 2:
            cout << "\nОбъекты:\n";
            for (const auto& object : objectRepo.findAll()) {
                object->display();
            }
            break;
        case 3:
            cout << "\nКонтракты:\n";
            for (const auto& contract : contractRepo.findAll()) {
                contract->display();
            }
            break;
        case 0: return;
        }
    } while (choice != 0);
}

void addDataMenu() {
    int choice;
    do {
        cout << "\n__________ДОБАВЛЕНИЕ ДАННЫХ__________" << endl;
        cout << "1. Добавить клиента" << endl;
        cout << "2. Добавить объект" << endl;
        cout << "3. Добавить контракт" << endl;
        cout << "0. Назад" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 3);

        switch (choice) {
        case 1: {
            int id = getNextId(clientRepo);
            cout << "Автоматически сгенерированный ID: " << id << endl;
            string company = safeInputAlphaString("Компания: ");
            string contact = safeInputAlphaString("Контактное лицо: ");
            string phone = safeInputPhone("Телефон: ");
            string email = safeInputEmail("Email: ");
            string address = safeInputString("Адрес: ");

            clientRepo.add(make_shared<Client>(id, company, contact, phone, email, address));
            clientRepo.saveToFile();
            cout << "Клиент добавлен!" << endl;
            break;
        }
        case 2: {
            int id = getNextId(objectRepo);
            cout << "Автоматически сгенерированный ID: " << id << endl;
            string name = safeInputAlphaString("Название: ");
            string address = safeInputString("Адрес: ");

            // Выбор типа объекта из списка
            string type = selectObjectType();

            double area = safeInputDouble("Площадь: ", 0.1, 100000.0);

            objectRepo.add(make_shared<ConstructionObject>(id, name, address, type, area));
            objectRepo.saveToFile();
            cout << "Объект добавлен!" << endl;
            break;
        }
        case 3: {
            int id = getNextId(contractRepo);
            cout << "Автоматически сгенерированный номер контракта: " << id << endl;

            // Показываем существующих клиентов
            cout << "\nСуществующие клиенты:" << endl;
            for (const auto& client : clientRepo.findAll()) {
                cout << "ID: " << client->getId() << ", Компания: " << client->getCompanyName() << endl;
            }
            int clientId = safeInputInt("ID клиента: ", 1, 10000);

            // Показываем существующие объекты
            cout << "\nСуществующие объекты:" << endl;
            for (const auto& object : objectRepo.findAll()) {
                cout << "ID: " << object->getId() << ", Название: " << object->getName() << endl;
            }
            int objectId = safeInputInt("ID объекта: ", 1, 10000);

            // Ввод даты через строку с проверкой формата
            string dateStr = safeInputDateString("Дата начала (в формате ДД.ММ.ГГГГ): ");
            size_t dot1 = dateStr.find('.');
            size_t dot2 = dateStr.find('.', dot1 + 1);
            int day = stoi(dateStr.substr(0, dot1));
            int month = stoi(dateStr.substr(dot1 + 1, dot2 - dot1 - 1));
            int year = stoi(dateStr.substr(dot2 + 1));
            Date date(day, month, year);

            int duration = safeInputInt("Срок (дни): ", 1, 3650);
            double amount = safeInputDouble("Сумма: ", 0.0, 1e9);

            // Выбор типа работ из списка
            string workType = selectWorkType();

            // Выбор статуса из списка
            string status = selectStatus();

            string manager = safeInputAlphaString("Менеджер: ");

            contractRepo.add(make_shared<Contract>(id, clientId, objectId, date, duration, amount, workType, status, manager));
            contractRepo.saveToFile();
            cout << "Контракт добавлен!" << endl;
            break;
        }
        case 0: return;
        }
    } while (choice != 0);
}

void deleteDataMenu() {
    int choice;
    do {
        cout << "\n__________УДАЛЕНИЕ ДАННЫХ__________" << endl;
        cout << "1. Удалить клиента" << endl;
        cout << "2. Удалить объект" << endl;
        cout << "3. Удалить контракт" << endl;
        cout << "0. Назад" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 3);

        int id;
        switch (choice) {
        case 1:
            id = safeInputInt("ID клиента: ", 1, 10000);
            if (clientRepo.remove(id)) {
                clientRepo.saveToFile();
                cout << "Клиент удален!" << endl;
            }
            else {
                cout << "Клиент не найден!" << endl;
            }
            break;
        case 2:
            id = safeInputInt("ID объекта: ", 1, 10000);
            if (objectRepo.remove(id)) {
                objectRepo.saveToFile();
                cout << "Объект удален!" << endl;
            }
            else {
                cout << "Объект не найден!" << endl;
            }
            break;
        case 3:
            id = safeInputInt("Номер контракта: ", 1, 10000);
            if (contractRepo.remove(id)) {
                contractRepo.saveToFile();
                cout << "Контракт удален!" << endl;
            }
            else {
                cout << "Контракт не найден!" << endl;
            }
            break;
        case 0: return;
        }
    } while (choice != 0);
}

void changeDataMenu() {
    int choice;
    do {
        cout << "\n__________РЕДАКТИРОВАНИЕ ДАННЫХ__________" << endl;
        cout << "1. Редактировать клиента" << endl;
        cout << "2. Редактировать объект" << endl;
        cout << "3. Редактировать контракт" << endl;
        cout << "0. Назад" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 3);

        switch (choice) {
        case 1: editClient(); break;
        case 2: editObject(); break;
        case 3: editContract(); break;
        case 0: return;
        }
    } while (choice != 0);
}

void editClient() {
    int id = safeInputInt("Введите ID клиента для редактирования: ", 1, 10000);

    auto client = clientRepo.find(id);
    if (!client) {
        cout << "Клиент не найден." << endl;
        return;
    }

    client->display();

    string newName, newContact, newPhone, newEmail, newAddress;

    cout << "Введите новое название компании (текущее: " << client->getCompanyName() << ", Enter для пропуска): ";
    getline(cin, newName);
    if (!newName.empty() && isValidAlphaString(newName)) client->setCompanyName(newName);
    else if (!newName.empty()) cout << "Некорректное название компании!" << endl;

    cout << "Введите новое контактное лицо (текущее: " << client->getContactPerson() << ", Enter для пропуска): ";
    getline(cin, newContact);
    if (!newContact.empty() && isValidAlphaString(newContact)) client->setContactPerson(newContact);
    else if (!newContact.empty()) cout << "Некорректное контактное лицо!" << endl;

    cout << "Введите новый телефон (текущий: " << client->getPhone() << ", Enter для пропуска): ";
    getline(cin, newPhone);
    if (!newPhone.empty() && isValidPhone(newPhone)) client->setPhone(newPhone);
    else if (!newPhone.empty()) cout << "Некорректный телефон!" << endl;

    cout << "Введите новый email (текущий: " << client->getEmail() << ", Enter для пропуска): ";
    getline(cin, newEmail);
    if (!newEmail.empty() && isValidEmail(newEmail)) client->setEmail(newEmail);
    else if (!newEmail.empty()) cout << "Некорректный email!" << endl;

    cout << "Введите новый адрес (текущий: " << client->getAddress() << ", Enter для пропуска): ";
    getline(cin, newAddress);
    if (!newAddress.empty()) client->setAddress(newAddress);

    clientRepo.saveToFile();
    cout << "Клиент успешно отредактирован!" << endl;
}

void editObject() {
    int id = safeInputInt("Введите ID объекта для редактирования: ", 1, 10000);

    auto object = objectRepo.find(id);
    if (!object) {
        cout << "Объект не найден." << endl;
        return;
    }

    object->display();

    string newName, newAddress, newType;
    double newArea;

    cout << "Введите новое название (текущее: " << object->getName() << ", Enter для пропуска): ";
    getline(cin, newName);
    if (!newName.empty() && isValidAlphaString(newName)) object->setName(newName);
    else if (!newName.empty()) cout << "Некорректное название!" << endl;

    cout << "Введите новый адрес (текущий: " << object->getAddress() << ", Enter для пропуска): ";
    getline(cin, newAddress);
    if (!newAddress.empty()) object->setAddress(newAddress);

    // Выбор типа объекта из списка при редактировании
    cout << "Изменить тип объекта? (1 - Да, 0 - Нет): ";
    int changeType = safeInputInt("", 0, 1);
    if (changeType == 1) {
        newType = selectObjectTypeForEdit(object->getType());
        object->setType(newType);
    }

    cout << "Введите новую площадь (текущая: " << object->getArea() << ", 0 - пропустить): ";
    string areaInput;
    getline(cin, areaInput);
    if (!areaInput.empty()) {
        try {
            size_t pos;
            newArea = stod(areaInput, &pos);
            // Проверяем, что вся строка была обработана как число
            if (pos == areaInput.length() && newArea > 0) {
                object->setArea(newArea);
            }
            else {
                cout << "Некорректная площадь!" << endl;
            }
        }
        catch (...) {
            cout << "Некорректная площадь!" << endl;
        }
    }

    objectRepo.saveToFile();
    cout << "Объект успешно отредактирован!" << endl;
}

void editContract() {
    int id = safeInputInt("Введите номер контракта для редактирования: ", 1, 10000);

    auto contract = contractRepo.find(id);
    if (!contract) {
        cout << "Контракт не найден." << endl;
        return;
    }

    contract->display();

    string newWorkType, newStatus, newManager, dateInput;
    Date newDate;
    int newDuration;
    double newAmount;

    // Выбор типа работ из списка при редактировании
    cout << "Изменить тип работ? (1 - Да, 0 - Нет): ";
    int changeWorkType = safeInputInt("", 0, 1);
    if (changeWorkType == 1) {
        newWorkType = selectWorkTypeForEdit(contract->getWorkType());
        contract->setWorkType(newWorkType);
    }

    // Выбор статуса из списка при редактировании
    cout << "Изменить статус? (1 - Да, 0 - Нет): ";
    int changeStatus = safeInputInt("", 0, 1);
    if (changeStatus == 1) {
        newStatus = selectStatusForEdit(contract->getStatus());
        contract->setStatus(newStatus);
    }

    cout << "Введите нового менеджера (текущий: " << contract->getManager() << ", Enter для пропуска): ";
    getline(cin, newManager);
    if (!newManager.empty() && isValidAlphaString(newManager)) contract->setManager(newManager);
    else if (!newManager.empty()) cout << "Некорректное имя менеджера!" << endl;

    cout << "Введите новую дату начала (в формате ДД.ММ.ГГГГ, Enter для пропуска): ";
    getline(cin, dateInput);
    if (!dateInput.empty() && isValidDateString(dateInput)) {
        size_t dot1 = dateInput.find('.');
        size_t dot2 = dateInput.find('.', dot1 + 1);
        int day = stoi(dateInput.substr(0, dot1));
        int month = stoi(dateInput.substr(dot1 + 1, dot2 - dot1 - 1));
        int year = stoi(dateInput.substr(dot2 + 1));
        contract->setStartDate(Date(day, month, year));
    }
    else if (!dateInput.empty()) {
        cout << "Некорректная дата!" << endl;
    }

    cout << "Введите новый срок (дни, 0 - пропустить): ";
    string durationInput;
    getline(cin, durationInput);
    if (!durationInput.empty()) {
        try {
            size_t pos;
            newDuration = stoi(durationInput, &pos);
            // Проверяем, что вся строка была обработана как число
            if (pos == durationInput.length() && newDuration > 0) {
                contract->setDuration(newDuration);
            }
            else {
                cout << "Некорректный срок!" << endl;
            }
        }
        catch (...) {
            cout << "Некорректный срок!" << endl;
        }
    }

    cout << "Введите новую сумму (0 - пропустить): ";
    string amountInput;
    getline(cin, amountInput);
    if (!amountInput.empty()) {
        try {
            size_t pos;
            newAmount = stod(amountInput, &pos);
            // Проверяем, что вся строка была обработана как число
            if (pos == amountInput.length() && newAmount > 0) {
                contract->setAmount(newAmount);
            }
            else {
                cout << "Некорректная сумма!" << endl;
            }
        }
        catch (...) {
            cout << "Некорректная сумма!" << endl;
        }
    }

    contractRepo.saveToFile();
    cout << "Контракт успешно отредактирован!" << endl;
}

void searchDataMenu() {
    int choice;
    do {
        cout << "\n__________ПОИСК ДАННЫХ__________" << endl;
        cout << "1. Поиск контрактов по сумме (больше или равно)" << endl;
        cout << "2. Поиск клиентов по названию компании" << endl;
        cout << "3. Поиск объектов по типу" << endl;
        cout << "4. Поиск контрактов по менеджеру" << endl;
        cout << "0. Назад" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 4);

        switch (choice) {
        case 1: {
            double minAmount = safeInputDouble("Введите минимальную сумму: ", 0, 1e9);
            auto results = contractRepo.search([minAmount](const shared_ptr<Contract>& c) {
                return c->getAmount() >= minAmount;
                });
            cout << "\nНайдено контрактов с суммой >= " << fixed << setprecision(0) << minAmount << ": " << results.size() << endl;
            cout.unsetf(ios_base::floatfield);
            cout << setprecision(6);
            for (const auto& contract : results) {
                contract->display();
            }
            break;
        }
        case 2: {
            string companyName = safeInputAlphaString("Введите название компании для поиска: ");
            auto results = clientRepo.search([companyName](const shared_ptr<Client>& c) {
                return c->getCompanyName().find(companyName) != string::npos;
                });
            cout << "\nНайдено клиентов с названием компании содержащим '" << companyName << "': " << results.size() << endl;
            for (const auto& client : results) {
                client->display();
            }
            break;
        }
        case 3: {
            string objectType = safeInputAlphaString("Введите тип объекта для поиска: ");
            auto results = objectRepo.search([objectType](const shared_ptr<ConstructionObject>& o) {
                return o->getType().find(objectType) != string::npos;
                });
            cout << "\nНайдено объектов с типом содержащим '" << objectType << "': " << results.size() << endl;
            for (const auto& object : results) {
                object->display();
            }
            break;
        }
        case 4: {
            string managerName = safeInputAlphaString("Введите имя менеджера для поиска: ");
            auto results = contractRepo.search([managerName](const shared_ptr<Contract>& c) {
                return c->getManager().find(managerName) != string::npos;
                });
            cout << "\nНайдено контрактов с менеджером содержащим '" << managerName << "': " << results.size() << endl;
            for (const auto& contract : results) {
                contract->display();
            }
            break;
        }
        case 0: return;
        }
    } while (choice != 0);
}

void sortDataMenu() {
    int choice;
    do {
        cout << "\n__________СОРТИРОВКА ДАННЫХ__________" << endl;
        cout << "1. Сортировать контракты по дате начала (по возрастанию)" << endl;
        cout << "2. Сортировать контракты по сумме (по убыванию)" << endl;
        cout << "3. Сортировать клиентов по названию компании" << endl;
        cout << "4. Сортировать объекты по площади (по возрастанию)" << endl;
        cout << "5. Сортировать контракты по сроку (по убыванию)" << endl;
        cout << "0. Назад" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 5);

        switch (choice) {
        case 1: {
            auto sorted = contractRepo.sort([](const shared_ptr<Contract>& a, const shared_ptr<Contract>& b) {
                return a->getStartDate() < b->getStartDate();
                });
            cout << "\nКонтракты отсортированы по дате начала (по возрастанию):\n";
            for (const auto& contract : sorted) {
                contract->display();
            }
            break;
        }
        case 2: {
            auto sorted = contractRepo.sort([](const shared_ptr<Contract>& a, const shared_ptr<Contract>& b) {
                return a->getAmount() > b->getAmount();
                });
            cout << "\nКонтракты отсортированы по сумме (по убыванию):\n";
            for (const auto& contract : sorted) {
                contract->display();
            }
            break;
        }
        case 3: {
            auto sorted = clientRepo.sort([](const shared_ptr<Client>& a, const shared_ptr<Client>& b) {
                return a->getCompanyName() < b->getCompanyName();
                });
            cout << "\nКлиенты отсортированы по названию компании (по алфавиту):\n";
            for (const auto& client : sorted) {
                client->display();
            }
            break;
        }
        case 4: {
            auto sorted = objectRepo.sort([](const shared_ptr<ConstructionObject>& a, const shared_ptr<ConstructionObject>& b) {
                return a->getArea() < b->getArea();
                });
            cout << "\nОбъекты отсортированы по площади (по возрастанию):\n";
            for (const auto& object : sorted) {
                object->display();
            }
            break;
        }
        case 5: {
            auto sorted = contractRepo.sort([](const shared_ptr<Contract>& a, const shared_ptr<Contract>& b) {
                return a->getDuration() > b->getDuration();
                });
            cout << "\nКонтракты отсортированы по сроку (по убыванию):\n";
            for (const auto& contract : sorted) {
                contract->display();
            }
            break;
        }
        case 0: return;
        }
    } while (choice != 0);
}

void generateReport() {
    try {
        ReportGenerator::generateContractsReport(contractRepo, clientRepo, objectRepo);
    }
    catch (const exception& e) {
        cerr << "Ошибка генерации отчета: " << e.what() << endl;
    }
}

void handleAccountsMenu() {
    int choice;
    do {
        cout << "\n__________УПРАВЛЕНИЕ УЧЕТНЫМИ ЗАПИСЯМИ__________" << endl;
        cout << "1. Просмотр пользователей" << endl;
        cout << "2. Удаление пользователя" << endl;
        cout << "0. Назад" << endl;

        choice = safeInputInt("Выберите действие: ", 0, 2);

        switch (choice) {
        case 1:
            cout << "\nПользователи:\n";
            for (const auto& user : userRepo.findAll()) {
                user->display();
            }
            break;
        case 2: {
            int id = safeInputInt("ID пользователя: ", 1, 10000);
            if (userRepo.remove(id)) {
                userRepo.saveToFile();
                cout << "Пользователь удален!" << endl;
            }
            else {
                cout << "Пользователь не найден!" << endl;
            }
            break;
        }
        case 0: return;
        }
    } while (choice != 0);
}