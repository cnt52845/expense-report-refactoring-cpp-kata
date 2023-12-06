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
    int get_amount() const { return amount; }

    ExpenseType type;

protected:
    int amount;
};

class DinnerExpense : public Expense {
public:
    DinnerExpense(int amount) : Expense(DINNER, amount) {}
};

class BreakfastExpense : public Expense {
public:
    BreakfastExpense(int amount) : Expense(BREAKFAST, amount) {}
};

class LodgingExpense : public Expense {
public:
    LodgingExpense(int amount) : Expense(LODGING, amount) {}
};

class ExpenseReport {
private:
    std::vector<std::shared_ptr<Expense>> expenses;

public:
    void add_expense(const std::shared_ptr<Expense>& expense) { expenses.push_back(expense); }

    void print_report(ReportPrinter& printer)
    {
        printer.print("Expense Report");
        printer.print("--------------");

        double total      = 0;
        double meal_total = 0;
        for (const std::shared_ptr<Expense>& expense : expenses) {
            double      surcharge = 0;
            std::string name;

            if (expense->type == DINNER) {
                surcharge = expense->get_amount() * 0.10;
                name      = "Dinner";
            }
            else if (expense->type == BREAKFAST) {
                surcharge = expense->get_amount() * 0.05;
                name      = "Breakfast";
            }
            else if (expense->type == LODGING) {
                surcharge = expense->get_amount() * 0.15;
                name      = "Lodging";
            }
            else {
                name = "Other";
            }

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
