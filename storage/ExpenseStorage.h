#pragma once

#include <vector>
#include <string>
#include "../models/models.hpp"

using namespace std;

// Save all expenses to a CSV file
bool saveExpensesToFile(
    const vector<Expense>& expenses,
    const string& filename
);

// Load expenses from a CSV file
bool loadExpensesFromFile(
    vector<Expense>& expenses,
    const string& filename
);