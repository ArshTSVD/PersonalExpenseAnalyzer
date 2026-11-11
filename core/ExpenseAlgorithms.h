#pragma once

#include <vector>
#include <string>
#include "../models/models.hpp"

using namespace std;

// Searching
int searchExpenseById(
    const vector<Expense>& expenses,
    const string& id
);

/*vector<Expense> searchExpensesByTitle(
    const vector<Expense>& expenses,
    const string& title
);*/

vector<Expense> searchExpensesByCategory(
    const vector<Expense>& expenses,
    const string& category
);

// Sorting
void mergeSortByAmount(vector<Expense>& expenses);

void sortExpensesByDate(vector<Expense>& expenses);

void sortExpensesByCategory(vector<Expense>& expenses);

// Sliding window
struct SpendingWindowResult {
    bool found;
    string startDate;
    string endDate;
    long long totalPaise;
};

SpendingWindowResult findHighestSpendingWindow(
    const vector<Expense>& expenses,
    int windowDays
);