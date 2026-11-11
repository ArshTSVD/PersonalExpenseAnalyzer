
#pragma once

#include <vector>
#include "../models/models.hpp"

// Divide an expense equally among all listed participants
bool calculateEqualSplit(
    long long totalAmountPaise,
    std::vector<Split>& participants
);

// Divide an expense equally among selected participants
bool calculateSelectedMembersSplit(
    long long totalAmountPaise,
    std::vector<Split>& selectedMembers
);

// Validate manually specified exact shares
bool validateExactSplit(
    long long totalAmountPaise,
    const std::vector<Split>& splits
);

// Main function: calculate or validate based on split type
bool calculateExpenseSplits(
    GroupExpense& expense
);
