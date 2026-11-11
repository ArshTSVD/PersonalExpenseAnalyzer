
#pragma once

#include <string>
#include <vector>
#include <map>

#include "../models/models.hpp"

// Record an actual payment between two group members.

bool recordSettlement(
    const std::map<std::string, long long>& outstandingBalances,
    std::vector<Settlement>& settlementHistory,
    const std::string& groupId,
    const std::string& fromUserId,
    const std::string& toUserId,
    long long amountPaise,
    const std::string& date,
    const std::string& note
);

// Calculate outstanding balances after recorded payments.

std::map<std::string, long long> calculateOutstandingBalances(
    const std::map<std::string, long long>& originalBalances,
    const std::vector<Settlement>& settlementHistory,
    const std::string& groupId
);

// Retrieve settlement history for a particular group.

std::vector<Settlement> getSettlementHistory(
    const std::vector<Settlement>& settlementHistory,
    const std::string& groupId
);

// Generate a unique settlement ID.

std::string generateSettlementId(
    const std::vector<Settlement>& settlementHistory
);
