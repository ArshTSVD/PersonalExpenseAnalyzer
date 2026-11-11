
#pragma once

#include <string>
#include <vector>
#include <map>

#include "../models/models.hpp"

// A proposed payment between two group members.
// This is NOT yet a confirmed payment.

struct SettlementTransfer {
    std::string fromUserId;
    std::string toUserId;
    long long amountPaise;
};

// Generate a settlement plan from member balances.

bool generateSettlementPlan(
    const std::map<std::string, long long>& balances,
    std::vector<SettlementTransfer>& transfers
);

// Validate whether the generated plan settles all balances.

bool validateSettlementPlan(
    const std::map<std::string, long long>& balances,
    const std::vector<SettlementTransfer>& transfers
);
