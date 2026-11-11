
#include "ActivityList.hpp"

#include <iostream>

using namespace std;


// ==========================================
// NODE CONSTRUCTOR
// ==========================================

ActivityNode::ActivityNode(
    const string& description,
    const string& date
) {

    this->description = description;
    this->date = date;

    next = nullptr;
}


// ==========================================
// LIST CONSTRUCTOR
// ==========================================

ActivityList::ActivityList() {

    head = nullptr;
    tail = nullptr;

    count = 0;
}


// ==========================================
// LIST DESTRUCTOR
// ==========================================

ActivityList::~ActivityList() {

    clear();
}


// ==========================================
// ADD ACTIVITY
// ==========================================

void ActivityList::addActivity(
    const string& description,
    const string& date
) {

    ActivityNode* newNode =
        new ActivityNode(description, date);

    // First node
    if (head == nullptr) {

        head = newNode;
        tail = newNode;
    }

    // Append at the end
    else {

        tail->next = newNode;

        tail = newNode;
    }

    count++;
}


// ==========================================
// DISPLAY ACTIVITIES
// ==========================================

void ActivityList::displayActivities() const {

    cout << "\n===== ACTIVITY HISTORY =====\n";

    if (isEmpty()) {

        cout << "No activities recorded.\n";

        return;
    }

    ActivityNode* current = head;

    int number = 1;

    while (current != nullptr) {

        cout << number << ". "
             << current->description
             << " | Date: "
             << current->date
             << "\n";

        current = current->next;

        number++;
    }
}


// ==========================================
// CHECK EMPTY
// ==========================================

bool ActivityList::isEmpty() const {

    return head == nullptr;
}


// ==========================================
// GET SIZE
// ==========================================

int ActivityList::size() const {

    return count;
}


// ==========================================
// CLEAR LIST
// ==========================================

void ActivityList::clear() {

    ActivityNode* current = head;

    while (current != nullptr) {

        ActivityNode* nextNode = current->next;

        delete current;

        current = nextNode;
    }

    head = nullptr;
    tail = nullptr;

    count = 0;
}
