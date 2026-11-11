
#include "ReminderQueue.hpp"


// Add a reminder at the rear
void ReminderQueue::enqueue(const Reminder& reminder) {

    reminders.push_back(reminder);
}


// Remove the oldest reminder
bool ReminderQueue::dequeue(Reminder& reminder) {

    if (isEmpty()) {
        return false;
    }

    reminder = reminders[frontIndex];

    frontIndex++;

    // Reset storage when all reminders are processed
    if (frontIndex == reminders.size()) {

        reminders.clear();

        frontIndex = 0;
    }

    return true;
}


// View the oldest reminder
bool ReminderQueue::peek(Reminder& reminder) const {

    if (isEmpty()) {
        return false;
    }

    reminder = reminders[frontIndex];

    return true;
}


// Check whether queue is empty
bool ReminderQueue::isEmpty() const {

    return frontIndex >= reminders.size();
}


// Number of pending reminders
int ReminderQueue::size() const {

    return static_cast<int>(
        reminders.size() - frontIndex
    );
}


// Remove all pending reminders
void ReminderQueue::clear() {

    reminders.clear();

    frontIndex = 0;
}
