#pragma once

#include <string>
#include <vector>

#include "ApplicationState.hpp"

class ExpenseService
{
private:
    ApplicationState& state;

public:
    explicit ExpenseService(ApplicationState& appState);

    std::vector<Expense> getAllExpenses() const;

    const Expense* getExpenseById(
        const std::string& expenseId
    ) const;

    std::string createExpense(Expense expense);

    bool updateExpense(
        const std::string& expenseId,
        const Expense& updatedData
    );

    bool removeExpense(const std::string& expenseId);
};