#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

class ReportPrinter {
public:
    virtual void print(const std::string& text) = 0;
};

bool
is_weekend(const std::string& date_str)
{
    std::tm            date{};
    std::istringstream ss(date_str);
    ss >> std::get_time(&date, "%Y-%m-%d");
    std::mktime(&date);

    int day_of_week = date.tm_wday;
    // Check if it's Saturday (6) or Sunday (0)
    return day_of_week == 6 || day_of_week == 0;
}

class Expense {
public:
    Expense(int amount, std::string date = "2023-12-06") : amount(amount), date(date) {}
    virtual double get_surcharge() = 0;
    virtual bool   is_meal()       = 0;
    virtual bool   is_overage()    = 0;

    int         amount;
    std::string date;
};

class DinnerExpense : public Expense {
public:
    DinnerExpense(int amount, std::string date = "2023-12-06") : Expense(amount, date) {}
    double get_surcharge() override
    {
        if (is_weekend(date)) {
            return amount * 0.15;
        }
        return amount * 0.10;
    }
    bool is_meal() override { return true; }
    bool is_overage() override { return (amount > 5000); }
};

class BreakfastExpense : public Expense {
public:
    BreakfastExpense(int amount, std::string date = "2023-12-06") : Expense(amount, date) {}
    double get_surcharge() override { return is_weekend(date) ? amount * 0.10 : amount * 0.05; }
    bool   is_meal() override { return true; }
    bool   is_overage() override { return (amount > 1000); }
};

class LodgingExpense : public Expense {
public:
    LodgingExpense(int amount, std::string date = "2023-12-06") : Expense(amount, date) {}
    double get_surcharge() override { return amount * 0.15; }
    bool   is_meal() override { return false; }
    bool   is_overage() override { return false; }
};

class TransportationExpense : public Expense {
public:
    TransportationExpense(int amount, std::string date = "2023-12-06") : Expense(amount, date) {}
    double get_surcharge() override { return amount * 0.05; }
    bool   is_meal() override { return false; }
    bool   is_overage() override { return false; }
};

class SuppliesExpense : public Expense {
public:
    SuppliesExpense(int amount, std::string date = "2023-12-06") : Expense(amount, date) {}
    double get_surcharge() override { return 0.00; }
    bool   is_meal() override { return false; }
    bool   is_overage() override { return false; }
};

template <typename T>
inline bool
is_instance_of(const std::shared_ptr<Expense>& expense)
{
    return std::dynamic_pointer_cast<T>(expense) != nullptr;
}

class ExpenseNamer {
public:
    virtual ~ExpenseNamer()                                               = default;
    virtual std::string get_name(const std::shared_ptr<Expense>& expense) = 0;
};

class ExpenseNamerEnglish : public ExpenseNamer {
public:
    virtual ~ExpenseNamerEnglish() = default;
    std::string get_name(const std::shared_ptr<Expense>& expense) override
    {
        if (is_instance_of<DinnerExpense>(expense)) {
            return "Dinner";
        }
        if (is_instance_of<BreakfastExpense>(expense)) {
            return "Breakfast";
        }
        if (is_instance_of<LodgingExpense>(expense)) {
            return "Lodging";
        }
        if (is_instance_of<TransportationExpense>(expense)) {
            return "Transportation";
        }
        if (is_instance_of<SuppliesExpense>(expense)) {
            return "Supplies";
        }
        return "Other";
    }
};

class ExpenseReport {
private:
    std::vector<std::shared_ptr<Expense>> expenses;
    std::unique_ptr<ExpenseNamer>         namer = std::make_unique<ExpenseNamerEnglish>();

public:
    void add_expense(const std::shared_ptr<Expense>& expense) { expenses.push_back(expense); }

    void print_report(ReportPrinter& printer)
    {
        printer.print("Expense Report");
        printer.print("--------------");

        double total      = 0;
        double meal_total = 0;
        for (const std::shared_ptr<Expense>& expense : expenses) {
            double      surcharge = expense->get_surcharge();
            std::string name      = namer->get_name(expense);

            total += expense->amount + surcharge;
            if (expense->is_meal()) {
                meal_total += expense->amount + surcharge;
            }

            std::string meal_over_expenses_marker = (expense->is_overage()) ? "X" : "";

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << name << "\t" << expense->amount / 100.0
                << "\t" << meal_over_expenses_marker;
            printer.print(oss.str());
        }

        printer.print("--------------");
        std::ostringstream oss;
        oss << "Meal Total: " << std::fixed << std::setprecision(2) << meal_total / 100.0 << "\n";
        oss << "Total: " << std::fixed << std::setprecision(2) << total / 100.0;
        printer.print(oss.str());
    }
};
