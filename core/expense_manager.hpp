
/*#pragma once
#include <map>
#include "../models/models.hpp"*/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "../models/models.hpp"

using std::string;
using std::vector;
using std::map;
using std::unordered_map;

// Adds a new expense and returns its generated ID.
// Returns an empty string if the input is invalid.
string addExpense(
    vector<Expense>& expenses,
    const Expense& newExpense
);

// Finds an expense using its ID.
// Returns nullptr if it does not exist.
const Expense* findExpenseById(
    const vector<Expense>& expenses,
    const string& expenseId
);

// Updates an existing expense.
bool updateExpense(
    vector<Expense>& expenses,
    const string& expenseId,
    const Expense& updatedData
);

// Deletes an expense using its ID.
bool deleteExpense(
    vector<Expense>& expenses,
    const string& expenseId
);


// Sorting options
enum class ExpenseSortBy {
    DATE,
    AMOUNT,
    TITLE
};

enum class SortOrder {
    ASCENDING,
    DESCENDING
};

// Returns all expenses
vector<Expense> getAllExpenses(
    const vector<Expense>& expenses
);

// Search by title (case-insensitive)
vector<Expense> searchExpensesByTitle(
    const vector<Expense>& expenses,
    const string& keyword
);

// Filter by category
vector<Expense> filterExpensesByCategory(
    const vector<Expense>& expenses,
    const string& category
);

// Filter by date range (inclusive)
vector<Expense> filterExpensesByDateRange(
    const vector<Expense>& expenses,
    const string& startDate,
    const string& endDate
);

// Sort by selected field and order
vector<Expense> sortExpenses(
    const vector<Expense>& expenses,
    ExpenseSortBy sortBy,
    SortOrder order
);


// Overall expense analytics
struct ExpenseAnalytics {
    int totalExpenseCount;
    long long totalSpendingPaise;
    long long averageExpensePaise;

    bool hasExpenses;

    Expense highestExpense;
    Expense lowestExpense;
};

struct CategoryAnalytics {
    long long totalSpendingPaise;
    long long transactionCount;
    long long averageExpensePaise;
};

// Calculate overall statistics
ExpenseAnalytics calculateExpenseAnalytics(
    const vector<Expense>& expenses
);

// Category-wise spending
map<string, long long> calculateCategorySpending(
    const vector<Expense>& expenses
);

// Daily spending
map<string, long long> calculateDailySpending(
    const vector<Expense>& expenses
);

// Monthly spending
map<string, long long> calculateMonthlySpending(
    const vector<Expense>& expenses
);

unordered_map<string, CategoryAnalytics> calculateCategoryAnalytics(
    const vector<Expense>& expenses
);
