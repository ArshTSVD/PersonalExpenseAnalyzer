#include "application/ApplicationState.hpp"
#include <iostream>

int main()
{
    ApplicationState app;

    app.load();

    std::cout << "Current User: "
              << app.currentUserId << "\n";

    std::cout << "Expenses: "
              << app.expenses.size() << "\n";

    std::cout << "Groups: "
              << app.groups.size() << "\n";

    return 0;
}