
#include "expense_manager.hpp"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <map>

using namespace std;

// Generates a unique expense ID based on existing records.
static string generateExpenseId(
    const vector<Expense>& expenses
) {
    int maxNumber = 0;

    for (const Expense& expense : expenses) {

        if (expense.id.substr(0, 3) == "EXP") {

            try {
                int number = stoi(expense.id.substr(3));

                if (number > maxNumber) {
                    maxNumber = number;
                }
            }
            catch (...) {
                // Ignore IDs that do not have a valid number.
            }
        }
    }

    ostringstream idStream;

    idStream << "EXP"
             << setw(4)
             << setfill('0')
             << maxNumber + 1;

    return idStream.str();
}


// CREATE
string addExpense(
    vector<Expense>& expenses,
    const Expense& newExpense
) {
    // Basic input validation
    if (newExpense.title.empty() ||
        newExpense.category.empty() ||
        newExpense.date.empty() ||
        newExpense.amountPaise <= 0) {

        return "";
    }

    Expense expense = newExpense;

    // Generate ID automatically
    expense.id = generateExpenseId(expenses);

    expenses.push_back(expense);

    return expense.id;
}


// READ / SEARCH
const Expense* findExpenseById(
    const vector<Expense>& expenses,
    const string& expenseId
) {
    for (const Expense& expense : expenses) {

        if (expense.id == expenseId) {
            return &expense;
        }
    }

    return nullptr;
}


// UPDATE
bool updateExpense(
    vector<Expense>& expenses,
    const string& expenseId,
    const Expense& updatedData
) {
    for (Expense& expense : expenses) {

        if (expense.id == expenseId) {

            // Validate new data
            if (updatedData.title.empty() ||
                updatedData.category.empty() ||
                updatedData.date.empty() ||
                updatedData.amountPaise <= 0) {

                return false;
            }

            expense.title = updatedData.title;
            expense.category = updatedData.category;
            expense.amountPaise = updatedData.amountPaise;
            expense.date = updatedData.date;
            expense.note = updatedData.note;

            return true;
        }
    }

    return false;
}


// DELETE
bool deleteExpense(
    vector<Expense>& expenses,
    const string& expenseId
) {
    for (auto it = expenses.begin();
         it != expenses.end();
         ++it) {

        if (it->id == expenseId) {

            expenses.erase(it);

            return true;
        }
    }

    return false;
}


// Converts a string to lowercase for case-insensitive comparison.
static string toLowerCase(const string& text) {

    string result = text;

    for (char& ch : result) {
        ch = static_cast<char>(
            tolower(static_cast<unsigned char>(ch))
        );
    }

    return result;
}


// VIEW ALL
vector<Expense> getAllExpenses(
    const vector<Expense>& expenses
) {
    return expenses;
}


// SEARCH BY TITLE
vector<Expense> searchExpensesByTitle(
    const vector<Expense>& expenses,
    const string& keyword
) {
    vector<Expense> results;

    string searchKey = toLowerCase(keyword);

    for (const Expense& expense : expenses) {

        string title = toLowerCase(expense.title);

        if (title.find(searchKey) != string::npos) {
            results.push_back(expense);
        }
    }

    return results;
}


// FILTER BY CATEGORY
vector<Expense> filterExpensesByCategory(
    const vector<Expense>& expenses,
    const string& category
) {
    vector<Expense> results;

    string target = toLowerCase(category);

    for (const Expense& expense : expenses) {

        if (toLowerCase(expense.category) == target) {
            results.push_back(expense);
        }
    }

    return results;
}


// FILTER BY DATE RANGE
vector<Expense> filterExpensesByDateRange(
    const vector<Expense>& expenses,
    const string& startDate,
    const string& endDate
) {
    vector<Expense> results;

    if (startDate > endDate) {
        return results;
    }

    for (const Expense& expense : expenses) {

        if (expense.date >= startDate &&
            expense.date <= endDate) {

            results.push_back(expense);
        }
    }

    return results;
}


// SORT EXPENSES
vector<Expense> sortExpenses(
    const vector<Expense>& expenses,
    ExpenseSortBy sortBy,
    SortOrder order
) {
    vector<Expense> results = expenses;

    stable_sort(
        results.begin(),
        results.end(),
        [sortBy, order](const Expense& a, const Expense& b) {

            bool ascending = false;

            switch (sortBy) {

                case ExpenseSortBy::DATE:
                    ascending = a.date < b.date;
                    break;

                case ExpenseSortBy::AMOUNT:
                    ascending = a.amountPaise < b.amountPaise;
                    break;

                case ExpenseSortBy::TITLE:
                    ascending =
                        toLowerCase(a.title) <
                        toLowerCase(b.title);
                    break;
            }

            if (order == SortOrder::ASCENDING) {
                return ascending;
            }

            // Reverse comparison for descending order
            switch (sortBy) {

                case ExpenseSortBy::DATE:
                    return a.date > b.date;

                case ExpenseSortBy::AMOUNT:
                    return a.amountPaise > b.amountPaise;

                case ExpenseSortBy::TITLE:
                    return toLowerCase(a.title) >
                           toLowerCase(b.title);
            }

            return false;
        }
    );

    return results;
}


// OVERALL EXPENSE ANALYTICS
ExpenseAnalytics calculateExpenseAnalytics(
    const vector<Expense>& expenses
) {
    ExpenseAnalytics result;

    result.totalExpenseCount = expenses.size();
    result.totalSpendingPaise = 0;
    result.averageExpensePaise = 0;
    result.hasExpenses = false;

    if (expenses.empty()) {
        return result;
    }

    result.hasExpenses = true;

    result.highestExpense = expenses[0];
    result.lowestExpense = expenses[0];

    for (const Expense& expense : expenses) {

        result.totalSpendingPaise += expense.amountPaise;

        if (expense.amountPaise >
            result.highestExpense.amountPaise) {

            result.highestExpense = expense;
        }

        if (expense.amountPaise <
            result.lowestExpense.amountPaise) {

            result.lowestExpense = expense;
        }
    }

    result.averageExpensePaise =
        result.totalSpendingPaise / result.totalExpenseCount;

    return result;
}


// CATEGORY-WISE SPENDING
map<string, long long> calculateCategorySpending(
    const vector<Expense>& expenses
) {
    map<string, long long> categoryTotals;

    for (const Expense& expense : expenses) {

        categoryTotals[expense.category] +=
            expense.amountPaise;
    }

    return categoryTotals;
}


// DAILY SPENDING
map<string, long long> calculateDailySpending(
    const vector<Expense>& expenses
) {
    map<string, long long> dailyTotals;

    for (const Expense& expense : expenses) {

        dailyTotals[expense.date] +=
            expense.amountPaise;
    }

    return dailyTotals;
}


// MONTHLY SPENDING
map<string, long long> calculateMonthlySpending(
    const vector<Expense>& expenses
) {
    map<string, long long> monthlyTotals;

    for (const Expense& expense : expenses) {

        // Date format: YYYY-MM-DD
        if (expense.date.length() < 7) {
            continue;
        }

        string month = expense.date.substr(0, 7);

        monthlyTotals[month] +=
            expense.amountPaise;
    }

    return monthlyTotals;
}

// HASH MAP: ADVANCED CATEGORY ANALYTICS
unordered_map<string, CategoryAnalytics> calculateCategoryAnalytics(
    const vector<Expense>& expenses
) {

    unordered_map<string, CategoryAnalytics> categoryData;

    for (const Expense& expense : expenses) {

        string category = toLowerCase(expense.category);

        categoryData[category].totalSpendingPaise +=
            expense.amountPaise;

        categoryData[category].transactionCount++;
    }

    // Calculate average expense for each category.
    for (auto& entry : categoryData) {

        CategoryAnalytics& data = entry.second;

        if (data.transactionCount > 0) {

            data.averageExpensePaise =
                data.totalSpendingPaise / data.transactionCount;
        }
    }

    return categoryData;
}
