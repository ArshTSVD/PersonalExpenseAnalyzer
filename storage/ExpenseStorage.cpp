#include "ExpenseStorage.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

// ==========================================
// CSV HELPER: ESCAPE A FIELD
// ==========================================

static string escapeCSV(const string& value) {

    string result = "\"";

    for (char ch : value) {

        if (ch == '"') {
            result += "\"\"";
        }
        else {
            result += ch;
        }
    }

    result += "\"";

    return result;
}


// ==========================================
// CSV HELPER: PARSE A LINE
// ==========================================

static bool parseCSVLine(
    const string& line,
    vector<string>& fields
) {

    fields.clear();

    string field;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.size(); i++) {

        char ch = line[i];

        if (ch == '"') {

            if (insideQuotes &&
                i + 1 < line.size() &&
                line[i + 1] == '"') {

                field += '"';
                i++;
            }
            else {
                insideQuotes = !insideQuotes;
            }
        }
        else if (ch == ',' && !insideQuotes) {

            fields.push_back(field);
            field.clear();
        }
        else {
            field += ch;
        }
    }

    if (insideQuotes) {
        return false;
    }

    fields.push_back(field);

    return true;
}


// ==========================================
// SAVE EXPENSES
// ==========================================

bool saveExpensesToFile(
    const vector<Expense>& expenses,
    const string& filename
) {

    ofstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    // CSV header
    file << "id,userId,title,category,amountPaise,date,note\n";

    for (const Expense& expense : expenses) {

        file
            << escapeCSV(expense.id) << ","
            << escapeCSV(expense.userId) << ","
            << escapeCSV(expense.title) << ","
            << escapeCSV(expense.category) << ","
            << expense.amountPaise << ","
            << escapeCSV(expense.date) << ","
            << escapeCSV(expense.note)
            << "\n";
    }

    file.close();

    return !file.fail();
}


// ==========================================
// LOAD EXPENSES
// ==========================================

bool loadExpensesFromFile(
    vector<Expense>& expenses,
    const string& filename
) {

    ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    string line;

    // Read and verify the CSV header
    if (!getline(file, line)) {
        return false;
    }

    if (line != "id,userId,title,category,amountPaise,date,note" &&
        line != "id,userId,title,category,amountPaise,date,note\r") {

        return false;
    }

    vector<Expense> loadedExpenses;
    vector<string> fields;

    while (getline(file, line)) {

        if (line.empty()) {
            continue;
        }

        if (!parseCSVLine(line, fields)) {
            return false;
        }

        if (fields.size() != 7) {
            return false;
        }

        Expense expense;

        expense.id = fields[0];
        expense.userId = fields[1];
        expense.title = fields[2];
        expense.category = fields[3];

        try {

            size_t processed = 0;

            expense.amountPaise = stoll(fields[4], &processed);

            if (processed != fields[4].size() ||
                expense.amountPaise < 0) {

                return false;
            }
        }
        catch (...) {
            return false;
        }

        expense.date = fields[5];
        expense.note = fields[6];

        loadedExpenses.push_back(expense);
    }

    if (file.bad()) {
        return false;
    }

    // Replace existing data only after successful loading.
    expenses = loadedExpenses;

    return true;
}