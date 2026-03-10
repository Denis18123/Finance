#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cstdio>   /
#include <clocale>  

namespace DateUtils
{
    struct SimpleDate {
        int year;
        int month;
        int day;
    };

    SimpleDate parseDate(const std::string& s) {
        SimpleDate d{};
        std::sscanf(s.c_str(), "%d-%d-%d", &d.year, &d.month, &d.day);
        return d;
    }

    SimpleDate getCurrentSimpleDate() {
        std::time_t now = std::time(nullptr);
        std::tm* ltm = std::localtime(&now);
        SimpleDate d;
        d.year = 1900 + ltm->tm_year;
        d.month = 1 + ltm->tm_mon;
        d.day = ltm->tm_mday;
        return d;
    }

    std::string getCurrentDate()
    {
        SimpleDate d = getCurrentSimpleDate();
        std::stringstream ss;
        ss << d.year << "-"
            << std::setfill('0') << std::setw(2) << d.month << "-"
            << std::setfill('0') << std::setw(2) << d.day;
        return ss.str();
    }

    bool isCurrentDay(const std::string& date_str)
    {
        return date_str == getCurrentDate();
    }

    bool isCurrentMonth(const std::string& date_str)
    {
        SimpleDate d = parseDate(date_str);
        SimpleDate cur = getCurrentSimpleDate();
        return d.year == cur.year && d.month == cur.month;
    }

    int weekOfYear(const SimpleDate& d) {
        std::tm time_in = {};
        time_in.tm_year = d.year - 1900;
        time_in.tm_mon = d.month - 1;
        time_in.tm_mday = d.day;
        std::mktime(&time_in);

        int day_of_year = time_in.tm_yday; 
        int wday = time_in.tm_wday;        
        if (wday == 0) wday = 7;           

        int week = (day_of_year + 7 - wday) / 7 + 1;
        return week;
    }

    bool isCurrentWeek(const std::string& date_str)
    {
        SimpleDate d = parseDate(date_str);
        SimpleDate cur = getCurrentSimpleDate();
        if (d.year != cur.year) return false;
        return weekOfYear(d) == weekOfYear(cur);
    }
}

// категории

class Category
{
public:
    std::string name;

    Category(const std::string& n) : name(n) {}

    void display() const {
        std::cout << "Категория: " << name << std::endl;
    }
};

// кошельки

class Wallet
{
protected:
    std::string name;
    double balance;

public:
    Wallet(const std::string& n, double initial_balance = 0.0)
        : name(n), balance(initial_balance)
    {
    }

    virtual ~Wallet() = default;

    virtual void deposit(double amount)
    {
        balance += amount;
        std::cout << "Пополнение кошелька '" << name << "' на " << amount
            << ". Новый баланс: " << balance << std::endl;
    }

    virtual bool withdraw(double amount)
    {
        if (balance >= amount)
        {
            balance -= amount;
            std::cout << "Списание с кошелька '" << name << "' на " << amount
                << ". Новый баланс: " << balance << std::endl;
            return true;
        }
        std::cout << "Недостаточно денег на кошельке '" << name << "'." << std::endl;
        return false;
    }

    double getBalance() const { return balance; }
    std::string getName() const { return name; }

    virtual void display() const {
        std::cout << "Кошелек: " << name << ", Баланс: " << balance << std::endl;
    }
};

class DebitCard : public Wallet
{
public:
    DebitCard(const std::string& n, double initial_balance = 0.0)
        : Wallet(n, initial_balance)
    {
    }

    void display() const override
    {
        std::cout << "Дебетовая карта: " << name << ", Баланс: " << balance << std::endl;
    }
};

class CreditCard : public Wallet
{
private:
    double creditLimit;

public:
    CreditCard(const std::string& n, double limit)
        : Wallet(n, 0.0), creditLimit(limit)
    {
    }

    bool withdraw(double amount) override
    {
        if ((balance - amount) >= -creditLimit) { // можно уйти в минус до лимита
            balance -= amount;
            std::cout << "Списание с кредитной карты '" << name << "' на " << amount
                << ". Текущий долг: " << -balance << std::endl;
            return true;
        }
        std::cout << "Превышен кредитный лимит на карте '" << name << "'." << std::endl;
        return false;
    }

    void display() const override {
        std::cout << "Кредитная карта: " << name
            << ", Лимит: " << creditLimit
            << ", Текущий долг: " << -balance << std::endl;
    }
};

// 

class Expense {
public:
    double amount;
    Category category;
    std::string description;
    std::string date; // yyyy-mm-dd

    Expense(double amt, const Category& cat, const std::string& desc, const std::string& d)
        : amount(amt), category(cat), description(desc), date(d)
    {
    }

    void display() const {
        std::cout << "Дата: " << date
            << ", Сумма: " << amount
            << ", Категория: " << category.name
            << ", Описание: " << description << std::endl;
    }
};

// ---------- FinancialManager ----------

class FinancialManager
{
public:
    std::vector<Wallet*> wallet;
    std::vector<Expense> expenses;
    std::vector<Category> categories;

    void addWallet(Wallet* w)
    {
        wallet.push_back(w);
    }

    void addCategory(const Category& c)
    {
        categories.push_back(c);
    }

    Category* findCategory(const std::string& name)
    {
        for (auto& cat : categories)
        {
            if (cat.name == name)
            {
                return &cat;
            }
        }
        return nullptr;
    }

    void addExpense(double amount,
        const std::string& category_name,
        const std::string& description,
        const std::string& wallet_name)
    {
        Wallet* target_wallet = nullptr;
        for (auto& w : wallet)
        {
            if (w->getName() == wallet_name)
            {
                target_wallet = w;
                break;
            }
        }

        if (!target_wallet)
        {
            std::cout << "Кошелек '" << wallet_name << "' не найден." << std::endl;
            return;
        }

        if (amount <= 0) {
            std::cout << "Сумма должна быть положительной." << std::endl;
            return;
        }

        if (target_wallet->withdraw(amount))
        {
            Category* cat = findCategory(category_name);
            if (!cat)
            {
                std::cout << "Категория '" << category_name << "' не найдена. Создаю новую." << std::endl;
                addCategory(Category(category_name));
                cat = findCategory(category_name);
            }
            expenses.emplace_back(amount, *cat, description, DateUtils::getCurrentDate());
            std::cout << "Затрата добавлена." << std::endl;
        }
    }

    void generateReport(const std::string& period_type) const
    {
        std::cout << "\n--- Отчет по затратам за " << period_type << " ---" << std::endl;
        double total_expenses = 0.0;
        std::map<std::string, double> category_summary;

        for (const auto& exp : expenses)
        {
            bool include_in_report = false;
            if (period_type == "день" && DateUtils::isCurrentDay(exp.date))
            {
                include_in_report = true;
            }
            else if (period_type == "неделя" && DateUtils::isCurrentWeek(exp.date))
            {
                include_in_report = true;
            }
            else if (period_type == "месяц" && DateUtils::isCurrentMonth(exp.date))
            {
                include_in_report = true;
            }

        }

        std::cout << "\nОбщая сумма затрат за " << period_type << ": " << total_expenses << std::endl;
        std::cout << "По категориям:\n";
        for (const auto& pair : category_summary)
        {
            std::cout << "-" << pair.first << ":" << pair.second << std::endl;
        }
    }

    void generateTopExpenses(const std::string& periodtype) const
    {
        std::cout << "\n--- ТОП-3 затрат за " << periodtype << " ---" << std::endl;
        std::vector<Expense> relevantexpenses;

        for (const auto& exp : expenses)
        {
            bool includeinreport = false;
            if (periodtype == "неделя" && DateUtils::isCurrentWeek(exp.date))
            {
                includeinreport = true;
            }
            else if (periodtype == "месяц" && DateUtils::isCurrentMonth(exp.date))
            {
                includeinreport = true;
            }

            if (includeinreport)
            {
                relevantexpenses.push_back(exp);
            }
        }

        std::sort(
            relevantexpenses.begin(),
            relevantexpenses.end(),
            [] (const Expense& a, const Expense& b)
        {
            return a.amount > b.amount;
        }
        );

        for (int i = 0; i < std::min<int>(relevantexpenses.size(), 3); ++i)
        {
            std::cout << (i + 1) << ". ";
            relevantexpenses.size();
        }
    }

    void generateTopCategories(const std::string& periodtype) const
    {
        std::cout << "\n--- ТОП-3 категорий по затратам за " << periodtype << " ---" << std::endl;
        std::map<std::string, double> categorytotals;

        for (const auto& exp : expenses)
        {
            bool includeinreport = false;
            if (periodtype == "неделя" && DateUtils::isCurrentWeek(exp.date))
            {
                includeinreport = true;
            }
            else if (periodtype == "месяц" && DateUtils::isCurrentMonth(exp.date))
            {
                includeinreport = true;
            }

            if (includeinreport)
            {
                categorytotals;
            }
        }

        std::vector<std::pair<std::string, double>> sortedcategories(
            categorytotals.begin(), categorytotals.end()
        );

        std::sort(
            sortedcategories.begin(),
            sortedcategories.end(),
            [](const std::pair<std::string, double>& a,
                const std::pair<std::string, double>& b)
            {
                return a.second > b.second;
            }
        );

        for (int i = 0; i < std::min<int>(sortedcategories.size(), 3); ++i)
        {
            std::cout << (i + 1) << ". Категория: " << sortedcategories[i].first
                << ", Общая сумма: " << sortedcategories[i].second << std::endl;
        }
    }

    void saveReportsToFile(const std::string& filename) const
    {
        std::ofstream outfile(filename);
        if (!outfile.is_open())
        {
            std::cerr << "Ошибка: Не удалось открыть файл " << filename << std::endl;
            return;
        }

        outfile << "--- Отчеты и Рейтинги Финансовой Системы ---\n\n";

        auto saveReportForPeriod = [&](const std::string& periodtype) {
            double totalexpenses = 0.0;
            std::map<std::string, double> categorysummary;
            for (const auto& pair : categorysummary)
            {
                outfile << "-" << pair.first << ":" << pair.second << "\n";
            }
          

            for (const auto& exp : expenses)
            {
                bool include_in_report = false;
                if (periodtype == "день" && DateUtils::isCurrentDay(exp.date))
                    include_in_report = true;
                else if (periodtype == "неделя" && DateUtils::isCurrentWeek(exp.date))
                    include_in_report = true;
                else if (periodtype == "месяц" && DateUtils::isCurrentMonth(exp.date))
                    include_in_report = true;

                if (include_in_report)
                {
                    totalexpenses += exp.amount;
                    categorysummary[exp.category.name] += exp.amount;
                }
            }

            outfile << "Отчет за " << periodtype << ":\n";
            outfile << "Общая сумма: " << totalexpenses << "\n";
            outfile << "По категориям:\n";
            for (const auto& pair : categorysummary)
            {
                outfile << "  - " << pair.first << ": " << pair.second << "\n";
            }
            outfile << "\n";
            };

        saveReportForPeriod("день");
        saveReportForPeriod("неделя");
        saveReportForPeriod("месяц");

        auto saveTopExpenses = [&](const std::string& period_type) {
            outfile << "ТОП-3 затрат за " << period_type << ":\n";
            std::vector<Expense> relevant_expenses;

            for (const auto& exp : expenses)
            {
                bool include_in_report = false;
                if (period_type == "неделя" && DateUtils::isCurrentWeek(exp.date))
                    include_in_report = true;
                else if (period_type == "месяц" && DateUtils::isCurrentMonth(exp.date))
                    include_in_report = true;

                if (include_in_report)
                    relevant_expenses.push_back(exp);
            }

            std::sort(
                relevant_expenses.begin(),
                relevant_expenses.end(),
                [](const Expense& a, const Expense& b)
                {
                    return a.amount > b.amount;
                }
            );

            for (int i = 0; i < std::min<int>(relevant_expenses.size(), 3); ++i)
            {
                outfile << (i + 1) << ". "
                    << relevant_expenses[i].date << ", "
                    << relevant_expenses[i].amount << ", "
                    << relevant_expenses[i].category.name << ", "
                    << relevant_expenses[i].description << "\n";
            }
            outfile << "\n";
            };

        saveTopExpenses("неделя");
        saveTopExpenses("месяц");

        auto saveTopCategories = [&](const std::string& period_type) {
            outfile << "ТОП-3 категорий за " << period_type << ":\n";
            std::map<std::string, double> category_totals;

            for (const auto& exp : expenses)
            {
                bool include_in_report = false;
                if (period_type == "неделя" && DateUtils::isCurrentWeek(exp.date))
                    include_in_report = true;
                else if (period_type == "месяц" && DateUtils::isCurrentMonth(exp.date))
                    include_in_report = true;

                if (include_in_report)
                    category_totals[exp.category.name] += exp.amount;
            }

            std::vector<std::pair<std::string, double>> sorted_categories(
                category_totals.begin(), category_totals.end()
            );

            std::sort(
                sorted_categories.begin(),
                sorted_categories.end(),
                [](const std::pair<std::string, double>& a,
                    const std::pair<std::string, double>& b)
                {
                    return a.second > b.second;
                }
            );

            for (int i = 0; i < std::min<int>(sorted_categories.size(), 3); ++i)
            {
                outfile << (i + 1) << ". " << sorted_categories[i].first
                    << ": " << sorted_categories[i].second << "\n";
            }
            outfile << "\n";
            }; saveTopCategories("неделя");
        saveTopCategories("месяц");

        outfile.close();
        std::cout << "Отчеты и рейтинги сохранены в файл '" << filename << "'" << std::endl;
    }

    void runInteractive()
    {
        while (true)
        {
            std::cout << "\n=== Ввод новой траты ===\n";

            double amount;
            std::cout << "Введите сумму (0 для выхода): ";
            std::cin >> amount;

            if (!std::cin || amount == 0.0)
            {
                std::cout << "Выход из режима ввода трат.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }

            if (amount < 0)
            {
                std::cout << "Сумма не может быть отрицательной.\n";
                continue;
            }

            if (wallet.empty())
            {
                std::cout << "Нет ни одного кошелька. Сначала добавьте кошельки.\n";
                break;
            }

            std::cout << "\nВыберите кошелек, с которого списать:\n";
            for (size_t i = 0; i < wallet.size(); ++i)
            {
                std::cout << "  " << (i + 1) << ") " << wallet[i]->getName()
                    << " (баланс: " << wallet[i]->getBalance() << ")\n";
            }

            int walletChoice;
            std::cout << "Введите номер кошелька: ";
            std::cin >> walletChoice;

            if (!std::cin || walletChoice < 1 || walletChoice >(int)wallet.size())
            {
                std::cout << "Некорректный выбор кошелька.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            std::string walletname = wallet[walletChoice - 1] ->getName();

            std::cout << "\nВыберите категорию или создайте новую:\n";
            for (size_t i = 0; i < categories.size(); ++i)
            {
                std::cout << "  " << (i + 1) << ") " << categories[i].name << "\n";
            }
            std::cout << "  0) Создать новую категорию\n";

            int categoryChoice;
            std::cout << "Введите номер категории: ";
            std::cin >> categoryChoice;

            std::string categoryname;

            if (!std::cin)
            {
                std::cout << "Некорректный ввод.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }

            if (categoryChoice == 0)
            {
                std::cout << "Введите название новой категории: ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::getline(std::cin, categoryname);
                if (categoryname.empty())
                {
                    std::cout << "Имя категории не может быть пустым.\n";
                    continue;
                }
                addCategory(Category(categoryname));
            }
            else if (categoryChoice >= 1 && categoryChoice <= (int)categories.size())
            {
                categoryname = categories[categoryChoice - 1].name;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            else
            {
                std::cout << "Некорректный номер категории.\n";
                continue;
            }

            std::cout << "Введите описание (комментарий к трате): ";
            std::string description;
            std::getline(std::cin, description);

            addExpense(amount, categoryname, description, walletname);

            std::cout << "\nДобавить еще одну трату? (1 - да, 0 - нет): ";
            int cont;
            std::cin >> cont;
            if (!std::cin || cont == 0)
            {
                std::cout << "Выход из режима ввода трат.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); break;
            }
        }
    }

    ~FinancialManager()
    {
        for (Wallet* w : wallet)
        {
            delete w;
        }
    }
};

int main()
{
    std::setlocale(LC_ALL, "");

    FinancialManager manager;

    manager.addCategory(Category("Бургер кинг"));
    manager.addCategory(Category("Такси"));
    manager.addCategory(Category("Клубы"));
    manager.addCategory(Category("Покупки"));

    manager.addWallet(new DebitCard("Основная карта", 10000.0));
    manager.addWallet(new CreditCard("Кредитка Сбербанка", 50000.0));
    manager.addWallet(new Wallet("Наличные", 2500.0));

    for (auto& w : manager.wallet)
    {
        if (w->getName() == "Основная карта")
        {
            w->deposit(2000.0);
        }
    }

    manager.runInteractive();

    manager.generateReport("день");
    manager.generateReport("неделя");
    manager.generateReport("месяц");

    manager.generateTopExpenses("неделя");
    manager.generateTopExpenses("месяц");
    manager.generateTopCategories("неделя");
    manager.generateTopCategories("месяц");

    manager.saveReportsToFile("financial_report.txt");

    return 0;
}