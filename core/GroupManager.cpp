
#include "GroupManager.h"

#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <cctype>

using namespace std;


// ==========================================
// CHECK USER EXISTENCE
// ==========================================

bool userExists(
    const vector<User>& users,
    const string& userId
) {
    for (const User& user : users) {

        if (user.id == userId) {
            return true;
        }
    }

    return false;
}


// ==========================================
// GENERATE UNIQUE GROUP ID
// ==========================================

string generateGroupId(
    const vector<Group>& groups
) {
    int nextNumber = 1;

    for (const Group& group : groups) {

        if (group.id.size() > 3 &&
            group.id.substr(0, 3) == "GRP") {

            try {
                int number = stoi(group.id.substr(3));

                if (number >= nextNumber) {
                    nextNumber = number + 1;
                }
            }
            catch (...) {
                // Ignore malformed group IDs
            }
        }
    }

    stringstream idStream;

    idStream << "GRP"
             << setfill('0')
             << setw(4)
             << nextNumber;

    return idStream.str();
}


// ==========================================
// GENERATE UNIQUE JOINING CODE
// ==========================================

string generateGroupCode(
    const vector<Group>& groups
) {
    static const string characters =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    static random_device rd;
    static mt19937 generator(rd());

    uniform_int_distribution<int> distribution(
        0,
        static_cast<int>(characters.size()) - 1
    );

    string code;

    bool alreadyExists;

    do {
        code.clear();

        for (int i = 0; i < 6; i++) {
            code += characters[distribution(generator)];
        }

        alreadyExists = false;

        for (const Group& group : groups) {

            if (group.groupCode == code) {
                alreadyExists = true;
                break;
            }
        }

    } while (alreadyExists);

    return code;
}


// ==========================================
// CREATE GROUP
// ==========================================

bool createGroup(
    vector<Group>& groups,
    const vector<User>& users,
    const string& creatorId,
    const string& groupName,
    Group& createdGroup
) {

    if (!userExists(users, creatorId)) {
        return false;
    }

    if (groupName.empty()) {
        return false;
    }

    // Generate group details
    createdGroup.id = generateGroupId(groups);

    createdGroup.name = groupName;

    createdGroup.createdBy = creatorId;

    createdGroup.groupCode = generateGroupCode(groups);

    // Creator automatically becomes first member
    createdGroup.memberIds.clear();

    createdGroup.memberIds.push_back(creatorId);

    // Store group
    groups.push_back(createdGroup);

    return true;
}


// ==========================================
// FIND GROUP BY ID
// ==========================================

const Group* findGroupById(
    const vector<Group>& groups,
    const string& groupId
) {

    for (const Group& group : groups) {

        if (group.id == groupId) {
            return &group;
        }
    }

    return nullptr;
}


// ==========================================
// FIND GROUP BY JOINING CODE
// ==========================================

const Group* findGroupByCode(
    const vector<Group>& groups,
    const string& groupCode
) {

    string targetCode = groupCode;

    // Convert code to uppercase
    transform(
        targetCode.begin(),
        targetCode.end(),
        targetCode.begin(),
        [](unsigned char ch) {
            return static_cast<char>(toupper(ch));
        }
    );

    for (const Group& group : groups) {

        if (group.groupCode == targetCode) {
            return &group;
        }
    }

    return nullptr;
}


// ==========================================
// ADD MEMBER TO GROUP
// ==========================================

bool addMemberToGroup(
    vector<Group>& groups,
    const vector<User>& users,
    const string& groupId,
    const string& userId
) {

    if (!userExists(users, userId)) {
        return false;
    }

    for (Group& group : groups) {

        if (group.id == groupId) {

            // Prevent duplicate membership
            for (const string& memberId : group.memberIds) {

                if (memberId == userId) {
                    return false;
                }
            }

            group.memberIds.push_back(userId);

            return true;
        }
    }

    return false;
}


// ==========================================
// JOIN GROUP USING CODE
// ==========================================

bool joinGroupByCode(
    vector<Group>& groups,
    const vector<User>& users,
    const string& groupCode,
    const string& userId
) {

    const Group* group = findGroupByCode(
        groups,
        groupCode
    );

    if (group == nullptr) {
        return false;
    }

    return addMemberToGroup(
        groups,
        users,
        group->id,
        userId
    );
}
