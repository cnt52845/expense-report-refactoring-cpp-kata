#include <sstream>
#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "expense_report.h"

class MockReportPrinter : public ReportPrinter {
private:
    std::string text;

public:
    void print(const std::string& text) override { this->text += text + "\n"; }

    std::string get_text() const { return text; }
};

class ExpenseReportTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        report  = std::make_unique<ExpenseReport>();
        printer = std::make_unique<MockReportPrinter>();
    }

    std::unique_ptr<ExpenseReport>     report;
    std::unique_ptr<MockReportPrinter> printer;
};

TEST_F(ExpenseReportTest, print_empty)
{
    report->print_report(*printer);

    std::string expected_output = "Expense Report\n"
                                  "--------------\n"
                                  "--------------\n"
                                  "Meal Total: 0.00\n"
                                  "Total: 0.00\n";
    EXPECT_EQ(printer->get_text(), expected_output);
}

TEST_F(ExpenseReportTest, print_one_dinner)
{
    report->add_expense(DinnerExpense(3000));
    report->print_report(*printer);

    std::string expected_output = "Expense Report\n"
                                  "--------------\n"
                                  "Dinner\t30.00\t\n"
                                  "--------------\n"
                                  "Meal Total: 33.00\n"
                                  "Total: 33.00\n";
    EXPECT_EQ(printer->get_text(), expected_output);
}

TEST_F(ExpenseReportTest, print_two_meals_and_dinner_over)
{
    report->add_expense(DinnerExpense(6000));
    report->add_expense(BreakfastExpense(1000));
    report->print_report(*printer);

    std::string expected_output = "Expense Report\n"
                                  "--------------\n"
                                  "Dinner\t60.00\tX\n"
                                  "Breakfast\t10.00\t\n"
                                  "--------------\n"
                                  "Meal Total: 76.50\n"
                                  "Total: 76.50\n";
    EXPECT_EQ(printer->get_text(), expected_output);
}

TEST_F(ExpenseReportTest, print_mix_and_dinner_over)
{
    report->add_expense(DinnerExpense(5000));
    report->add_expense(BreakfastExpense(2000));
    report->add_expense(LodgingExpense(3000));
    report->print_report(*printer);

    std::string expected_output = "Expense Report\n"
                                  "--------------\n"
                                  "Dinner\t50.00\t\n"
                                  "Breakfast\t20.00\tX\n"
                                  "Lodging\t30.00\t\n"
                                  "--------------\n"
                                  "Meal Total: 76.00\n"
                                  "Total: 110.50\n";
    EXPECT_EQ(printer->get_text(), expected_output);
}
