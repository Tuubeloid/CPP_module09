#pragma once

#include <iostream>
#include <stack>
#include <string>
#include <sstream>

class RPN
{
    public:
        RPN();
        ~RPN();
        void calculate(std::string str);
        void printResult();
        void printError();
        void printStack();

    private:
        std::stack<float> _stack;
        float _result;
        bool _error;
};