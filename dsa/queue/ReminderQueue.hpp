
#pragma once

#include <vector>

#include "../../models/models.hpp"

class ReminderQueue {

private:
    std::vector<Reminder> reminders;
    size_t frontIndex = 0;

public:

    void enqueue(const Reminder& reminder);

    bool dequeue(Reminder& reminder);

    bool peek(Reminder& reminder) const;

    bool isEmpty() const;

    int size() const;

    void clear();
};
