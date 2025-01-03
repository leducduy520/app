#pragma once
#include "module_manager.h"

using namespace dld;

class Calculator : public ModuleInterface
{
public:
    Calculator();
    void execute() override;
    void shutdown() override;

private:
    bool m_finished;
    template <typename T>
    T add(T num1, T num2) const;
    template <typename T>
    T subtract(T num1, T num2) const;
    template <typename T>
    T multiply(T num1, T num2) const;
    template <typename T>
    T divide(T num1, T num2) const;
};

template <typename T>
inline T Calculator::add(T num1, T num2) const
{
    return num1 + num2;
}

template <typename T>
inline T Calculator::subtract(T num1, T num2) const
{
    return num1 - num2;
}

template <typename T>
inline T Calculator::multiply(T num1, T num2) const
{
    return num1 * num2;
}

template <typename T>
inline T Calculator::divide(T num1, T num2) const
{
    return num1 / num2;
}
