#include "ExpenseApi.hpp"

#include <sstream>
#include <iomanip>

using namespace std;

namespace
{
    // Safely escape strings for JSON.
    string escapeJSON(const string& input)
    {
        ostringstream output;

        for (unsigned char ch : input)
        {
            switch (ch)
            {
                case '"':
                    output << "\\\"";
                    break;

                case '\\':
                    output << "\\\\";
                    break;

                case '\n':
                    output << "\\n";
                    break;

                case '\r':
                    output << "\\r";
                    break;

                case '\t':
                    output << "\\t";
                    break;

                default:
                    if (ch < 32)
                    {
                        output << "\\u"
                               << hex
                               << setw(4)
                               << setfill('0')
                               << static_cast<int>(ch)
                               << dec;
                    }
                    else
                    {
                        output << static_cast<char>(ch);
                    }
            }
        }

        return output.str();
    }

    // Convert one expense into JSON.
    string expenseToJSON(const Expense& expense)
    {
        ostringstream json;

        json << "{"
             << "\"id\":\"" << escapeJSON(expense.id) << "\","
             << "\"userId\":\"" << escapeJSON(expense.userId) << "\","
             << "\"title\":\"" << escapeJSON(expense.title) << "\","
             << "\"category\":\"" << escapeJSON(expense.category) << "\","
             << "\"amountPaise\":" << expense.amountPaise << ","
             << "\"date\":\"" << escapeJSON(expense.date) << "\","
             << "\"note\":\"" << escapeJSON(expense.note) << "\""
             << "}";

        return json.str();
    }

    string successResponse(const string& data)
    {
        return "{\"success\":true,"
               "\"message\":\"Request successful\","
               "\"data\":" + data + "}";
    }

    string errorResponse(const string& message)
    {
        return "{\"success\":false,"
               "\"message\":\"" + escapeJSON(message) + "\","
               "\"data\":null}";
    }
}

ExpenseApi::ExpenseApi(ExpenseService& service)
    : expenseService(service)
{
}

string ExpenseApi::getAllExpenses() const
{
    vector<Expense> expenses =
        expenseService.getAllExpenses();

    ostringstream json;

    json << "[";

    for (size_t i = 0; i < expenses.size(); i++)
    {
        if (i > 0)
            json << ",";

        json << expenseToJSON(expenses[i]);
    }

    json << "]";

    return successResponse(json.str());
}

string ExpenseApi::getExpenseById(
    const string& expenseId
) const
{
    const Expense* expense =
        expenseService.getExpenseById(expenseId);

    if (expense == nullptr)
        return errorResponse("Expense not found");

    return successResponse(expenseToJSON(*expense));
}

string ExpenseApi::createExpense(Expense expense)
{
    string id = expenseService.createExpense(expense);

    if (id.empty())
        return errorResponse("Could not create expense");

    return successResponse(
        "{\"id\":\"" + escapeJSON(id) + "\"}"
    );
}

string ExpenseApi::updateExpense(
    const string& expenseId,
    const Expense& updatedData
)
{
    if (!expenseService.updateExpense(expenseId, updatedData))
        return errorResponse("Could not update expense");

    return successResponse(
        "{\"id\":\"" + escapeJSON(expenseId) + "\"}"
    );
}

string ExpenseApi::deleteExpense(
    const string& expenseId
) const
{
    if (!expenseService.removeExpense(expenseId))
        return errorResponse("Could not delete expense");

    return successResponse(
        "{\"id\":\"" + escapeJSON(expenseId) + "\"}"
    );
}