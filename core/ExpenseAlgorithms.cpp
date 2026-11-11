#include "ExpenseAlgorithms.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <limits>
#include <unordered_map> 

// ==========================================
// HELPER: CASE-INSENSITIVE STRING CONVERSION
// ==========================================

static string toLowerCase(const string& value) {

    string result = value;

    for (char& ch : result) {
        ch = static_cast<char>(
            tolower(static_cast<unsigned char>(ch))
        );
    }

    return result;
}


// ==========================================
// SEARCHING
// ==========================================

// Search expense using its unique ID
int searchExpenseById(
    const vector<Expense>& expenses,
    const string& id
) {

    for (int i = 0; i < static_cast<int>(expenses.size()); i++) {

        if (expenses[i].id == id) {
            return i;
        }
    }

    return -1;
}


// Search expenses by title
/*vector<Expense> searchExpensesByTitle(
    const vector<Expense>& expenses,
    const string& title
) {

    vector<Expense> results;

    string searchText = toLowerCase(title);

    for (const Expense& expense : expenses) {

        string expenseTitle = toLowerCase(expense.title);

        if (expenseTitle.find(searchText) != string::npos) {
            results.push_back(expense);
        }
    }

    return results;
}*/   //rd


// Search expenses by category
vector<Expense> searchExpensesByCategory(
    const vector<Expense>& expenses,
    const string& category
) {

    vector<Expense> results;

    string searchCategory = toLowerCase(category);

    for (const Expense& expense : expenses) {

        string expenseCategory = toLowerCase(expense.category);

        if (expenseCategory.find(searchCategory) != string::npos) {
            results.push_back(expense);
        }
    }

    return results;
}


// ==========================================
// SORTING: MERGE SORT BY AMOUNT
// ==========================================

static void merge(
    vector<Expense>& expenses,
    int left,
    int mid,
    int right
) {

    vector<Expense> temp;

    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right) {

        if (expenses[i].amountPaise <= expenses[j].amountPaise) {
            temp.push_back(expenses[i]);
            i++;
        }
        else {
            temp.push_back(expenses[j]);
            j++;
        }
    }

    while (i <= mid) {
        temp.push_back(expenses[i]);
        i++;
    }

    while (j <= right) {
        temp.push_back(expenses[j]);
        j++;
    }

    for (int k = 0; k < static_cast<int>(temp.size()); k++) {
        expenses[left + k] = temp[k];
    }
}


static void mergeSort(
    vector<Expense>& expenses,
    int left,
    int right
) {

    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;

    mergeSort(expenses, left, mid);
    mergeSort(expenses, mid + 1, right);

    merge(expenses, left, mid, right);
}


void mergeSortByAmount(vector<Expense>& expenses) {

    if (expenses.size() <= 1) {
        return;
    }

    mergeSort(
        expenses,
        0,
        static_cast<int>(expenses.size()) - 1
    );
}


// ==========================================
// SORT BY DATE
// ==========================================

void sortExpensesByDate(vector<Expense>& expenses) {

    // YYYY-MM-DD dates can be sorted lexicographically.
    sort(
        expenses.begin(),
        expenses.end(),
        [](const Expense& a, const Expense& b) {
            return a.date < b.date;
        }
    );
}


// ==========================================
// SORT BY CATEGORY
// ==========================================

void sortExpensesByCategory(vector<Expense>& expenses) {

    sort(
        expenses.begin(),
        expenses.end(),
        [](const Expense& a, const Expense& b) {
            return toLowerCase(a.category)
                 < toLowerCase(b.category);
        }
    );
}


// ==========================================
// DATE HELPERS FOR SLIDING WINDOW
// ==========================================

// Convert YYYY-MM-DD into an integer day number.
// Returns false if the date is invalid.

static bool parseDate(const string& date, long long& dayNumber) {

    if (date.size() != 10 ||
        date[4] != '-' ||
        date[7] != '-') {
        return false;
    }

    for (int i = 0; i < 10; i++) {

        if (i == 4 || i == 7) {
            continue;
        }

        if (!isdigit(static_cast<unsigned char>(date[i]))) {
            return false;
        }
    }

    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    if (year < 1 || month < 1 || month > 12) {
        return false;
    }

    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool leapYear =
        (year % 400 == 0) ||
        (year % 4 == 0 && year % 100 != 0);

    if (leapYear) {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1]) {
        return false;
    }

    // Gregorian calendar conversion
    year -= (month <= 2);

    long long era = year / 400;
    long long yearOfEra = year - era * 400;

    int adjustedMonth = month + (month > 2 ? -3 : 9);

    long long dayOfYear =
        (153 * adjustedMonth + 2) / 5 + day - 1;

    long long dayOfEra =
        yearOfEra * 365 +
        yearOfEra / 4 -
        yearOfEra / 100 +
        dayOfYear;

    dayNumber = era * 146097 + dayOfEra;

    return true;
}


// Convert day number back into YYYY-MM-DD

static string formatDate(long long dayNumber) {

    long long era = dayNumber / 146097;
    long long dayOfEra = dayNumber - era * 146097;

    long long yearOfEra =
        (dayOfEra -
         dayOfEra / 1460 +
         dayOfEra / 36524 -
         dayOfEra / 146096) / 365;

    long long year = yearOfEra + era * 400;

    long long dayOfYear =
        dayOfEra -
        (365 * yearOfEra +
         yearOfEra / 4 -
         yearOfEra / 100);

    long long monthPart = (5 * dayOfYear + 2) / 153;

    int day = static_cast<int>(
        dayOfYear - (153 * monthPart + 2) / 5 + 1
    );

    int month = static_cast<int>(
        monthPart + (monthPart < 10 ? 3 : -9)
    );

    year += (month <= 2);

    string result =
        (year < 10 ? "000" :
         year < 100 ? "00" :
         year < 1000 ? "0" : "") +
        to_string(year);

    if (month < 10) {
        result += "-0" + to_string(month);
    }
    else {
        result += "-" + to_string(month);
    }

    if (day < 10) {
        result += "-0" + to_string(day);
    }
    else {
        result += "-" + to_string(day);
    }

    return result;
}


// ==========================================
// SLIDING WINDOW
// ==========================================

SpendingWindowResult findHighestSpendingWindow(
    const vector<Expense>& expenses,
    int windowDays
) {

    SpendingWindowResult result;

    result.found = false;
    result.startDate = "";
    result.endDate = "";
    result.totalPaise = 0;

    if (expenses.empty() || windowDays <= 0) {
        return result;
    }

    // Aggregate all expenses by calendar day.
    map<long long, long long> dailyTotals;

    for (const Expense& expense : expenses) {

        long long dayNumber;

        if (!parseDate(expense.date, dayNumber)) {
            return result;
        }

        if (expense.amountPaise < 0 ||
            dailyTotals[dayNumber] >
                numeric_limits<long long>::max() -
                expense.amountPaise) {
            return result;
        }

        dailyTotals[dayNumber] += expense.amountPaise;
    }

    vector<pair<long long, long long> > dailyData;

    for (const auto& entry : dailyTotals) {
        dailyData.push_back(entry);
    }

    // Sliding window over occupied days.
    size_t left = 0;
    size_t right = 0;

    long long currentTotal = 0;
    long long highestTotal = -1;

    for (left = 0; left < dailyData.size(); left++) {

        // Remove days that are outside the current window.
        while (right < dailyData.size() &&
               dailyData[right].first -
               dailyData[left].first < windowDays) {

            if (currentTotal >
                numeric_limits<long long>::max() -
                dailyData[right].second) {
                return result;
            }

            currentTotal += dailyData[right].second;
            right++;
        }

        if (currentTotal > highestTotal) {

            highestTotal = currentTotal;

            result.found = true;
            result.startDate =
                formatDate(dailyData[left].first);

            result.endDate =
                formatDate(
                    dailyData[left].first + windowDays - 1
                );

            result.totalPaise = currentTotal;
        }

        // Remove the leftmost day before moving the window.
        currentTotal -= dailyData[left].second;
    }

    return result;
}