
#pragma once

#include <string>
#include <vector>

#include "../models/models.hpp"

// Generate a unique group expense ID
std::string generateGroupExpenseId(
    const std::vector<GroupExpense>& expenses
);

// Add a group expense
bool addGroupExpense(
    std::vector<GroupExpense>& expenses,
    const std::vector<Group>& groups,
    GroupExpense& expense
);

// Find group expense by ID
const GroupExpense* findGroupExpenseById(
    const std::vector<GroupExpense>& expenses,
    const std::string& expenseId
);

// Update a group expense
bool updateGroupExpense(
    std::vector<GroupExpense>& expenses,
    const std::vector<Group>& groups,
    const std::string& expenseId,
    const GroupExpense& updatedExpense
);

// Delete a group expense
bool deleteGroupExpense(
    std::vector<GroupExpense>& expenses,
    const std::string& expenseId
);

// Get all expenses belonging to a group
std::vector<const GroupExpense*> getGroupExpenses(
    const std::vector<GroupExpense>& expenses,
    const std::string& groupId
);
