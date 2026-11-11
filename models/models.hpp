
#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

// 1. User model
struct User {
    string id;
    string name;
};

// 2. Personal expense model
struct Expense {
    string id;
    string userId;

    string title;
    string category;

    long long amountPaise;

    string date;       // Format: YYYY-MM-DD
    string note;
};

// 3. Group model
struct Group {
    string id;
    string name;
    string createdBy;

    string groupCode;

    vector<string> memberIds;
};

// 4. Split type
enum class SplitType {
    EQUAL,
    SELECTED_MEMBERS,
    EXACT_AMOUNT
};

// 5. Individual member's share
struct Split {
    string userId;
    long long sharePaise;
};

// 6. Group expense model
struct GroupExpense {
    string id;
    string groupId;

    string description;
    string category;

    string paidByUserId;
    long long amountPaise;

    string date;

    SplitType splitType;
    vector<Split> splits;
};

// 7. Settlement model
struct Settlement {
    string id;
    string groupId;

    string fromUserId;
    string toUserId;

    long long amountPaise;

    string date;
    string note;
};

// 8. Reminder period
enum class ReminderPeriod {
    DAILY,
    WEEKLY,
    MONTHLY
};

// 9. Spending reminder model
struct Reminder {
    string id;
    string userId;

    string category;
    long long limitPaise;

    ReminderPeriod period;
    bool isActive;
};
