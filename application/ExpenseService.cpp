#include "ExpenseService.hpp"

#include "../core/expense_manager.hpp"

using namespace std;

ExpenseService::ExpenseService(ApplicationState& appState)
    : state(appState)
{
}

vector<Expense> ExpenseService::getAllExpenses() const
{
    return state.expenses;
}

const Expense* ExpenseService::getExpenseById(
    const string& expenseId
) const
{
    return findExpenseById(state.expenses, expenseId);
}

string ExpenseService::createExpense(Expense expense)
{
    // Attach the current user's ID.
    expense.userId = state.currentUserId;

    string newId = addExpense(state.expenses, expense);

    if (newId.empty())
        return "";

    // Save all application data.
    if (!state.save())
    {
        deleteExpense(state.expenses, newId);
        return "";
    }

    return newId;
}

bool ExpenseService::updateExpense(
    const string& expenseId,
    const Expense& updatedData
)
{
    // Preserve the original data in case saving fails.
    const Expense* existing =
        findExpenseById(state.expenses, expenseId);

    if (existing == nullptr)
        return false;

    Expense previousExpense = *existing;

    // Call the global function from expense_manager.cpp.
    if (!::updateExpense(state.expenses, expenseId, updatedData))
        return false;

    if (!state.save())
    {
        // Restore the original expense if saving fails.
        ::updateExpense(
            state.expenses,
            expenseId,
            previousExpense
        );

        return false;
    }

    return true;
}

bool ExpenseService::removeExpense(const string& expenseId)
{
    const Expense* existing =
        findExpenseById(state.expenses, expenseId);

    if (existing == nullptr)
        return false;

    Expense deletedExpense = *existing;

    if (!deleteExpense(state.expenses, expenseId))
        return false;

    if (!state.save())
    {
        // Restore the deleted expense if saving fails.
        state.expenses.push_back(deletedExpense);
        return false;
    }

    return true;
}