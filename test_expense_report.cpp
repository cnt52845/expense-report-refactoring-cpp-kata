#include <memory>
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
        report   = std::make_shared<ExpenseReport>();
        reporter = std::make_shared<ExpenseReporter>(report);
        printer  = std::make_shared<MockReportPrinter>();
    }

    std::shared_ptr<ExpenseReport>     report;
    std::shared_ptr<ExpenseReporter>   reporter;
    std::shared_ptr<MockReportPrinter> printer;
};

TEST_F(ExpenseReportTest, print_empty)
{
    reporter->print_report(*printer);

    std::string expected_output = "Expense Report\n"
                                  "--------------\n"
                                  "--------------\n"
                                  "Meal Total: 0.00\n"
                                  "Total: 0.00\n";
    EXPECT_EQ(printer->get_text(), expected_output);
}

TEST_F(ExpenseReportTest, print_one_dinner)
{
    report->add_expense(std::make_shared<DinnerExpense>(3000));
    reporter->print_report(*printer);

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
    report->add_expense(std::make_shared<DinnerExpense>(6000));
    report->add_expense(std::make_shared<BreakfastExpense>(1000));
    reporter->print_report(*printer);

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
    report->add_expense(std::make_shared<DinnerExpense>(5000));
    report->add_expense(std::make_shared<BreakfastExpense>(2000));
    report->add_expense(std::make_shared<LodgingExpense>(3000));
    reporter->print_report(*printer);

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
