#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

class ReportPrinter {
public:
    virtual void print(const std::string& text) = 0;
};

enum ExpenseType { DINNER, BREAKFAST, LODGING };

class Expense {
public:
    Expense(ExpenseType type, int amount) : type(type), amount(amount) {}
    int            get_amount() const { return amount; }
    virtual double get_surcharge() = 0;

    ExpenseType type;

protected:
    int amount;
};

class DinnerExpense : public Expense {
public:
    DinnerExpense(int amount) : Expense(DINNER, amount) {}
    double get_surcharge() override { return amount * 0.10; }
};

class BreakfastExpense : public Expense {
public:
    BreakfastExpense(int amount) : Expense(BREAKFAST, amount) {}
    double get_surcharge() override { return amount * 0.05; }
};

class LodgingExpense : public Expense {
public:
    LodgingExpense(int amount) : Expense(LODGING, amount) {}
    double get_surcharge() override { return amount * 0.15; }
};

template <typename T>
inline bool
is_instance_of(const std::shared_ptr<Expense>& ptr)
{
    return std::dynamic_pointer_cast<T>(ptr) != nullptr;
}

class ExpenseNamer {
public:
    virtual std::string get_name(const std::shared_ptr<Expense>& expense) = 0;
};

class ExpenseReportNamer : public ExpenseNamer {
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
        return "Other";
    }
};

class ExpenseReport {
private:
    std::vector<std::shared_ptr<Expense>> expenses;
    std::shared_ptr<ExpenseNamer>         namer = std::make_shared<ExpenseReportNamer>();

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

            total += expense->get_amount() + surcharge;
            if (expense->type == DINNER || expense->type == BREAKFAST) {
                meal_total += expense->get_amount() + surcharge;
            }

            std::string meal_over_expenses_marker =
                ((expense->type == DINNER && expense->get_amount() > 5000) ||
                 (expense->type == BREAKFAST && expense->get_amount() > 1000))
                    ? "X"
                    : "";

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << name << "\t"
                << expense->get_amount() / 100.0 << "\t" << meal_over_expenses_marker;
            printer.print(oss.str());
        }

        printer.print("--------------");
        std::ostringstream oss;
        oss << "Meal Total: " << std::fixed << std::setprecision(2) << meal_total / 100.0 << "\n";
        oss << "Total: " << std::fixed << std::setprecision(2) << total / 100.0;
        printer.print(oss.str());
    }
};
