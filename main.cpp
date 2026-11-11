#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <climits>
#include <limits>
#include <map>
#include <algorithm>

#include "core/GroupManager.h"
#include "core/expense_manager.hpp"
#include "storage/ExpenseStorage.h"
#include "core/GroupExpenseManager.h"
#include "core/SplitEngine.h"
#include "core/BalanceEngine.h"
#include "core/SettlementEngine.h"
#include "core/SettlementManager.h"
#include "core/budget_manager.hpp"
#include "dsa/queue/ReminderQueue.hpp"
#include "dsa/linkedlist/ActivityList.hpp"
#include "dsa/stack/UndoStack.hpp"
#include "core/ExpenseAlgorithms.h"
#include "storage/storage_manager.hpp"

using namespace std;

const string DATA_FILE = "storage/expenses.csv";
const string CURRENT_USER_ID = "USR001";

bool saveAllData(
    const vector<Expense> &expenses,
    const vector<User> &users,
    const vector<Group> &groups,
    const vector<GroupExpense> &groupExpenses,
    const vector<Settlement> &settlements,
    const vector<Budget> &budgets)
{
    bool success = true;

    if (!saveExpensesToFile(expenses, DATA_FILE))
        success = false;

    if (!saveUsers(users, "storage/users.txt"))
        success = false;

    if (!saveGroups(groups, "storage/groups.txt"))
        success = false;

    if (!saveGroupExpenses(groupExpenses, "storage/group_expenses.txt"))
        success = false;

    if (!saveSettlements(settlements, "storage/settlements.txt"))
        success = false;

    if (!saveBudgets(budgets, "storage/budgets.txt"))
        success = false;

    return success;
}

void viewSettlementPlan(
    const vector<Group> &groups,
    const vector<GroupExpense> &groupExpenses,
    const vector<User> &users);

void settlementManagementMenu(
    const vector<Group> &groups,
    const vector<GroupExpense> &groupExpenses,
    const vector<User> &users,
    vector<Settlement> &settlements);

void viewActivityHistory(ActivityList &activityList);
void undoLastExpenseOperation(
    vector<Expense> &expenses,
    UndoStack &undoStack,
    ActivityList &activityList);

void displaySortedExpenses(const vector<Expense> &expenses);

void expenseSortingMenu(const vector<Expense> &expenses);

void viewHighestSpendingWindow(const vector<Expense> &expenses);

void viewAdvancedCategoryAnalytics(const vector<Expense> &expenses);
// ==========================================
// DISPLAY MONEY
// ==========================================

void displayMoney(long long paise)
{

    cout << "Rs. "
         << paise / 100
         << "."
         << setw(2)
         << setfill('0')
         << paise % 100
         << setfill(' ');
}

// ==========================================
// INPUT MONEY
// ==========================================

bool convertToPaise(const string &input, long long &paise)
{

    size_t dot = input.find('.');

    string rupees = input.substr(0, dot);
    string fraction = "";

    if (dot != string::npos)
    {
        fraction = input.substr(dot + 1);
    }

    if (rupees.empty())
    {
        return false;
    }

    if (fraction.size() > 2)
    {
        return false;
    }

    try
    {

        for (char c : rupees)
        {
            if (c < '0' || c > '9')
            {
                return false;
            }
        }

        for (char c : fraction)
        {
            if (c < '0' || c > '9')
            {
                return false;
            }
        }

        long long whole = stoll(rupees);

        while (fraction.size() < 2)
        {
            fraction += "0";
        }

        long long decimal = fraction.empty()
                                ? 0
                                : stoll(fraction);

        if (whole >
            (LLONG_MAX - decimal) / 100)
        {
            return false;
        }

        paise = whole * 100 + decimal;

        return paise > 0;
    }
    catch (...)
    {
        return false;
    }
}

// ==========================================
// DISPLAY ONE EXPENSE
// ==========================================

void displayExpense(const Expense &e)
{

    cout << "\nExpense ID: " << e.id
         << "\nTitle: " << e.title
         << "\nCategory: " << e.category
         << "\nAmount: ";

    displayMoney(e.amountPaise);

    cout << "\nDate: " << e.date
         << "\nNote: " << e.note
         << "\n----------------------------\n";
}

// ==========================================
// VIEW ALL EXPENSES
// ==========================================

void viewExpenses(const vector<Expense> &expenses)
{

    cout << "\n========== ALL EXPENSES ==========\n";

    if (expenses.empty())
    {
        cout << "No expenses available.\n";
        return;
    }

    for (const Expense &e : expenses)
    {
        displayExpense(e);
    }
}

// ==========================================
// ADD EXPENSE
// ==========================================

void addNewExpense(
    vector<Expense> &expenses,
    ActivityList &activityList,
    UndoStack &undoStack)
{
    Expense e;
    e.userId = CURRENT_USER_ID;

    string amountInput;

    cout << "\nEnter expense title: ";
    getline(cin, e.title);

    cout << "Enter category: ";
    getline(cin, e.category);

    cout << "Enter amount in rupees: ";
    getline(cin, amountInput);

    if (!convertToPaise(amountInput, e.amountPaise))
    {
        cout << "Invalid amount.\n";
        return;
    }

    cout << "Enter date (YYYY-MM-DD): ";
    getline(cin, e.date);

    cout << "Enter note: ";
    getline(cin, e.note);

    string id = addExpense(expenses, e);

    if (id.empty())
    {
        cout << "Could not add expense.\n";
        return;
    }

    // Find the newly created expense and save its complete snapshot
    const Expense *added = findExpenseById(expenses, id);

    if (added != nullptr)
    {
        UndoAction action;
        action.operation = UndoOperation::ADD_EXPENSE;
        action.expenseData = *added;

        undoStack.push(action);
    }

    cout << "\nExpense added successfully!\n";
    cout << "Generated ID: " << id << "\n";

    activityList.addActivity(
        "Added expense: " + e.title + " (" + id + ")",
        e.date);

    if (!saveExpensesToFile(expenses, DATA_FILE))
        cout << "Warning: Could not save data to file.\n";
}

// ==========================================
// UPDATE EXPENSE
// ==========================================

void updateExistingExpense(
    vector<Expense> &expenses,
    ActivityList &activityList,
    UndoStack &undoStack)
{
    string id;

    cout << "\nEnter expense ID to update: ";
    getline(cin, id);

    const Expense *existing = findExpenseById(expenses, id);

    if (existing == nullptr)
    {
        cout << "Expense not found.\n";
        return;
    }

    // Preserve the original version before modification
    Expense previousExpense = *existing;

    cout << "\nCurrent expense:";
    displayExpense(previousExpense);

    Expense updated;
    updated.userId = previousExpense.userId;

    string amountInput;

    cout << "Enter new title: ";
    getline(cin, updated.title);

    cout << "Enter new category: ";
    getline(cin, updated.category);

    cout << "Enter new amount: ";
    getline(cin, amountInput);

    if (!convertToPaise(amountInput, updated.amountPaise))
    {
        cout << "Invalid amount.\n";
        return;
    }

    cout << "Enter new date (YYYY-MM-DD): ";
    getline(cin, updated.date);

    cout << "Enter new note: ";
    getline(cin, updated.note);

    if (updateExpense(expenses, id, updated))
    {
        // Save the previous version for undo
        UndoAction action;
        action.operation = UndoOperation::UPDATE_EXPENSE;
        action.expenseData = previousExpense;

        undoStack.push(action);

        cout << "Expense updated successfully.\n";

        activityList.addActivity(
            "Updated expense: " + updated.title + " (" + id + ")",
            updated.date);

        if (!saveExpensesToFile(expenses, DATA_FILE))
            cout << "Warning: Could not save data.\n";
    }
    else
    {
        cout << "Update failed.\n";
    }
}

// ==========================================
// DELETE EXPENSE
// ==========================================

void deleteExistingExpense(
    vector<Expense> &expenses,
    ActivityList &activityList,
    UndoStack &undoStack)
{
    string id;

    cout << "\nEnter expense ID to delete: ";
    getline(cin, id);

    const Expense *existing = findExpenseById(expenses, id);

    if (existing == nullptr)
    {
        cout << "Expense not found.\n";
        return;
    }

    // Preserve complete expense before deletion
    Expense deletedExpense = *existing;

    if (deleteExpense(expenses, id))
    {
        UndoAction action;
        action.operation = UndoOperation::DELETE_EXPENSE;
        action.expenseData = deletedExpense;

        undoStack.push(action);

        cout << "Expense deleted successfully.\n";

        activityList.addActivity(
            "Deleted expense: " + deletedExpense.title + " (" + id + ")",
            deletedExpense.date);

        if (!saveExpensesToFile(expenses, DATA_FILE))
            cout << "Warning: Could not save data.\n";
    }
    else
    {
        cout << "Expense deletion failed.\n";
    }
}

// ==========================================
// SEARCH EXPENSE
// ==========================================

void searchExpense(
    const vector<Expense> &expenses)
{

    string keyword;

    cout << "\nEnter title or keyword: ";
    getline(cin, keyword);

    vector<Expense> results =
        searchExpensesByTitle(expenses, keyword);

    cout << "\nSearch results: "
         << results.size()
         << "\n";

    viewExpenses(results);
}

// ==========================================
// EXPENSE ANALYTICS
// ==========================================

void showAnalytics(
    const vector<Expense> &expenses)
{

    ExpenseAnalytics analytics =
        calculateExpenseAnalytics(expenses);

    cout << "\n========== EXPENSE ANALYTICS ==========\n";

    if (!analytics.hasExpenses)
    {
        cout << "No expenses available for analysis.\n";
        return;
    }

    cout << "Total transactions: "
         << analytics.totalExpenseCount << "\n";

    cout << "Total spending: ";
    displayMoney(analytics.totalSpendingPaise);

    cout << "\nAverage expense: ";
    displayMoney(analytics.averageExpensePaise);

    cout << "\n\nHighest expense:";
    displayExpense(analytics.highestExpense);

    cout << "\nLowest expense:";
    displayExpense(analytics.lowestExpense);
}

// ==========================================
// GENERATE USER ID
// ==========================================

string generateUserId(const vector<User> &users)
{

    int nextNumber = 1;

    for (const User &user : users)
    {

        if (user.id.size() > 3 &&
            user.id.substr(0, 3) == "USR")
        {

            try
            {
                int number = stoi(user.id.substr(3));

                if (number >= nextNumber)
                {
                    nextNumber = number + 1;
                }
            }
            catch (...)
            {
                // Ignore invalid IDs
            }
        }
    }

    stringstream ss;
    ss << "USR" << setw(4) << setfill('0') << nextNumber;

    return ss.str();
}

// ==========================================
// DISPLAY GROUPS
// ==========================================

void displayGroups(const vector<Group> &groups)
{

    cout << "\n========== YOUR GROUPS ==========\n";

    if (groups.empty())
    {
        cout << "No groups created yet.\n";
        return;
    }

    for (const Group &group : groups)
    {

        cout << "\nGroup ID: " << group.id;
        cout << "\nName: " << group.name;
        cout << "\nJoining Code: " << group.groupCode;
        cout << "\nMembers: " << group.memberIds.size();

        cout << "\nMember IDs: ";

        for (const string &id : group.memberIds)
        {
            cout << id << " ";
        }

        cout << "\n-------------------------\n";
    }
}

string getUserName(
    const vector<User> &users,
    const string &userId)
{
    for (const User &user : users)
    {
        if (user.id == userId)
        {
            return user.name;
        }
    }

    return "Unknown User";
}

void createNewGroupExpense(
    vector<GroupExpense> &groupExpenses,
    const vector<Group> &groups,
    const vector<User> &users)
{
    string groupId;

    cout << "\nEnter Group ID: ";
    getline(cin, groupId);

    const Group *group = findGroupById(groups, groupId);

    if (group == nullptr)
    {
        cout << "Group not found.\n";
        return;
    }

    if (group->memberIds.empty())
    {
        cout << "This group has no members.\n";
        return;
    }

    GroupExpense expense;
    expense.groupId = groupId;

    cout << "\nEnter expense description: ";
    getline(cin, expense.description);

    if (expense.description.empty())
    {
        cout << "Description cannot be empty.\n";
        return;
    }

    cout << "Enter category: ";
    getline(cin, expense.category);

    string amountInput;

    cout << "Enter total amount in rupees: ";
    getline(cin, amountInput);

    if (!convertToPaise(amountInput, expense.amountPaise))
    {
        cout << "Invalid amount.\n";
        return;
    }

    cout << "Enter date (YYYY-MM-DD): ";
    getline(cin, expense.date);

    cout << "\nGroup members:\n";

    for (const string &memberId : group->memberIds)
    {
        cout << memberId << " - "
             << getUserName(users, memberId) << "\n";
    }

    cout << "\nEnter payer's User ID: ";
    getline(cin, expense.paidByUserId);

    bool payerFound = false;

    for (const string &memberId : group->memberIds)
    {
        if (memberId == expense.paidByUserId)
        {
            payerFound = true;
            break;
        }
    }

    if (!payerFound)
    {
        cout << "Payer must be a member of this group.\n";
        return;
    }

    cout << "\nChoose split type:\n";
    cout << "1. Equal split among all group members\n";
    cout << "2. Equal split among selected members\n";
    cout << "3. Exact amount for each member\n";

    cout << "Enter choice: ";

    string splitChoiceInput;
    getline(cin, splitChoiceInput);

    int splitChoice;

    try
    {
        size_t processed = 0;
        splitChoice = stoi(splitChoiceInput, &processed);

        if (processed != splitChoiceInput.size())
        {
            cout << "Invalid split choice.\n";
            return;
        }
    }
    catch (...)
    {
        cout << "Invalid split choice.\n";
        return;
    }

    // ======================================
    // EQUAL SPLIT
    // ======================================

    if (splitChoice == 1)
    {

        expense.splitType = SplitType::EQUAL;

        for (const string &memberId : group->memberIds)
        {

            Split split;
            split.userId = memberId;
            split.sharePaise = 0;

            expense.splits.push_back(split);
        }
    }

    // ======================================
    // SELECTED MEMBERS SPLIT
    // ======================================

    else if (splitChoice == 2)
    {

        expense.splitType = SplitType::SELECTED_MEMBERS;

        cout << "\nEnter number of participants: ";

        string countInput;
        getline(cin, countInput);

        int count;

        try
        {
            size_t processed = 0;
            count = stoi(countInput, &processed);

            if (processed != countInput.size() ||
                count <= 0 ||
                count > static_cast<int>(group->memberIds.size()))
            {

                cout << "Invalid participant count.\n";
                return;
            }
        }
        catch (...)
        {
            cout << "Invalid participant count.\n";
            return;
        }

        for (int i = 0; i < count; i++)
        {

            string memberId;

            cout << "Enter participant " << i + 1
                 << " User ID: ";

            getline(cin, memberId);

            bool isMember = false;

            for (const string &id : group->memberIds)
            {
                if (id == memberId)
                {
                    isMember = true;
                    break;
                }
            }

            bool alreadySelected = false;

            for (const Split &split : expense.splits)
            {
                if (split.userId == memberId)
                {
                    alreadySelected = true;
                    break;
                }
            }

            if (!isMember || alreadySelected)
            {
                cout << "Invalid or duplicate member ID.\n";
                return;
            }

            Split split;
            split.userId = memberId;
            split.sharePaise = 0;

            expense.splits.push_back(split);
        }
    }

    // ======================================
    // EXACT AMOUNT SPLIT
    // ======================================

    else if (splitChoice == 3)
    {

        expense.splitType = SplitType::EXACT_AMOUNT;

        cout << "\nEnter number of participants: ";

        string countInput;
        getline(cin, countInput);

        int count;

        try
        {
            size_t processed = 0;
            count = stoi(countInput, &processed);

            if (processed != countInput.size() ||
                count <= 0 ||
                count > static_cast<int>(group->memberIds.size()))
            {

                cout << "Invalid participant count.\n";
                return;
            }
        }
        catch (...)
        {
            cout << "Invalid participant count.\n";
            return;
        }

        for (int i = 0; i < count; i++)
        {

            string memberId;
            string shareInput;
            long long sharePaise;

            cout << "\nEnter participant "
                 << i + 1 << " User ID: ";

            getline(cin, memberId);

            bool isMember = false;

            for (const string &id : group->memberIds)
            {
                if (id == memberId)
                {
                    isMember = true;
                    break;
                }
            }

            bool alreadySelected = false;

            for (const Split &split : expense.splits)
            {
                if (split.userId == memberId)
                {
                    alreadySelected = true;
                    break;
                }
            }

            if (!isMember || alreadySelected)
            {
                cout << "Invalid or duplicate member ID.\n";
                return;
            }

            cout << "Enter this member's share in rupees: ";
            getline(cin, shareInput);

            if (!convertToPaise(shareInput, sharePaise))
            {
                cout << "Invalid share amount.\n";
                return;
            }

            Split split;
            split.userId = memberId;
            split.sharePaise = sharePaise;

            expense.splits.push_back(split);
        }
    }

    else
    {
        cout << "Invalid split type.\n";
        return;
    }

    // ======================================
    // CALCULATE AND VALIDATE SPLITS
    // ======================================

    if (!calculateExpenseSplits(expense))
    {
        cout << "\nSplit calculation failed.\n";
        cout << "Please verify participants and amounts.\n";
        return;
    }

    // ======================================
    // SAVE GROUP EXPENSE IN MEMORY
    // ======================================

    if (addGroupExpense(
            groupExpenses,
            groups,
            expense))
    {

        cout << "\nGroup expense created successfully!\n";
        cout << "Generated Expense ID: "
             << expense.id << "\n";

        cout << "\nCalculated shares:\n";

        for (const Split &split : expense.splits)
        {

            cout << getUserName(users, split.userId)
                 << ": ";

            displayMoney(split.sharePaise);
            cout << "\n";
        }
    }
    else
    {
        cout << "Could not create group expense.\n";
    }
}

void displayGroupExpenses(
    const vector<GroupExpense> &expenses,
    const vector<User> &users,
    const string &groupId)
{
    vector<const GroupExpense *> results =
        getGroupExpenses(expenses, groupId);

    cout << "\n========== GROUP EXPENSES ==========\n";

    if (results.empty())
    {
        cout << "No expenses recorded for this group.\n";
        return;
    }

    for (const GroupExpense *expense : results)
    {

        cout << "\nExpense ID: " << expense->id;
        cout << "\nDescription: " << expense->description;
        cout << "\nCategory: " << expense->category;

        cout << "\nTotal: ";
        displayMoney(expense->amountPaise);

        cout << "\nPaid by: "
             << getUserName(users, expense->paidByUserId);

        cout << "\nDate: " << expense->date;

        cout << "\nSplit type: ";

        switch (expense->splitType)
        {
        case SplitType::EQUAL:
            cout << "Equal";
            break;

        case SplitType::SELECTED_MEMBERS:
            cout << "Selected Members";
            break;

        case SplitType::EXACT_AMOUNT:
            cout << "Exact Amount";
            break;
        }

        cout << "\nMember shares:\n";

        for (const Split &split : expense->splits)
        {

            cout << "  "
                 << getUserName(users, split.userId)
                 << " (" << split.userId << "): ";

            displayMoney(split.sharePaise);
            cout << "\n";
        }

        cout << "----------------------------\n";
    }
}

// ==========================================
// VIEW GROUP BALANCES
// ==========================================

void viewGroupBalances(
    const vector<Group> &groups,
    const vector<GroupExpense> &groupExpenses,
    const vector<User> &users)
{

    string groupId;

    cout << "\nEnter Group ID: ";
    getline(cin, groupId);

    if (findGroupById(groups, groupId) == nullptr)
    {
        cout << "Group not found.\n";
        return;
    }

    map<string, long long> balances =
        calculateGroupBalances(
            groups,
            groupExpenses,
            groupId);

    if (balances.empty())
    {
        cout << "Unable to calculate group balances.\n";
        return;
    }

    cout << "\n========== GROUP BALANCE SUMMARY ==========\n";

    displayGroupBalances(users, balances);
}

// ==========================================
// GROUP MANAGEMENT MENU
// ==========================================

void groupManagementMenu(
    vector<User> &users,
    vector<Group> &groups,
    vector<GroupExpense> &groupExpenses,
    vector<Settlement> &settlements)

{

    int choice;

    do
    {

        cout << "\n========== GROUP MANAGEMENT ==========\n";

        cout << "1. Create Group\n";
        cout << "2. View Groups\n";
        cout << "3. Add Member to Group\n";
        cout << "4. Add Group Expense\n";
        cout << "5. View Group Expenses\n";
        cout << "6. View Group Balances\n";
        cout << "7. Generate Settlement Plan\n";
        cout << "8. Manage Settlements\n";
        cout << "0. Back to Main Menu\n";

        cout << "\nEnter choice: ";

        string input;
        getline(cin, input);

        try
        {
            size_t processed = 0;
            choice = stoi(input, &processed);

            if (processed != input.size())
            {
                choice = -1;
            }
        }
        catch (...)
        {
            choice = -1;
        }

        // CREATE GROUP
        if (choice == 1)
        {

            string groupName;

            cout << "\nEnter group name: ";
            getline(cin, groupName);

            Group createdGroup;

            if (createGroup(
                    groups,
                    users,
                    CURRENT_USER_ID,
                    groupName,
                    createdGroup))
            {

                cout << "\nGroup created successfully!\n";
                cout << "Group ID: " << createdGroup.id << "\n";
                cout << "Joining Code: "
                     << createdGroup.groupCode << "\n";
            }
            else
            {
                cout << "Group creation failed.\n";
            }
        }

        // VIEW GROUPS
        else if (choice == 2)
        {

            displayGroups(groups);
        }

        // ADD MEMBER
        else if (choice == 3)
        {

            string groupId;
            string memberName;

            cout << "\nEnter Group ID: ";
            getline(cin, groupId);

            if (findGroupById(groups, groupId) == nullptr)
            {
                cout << "Group not found.\n";
                continue;
            }

            cout << "Enter new member name: ";
            getline(cin, memberName);

            if (memberName.empty())
            {
                cout << "Member name cannot be empty.\n";
                continue;
            }

            User newUser;

            newUser.id = generateUserId(users);
            newUser.name = memberName;

            users.push_back(newUser);

            if (addMemberToGroup(
                    groups,
                    users,
                    groupId,
                    newUser.id))
            {

                cout << "\nMember added successfully!\n";
                cout << "Member ID: " << newUser.id << "\n";
                cout << "Member Name: " << newUser.name << "\n";
            }
            else
            {

                users.pop_back();

                cout << "Could not add member.\n";
            }
        }
        else if (choice == 4)
        {

            createNewGroupExpense(
                groupExpenses,
                groups,
                users);
        }

        else if (choice == 5)
        {

            string groupId;

            cout << "\nEnter Group ID: ";
            getline(cin, groupId);

            if (findGroupById(groups, groupId) == nullptr)
            {
                cout << "Group not found.\n";
            }
            else
            {
                displayGroupExpenses(
                    groupExpenses,
                    users,
                    groupId);
            }
        }

        else if (choice == 6)
        {

            viewGroupBalances(
                groups,
                groupExpenses,
                users);
        }

        else if (choice == 7)
        {
            viewSettlementPlan(groups, groupExpenses, users);
        }

        else if (choice == 8)
        {
            settlementManagementMenu(
                groups,
                groupExpenses,
                users,
                settlements);
        }

        else if (choice != 0)
        {

            cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}

void viewSettlementPlan(
    const vector<Group> &groups,
    const vector<GroupExpense> &groupExpenses,
    const vector<User> &users)
{
    string groupId;

    cout << "\nEnter Group ID: ";
    getline(cin, groupId);

    const Group *group = findGroupById(groups, groupId);

    if (group == nullptr)
    {
        cout << "Group not found!\n";
        return;
    }

    // Step 1: Calculate current group balances
    map<string, long long> balances =
        calculateGroupBalances(groups, groupExpenses, groupId);

    if (balances.empty())
    {
        cout << "Unable to calculate group balances.\n";
        return;
    }

    // Step 2: Generate settlement transfers
    vector<SettlementTransfer> transfers;

    bool generated =
        generateSettlementPlan(balances, transfers);

    if (!generated)
    {
        cout << "Could not generate settlement plan.\n";
        return;
    }

    // Step 3: Validate the settlement plan
    if (!validateSettlementPlan(balances, transfers))
    {
        cout << "Settlement plan validation failed!\n";
        return;
    }

    // Step 4: Display transfers
    cout << "\n========== SETTLEMENT PLAN ==========\n";

    if (transfers.empty())
    {
        cout << "All group expenses are already settled!\n";
        return;
    }

    for (const SettlementTransfer &transfer : transfers)
    {

        cout << getUserName(users, transfer.fromUserId)
             << " pays "
             << getUserName(users, transfer.toUserId)
             << " : Rs. "
             << fixed << setprecision(2)
             << transfer.amountPaise / 100.0
             << "\n";
    }

    cout << "=====================================\n";
    cout << "Total transactions required: "
         << transfers.size() << "\n";
}

void settlementManagementMenu(
    const vector<Group> &groups,
    const vector<GroupExpense> &groupExpenses,
    const vector<User> &users,
    vector<Settlement> &settlements)
{
    string groupId;

    cout << "\nEnter Group ID: ";
    cin >> groupId;

    const Group *group = findGroupById(groups, groupId);

    if (group == nullptr)
    {
        cout << "Group not found!\n";
        return;
    }

    int choice;

    do
    {
        cout << "\n========== SETTLEMENT MANAGEMENT ==========\n";
        cout << "1. Record Payment\n";
        cout << "2. View Outstanding Balances\n";
        cout << "3. View Settlement History\n";
        cout << "0. Back\n";
        cout << "Enter choice: ";

        cin >> choice;

        map<string, long long> originalBalances =
            calculateGroupBalances(groups, groupExpenses, groupId);

        map<string, long long> outstandingBalances =
            calculateOutstandingBalances(
                originalBalances, settlements, groupId);

        if (choice == 1)
        {

            cout << "\n========== RECORD PAYMENT ==========\n";

            string fromUserId, toUserId, date, note;
            string amountInput;
            long long amountPaise;

            cout << "Enter payer's User ID: ";
            cin >> fromUserId;

            cout << "Enter receiver's User ID: ";
            cin >> toUserId;

            cout << "Enter payment amount in rupees: ";
            cin >> amountInput;

            if (!convertToPaise(amountInput, amountPaise))
            {
                cout << "Invalid payment amount!\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Enter date (YYYY-MM-DD): ";
            getline(cin, date);

            cout << "Enter payment note: ";
            getline(cin, note);

            bool success = recordSettlement(
                outstandingBalances,
                settlements,
                groupId,
                fromUserId,
                toUserId,
                amountPaise,
                date,
                note);

            if (success)
            {
                cout << "\nPayment recorded successfully!\n";
            }
            else
            {
                cout << "\nPayment could not be recorded.\n";
                cout << "Check the user IDs, payment amount and outstanding balances.\n";
            }
        }

        else if (choice == 2)
        {

            cout << "\n========== OUTSTANDING BALANCES ==========\n";

            displayGroupBalances(users, outstandingBalances);
        }

        else if (choice == 3)
        {

            cout << "\n========== SETTLEMENT HISTORY ==========\n";

            vector<Settlement> history =
                getSettlementHistory(settlements, groupId);

            if (history.empty())
            {
                cout << "No payments recorded yet.\n";
            }
            else
            {
                for (const Settlement &s : history)
                {

                    cout << "\nSettlement ID: " << s.id << "\n";

                    cout << getUserName(users, s.fromUserId)
                         << " (" << s.fromUserId << ") paid "
                         << getUserName(users, s.toUserId)
                         << " (" << s.toUserId << ")\n";

                    cout << "Amount: Rs. "
                         << fixed << setprecision(2)
                         << s.amountPaise / 100.0 << "\n";

                    cout << "Date: " << s.date << "\n";
                    cout << "Note: " << s.note << "\n";
                }
            }
        }

        else if (choice != 0)
        {
            cout << "Invalid choice!\n";
        }

    } while (choice != 0);
}

void createNewBudget(
    vector<Budget> &budgets,
    const string &currentUserId)
{
    Budget budget;

    budget.userId = currentUserId;

    cout << "\n========== CREATE BUDGET ==========\n";

    cout << "Enter category (Food, Travel, Shopping, ALL, etc.): ";
    getline(cin, budget.category);

    if (budget.category.empty())
    {
        cout << "Category cannot be empty.\n";
        return;
    }

    // Normalize ALL category
    if (budget.category == "all" ||
        budget.category == "All")
    {
        budget.category = "ALL";
    }

    // Select budget period
    cout << "\nSelect budget period:\n";
    cout << "1. Daily\n";
    cout << "2. Weekly\n";
    cout << "3. Monthly\n";
    cout << "Enter choice: ";

    string periodInput;
    getline(cin, periodInput);

    if (periodInput == "1")
    {
        budget.period = BudgetPeriod::DAILY;
    }
    else if (periodInput == "2")
    {
        budget.period = BudgetPeriod::WEEKLY;
    }
    else if (periodInput == "3")
    {
        budget.period = BudgetPeriod::MONTHLY;
    }
    else
    {
        cout << "Invalid budget period.\n";
        return;
    }

    // Enter spending limit
    string amountInput;

    cout << "\nEnter spending limit in rupees: ";
    getline(cin, amountInput);

    if (!convertToPaise(amountInput, budget.limitPaise))
    {
        cout << "Invalid budget amount.\n";
        return;
    }

    // Check for an existing budget with the same category and period
    for (const Budget &existing : budgets)
    {

        if (existing.userId == currentUserId &&
            existing.category == budget.category &&
            existing.period == budget.period)
        {

            cout << "\nA budget already exists for this category "
                 << "and period.\n";

            return;
        }
    }

    // Save budget
    if (addBudget(budgets, budget))
    {

        cout << "\nBudget created successfully!\n";

        cout << "Category: " << budget.category << "\n";

        cout << "Period: ";

        if (budget.period == BudgetPeriod::DAILY)
            cout << "Daily";
        else if (budget.period == BudgetPeriod::WEEKLY)
            cout << "Weekly";
        else
            cout << "Monthly";

        cout << "\nLimit: ";
        displayMoney(budget.limitPaise);
        cout << "\n";
    }
    else
    {
        cout << "Could not create budget.\n";
    }
}

string getBudgetPeriodName(BudgetPeriod period)
{
    if (period == BudgetPeriod::DAILY)
        return "Daily";

    if (period == BudgetPeriod::WEEKLY)
        return "Weekly";

    return "Monthly";
}

void viewAllBudgets(const vector<Budget> &budgets,
                    const string &currentUserId)
{

    cout << "\n========== YOUR BUDGETS ==========\n";

    bool found = false;

    for (const Budget &budget : budgets)
    {

        if (budget.userId != currentUserId)
            continue;

        found = true;

        cout << "\nCategory: " << budget.category;
        cout << "\nPeriod: " << getBudgetPeriodName(budget.period);
        cout << "\nLimit: ";
        displayMoney(budget.limitPaise);

        cout << "\n----------------------------------\n";
    }

    if (!found)
    {
        cout << "No budgets created yet.\n";
    }
}

string generateReminderId(int &counter);
ReminderPeriod convertReminderPeriod(BudgetPeriod period);
void viewPendingReminders(ReminderQueue &reminderQueue);

void viewPendingReminders(ReminderQueue &reminderQueue)
{
    if (reminderQueue.isEmpty())
    {
        cout << "\nNo pending reminders.\n";
        return;
    }

    cout << "\n========== PENDING REMINDERS ==========\n";

    int count = reminderQueue.size();

    for (int i = 0; i < count; i++)
    {
        Reminder reminder;

        if (reminderQueue.dequeue(reminder))
        {
            cout << "\nReminder ID: " << reminder.id;
            cout << "\nUser ID: " << reminder.userId;
            cout << "\nCategory: " << reminder.category;

            cout << "\nBudget Limit: ";
            displayMoney(reminder.limitPaise);

            cout << "\nPeriod: ";

            if (reminder.period == ReminderPeriod::DAILY)
                cout << "Daily";
            else if (reminder.period == ReminderPeriod::WEEKLY)
                cout << "Weekly";
            else
                cout << "Monthly";

            cout << "\nStatus: "
                 << (reminder.isActive ? "Active" : "Inactive");

            cout << "\n---------------------------------------\n";

            // Re-enqueue to preserve the original queue
            reminderQueue.enqueue(reminder);
        }
    }

    cout << "\nTotal Pending Reminders: " << count << "\n";
    cout << "=======================================\n";
}

string generateReminderId(int &counter)
{
    ostringstream id;

    id << "REM"
       << setw(4)
       << setfill('0')
       << counter++;

    return id.str();
}

ReminderPeriod convertReminderPeriod(BudgetPeriod period)
{
    if (period == BudgetPeriod::DAILY)
        return ReminderPeriod::DAILY;

    if (period == BudgetPeriod::WEEKLY)
        return ReminderPeriod::WEEKLY;

    return ReminderPeriod::MONTHLY;
}

void processReminders(ReminderQueue &reminderQueue)
{

    if (reminderQueue.isEmpty())
    {
        cout << "\nNo reminders to process.\n";
        return;
    }

    cout << "\n========== PROCESSING REMINDERS ==========\n";

    Reminder reminder;
    int processed = 0;

    while (reminderQueue.dequeue(reminder))
    {

        cout << "\nProcessing Reminder: " << reminder.id;
        cout << "\nCategory: " << reminder.category;

        cout << "\nBudget Limit: ";
        displayMoney(reminder.limitPaise);

        cout << "\nReminder processed successfully.\n";

        processed++;
    }

    cout << "\nTotal reminders processed: "
         << processed << "\n";

    cout << "==========================================\n";
}

void checkBudgetStatus(
    const vector<Budget> &budgets,
    const vector<Expense> &expenses,
    const string &currentUserId,
    ReminderQueue &reminderQueue,
    int &nextReminderId,
    vector<string> &alertedBudgetKeys)
{
    vector<const Budget *> userBudgets;

    for (const Budget &budget : budgets)
    {
        if (budget.userId == currentUserId)
        {
            userBudgets.push_back(&budget);
        }
    }

    if (userBudgets.empty())
    {
        cout << "\nNo budgets available. Create a budget first.\n";
        return;
    }

    cout << "\n========== SELECT BUDGET ==========\n";

    for (size_t i = 0; i < userBudgets.size(); i++)
    {
        cout << i + 1 << ". "
             << userBudgets[i]->category
             << " - "
             << getBudgetPeriodName(userBudgets[i]->period)
             << " - ";

        displayMoney(userBudgets[i]->limitPaise);
        cout << "\n";
    }

    cout << "\nEnter budget number: ";

    string input;
    getline(cin, input);

    int selected;

    try
    {
        size_t processed = 0;
        selected = stoi(input, &processed);

        if (processed != input.size() ||
            selected < 1 ||
            selected > static_cast<int>(userBudgets.size()))
        {
            cout << "Invalid budget selection.\n";
            return;
        }
    }
    catch (...)
    {
        cout << "Invalid input.\n";
        return;
    }

    const Budget &budget = *userBudgets[selected - 1];

    string startDate, endDate;

    cout << "\nEnter start date (YYYY-MM-DD): ";
    getline(cin, startDate);

    cout << "Enter end date (YYYY-MM-DD): ";
    getline(cin, endDate);

    BudgetStatus status = evaluateBudget(
        expenses,
        budget,
        startDate,
        endDate);

    cout << "\n========== BUDGET STATUS ==========\n";

    cout << "Category: " << budget.category << "\n";
    cout << "Period: " << getBudgetPeriodName(budget.period) << "\n";

    cout << "Budget Limit: ";
    displayMoney(status.budgetLimitPaise);

    cout << "\nActual Spending: ";
    displayMoney(status.actualSpendingPaise);

    cout << "\nRemaining Budget: ";
    displayMoney(status.remainingPaise);

    cout << "\nPercentage Used: "
         << fixed << setprecision(2)
         << status.percentageUsed << "%\n";

    cout << "\nStatus: ";

    if (status.alert == BudgetAlert::WITHIN_BUDGET)
    {
        cout << "Within Budget";
    }
    else if (status.alert == BudgetAlert::WARNING)
    {
        cout << "WARNING - 80% or more of budget used";
    }
    else
    {
        cout << "BUDGET EXCEEDED!";
    }

    cout << "\n===================================\n";

    // Generate reminder if budget is at warning or exceeded level

    if (status.alert == BudgetAlert::WARNING ||
        status.alert == BudgetAlert::EXCEEDED)
    {
        string alertKey =
            budget.userId + "_" +
            budget.category + "_" +
            getBudgetPeriodName(budget.period) + "_" +
            startDate + "_" + endDate;

        bool alreadyAlerted = false;

        for (const string &key : alertedBudgetKeys)
        {
            if (key == alertKey)
            {
                alreadyAlerted = true;
                break;
            }
        }

        if (!alreadyAlerted)
        {
            Reminder reminder;

            reminder.id = generateReminderId(nextReminderId);
            reminder.userId = budget.userId;
            reminder.category = budget.category;
            reminder.limitPaise = budget.limitPaise;
            reminder.period = convertReminderPeriod(budget.period);
            reminder.isActive = true;

            reminderQueue.enqueue(reminder);

            alertedBudgetKeys.push_back(alertKey);

            cout << "\n[Reminder Generated] "
                 << "Budget alert added to reminder queue.\n";
        }
        else
        {
            cout << "\nA reminder for this budget period "
                 << "has already been generated.\n";
        }
    }
}

void viewActivityHistory(ActivityList &activityList)
{
    cout << "\n========== ACTIVITY HISTORY ==========\n";

    if (activityList.isEmpty())
    {
        cout << "No activities recorded yet.\n";
        return;
    }

    activityList.displayActivities();

    cout << "\nTotal activities: "
         << activityList.size() << "\n";

    cout << "======================================\n";
}

void undoLastExpenseOperation(
    vector<Expense> &expenses,
    UndoStack &undoStack,
    ActivityList &activityList)
{
    if (undoStack.isEmpty())
    {
        cout << "\nNothing to undo.\n";
        return;
    }

    UndoAction action;

    if (!undoStack.pop(action))
    {
        cout << "\nCould not retrieve undo operation.\n";
        return;
    }

    const Expense &oldExpense = action.expenseData;

    bool success = false;

    switch (action.operation)
    {
    case UndoOperation::ADD_EXPENSE:
    {
        success = deleteExpense(expenses, oldExpense.id);

        if (success)
            cout << "\nUndo successful: Added expense removed.\n";

        break;
    }

    case UndoOperation::UPDATE_EXPENSE:
    {
        success = updateExpense(
            expenses,
            oldExpense.id,
            oldExpense);

        if (success)
            cout << "\nUndo successful: Previous expense restored.\n";

        break;
    }

    case UndoOperation::DELETE_EXPENSE:
    {
        // Restore original record, including its original ID
        expenses.push_back(oldExpense);
        success = true;

        cout << "\nUndo successful: Deleted expense restored.\n";
        break;
    }
    }

    if (success)
    {
        activityList.addActivity(
            "Undo operation performed for expense: " + oldExpense.id,
            oldExpense.date);

        if (!saveExpensesToFile(expenses, DATA_FILE))
            cout << "Warning: Could not save data.\n";
    }
    else
    {
        cout << "\nUndo operation failed.\n";
    }
}

void displaySortedExpenses(const vector<Expense> &expenses)
{

    if (expenses.empty())
    {
        cout << "\nNo expenses available.\n";
        return;
    }

    cout << "\n========== SORTED EXPENSES ==========\n";

    for (const Expense &expense : expenses)
    {

        cout << "\nID: " << expense.id;
        cout << "\nTitle: " << expense.title;
        cout << "\nCategory: " << expense.category;
        cout << "\nDate: " << expense.date;

        cout << "\nAmount: Rs. "
             << expense.amountPaise / 100
             << "."
             << setw(2) << setfill('0')
             << expense.amountPaise % 100
             << setfill(' ')
             << "\n";
    }
}

void expenseSortingMenu(const vector<Expense> &expenses)
{

    if (expenses.empty())
    {
        cout << "\nNo expenses available to sort.\n";
        return;
    }

    int choice;

    do
    {
        cout << "\n========== EXPENSE SORTING ==========\n";
        cout << "1. Sort by amount (Ascending)\n";
        cout << "2. Sort by amount (Descending)\n";
        cout << "3. Sort by date\n";
        cout << "4. Sort by category\n";
        cout << "0. Back\n";

        cout << "\nEnter choice: ";
        cin >> choice;

        vector<Expense> sortedExpenses = expenses;

        switch (choice)
        {

        case 1:
            mergeSortByAmount(sortedExpenses);
            displaySortedExpenses(sortedExpenses);
            break;

        case 2:
            mergeSortByAmount(sortedExpenses);

            reverse(
                sortedExpenses.begin(),
                sortedExpenses.end());

            displaySortedExpenses(sortedExpenses);
            break;

        case 3:
            sortExpensesByDate(sortedExpenses);
            displaySortedExpenses(sortedExpenses);
            break;

        case 4:
            sortExpensesByCategory(sortedExpenses);
            displaySortedExpenses(sortedExpenses);
            break;

        case 0:
            cout << "\nReturning to main menu...\n";
            break;

        default:
            cout << "\nInvalid choice. Try again.\n";
        }

    } while (choice != 0);
}

void viewHighestSpendingWindow(const vector<Expense> &expenses)
{

    if (expenses.empty())
    {
        cout << "\nNo expenses available.\n";
        return;
    }

    int windowDays;

    cout << "\nEnter number of consecutive days: ";
    cin >> windowDays;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (windowDays <= 0)
    {
        cout << "\nInvalid number of days.\n";
        return;
    }

    SpendingWindowResult result =
        findHighestSpendingWindow(expenses, windowDays);

    if (!result.found)
    {
        cout << "\nUnable to calculate spending window.\n";
        cout << "Check whether expense dates and amounts are valid.\n";
        return;
    }

    cout << "\n========== HIGHEST SPENDING WINDOW ==========\n";

    cout << "Duration: " << windowDays << " days\n";

    cout << "Start Date: " << result.startDate << "\n";

    cout << "End Date: " << result.endDate << "\n";

    cout << "Total Spending: Rs. "
         << result.totalPaise / 100
         << "."
         << setw(2) << setfill('0')
         << result.totalPaise % 100
         << setfill(' ')
         << "\n";
}

void viewAdvancedCategoryAnalytics(const vector<Expense> &expenses)
{

    if (expenses.empty())
    {
        cout << "\nNo expenses available for analysis.\n";
        return;
    }

    unordered_map<string, CategoryAnalytics> categoryData =
        calculateCategoryAnalytics(expenses);

    cout << "\n========== ADVANCED CATEGORY ANALYTICS ==========\n";

    string highestCategory = "";
    long long highestSpending = -1;

    for (const auto &entry : categoryData)
    {

        const string &category = entry.first;
        const CategoryAnalytics &data = entry.second;

        cout << "\nCategory: " << category;

        cout << "\nTotal Spending: Rs. "
             << data.totalSpendingPaise / 100
             << "."
             << setw(2) << setfill('0')
             << data.totalSpendingPaise % 100
             << setfill(' ');

        cout << "\nTransactions: " << data.transactionCount;

        cout << "\nAverage Expense: Rs. "
             << data.averageExpensePaise / 100
             << "."
             << setw(2) << setfill('0')
             << data.averageExpensePaise % 100
             << setfill(' ') << "\n";

        if (data.totalSpendingPaise > highestSpending)
        {
            highestSpending = data.totalSpendingPaise;
            highestCategory = category;
        }
    }

    cout << "\n========== HIGHEST SPENDING CATEGORY ==========\n";
    cout << "Category: " << highestCategory;

    cout << "\nTotal Spending: Rs. "
         << highestSpending / 100
         << "."
         << setw(2) << setfill('0')
         << highestSpending % 100
         << setfill(' ') << "\n";
}

// ==========================================
// MAIN APPLICATION
// ==========================================

int main()
{

    vector<Expense> expenses;

    vector<User> users;
    User currentUser;
    currentUser.id = CURRENT_USER_ID;
    currentUser.name = "Arsh";

    users.push_back(currentUser);

    vector<Group> groups;
    vector<GroupExpense> groupExpenses;
    vector<Settlement> settlements;
    vector<Budget> budgets;
    ReminderQueue reminderQueue;
    int nextReminderId = 1;
    vector<string> alertedBudgetKeys;
    ActivityList activityList;
    UndoStack undoStack;

    cout << "\n====================================\n";
    cout << " PERSONAL EXPENSE ANALYZER\n";
    cout << "====================================\n";

    cout << "\n========== LOADING SAVED DATA ==========\n";

    // Personal expenses
    if (loadExpensesFromFile(expenses, DATA_FILE))
        cout << "Expenses loaded: " << expenses.size() << "\n";
    else
        cout << "No existing expenses found.\n";

    // Users
    if (loadUsers(users, "storage/users.txt"))
        cout << "Users loaded: " << users.size() << "\n";
    else
        cout << "No existing users file found.\n";

    // Ensure the current user always exists
    bool currentUserExists = false;

    for (const User &user : users)
    {
        if (user.id == CURRENT_USER_ID)
        {
            currentUserExists = true;
            break;
        }
    }

    if (!currentUserExists)
    {
        users.push_back(currentUser);
    }

    // Groups
    if (loadGroups(groups, "storage/groups.txt"))
        cout << "Groups loaded: " << groups.size() << "\n";
    else
        cout << "No existing groups file found.\n";

    // Group expenses
    if (loadGroupExpenses(groupExpenses, "storage/group_expenses.txt"))
        cout << "Group expenses loaded: " << groupExpenses.size() << "\n";
    else
        cout << "No existing group expenses file found.\n";

    // Settlements
    if (loadSettlements(settlements, "storage/settlements.txt"))
        cout << "Settlements loaded: " << settlements.size() << "\n";
    else
        cout << "No existing settlements file found.\n";

    // Budgets
    if (loadBudgets(budgets, "storage/budgets.txt"))
        cout << "Budgets loaded: " << budgets.size() << "\n";
    else
        cout << "No existing budgets file found.\n";

    cout << "========================================\n";

    int choice;

    do
    {

        cout << "\n========== MAIN MENU ==========\n";

        cout << "1. Add Expense\n";
        cout << "2. View All Expenses\n";
        cout << "3. Update Expense\n";
        cout << "4. Delete Expense\n";
        cout << "5. Search Expense\n";
        cout << "6. Expense Analytics\n";
        cout << "7. Save Data\n";
        cout << "8. Group Management\n";
        cout << "9. Create Budget\n";
        cout << "10. View All Budgets\n";
        cout << "11. Check Budget Status\n";
        cout << "12. View Pending Reminders\n";
        cout << "13. Process Reminders\n";
        cout << "14. View Activity History\n";
        cout << "15. Undo Last Expense Operation\n";
        cout << "16. Sort Expenses\n";
        cout << "17. Highest Spending Period\n";
        cout << "18. Advanced Category Analytics\n";
        cout << "0. Exit\n";

        cout << "\nEnter your choice: ";

        string choiceInput;
        getline(cin, choiceInput);

        try
        {
            size_t processed = 0;
            choice = stoi(choiceInput, &processed);

            if (processed != choiceInput.size())
            {
                choice = -1;
            }
        }
        catch (...)
        {
            choice = -1;
        }

        switch (choice)
        {

        case 1:
            addNewExpense(expenses, activityList, undoStack);
            break;

        case 2:
            viewExpenses(expenses);
            break;

        case 3:
            updateExistingExpense(expenses, activityList, undoStack);

            break;

        case 4:
            deleteExistingExpense(expenses, activityList, undoStack);
            break;

        case 5:
            searchExpense(expenses);
            break;

        case 6:
            showAnalytics(expenses);
            break;

        case 7:

            if (saveAllData(
                    expenses,
                    users,
                    groups,
                    groupExpenses,
                    settlements,
                    budgets))
            {
                cout << "\nAll application data saved successfully!\n";
            }
            else
            {
                cout << "\nSome data could not be saved. Check storage files.\n";
            }

            break;

        case 8:

            groupManagementMenu(
                users,
                groups,
                groupExpenses,
                settlements);

            if (!saveAllData(
                    expenses, users, groups,
                    groupExpenses, settlements, budgets))
            {
                cout << "Warning: Some group data could not be saved.\n";
            }

            break;

        case 9:

            createNewBudget(budgets, CURRENT_USER_ID);

            if (!saveBudgets(budgets, "storage/budgets.txt"))
                cout << "Warning: Budget data could not be saved.\n";

            break;

        case 10:
            viewAllBudgets(budgets, CURRENT_USER_ID);
            break;

        case 11:
            checkBudgetStatus(
                budgets,
                expenses,
                CURRENT_USER_ID,
                reminderQueue,
                nextReminderId,
                alertedBudgetKeys);
            break;

        case 12:
            viewPendingReminders(reminderQueue);
            break;

        case 13:
            processReminders(reminderQueue);
            break;

        case 14:
            viewActivityHistory(activityList);
            break;

        case 15:
            undoLastExpenseOperation(expenses, undoStack, activityList);
            break;

        case 16:
            expenseSortingMenu(expenses);
            break;

        case 17:
            viewHighestSpendingWindow(expenses);
            break;

        case 18:
            viewAdvancedCategoryAnalytics(expenses);
            break;

        case 0:

            if (saveAllData(
                    expenses,
                    users,
                    groups,
                    groupExpenses,
                    settlements,
                    budgets))
            {
                cout << "\nAll data saved successfully.\n";
            }
            else
            {
                cout << "\nWarning: Some data could not be saved.\n";
            }

            cout << "\nThank you for using "
                 << "Personal Expense Analyzer!\n";

            break;

        default:
            cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);

    return 0;
}
