
#include "BalanceEngine.h"

#include <iostream>
#include <iomanip>

using namespace std;


// ==========================================
// CALCULATE GROUP BALANCES
// ==========================================

map<string, long long> calculateGroupBalances(
    const vector<Group>& groups,
    const vector<GroupExpense>& expenses,
    const string& groupId
) {

    map<string, long long> balances;

    // Find the requested group
    const Group* selectedGroup = nullptr;

    for (const Group& group : groups) {

        if (group.id == groupId) {
            selectedGroup = &group;
            break;
        }
    }

    // Return empty result if group does not exist
    if (selectedGroup == nullptr) {
        return balances;
    }

    // Initialize every member's balance to zero
    for (const string& memberId : selectedGroup->memberIds) {
        balances[memberId] = 0;
    }

    // Process all expenses belonging to this group
    for (const GroupExpense& expense : expenses) {

        if (expense.groupId != groupId) {
            continue;
        }

        // Amount paid by the payer is a credit
        if (balances.find(expense.paidByUserId) == balances.end()) {
            return {};
        }

        balances[expense.paidByUserId] += expense.amountPaise;

        // Each participant's share is a debit
        for (const Split& split : expense.splits) {

            if (balances.find(split.userId) == balances.end()) {
                return {};
            }

            balances[split.userId] -= split.sharePaise;
        }
    }

    return balances;
}


// ==========================================
// VALIDATE BALANCES
// ==========================================

bool validateGroupBalances(
    const map<string, long long>& balances
) {

    if (balances.empty()) {
    return false;
}
    long long totalBalance = 0;

    for (const auto& entry : balances) {
        totalBalance += entry.second;
    }

    return totalBalance == 0;
}


// ==========================================
// DISPLAY GROUP BALANCES
// ==========================================

void displayGroupBalances(
    const vector<User>& users,
    const map<string, long long>& balances
) {

    cout << "\n===== GROUP BALANCES =====\n";

    for (const auto& entry : balances) {

        string memberName = entry.first;

        // Find the member's actual name
        for (const User& user : users) {

            if (user.id == entry.first) {
                memberName = user.name;
                break;
            }
        }

        cout << memberName << " : ";

        if (entry.second > 0) {

            cout << "Should receive Rs. ";

        }
        else if (entry.second < 0) {

            cout << "Should pay Rs. ";

        }
        else {

            cout << "Settled (Rs. 0.00)\n";
            continue;
        }

        cout << fixed << setprecision(2)
             << (entry.second < 0
                    ? -entry.second
                    : entry.second) / 100.0
             << "\n";
    }

    cout << "\nBalance validation: "
         << (validateGroupBalances(balances)
                ? "PASSED"
                : "FAILED")
         << "\n";
}
