
#include "budget_manager.hpp"

#include <cctype>

// Helper: lowercase string
static string budgetToLower(string text) {

    for (char& ch : text) {
        ch = static_cast<char>(
            tolower(static_cast<unsigned char>(ch))
        );
    }

    return text;
}


// ADD BUDGET
bool addBudget(
    vector<Budget>& budgets,
    const Budget& budget
) {
    if (budget.userId.empty() ||
        budget.category.empty() ||
        budget.limitPaise <= 0) {

        return false;
    }

    budgets.push_back(budget);

    return true;
}


// EVALUATE BUDGET
BudgetStatus evaluateBudget(
    const vector<Expense>& expenses,
    const Budget& budget,
    const string& startDate,
    const string& endDate
) {

    BudgetStatus result;

    result.budgetLimitPaise = budget.limitPaise;
    result.actualSpendingPaise = 0;
    result.remainingPaise = budget.limitPaise;
    result.percentageUsed = 0.0;
    result.alert = BudgetAlert::WITHIN_BUDGET;

    // Invalid input
    if (budget.limitPaise <= 0 ||
        startDate.length() != 10 ||
        endDate.length() != 10 ||
        startDate > endDate) {

        return result;
    }

    string targetCategory = budgetToLower(budget.category);

    // Calculate matching expenses
    for (const Expense& expense : expenses) {

        // Match user
        if (expense.userId != budget.userId) {
            continue;
        }

        // Match date range
        if (expense.date < startDate ||
            expense.date > endDate) {

            continue;
        }

        // Match category, unless overall budget
        if (targetCategory != "all" &&
            budgetToLower(expense.category) != targetCategory) {

            continue;
        }

        result.actualSpendingPaise += expense.amountPaise;
    }

    // Remaining budget
    result.remainingPaise =
        result.budgetLimitPaise -
        result.actualSpendingPaise;

    // Percentage consumed
    result.percentageUsed =
        (static_cast<double>(result.actualSpendingPaise) /
         result.budgetLimitPaise) * 100.0;

    // Generate alert
    if (result.actualSpendingPaise >=
        result.budgetLimitPaise) {

        result.alert = BudgetAlert::EXCEEDED;
    }
    else if (result.actualSpendingPaise * 100 >=
             result.budgetLimitPaise * 80) {

        result.alert = BudgetAlert::WARNING;
    }
    else {

        result.alert = BudgetAlert::WITHIN_BUDGET;
    }

    return result;
}
