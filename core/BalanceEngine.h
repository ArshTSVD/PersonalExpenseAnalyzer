
#pragma once

#include <string>
#include <vector>
#include <map>

#include "../models/models.hpp"

// Calculate net balance of every member in a group.
// Positive = money to receive.
// Negative = money to pay.

std::map<std::string, long long> calculateGroupBalances(
    const std::vector<Group>& groups,
    const std::vector<GroupExpense>& expenses,
    const std::string& groupId
);

// Check whether all group balances add up to zero.

bool validateGroupBalances(
    const std::map<std::string, long long>& balances
);

// Display group balances.

void displayGroupBalances(
    const std::vector<User>& users,
    const std::map<std::string, long long>& balances
);
