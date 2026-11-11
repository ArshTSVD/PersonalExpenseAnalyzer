
#pragma once

#include <string>
#include <vector>

#include "../models/models.hpp"
#include "../core/expense_manager.hpp"
#include "../core/budget_manager.hpp"

class ApplicationState
{
public:

    // Current logged-in user
    std::string currentUserId;

    // Application data
    std::vector<Expense> expenses;
    std::vector<User> users;
    std::vector<Group> groups;
    std::vector<GroupExpense> groupExpenses;
    std::vector<Settlement> settlements;
    std::vector<Budget> budgets;

    // Constructor
    ApplicationState();

    // Load all saved application data
    void load();

    // Save all application data
    bool save() const;
};
