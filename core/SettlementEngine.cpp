
#include "SettlementEngine.h"

#include <algorithm>
#include <climits>

using namespace std;


// ==========================================
// GENERATE SETTLEMENT PLAN
// ==========================================

bool generateSettlementPlan(
    const map<string, long long>& balances,
    vector<SettlementTransfer>& transfers
) {

    transfers.clear();

    if (balances.empty()) {
        return false;
    }

    vector<pair<string, long long>> debtors;
    vector<pair<string, long long>> creditors;

    long long totalBalance = 0;

    // --------------------------------------
    // Separate debtors and creditors
    // --------------------------------------

    for (const auto& entry : balances) {

        const string& userId = entry.first;
        long long balance = entry.second;

        if (balance == LLONG_MIN) {
            return false;
        }

        totalBalance += balance;

        if (balance < 0) {

            debtors.push_back({
                userId, -balance
            });

        }
        else if (balance > 0) {

            creditors.push_back({
                userId, balance
            });
        }
    }

    // A valid group must have zero net balance.
    if (totalBalance != 0) {
        return false;
    }

    // No payments are necessary if everyone is settled.
    if (debtors.empty() && creditors.empty()) {
        return true;
    }

    // --------------------------------------
    // Greedy settlement algorithm
    // --------------------------------------

    size_t debtorIndex = 0;
    size_t creditorIndex = 0;

    while (
        debtorIndex < debtors.size() &&
        creditorIndex < creditors.size()
    ) {

        long long amount = min(
            debtors[debtorIndex].second,
            creditors[creditorIndex].second
        );

        if (amount <= 0) {
            return false;
        }

        // Generate proposed transaction
        SettlementTransfer transfer;

        transfer.fromUserId =
            debtors[debtorIndex].first;

        transfer.toUserId =
            creditors[creditorIndex].first;

        transfer.amountPaise = amount;

        transfers.push_back(transfer);

        // Reduce outstanding balances
        debtors[debtorIndex].second -= amount;

        creditors[creditorIndex].second -= amount;

        // Move to next debtor if current one is settled
        if (debtors[debtorIndex].second == 0) {
            debtorIndex++;
        }

        // Move to next creditor if current one is settled
        if (creditors[creditorIndex].second == 0) {
            creditorIndex++;
        }
    }

    // Both lists must be completely settled.
    return (
        debtorIndex == debtors.size() &&
        creditorIndex == creditors.size()
    );
}


// ==========================================
// VALIDATE SETTLEMENT PLAN
// ==========================================

bool validateSettlementPlan(
    const map<string, long long>& balances,
    const vector<SettlementTransfer>& transfers
) {

    if (balances.empty()) {
        return false;
    }

    // Start with the original balances.
    map<string, long long> remainingBalances = balances;

    for (const SettlementTransfer& transfer : transfers) {

        if (transfer.amountPaise <= 0) {
            return false;
        }

        if (transfer.fromUserId == transfer.toUserId) {
            return false;
        }

        // Both users must exist.
        if (
            remainingBalances.find(transfer.fromUserId)
                == remainingBalances.end() ||

            remainingBalances.find(transfer.toUserId)
                == remainingBalances.end()
        ) {
            return false;
        }

        long long& fromBalance =
            remainingBalances[transfer.fromUserId];

        long long& toBalance =
            remainingBalances[transfer.toUserId];

        // The payer must owe at least this amount.
        if (
            fromBalance >= 0 ||
            transfer.amountPaise > -fromBalance
        ) {
            return false;
        }

        // The receiver must be entitled to at least this amount.
        if (
            toBalance <= 0 ||
            transfer.amountPaise > toBalance
        ) {
            return false;
        }

        // Apply the proposed payment.
        fromBalance += transfer.amountPaise;
        toBalance -= transfer.amountPaise;
    }

    // Every balance must become exactly zero.
    for (const auto& entry : remainingBalances) {

        if (entry.second != 0) {
            return false;
        }
    }

    return true;
}
