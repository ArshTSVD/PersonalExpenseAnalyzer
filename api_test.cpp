#include "application/ApplicationState.hpp"
#include "application/ExpenseService.hpp"
#include "api/ExpenseApi.hpp"

#include <iostream>

using namespace std;

int main()
{
    ApplicationState app;
    app.load();

    ExpenseService service(app);
    ExpenseApi api(service);

    cout << "===== EXPENSE API TEST =====\n\n";

    cout << "1. GET ALL EXPENSES\n";
    cout << api.getAllExpenses() << "\n\n";

    cout << "2. GET EXPENSE BY ID\n";
    cout << api.getExpenseById("EXP0001") << "\n\n";

    cout << "3. GET INVALID EXPENSE\n";
    cout << api.getExpenseById("INVALID_ID") << "\n\n";

    cout << "===== API TEST COMPLETED =====\n";

    return 0;
}