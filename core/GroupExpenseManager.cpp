
#include "GroupExpenseManager.h"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stdexcept>

using namespace std;


// ==========================================
// CHECK WHETHER USER IS A GROUP MEMBER
// ==========================================

static bool isGroupMember(
    const Group& group,
    const string& userId
) {
    return find(
        group.memberIds.begin(),
        group.memberIds.end(),
        userId
    ) != group.memberIds.end();
}


// ==========================================
// VALIDATE GROUP EXPENSE
// ==========================================

static bool validateGroupExpense(
    const vector<Group>& groups,
    const GroupExpense& expense
) {

    if (expense.groupId.empty() ||
        expense.paidByUserId.empty() ||
        expense.description.empty() ||
        expense.amountPaise <= 0 ||
        expense.date.empty() ||
        expense.splits.empty()) {

        return false;
    }

    // Find the associated group
    const Group* selectedGroup = nullptr;

    for (const Group& group : groups) {

        if (group.id == expense.groupId) {
            selectedGroup = &group;
            break;
        }
    }

    if (selectedGroup == nullptr) {
        return false;
    }

    // Payer must belong to the group
    if (!isGroupMember(*selectedGroup, expense.paidByUserId)) {
        return false;
    }

    // Check participant IDs and duplicates
    vector<string> participantIds;

    long long totalShares = 0;

    for (const Split& split : expense.splits) {

        if (!isGroupMember(*selectedGroup, split.userId)) {
            return false;
        }

        if (find(
            participantIds.begin(),
            participantIds.end(),
            split.userId
        ) != participantIds.end()) {

            return false;
        }

        participantIds.push_back(split.userId);

        if (expense.splitType == SplitType::EXACT_AMOUNT) {

            if (split.sharePaise <= 0) {
                return false;
            }

            totalShares += split.sharePaise;
        }
    }

    // Exact amounts must add up to the expense total
    if (expense.splitType == SplitType::EXACT_AMOUNT &&
        totalShares != expense.amountPaise) {

        return false;
    }

    return true;
}


// ==========================================
// GENERATE UNIQUE EXPENSE ID
// ==========================================

string generateGroupExpenseId(
    const vector<GroupExpense>& expenses
) {

    int nextNumber = 1;

    for (const GroupExpense& expense : expenses) {

        if (expense.id.size() > 4 &&
            expense.id.substr(0, 4) == "GEXP") {

            try {

                int number = stoi(expense.id.substr(4));

                if (number >= nextNumber) {
                    nextNumber = number + 1;
                }
            }
            catch (...) {
                // Ignore malformed IDs
            }
        }
    }

    stringstream idStream;

    idStream << "GEXP"
             << setfill('0')
             << setw(4)
             << nextNumber;

    return idStream.str();
}


// ==========================================
// ADD GROUP EXPENSE
// ==========================================

bool addGroupExpense(
    vector<GroupExpense>& expenses,
    const vector<Group>& groups,
    GroupExpense& expense
) {

    if (!validateGroupExpense(groups, expense)) {
        return false;
    }

    expense.id = generateGroupExpenseId(expenses);

    expenses.push_back(expense);

    return true;
}


// ==========================================
// FIND GROUP EXPENSE
// ==========================================

const GroupExpense* findGroupExpenseById(
    const vector<GroupExpense>& expenses,
    const string& expenseId
) {

    for (const GroupExpense& expense : expenses) {

        if (expense.id == expenseId) {
            return &expense;
        }
    }

    return nullptr;
}


// ==========================================
// UPDATE GROUP EXPENSE
// ==========================================

bool updateGroupExpense(
    vector<GroupExpense>& expenses,
    const vector<Group>& groups,
    const string& expenseId,
    const GroupExpense& updatedExpense
) {

    for (GroupExpense& expense : expenses) {

        if (expense.id == expenseId) {

            GroupExpense candidate = updatedExpense;

            // Preserve the original ID
            candidate.id = expenseId;

            if (!validateGroupExpense(groups, candidate)) {
                return false;
            }

            expense = candidate;

            return true;
        }
    }

    return false;
}


// ==========================================
// DELETE GROUP EXPENSE
// ==========================================

bool deleteGroupExpense(
    vector<GroupExpense>& expenses,
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


// ==========================================
// GET EXPENSES OF A GROUP
// ==========================================

vector<const GroupExpense*> getGroupExpenses(
    const vector<GroupExpense>& expenses,
    const string& groupId
) {

    vector<const GroupExpense*> result;

    for (const GroupExpense& expense : expenses) {

        if (expense.groupId == groupId) {
            result.push_back(&expense);
        }
    }

    return result;
}
