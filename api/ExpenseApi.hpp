#pragma once

#include <string>

#include "../application/ExpenseService.hpp"

class ExpenseApi
{
private:
    ExpenseService& expenseService;

public:
    explicit ExpenseApi(ExpenseService& service);

    std::string getAllExpenses() const;

    std::string getExpenseById(
        const std::string& expenseId
    ) const;

    std::string createExpense(Expense expense);

    std::string updateExpense(
        const std::string& expenseId,
        const Expense& updatedData
    );

    std::string deleteExpense(
        const std::string& expenseId
    ) const;
};