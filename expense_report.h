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
    double                                total      = 0;
    double                                meal_total = 0;

    void add_to_totals(const std::shared_ptr<Expense>& expense)
    {
        double subtotal = expense->get_amount() + expense->get_surcharge();
        total += subtotal;
        if (expense->is_meal()) {
            meal_total += subtotal;
        }
    }

public:
    void add_expense(const std::shared_ptr<Expense>& expense) { expenses.push_back(expense); }

    void total_up_expenses()
    {
        for (const std::shared_ptr<Expense>& expense : expenses) {
            add_to_totals(expense);
        }
    }

    const std::vector<std::shared_ptr<Expense>>& get_expenses() { return expenses; }

    double get_expenses_total() { return total; }

    double get_meal_expenses_total() { return meal_total; }
};

class ExpenseReporter {
private:
    const std::shared_ptr<ExpenseReport> report;
    std::shared_ptr<ExpenseNamer>        namer = std::make_shared<ExpenseReportNamer>();
    ReportPrinter*                       printer;

    void print_expenses_and_totals()
    {
        print_header();
        print_expenses();
        print_totals();
    }

    void print_header()
    {
        printer->print("Expense Report");
        printer->print("--------------");
    }

    void print_expenses()
    {
        for (const std::shared_ptr<Expense>& expense : report->get_expenses()) {
            print_expense(expense);
        }
    }

    void print_expense(const std::shared_ptr<Expense>& expense)
    {
        std::ostringstream oss;
        oss << namer->get_name(expense) << "\t" << print_amount(expense->get_amount()) << "\t"
            << (expense->is_overage() ? "X" : "");
        printer->print(oss.str());
    }

    void print_totals()
    {
        printer->print("--------------");
        std::ostringstream oss;
        oss << "Meal Total: " << print_amount(report->get_meal_expenses_total()) << "\n";
        oss << "Total: " << print_amount(report->get_expenses_total());
        printer->print(oss.str());
    }

    std::string print_amount(double amount_in_pennies)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << pennies_to_dollars(amount_in_pennies);
        return oss.str();
    }

    double pennies_to_dollars(double pennies) { return pennies / 100.0; }

public:
    ExpenseReporter(std::shared_ptr<ExpenseReport>& report) : report(report) {}
    void print_report(ReportPrinter& printer)
    {
        this->printer = &printer;
        report->total_up_expenses();
        print_expenses_and_totals();
    }
};
