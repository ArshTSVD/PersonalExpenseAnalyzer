
#include <iostream>
#include "httplib.h"

#include "../application/ApplicationState.hpp"
#include "../application/ExpenseService.hpp"
#include "ExpenseApi.hpp"
#include <nlohmann/json.hpp>

int main()
{
    // 1. Load application data
    ApplicationState state;
    state.load();

    // 2. Create service and API objects
    ExpenseService expenseService(state);
    ExpenseApi expenseApi(expenseService);

    // 3. Create HTTP server
    httplib::Server server;
    // Enable CORS for frontend integration
    server.set_default_headers({
    {"Access-Control-Allow-Origin", "*"},
    {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
    {"Access-Control-Allow-Headers", "Content-Type"}
});

// Handle browser preflight requests
server.Options(R"(/api/.*)",
    [](const httplib::Request& req, httplib::Response& res)
    {
        res.status = 204;
    }
);

    // 4. GET all expenses
    server.Get("/api/expenses",
        [&](const httplib::Request& req, httplib::Response& res)
        {
            std::string result = expenseApi.getAllExpenses();

            res.set_content(result, "application/json");
        }
    );

    // 5. GET expense by ID
    server.Get("/api/expenses/:id",
        [&](const httplib::Request& req, httplib::Response& res)
        {
            std::string expenseId = req.path_params.at("id");

            std::string result = expenseApi.getExpenseById(expenseId);

            res.set_content(result, "application/json");
        }
    );


server.Post("/api/expenses",
    [&](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            auto body = nlohmann::json::parse(req.body);

            Expense expense;

            expense.title = body.value("title", std::string(""));
            expense.category = body.value("category", std::string(""));
            expense.amountPaise = body.value("amountPaise", 0LL);
            expense.date = body.value("date", std::string(""));
            expense.note = body.value("note", std::string(""));

            if (expense.title.empty() ||
                expense.category.empty() ||
                expense.amountPaise <= 0 ||
                expense.date.empty())
            {
                res.status = 400;
                res.set_content(
                    R"({"success":false,"message":"Invalid expense data"})",
                    "application/json"
                );
                return;
            }

            std::string result = expenseApi.createExpense(expense);

            res.set_content(result, "application/json");
        }
        catch (const std::exception& e)
        {
            res.status = 400;
            res.set_content(
                R"({"success":false,"message":"Invalid JSON request"})",
                "application/json"
            );
        }
    }
);


server.Put("/api/expenses/:id",
    [&](const httplib::Request& req, httplib::Response& res)
    {
        try
        {
            auto body = nlohmann::json::parse(req.body);

            Expense updatedExpense;
            updatedExpense.title = body.value("title", std::string(""));
            updatedExpense.category = body.value("category", std::string(""));
            updatedExpense.amountPaise = body.value("amountPaise", 0LL);
            updatedExpense.date = body.value("date", std::string(""));
            updatedExpense.note = body.value("note", std::string(""));

            if (updatedExpense.title.empty() ||
                updatedExpense.category.empty() ||
                updatedExpense.amountPaise <= 0 ||
                updatedExpense.date.empty())
            {
                res.status = 400;
                res.set_content(
                    R"({"success":false,"message":"Invalid expense data"})",
                    "application/json"
                );
                return;
            }

            std::string id = req.path_params.at("id");
            std::string result = expenseApi.updateExpense(id, updatedExpense);

            res.set_content(result, "application/json");
        }
        catch (const std::exception& e)
        {
            res.status = 400;
            res.set_content(
                R"({"success":false,"message":"Invalid JSON request"})",
                "application/json"
            );
        }
    }
);


server.Delete("/api/expenses/:id",
    [&](const httplib::Request& req, httplib::Response& res)
    {
        std::string id = req.path_params.at("id");
        std::string result = expenseApi.deleteExpense(id);

        res.set_content(result, "application/json");
    }
);



    // 6. Start server
    std::cout << "====================================\n";
    std::cout << " PERSONAL EXPENSE ANALYZER SERVER\n";
    std::cout << "====================================\n";
    std::cout << "Server running at: http://localhost:8080\n";
    std::cout << "Press Ctrl + C to stop the server.\n";

    if (!server.listen("127.0.0.1", 8080))
    {
        std::cerr << "Failed to start server!\n";
        return 1;
    }

    return 0;
}
