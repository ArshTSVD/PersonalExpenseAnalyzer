
#include "UndoStack.hpp"


// Add an operation to the top of the stack
void UndoStack::push(const UndoAction& action) {

    actions.push_back(action);
}


// Remove the most recent operation
bool UndoStack::pop(UndoAction& action) {

    if (actions.empty()) {
        return false;
    }

    action = actions.back();

    actions.pop_back();

    return true;
}


// View the most recent operation without removing it
bool UndoStack::peek(UndoAction& action) const {

    if (actions.empty()) {
        return false;
    }

    action = actions.back();

    return true;
}


// Check whether stack is empty
bool UndoStack::isEmpty() const {

    return actions.empty();
}


// Return number of stored operations
int UndoStack::size() const {

    return static_cast<int>(actions.size());
}


// Clear all operations
void UndoStack::clear() {

    actions.clear();
}
