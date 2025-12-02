#ifndef INPUT_VALIDATION_H
#define INPUT_VALIDATION_H

#include <iostream>
#include <string>
#include <algorithm>
#include <limits>
#include <regex>
#include <climits>
#include <vector>

using namespace std;

// Функция для преобразования строки к нижнему регистру
string toLowercase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Функция для проверки валидности алфавитной строки
bool isValidAlphaString(const string& str) {
    if (str.empty()) {
        return false;
    }

    for (char c : str) {
        if (!((c >= 'а' && c <= 'я') || (c >= 'А' && c <= 'Я') ||
            (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            c == ' ' || c == '-' || c == '.' || c == ',' || c == '_')) {
            return false;
        }
    }
    return true;
}

// Функция для проверки валидности email
bool isValidEmail(const string& email) {
    if (email.empty()) return false;
    regex emailPattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return regex_match(email, emailPattern);
}

// Функция для проверки валидности телефона
bool isValidPhone(const string& phone) {
    if (phone.empty()) {
        return false;
    }

    for (char c : phone) {
        if (!isdigit(c) && c != ' ' && c != '(' && c != ')' && c != '+' && c != '-') {
            return false;
        }
    }
    return true;
}

// Функция для проверки валидности даты в формате DD.MM.YYYY
bool isValidDateString(const string& date) {
    if (date.empty()) {
        return false;
    }

    regex datePattern(R"(^(0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[0-2])\.\d{4}$)");
    if (!regex_match(date, datePattern)) return false;

    size_t dot1 = date.find('.');
    size_t dot2 = date.find('.', dot1 + 1);
    int day = stoi(date.substr(0, dot1));
    int month = stoi(date.substr(dot1 + 1, dot2 - dot1 - 1));
    int year = stoi(date.substr(dot2 + 1));

    bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (month == 2) return (isLeap) ? day <= 29 : day <= 28;
    if (month == 4 || month == 6 || month == 9 || month == 11) return day <= 30;
    return true;
}

// Улучшенный безопасный ввод целого числа
int safeInputInt(const string& prompt, int minVal = 1, int maxVal = 10000) {
    string input;
    int value;

    while (true) {
        cout << prompt;
        getline(cin, input);

        if (input.empty()) {
            cout << "Ошибка! Поле не может быть пустым. Пожалуйста, введите число: ";
            continue;
        }

        try {
            size_t pos;
            value = stoi(input, &pos);

            // Проверяем, что вся строка была обработана как число
            if (pos != input.length()) {
                cout << "Ошибка! Введите только число без лишних символов: ";
                continue;
            }

            if (value < minVal || value > maxVal) {
                cout << "Ошибка! Число должно быть в диапазоне от " << minVal << " до " << maxVal << "." << endl;
                cout << "Пожалуйста, введите корректное число: ";
                continue;
            }

            return value;
        }
        catch (const exception&) {
            cout << "Ошибка ввода! Пожалуйста, введите целое число: ";
        }
    }
}

// Улучшенный безопасный ввод числа с плавающей точкой
double safeInputDouble(const string& prompt, double minVal = 0.0, double maxVal = 1e9) {
    string input;
    double value;

    while (true) {
        cout << prompt;
        getline(cin, input);

        if (input.empty()) {
            cout << "Ошибка! Поле не может быть пустым. Пожалуйста, введите число: ";
            continue;
        }

        try {
            size_t pos;
            value = stod(input, &pos);

            // Проверяем, что вся строка была обработана как число
            if (pos != input.length()) {
                cout << "Ошибка! Введите только число без лишних символов: ";
                continue;
            }

            if (value < minVal || value > maxVal) {
                cout << "Ошибка! Число должно быть в диапазоне от " << minVal << " до " << maxVal << "." << endl;
                cout << "Пожалуйста, введите корректное число: ";
                continue;
            }

            return value;
        }
        catch (const exception&) {
            cout << "Ошибка ввода! Пожалуйста, введите число: ";
        }
    }
}

// Безопасный ввод строки (не пустой)
string safeInputString(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        if (value.empty()) {
            cout << "Ошибка! Поле не может быть пустым. Пожалуйста, заполните это поле: ";
            continue;
        }
        return value;
    }
}

// Безопасный ввод алфавитной строки
string safeInputAlphaString(const string& prompt) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        if (value.empty()) {
            cout << "Ошибка! Поле не может быть пустым. Пожалуйста, заполните это поле: ";
            continue;
        }
        if (isValidAlphaString(value)) {
            return value;
        }
        else {
            cout << "Ошибка! Поле может содержать только буквы, пробелы, дефисы и точки." << endl;
            cout << "Пожалуйста, введите корректное значение: ";
        }
    }
}

// Безопасный ввод email
string safeInputEmail(const string& prompt) {
    string email;
    while (true) {
        cout << prompt;
        getline(cin, email);
        if (email.empty()) {
            cout << "Ошибка! Поле email не может быть пустым. Пожалуйста, введите email: ";
            continue;
        }
        if (isValidEmail(email)) {
            return email;
        }
        else {
            cout << "Ошибка! Email должен быть в формате example@domain.com" << endl;
            cout << "Пожалуйста, введите корректный email: ";
        }
    }
}

// Безопасный ввод телефона
string safeInputPhone(const string& prompt) {
    string phone;
    while (true) {
        cout << prompt;
        getline(cin, phone);
        if (phone.empty()) {
            cout << "Ошибка! Поле не может быть пустым. Пожалуйста, введите номер телефона: ";
            continue;
        }
        if (isValidPhone(phone)) {
            return phone;
        }
        else {
            cout << "Ошибка! Номер телефона может содержать только цифры, пробелы, '+', '-', '(', ')'." << endl;
            cout << "Пожалуйста, введите корректный номер: ";
        }
    }
}

// Безопасный ввод даты в формате DD.MM.YYYY
string safeInputDateString(const string& prompt) {
    string date;
    while (true) {
        cout << prompt;
        getline(cin, date);
        if (date.empty()) {
            cout << "Ошибка! Поле не может быть пустым. Пожалуйста, введите дату: ";
            continue;
        }
        if (isValidDateString(date)) {
            return date;
        }
        else {
            cout << "Ошибка! Дата должна быть в формате DD.MM.YYYY и быть корректной." << endl;
            cout << "Пример: 15.12.2023" << endl;
            cout << "Пожалуйста, введите корректную дату: ";
        }
    }
}

// Безопасный ввод логина
string safeInputLogin(const string& prompt) {
    string login;
    while (true) {
        cout << prompt;
        getline(cin, login);
        if (login.empty()) {
            cout << "Ошибка! Логин не может быть пустым. Пожалуйста, введите логин: ";
            continue;
        }
        // Проверка на допустимые символы в логине
        bool valid = true;
        for (char c : login) {
            if (!isalnum(c) && c != '_' && c != '-') {
                valid = false;
                break;
            }
        }
        if (valid) {
            return login;
        }
        else {
            cout << "Ошибка! Логин может содержать только буквы, цифры, символы '_' и '-'." << endl;
            cout << "Пожалуйста, введите корректный логин: ";
        }
    }
}

// Функция для выбора типа работ из списка
string selectWorkType() {
    vector<string> workTypes = {
        "Строительство жилого дома",
        "Отделочные работы",
        "Ремонтные работы",
        "Проектирование",
        "Инженерные системы"
    };

    cout << "\nВыберите тип работ:" << endl;
    for (size_t i = 0; i < workTypes.size(); ++i) {
        cout << i + 1 << ". " << workTypes[i] << endl;
    }

    int choice = safeInputInt("Введите номер типа работ: ", 1, static_cast<int>(workTypes.size()));
    return workTypes[choice - 1];
}

// Функция для выбора типа объекта из списка
string selectObjectType() {
    vector<string> objectTypes = {
        "Многоквартирный дом",
        "Торговый центр",
        "Офисное здание",
        "Промышленный объект",
        "Частный дом"
    };

    cout << "\nВыберите тип объекта:" << endl;
    for (size_t i = 0; i < objectTypes.size(); ++i) {
        cout << i + 1 << ". " << objectTypes[i] << endl;
    }

    int choice = safeInputInt("Введите номер типа объекта: ", 1, static_cast<int>(objectTypes.size()));
    return objectTypes[choice - 1];
}

// Функция для выбора статуса из списка
string selectStatus() {
    vector<string> statuses = {
        "планируется",
        "в работе",
        "завершен",
        "приостановлен",
        "отменен"
    };

    cout << "\nВыберите статус:" << endl;
    for (size_t i = 0; i < statuses.size(); ++i) {
        cout << i + 1 << ". " << statuses[i] << endl;
    }

    int choice = safeInputInt("Введите номер статуса: ", 1, static_cast<int>(statuses.size()));
    return statuses[choice - 1];
}

// Функция для выбора статуса при поиске
string selectStatusForSearch() {
    vector<string> statuses = {
        "планируется",
        "в работе",
        "завершен",
        "приостановлен",
        "отменен"
    };

    cout << "\nВыберите статус для поиска:" << endl;
    for (size_t i = 0; i < statuses.size(); ++i) {
        cout << i + 1 << ". " << statuses[i] << endl;
    }

    int choice = safeInputInt("Введите номер статуса: ", 1, static_cast<int>(statuses.size()));
    return statuses[choice - 1];
}

// Функция для выбора типа работ при редактировании
string selectWorkTypeForEdit(const string& currentValue) {
    vector<string> workTypes = {
        "Строительство жилого дома",
        "Отделочные работы",
        "Ремонтные работы",
        "Проектирование",
        "Инженерные системы"
    };

    cout << "\nТекущий тип работ: " << currentValue << endl;
    cout << "Выберите новый тип работ (0 - оставить текущий):" << endl;
    for (size_t i = 0; i < workTypes.size(); ++i) {
        cout << i + 1 << ". " << workTypes[i] << endl;
    }

    int choice = safeInputInt("Введите номер типа работ: ", 0, static_cast<int>(workTypes.size()));
    if (choice == 0) {
        return currentValue;
    }
    return workTypes[choice - 1];
}

// Функция для выбора статуса при редактировании
string selectStatusForEdit(const string& currentValue) {
    vector<string> statuses = {
        "планируется",
        "в работе",
        "завершен",
        "приостановлен",
        "отменен"
    };

    cout << "\nТекущий статус: " << currentValue << endl;
    cout << "Выберите новый статус (0 - оставить текущий):" << endl;
    for (size_t i = 0; i < statuses.size(); ++i) {
        cout << i + 1 << ". " << statuses[i] << endl;
    }

    int choice = safeInputInt("Введите номер статуса: ", 0, static_cast<int>(statuses.size()));
    if (choice == 0) {
        return currentValue;
    }
    return statuses[choice - 1];
}

// Функция для выбора типа объекта при редактировании
string selectObjectTypeForEdit(const string& currentValue) {
    vector<string> objectTypes = {
        "Многоквартирный дом",
        "Торговый центр",
        "Офисное здание",
        "Промышленный объект",
        "Частный дом"
    };

    cout << "\nТекущий тип объекта: " << currentValue << endl;
    cout << "Выберите новый тип объекта (0 - оставить текущий):" << endl;
    for (size_t i = 0; i < objectTypes.size(); ++i) {
        cout << i + 1 << ". " << objectTypes[i] << endl;
    }

    int choice = safeInputInt("Введите номер типа объекта: ", 0, static_cast<int>(objectTypes.size()));
    if (choice == 0) {
        return currentValue;
    }
    return objectTypes[choice - 1];
}

#endif // INPUT_VALIDATION_H