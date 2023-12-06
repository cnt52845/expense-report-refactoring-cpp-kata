#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <string>

class ReportPrinter {
public:
    virtual void print(const std::string& text) = 0;
};

class Expense {
public:
    Expense(int amount) : amount(amount) {}
    int            get_amount() const { return amount; }
    virtual double get_surcharge() = 0;
    virtual bool   is_meal()       = 0;
    virtual bool   is_overage()    = 0;

private:
    int amount;
};

class DinnerExpense : public Expense {
public:
    DinnerExpense(int amount) : Expense(amount) {}
    double get_surcharge() override { return get_amount() * 0.10; }
    bool   is_meal() override { return true; }
    bool   is_overage() override { return get_amount() > 5000; }
};

class BreakfastExpense : public Expense {
public:
    BreakfastExpense(int amount) : Expense(amount) {}
    double get_surcharge() override { return get_amount() * 0.05; }
    bool   is_meal() override { return true; }
    bool   is_overage() override { return get_amount() > 1000; }
};

class LodgingExpense : public Expense {
public:
    LodgingExpense(int amount) : Expense(amount) {}
    double get_surcharge() override { return get_amount() * 0.15; }
    bool   is_meal() override { return false; }
    bool   is_overage() override { return false; }
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
            double      surcharge                 = expense->get_surcharge();
            std::string name                      = namer->get_name(expense);
            std::string meal_over_expenses_marker = "";

            total += expense->get_amount() + surcharge;
            if (expense->is_meal()) {
                meal_total += expense->get_amount() + surcharge;
                if (expense->is_overage()) {
                    meal_over_expenses_marker = "X";
                }
            }

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

class ExpenseReporter {
private:
    const std::shared_ptr<ExpenseReport> report;

public:
    ExpenseReporter(std::shared_ptr<ExpenseReport>& report) : report(report) {}
    void print_report(ReportPrinter& printer) { report->print_report(printer); }
};