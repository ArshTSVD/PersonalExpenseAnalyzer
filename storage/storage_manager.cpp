
#include "storage_manager.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;


// ==========================================
// SAVE EXPENSES
// ==========================================

bool saveExpenses(
    const vector<Expense>& expenses,
    const string& filePath
) {
    ofstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    for (const Expense& expense : expenses) {

        file << "EXPENSE "
             << quoted(expense.id) << " "
             << quoted(expense.userId) << " "
             << quoted(expense.title) << " "
             << quoted(expense.category) << " "
             << expense.amountPaise << " "
             << quoted(expense.date)
             << "\n";
    }

    return file.good();
}


// ==========================================
// LOAD EXPENSES
// ==========================================

bool loadExpenses(
    vector<Expense>& expenses,
    const string& filePath
) {
    ifstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    vector<Expense> loadedExpenses;

    string recordType;

    while (file >> recordType) {

        if (recordType != "EXPENSE") {
            return false;
        }

        Expense expense;

        if (!(file >> quoted(expense.id)
                  >> quoted(expense.userId)
                  >> quoted(expense.title)
                  >> quoted(expense.category)
                  >> expense.amountPaise
                  >> quoted(expense.date))) {

            return false;
        }

        loadedExpenses.push_back(expense);
    }

    if (file.bad()) {
        return false;
    }

    // Replace existing data only after successful loading
    expenses = loadedExpenses;

    return true;
}


// ==========================================
// SAVE BUDGETS
// ==========================================

bool saveBudgets(
    const vector<Budget>& budgets,
    const string& filePath
) {
    ofstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    for (const Budget& budget : budgets) {

        file << "BUDGET "
             << quoted(budget.userId) << " "
             << quoted(budget.category) << " "
             << budget.limitPaise << " "
             << static_cast<int>(budget.period)
             << "\n";
    }

    return file.good();
}


// ==========================================
// LOAD BUDGETS
// ==========================================

bool loadBudgets(
    vector<Budget>& budgets,
    const string& filePath
) {
    ifstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    vector<Budget> loadedBudgets;

    string recordType;

    while (file >> recordType) {

        if (recordType != "BUDGET") {
            return false;
        }

        Budget budget;
        int periodValue;

        if (!(file >> quoted(budget.userId)
                  >> quoted(budget.category)
                  >> budget.limitPaise
                  >> periodValue)) {

            return false;
        }

        // Validate budget period
        if (periodValue < 0 || periodValue > 2) {
            return false;
        }

        budget.period =
            static_cast<BudgetPeriod>(periodValue);

        loadedBudgets.push_back(budget);
    }

    if (file.bad()) {
        return false;
    }

    budgets = loadedBudgets;

    return true;
}


// ==========================================
// SAVE USERS
// ==========================================

bool saveUsers(
    const vector<User>& users,
    const string& filePath
) {
    ofstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    for (const User& user : users) {
        file << "USER "
             << quoted(user.id) << " "
             << quoted(user.name)
             << "\n";
    }

    return file.good();
}


// ==========================================
// LOAD USERS
// ==========================================

bool loadUsers(
    vector<User>& users,
    const string& filePath
) {
    ifstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    vector<User> loadedUsers;
    string recordType;

    while (file >> recordType) {

        if (recordType != "USER") {
            return false;
        }

        User user;

        if (!(file >> quoted(user.id)
                  >> quoted(user.name))) {
            return false;
        }

        if (user.id.empty() || user.name.empty()) {
            return false;
        }

        loadedUsers.push_back(user);
    }

    if (file.bad()) {
        return false;
    }

    users = loadedUsers;
    return true;
}


// ==========================================
// SAVE GROUPS
// ==========================================

bool saveGroups(
    const vector<Group>& groups,
    const string& filePath
) {
    ofstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    for (const Group& group : groups) {

        file << "GROUP "
             << quoted(group.id) << " "
             << quoted(group.name) << " "
             << quoted(group.createdBy) << " "
             << quoted(group.groupCode) << " "
             << group.memberIds.size();

        for (const string& memberId : group.memberIds) {
            file << " " << quoted(memberId);
        }

        file << "\n";
    }

    return file.good();
}


// ==========================================
// LOAD GROUPS
// ==========================================

bool loadGroups(
    vector<Group>& groups,
    const string& filePath
) {
    ifstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    vector<Group> loadedGroups;
    string recordType;

    while (file >> recordType) {

        if (recordType != "GROUP") {
            return false;
        }

        Group group;
        size_t memberCount;

        if (!(file >> quoted(group.id)
                  >> quoted(group.name)
                  >> quoted(group.createdBy)
                  >> quoted(group.groupCode)
                  >> memberCount)) {
            return false;
        }

        // Basic validation
        if (group.id.empty() ||
            group.name.empty() ||
            memberCount > 10000) {
            return false;
        }

        for (size_t i = 0; i < memberCount; i++) {

            string memberId;

            if (!(file >> quoted(memberId))) {
                return false;
            }

            if (memberId.empty()) {
                return false;
            }

            group.memberIds.push_back(memberId);
        }

        loadedGroups.push_back(group);
    }

    if (file.bad()) {
        return false;
    }

    groups = loadedGroups;
    return true;
}


// ==========================================
// SAVE GROUP EXPENSES
// ==========================================

bool saveGroupExpenses(
    const vector<GroupExpense>& expenses,
    const string& filePath
) {
    ofstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    for (const GroupExpense& expense : expenses) {

        file << "GROUP_EXPENSE "
             << quoted(expense.id) << " "
             << quoted(expense.groupId) << " "
             << quoted(expense.description) << " "
             << quoted(expense.category) << " "
             << quoted(expense.paidByUserId) << " "
             << expense.amountPaise << " "
             << quoted(expense.date) << " "
             << static_cast<int>(expense.splitType) << " "
             << expense.splits.size();

        for (const Split& split : expense.splits) {
            file << " "
                 << quoted(split.userId) << " "
                 << split.sharePaise;
        }

        file << "\n";
    }

    return file.good();
}


// ==========================================
// LOAD GROUP EXPENSES
// ==========================================

bool loadGroupExpenses(
    vector<GroupExpense>& expenses,
    const string& filePath
) {
    ifstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    vector<GroupExpense> loadedExpenses;
    string recordType;

    while (file >> recordType) {

        if (recordType != "GROUP_EXPENSE") {
            return false;
        }

        GroupExpense expense;
        int splitTypeValue;
        size_t splitCount;

        if (!(file >> quoted(expense.id)
                  >> quoted(expense.groupId)
                  >> quoted(expense.description)
                  >> quoted(expense.category)
                  >> quoted(expense.paidByUserId)
                  >> expense.amountPaise
                  >> quoted(expense.date)
                  >> splitTypeValue
                  >> splitCount)) {
            return false;
        }

        // Validate split type and basic fields
        if (expense.id.empty() ||
            expense.groupId.empty() ||
            expense.amountPaise <= 0 ||
            splitTypeValue < 0 ||
            splitTypeValue > 2 ||
            splitCount > 10000) {
            return false;
        }

        expense.splitType =
            static_cast<SplitType>(splitTypeValue);

        long long totalSplitPaise = 0;

        for (size_t i = 0; i < splitCount; i++) {

            Split split;

            if (!(file >> quoted(split.userId)
                      >> split.sharePaise)) {
                return false;
            }

            if (split.userId.empty() ||
                split.sharePaise < 0 ||
                totalSplitPaise >
                    expense.amountPaise - split.sharePaise) {
                return false;
            }

            totalSplitPaise += split.sharePaise;
            expense.splits.push_back(split);
        }

        if (totalSplitPaise != expense.amountPaise) {
            return false;
        }

        loadedExpenses.push_back(expense);
    }

    if (file.bad()) {
        return false;
    }

    expenses = loadedExpenses;
    return true;
}


// ==========================================
// SAVE SETTLEMENTS
// ==========================================

bool saveSettlements(
    const vector<Settlement>& settlements,
    const string& filePath
) {
    ofstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    for (const Settlement& settlement : settlements) {

        file << "SETTLEMENT "
             << quoted(settlement.id) << " "
             << quoted(settlement.groupId) << " "
             << quoted(settlement.fromUserId) << " "
             << quoted(settlement.toUserId) << " "
             << settlement.amountPaise << " "
             << quoted(settlement.date) << " "
             << quoted(settlement.note)
             << "\n";
    }

    return file.good();
}


// ==========================================
// LOAD SETTLEMENTS
// ==========================================

bool loadSettlements(
    vector<Settlement>& settlements,
    const string& filePath
) {
    ifstream file(filePath);

    if (!file.is_open()) {
        return false;
    }

    vector<Settlement> loadedSettlements;
    string recordType;

    while (file >> recordType) {

        if (recordType != "SETTLEMENT") {
            return false;
        }

        Settlement settlement;

        if (!(file >> quoted(settlement.id)
                  >> quoted(settlement.groupId)
                  >> quoted(settlement.fromUserId)
                  >> quoted(settlement.toUserId)
                  >> settlement.amountPaise
                  >> quoted(settlement.date)
                  >> quoted(settlement.note))) {
            return false;
        }

        if (settlement.id.empty() ||
            settlement.groupId.empty() ||
            settlement.fromUserId.empty() ||
            settlement.toUserId.empty() ||
            settlement.fromUserId == settlement.toUserId ||
            settlement.amountPaise <= 0) {
            return false;
        }

        loadedSettlements.push_back(settlement);
    }

    if (file.bad()) {
        return false;
    }

    settlements = loadedSettlements;
    return true;
}

