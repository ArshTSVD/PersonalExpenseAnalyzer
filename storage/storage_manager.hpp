
#pragma once

#include <string>
#include <vector>

#include "../core/expense_manager.hpp"
#include "../core/budget_manager.hpp"
#include "../models/models.hpp"


// Personal expenses (legacy text storage)
bool saveExpenses(
    const std::vector<Expense>& expenses,
    const std::string& filePath = "storage/expenses.txt"
);

bool loadExpenses(
    std::vector<Expense>& expenses,
    const std::string& filePath = "storage/expenses.txt"
);

// Budgets
bool saveBudgets(
    const std::vector<Budget>& budgets,
    const std::string& filePath = "storage/budgets.txt"
);

bool loadBudgets(
    std::vector<Budget>& budgets,
    const std::string& filePath = "storage/budgets.txt"
);

// Users
bool saveUsers(
    const std::vector<User>& users,
    const std::string& filePath = "storage/users.txt"
);

bool loadUsers(
    std::vector<User>& users,
    const std::string& filePath = "storage/users.txt"
);

// Groups
bool saveGroups(
    const std::vector<Group>& groups,
    const std::string& filePath = "storage/groups.txt"
);

bool loadGroups(
    std::vector<Group>& groups,
    const std::string& filePath = "storage/groups.txt"
);

// Group expenses and their individual splits
bool saveGroupExpenses(
    const std::vector<GroupExpense>& expenses,
    const std::string& filePath = "storage/group_expenses.txt"
);

bool loadGroupExpenses(
    std::vector<GroupExpense>& expenses,
    const std::string& filePath = "storage/group_expenses.txt"
);

// Settlements
bool saveSettlements(
    const std::vector<Settlement>& settlements,
    const std::string& filePath = "storage/settlements.txt"
);

bool loadSettlements(
    std::vector<Settlement>& settlements,
    const std::string& filePath = "storage/settlements.txt"
);
