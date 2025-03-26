#include "RPN.hpp"

//fix: ./RPN "5 -3 +"  # Expected: Error (input spec says only positive integers < 10)
//fix: ./RPN "2.5 2 +" # Expected: Error (floating point not allowed)


RPN::RPN()
{
    _result = 0;
    _error = false;
}

RPN::~RPN()
{
}

void RPN::calculate(std::string str)
{
    std::istringstream iss(str);
    std::string token;
    float a;
    float b;

    while (iss >> token)
    {
        if (token == "+" || token == "-" || token == "*" || token == "/")
        {
            if (_stack.size() < 2)
            {
                std::cout << "Error 1" << std::endl;
                _error = true;
                return;
            }
            a = _stack.top();
            _stack.pop();
            b = _stack.top();
            _stack.pop();
            if (token == "+")
                _stack.push(b + a);
            else if (token == "-")
                _stack.push(b - a);
            else if (token == "*")
                _stack.push(b * a);
            else if (token == "/")
            {
                /*
                if (a == 0)
                {
                    std::cout << "Error 2" << std::endl;
                    _error = true;
                    return;
                }
                */
                _stack.push(b / a);
            }
        }
        else
        {
            try
            {
                // also add check that no floating point numbers are allowed
                if (std::stoi(token) > 9 || std::stoi(token) < 0 || token.find('.') != std::string::npos)
                {
                    std::cout << "Error 3" << std::endl;
                    _error = true;
                    return;
                }
            }
            catch (std::exception &e)
            {
                std::cout << "Error 4" << std::endl;
                _error = true;
                return;
            }
            _stack.push(std::stoi(token));
        }
    }
    if (_stack.size() != 1)
    {
        std::cout << "Error 5" << std::endl;
        _error = true;
    }
    else
        _result = _stack.top();
}

void RPN::printResult()
{
    if (_error)
    {
        std::cerr << "Error" << std::endl;
    }
    else
    {
        std::cout << _result << std::endl;
    }
}

void RPN::printError()
{
    std::cerr << "Error" << std::endl;
}

void RPN::printStack()
{
    std::stack<float> tmp = _stack;

    while (!tmp.empty())
    {
        std::cout << tmp.top() << std::endl;
        tmp.pop();
    }
}