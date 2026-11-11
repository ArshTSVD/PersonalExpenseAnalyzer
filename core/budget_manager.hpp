
#pragma once

#include <string>
#include <vector>

#include "expense_manager.hpp"

using namespace std;

// Budget duration
enum class BudgetPeriod {
    DAILY,
    WEEKLY,
    MONTHLY
};

// Budget information
struct Budget {
    string userId;

    // Use "ALL" for an overall budget
    // Otherwise use a category such as "Food"
    string category;

    long long limitPaise;

    BudgetPeriod period;
};

// Budget alert status
enum class BudgetAlert {
    WITHIN_BUDGET,
    WARNING,
    EXCEEDED
};

// Result of budget evaluation
struct BudgetStatus {
    long long budgetLimitPaise;
    long long actualSpendingPaise;
    long long remainingPaise;

    double percentageUsed;

    BudgetAlert alert;
};

// Add a new budget
bool addBudget(
    vector<Budget>& budgets,
    const Budget& budget
);

// Evaluate expenses against a budget for a given date range
BudgetStatus evaluateBudget(
    const vector<Expense>& expenses,
    const Budget& budget,
    const string& startDate,
    const string& endDate
);
