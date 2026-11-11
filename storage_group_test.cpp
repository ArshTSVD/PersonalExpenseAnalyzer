
#include <iostream>
#include "storage/storage_manager.hpp"

using namespace std;

int main() {

    vector<GroupExpense> expenses;

    GroupExpense e;
    e.id = "GEXP001";
    e.groupId = "GRP001";
    e.description = "Goa Hotel";
    e.category = "Travel";
    e.paidByUserId = "USR001";
    e.amountPaise = 10000;
    e.date = "2026-11-10";
    e.splitType = SplitType::EQUAL;

    e.splits.push_back({"USR001", 5000});
    e.splits.push_back({"USR002", 5000});

    expenses.push_back(e);

    vector<Settlement> settlements;

    Settlement s;
    s.id = "SET0001";
    s.groupId = "GRP001";
    s.fromUserId = "USR002";
    s.toUserId = "USR001";
    s.amountPaise = 5000;
    s.date = "2026-11-10";
    s.note = "Hotel payment";

    settlements.push_back(s);

    if (!saveGroupExpenses(expenses, "storage/group_expenses_test.txt")) {
        cout << "Group expense save failed\n";
        return 1;
    }

    if (!saveSettlements(settlements, "storage/settlements_test.txt")) {
        cout << "Settlement save failed\n";
        return 1;
    }

    vector<GroupExpense> loadedExpenses;
    vector<Settlement> loadedSettlements;

    if (!loadGroupExpenses(loadedExpenses, "storage/group_expenses_test.txt")) {
        cout << "Group expense load failed\n";
        return 1;
    }

    if (!loadSettlements(loadedSettlements, "storage/settlements_test.txt")) {
        cout << "Settlement load failed\n";
        return 1;
    }

    cout << "Group expenses loaded: " << loadedExpenses.size() << "\n";
    cout << "Description: " << loadedExpenses[0].description << "\n";
    cout << "Splits loaded: " << loadedExpenses[0].splits.size() << "\n";

    cout << "\nSettlements loaded: " << loadedSettlements.size() << "\n";
    cout << "Settlement note: " << loadedSettlements[0].note << "\n";

    cout << "\nAll persistence tests passed!\n";

    return 0;
}
