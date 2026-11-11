#include "application/ApplicationState.hpp"
#include "application/ExpenseService.hpp"

#include <iostream>

using namespace std;

int main()
{
    ApplicationState app;
    app.load();

    ExpenseService expenseService(app);

    cout << "===== EXPENSE SERVICE TEST =====\n";

    vector<Expense> expenses =
        expenseService.getAllExpenses();

    cout << "Total Expenses: " << expenses.size() << "\n";

    if (!expenses.empty())
    {
        string firstId = expenses[0].id;

        const Expense* found =
            expenseService.getExpenseById(firstId);

        if (found != nullptr)
        {
            cout << "Search Test: PASSED\n";
            cout << "Found Expense: " << found->title << "\n";
            cout << "Expense ID: " << found->id << "\n";
        }
        else
        {
            cout << "Search Test: FAILED\n";
        }
    }

    cout << "===== TEST COMPLETED =====\n";

    return 0;
}