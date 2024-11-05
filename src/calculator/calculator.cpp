#include "calculator.hpp"
#include "thread_pool.hpp"
#include <iostream>
#include <limits>

using namespace std;
REGISTER_MODULE_CLASS(Calculator, NAME)

Calculator::Calculator() : ModuleInterface(NAME)
{}

void Calculator::execute()
{
    while (!m_finished)
    {
        double num1{}, num2{};
        char operation{};
        cout << "Enter two numbers and an operator (+, -, *, /): ";
        cin >> num1 >> operation >> num2;
        if (cin.fail())
        {
            cout << "Invalid expresion!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        if (m_finished)
        {
            return;
        }

        double result{};
        switch (operation)
        {
        case '+':
        {
            result = add(num1, num2);
        }
        break;
        case '-':
        {
            result = subtract(num1, num2);
        }
        break;
        case '*':
        {
            result = multiply(num1, num2);
        }
        break;
        case '/':
        {
            if (num2 == 0)
            {
                cout << "Error: Division by zero\n";
                return;
            }
            result = divide(num1, num2);
        }
        break;
        default:
        {
            cout << "Error: Invalid operator\n";
        }
        break;
        }
        cout << "Result: " << result << '\n';
    }
}

void Calculator::shutdown()
{
    m_finished = true;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
