
#pragma once

#include <string>

struct ActivityNode {

    std::string description;
    std::string date;

    ActivityNode* next;

    ActivityNode(
        const std::string& description,
        const std::string& date
    );
};


class ActivityList {

private:

    ActivityNode* head;
    ActivityNode* tail;

    int count;

public:

    ActivityList();

    ~ActivityList();

    // Prevent accidental shallow copies of owned nodes
    ActivityList(const ActivityList&) = delete;
    ActivityList& operator=(const ActivityList&) = delete;

    void addActivity(
        const std::string& description,
        const std::string& date
    );

    void displayActivities() const;

    bool isEmpty() const;

    int size() const;

    void clear();
};
