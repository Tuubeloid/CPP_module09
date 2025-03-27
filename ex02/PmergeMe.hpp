#pragma once

#include <iostream>
#include <deque>
#include <vector>
#include <chrono>
#include <algorithm>
#include <regex>
#include <sstream>
#include <iomanip>

class PmergeMe
{
public:
    PmergeMe();
    ~PmergeMe();
    PmergeMe(PmergeMe const &copy);
    PmergeMe &operator=(PmergeMe const &copy);

    void addNumber(std::string token);
    bool hasDuplicates();
    bool hasError() const;

    void sortVector();
    void sortDeque();

    void printBefore(char **argv);
    void printAfter();
    void printTiming();

    void fordJohnsonSortVector(std::vector<int>& vec);
    void fordJohnsonSortDeque(std::deque<int>& deq);

    void binaryInsertVector(std::vector<int>& main, std::vector<std::pair<int, int>>& pairs);
    void binaryInsertDeque(std::deque<int>& main, std::deque<std::pair<int, int>>& pairs);

    std::vector<int> createJacobsthalSequenceVector(size_t size);
    std::deque<int> createJacobsthalSequenceDeque(size_t size);

    int findPairValueVector(int a, std::vector<std::pair<int, int>>& pairs);
    int findPairValueDeque(int a, std::deque<std::pair<int, int>>& pairs);

private:
    std::deque<int> _deque;
    std::vector<int> _vector;

    double _vector_time;
    double _deque_time;
    bool _error;
};