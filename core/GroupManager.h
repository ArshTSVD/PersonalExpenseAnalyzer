
#pragma once

#include <string>
#include <vector>

#include "../models/models.hpp"

using namespace std;

// Check whether a user exists
bool userExists(
    const vector<User>& users,
    const string& userId
);

// Generate a unique group ID
string generateGroupId(
    const vector<Group>& groups
);

// Generate a unique joining code
string generateGroupCode(
    const vector<Group>& groups
);

// Create a group
bool createGroup(
    vector<Group>& groups,
    const vector<User>& users,
    const string& creatorId,
    const string& groupName,
    Group& createdGroup
);

// Find group using its ID
const Group* findGroupById(
    const vector<Group>& groups,
    const string& groupId
);

// Find group using its joining code
const Group* findGroupByCode(
    const vector<Group>& groups,
    const string& groupCode
);

// Add a user to an existing group
bool addMemberToGroup(
    vector<Group>& groups,
    const vector<User>& users,
    const string& groupId,
    const string& userId
);

// Join a group using its joining code
bool joinGroupByCode(
    vector<Group>& groups,
    const vector<User>& users,
    const string& groupCode,
    const string& userId
);
