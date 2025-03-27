#include "PmergeMe.hpp"

// Make new main where we wrap everything in a try catch block

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Error: No arguments provided." << std::endl;
            return 1;
        }

        PmergeMe sorter;

        for (int i = 1; i < argc; ++i)
        {
            sorter.addNumber(argv[i]);
            if (sorter.hasError())
                return 1;
        }

        //sorter.hasDuplicates();
        if (sorter.hasDuplicates())
        {
            std::cerr << "Error: Duplicate values detected." << std::endl;
            return 1;
        }

        sorter.printBefore(argv);
        sorter.sortVector();
        sorter.sortDeque();
        sorter.printAfter();
        sorter.printTiming();
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

/*
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Error: No arguments provided." << std::endl;
        return 1;
    }

    PmergeMe sorter;

    for (int i = 1; i < argc; ++i)
    {
        sorter.addNumber(argv[i]);
        if (sorter.hasError())
            return 1;
    }

    //sorter.hasDuplicates();
    if (sorter.hasDuplicates())
    {
        std::cerr << "Error: Duplicate values detected." << std::endl;
        return 1;
    }

    sorter.printBefore(argv);
    sorter.sortVector();
    sorter.sortDeque();
    sorter.printAfter();
    sorter.printTiming();

    return 0;
}*/