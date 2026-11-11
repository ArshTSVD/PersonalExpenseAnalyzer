
#pragma once

#include <vector>
#include <string>

#include "../../models/models.hpp"

enum class UndoOperation {
    ADD_EXPENSE,
    UPDATE_EXPENSE,
    DELETE_EXPENSE
};

struct UndoAction {
    UndoOperation operation;
    Expense expenseData;
};

class UndoStack {

private:
    std::vector<UndoAction> actions;

public:

    void push(const UndoAction& action);

    bool pop(UndoAction& action);

    bool peek(UndoAction& action) const;

    bool isEmpty() const;

    int size() const;

    void clear();
};
