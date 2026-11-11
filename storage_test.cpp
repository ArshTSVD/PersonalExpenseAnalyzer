
#include <iostream>
#include "storage/storage_manager.hpp"

using namespace std;

int main() {

    vector<User> users = {
        {"USR001", "Arsh"},
        {"USR002", "Rahul"},
        {"USR003", "Aman"}
    };

    vector<Group> groups;

    Group g;
    g.id = "GRP001";
    g.name = "Goa Trip";
    g.createdBy = "USR001";
    g.groupCode = "GOA123";
    g.memberIds = {"USR001", "USR002", "USR003"};

    groups.push_back(g);

    cout << "Saving users and groups...\n";

    if (!saveUsers(users, "storage/users_test.txt")) {
        cout << "User save failed!\n";
        return 1;
    }

    if (!saveGroups(groups, "storage/groups_test.txt")) {
        cout << "Group save failed!\n";
        return 1;
    }

    vector<User> loadedUsers;
    vector<Group> loadedGroups;

    cout << "Loading users and groups...\n";

    if (!loadUsers(loadedUsers, "storage/users_test.txt")) {
        cout << "User load failed!\n";
        return 1;
    }

    if (!loadGroups(loadedGroups, "storage/groups_test.txt")) {
        cout << "Group load failed!\n";
        return 1;
    }

    cout << "\nLoaded Users: " << loadedUsers.size() << "\n";
    for (const User& u : loadedUsers) {
        cout << u.id << " - " << u.name << "\n";
    }

    cout << "\nLoaded Groups: " << loadedGroups.size() << "\n";
    for (const Group& group : loadedGroups) {
        cout << group.name << "\n";
        cout << "Group Code: " << group.groupCode << "\n";
        cout << "Members: " << group.memberIds.size() << "\n";
    }

    cout << "\nPersistence test completed!\n";

    return 0;
}
