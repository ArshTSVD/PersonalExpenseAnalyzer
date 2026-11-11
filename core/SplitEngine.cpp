
#include "SplitEngine.h"

#include <unordered_set>

using namespace std;


// ==========================================
// VALIDATE PARTICIPANTS
// ==========================================

static bool validateParticipants(
    const vector<Split>& participants
) {

    if (participants.empty()) {
        return false;
    }

    unordered_set<string> userIds;

    for (const Split& participant : participants) {

        if (participant.userId.empty()) {
            return false;
        }

        // Reject duplicate participants
        if (!userIds.insert(participant.userId).second) {
            return false;
        }
    }

    return true;
}


// ==========================================
// EQUAL SPLIT
// ==========================================

bool calculateEqualSplit(
    long long totalAmountPaise,
    vector<Split>& participants
) {

    if (totalAmountPaise <= 0 ||
        !validateParticipants(participants)) {

        return false;
    }

    long long baseShare =
        totalAmountPaise / participants.size();

    long long remainder =
        totalAmountPaise % participants.size();

    for (size_t i = 0; i < participants.size(); i++) {

        participants[i].sharePaise = baseShare;

        // Distribute remaining paise one by one
        if (static_cast<long long>(i) < remainder) {
            participants[i].sharePaise++;
        }
    }

    return true;
}


// ==========================================
// SELECTED MEMBERS SPLIT
// ==========================================

bool calculateSelectedMembersSplit(
    long long totalAmountPaise,
    vector<Split>& selectedMembers
) {

    // Only the supplied selected members participate.
    // The calculation is equal among those members.

    return calculateEqualSplit(
        totalAmountPaise,
        selectedMembers
    );
}


// ==========================================
// VALIDATE EXACT SPLIT
// ==========================================

bool validateExactSplit(
    long long totalAmountPaise,
    const vector<Split>& splits
) {

    if (totalAmountPaise <= 0 ||
        !validateParticipants(splits)) {

        return false;
    }

    long long totalShares = 0;

    for (const Split& split : splits) {

        if (split.sharePaise <= 0) {
            return false;
        }

        // Prevent sum from exceeding the expense amount.
        // This also avoids integer overflow.
        if (split.sharePaise >
            totalAmountPaise - totalShares) {

            return false;
        }

        totalShares += split.sharePaise;
    }

    return totalShares == totalAmountPaise;
}


// ==========================================
// MAIN EXPENSE SPLIT FUNCTION
// ==========================================

bool calculateExpenseSplits(
    GroupExpense& expense
) {

    if (expense.amountPaise <= 0) {
        return false;
    }

    switch (expense.splitType) {

        case SplitType::EQUAL:

            return calculateEqualSplit(
                expense.amountPaise,
                expense.splits
            );


        case SplitType::SELECTED_MEMBERS:

            return calculateSelectedMembersSplit(
                expense.amountPaise,
                expense.splits
            );


        case SplitType::EXACT_AMOUNT:

            return validateExactSplit(
                expense.amountPaise,
                expense.splits
            );


        default:

            return false;
    }
}
