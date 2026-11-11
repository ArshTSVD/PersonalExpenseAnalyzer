
#include "SettlementManager.h"

#include <sstream>
#include <iomanip>
#include <climits>

using namespace std;


// ==========================================
// GENERATE SETTLEMENT ID
// ==========================================

string generateSettlementId(
    const vector<Settlement>& settlementHistory
) {

    int nextNumber = 1;

    for (const Settlement& settlement : settlementHistory) {

        const string& id = settlement.id;

        if (id.size() <= 3 || id.substr(0, 3) != "SET") {
            continue;
        }

        try {

            int number = stoi(id.substr(3));

            if (number >= nextNumber) {
                nextNumber = number + 1;
            }

        }
        catch (...) {
            // Ignore IDs that do not follow the expected format.
        }
    }

    ostringstream idStream;

    idStream << "SET"
             << setw(4)
             << setfill('0')
             << nextNumber;

    return idStream.str();
}


// ==========================================
// CALCULATE OUTSTANDING BALANCES
// ==========================================

map<string, long long> calculateOutstandingBalances(
    const map<string, long long>& originalBalances,
    const vector<Settlement>& settlementHistory,
    const string& groupId
) {

    map<string, long long> outstanding = originalBalances;

    if (groupId.empty()) {
        return {};
    }

    for (const Settlement& settlement : settlementHistory) {

        if (settlement.groupId != groupId) {
            continue;
        }

        if (settlement.amountPaise <= 0) {
            return {};
        }

        auto fromIt = outstanding.find(settlement.fromUserId);
        auto toIt = outstanding.find(settlement.toUserId);

        if (
            fromIt == outstanding.end() ||
            toIt == outstanding.end() ||
            settlement.fromUserId == settlement.toUserId
        ) {
            return {};
        }

        // A payer must have a negative outstanding balance.
        // A receiver must have a positive outstanding balance.
        if (
            fromIt->second >= 0 ||
            toIt->second <= 0 ||
            fromIt->second == LLONG_MIN ||
            settlement.amountPaise > -fromIt->second ||
            settlement.amountPaise > toIt->second
        ) {
            return {};
        }

        fromIt->second += settlement.amountPaise;
        toIt->second -= settlement.amountPaise;
    }

    return outstanding;
}


// ==========================================
// RECORD AN ACTUAL SETTLEMENT
// ==========================================

bool recordSettlement(
    const map<string, long long>& outstandingBalances,
    vector<Settlement>& settlementHistory,
    const string& groupId,
    const string& fromUserId,
    const string& toUserId,
    long long amountPaise,
    const string& date,
    const string& note
) {

    if (
        groupId.empty() ||
        fromUserId.empty() ||
        toUserId.empty() ||
        date.empty() ||
        fromUserId == toUserId ||
        amountPaise <= 0 ||
        outstandingBalances.empty()
    ) {
        return false;
    }

    auto fromIt = outstandingBalances.find(fromUserId);
    auto toIt = outstandingBalances.find(toUserId);

    if (
        fromIt == outstandingBalances.end() ||
        toIt == outstandingBalances.end()
    ) {
        return false;
    }

    if (
        fromIt->second >= 0 ||
        toIt->second <= 0 ||
        fromIt->second == LLONG_MIN
    ) {
        return false;
    }

    long long maximumPayment = -fromIt->second;

    if (toIt->second < maximumPayment) {
        maximumPayment = toIt->second;
    }

    if (amountPaise > maximumPayment) {
        return false;
    }

    Settlement settlement;

    settlement.id = generateSettlementId(settlementHistory);
    settlement.groupId = groupId;
    settlement.fromUserId = fromUserId;
    settlement.toUserId = toUserId;
    settlement.amountPaise = amountPaise;
    settlement.date = date;
    settlement.note = note;

    settlementHistory.push_back(settlement);

    return true;
}


// ==========================================
// GET GROUP SETTLEMENT HISTORY
// ==========================================

vector<Settlement> getSettlementHistory(
    const vector<Settlement>& settlementHistory,
    const string& groupId
) {

    vector<Settlement> result;

    if (groupId.empty()) {
        return result;
    }

    for (const Settlement& settlement : settlementHistory) {

        if (settlement.groupId == groupId) {
            result.push_back(settlement);
        }
    }

    return result;
}
