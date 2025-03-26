#include "RPN.hpp"

int main(int ac, char **av)
{
    RPN rpn;
    try
    {
        if (ac != 2)
        {
            throw std::string("Usage: RPN [expression]");
        }
        rpn.calculate(av[1]);
        rpn.printResult();
    }
    catch (std::string &e)
    {
        rpn.printError();
        return 1;
    }
    return 0;
}
