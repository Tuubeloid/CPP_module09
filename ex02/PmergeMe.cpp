#include "PmergeMe.hpp"

PmergeMe::PmergeMe() : _vector_time(0), _deque_time(0), _error(false) {}

PmergeMe::~PmergeMe() {}

PmergeMe::PmergeMe(PmergeMe const &copy)
{
    *this = copy;
}

PmergeMe &PmergeMe::operator=(PmergeMe const &copy)
{
    if (this != &copy)
	{
        _vector = copy._vector;
        _deque = copy._deque;
        _vector_time = copy._vector_time;
        _deque_time = copy._deque_time;
        _error = copy._error;
    }
    return (*this);
}

// ------------------------------
// Input Handling
// ------------------------------

bool PmergeMe::hasError() const
{
    return (_error);
}

// Check for duplicates in the input
bool PmergeMe::hasDuplicates()
{
	std::sort(_vector.begin(), _vector.end());
	auto it = std::unique(_vector.begin(), _vector.end());
	return (it != _vector.end());
}


// Adds a number to the internal containers if it's valid
void PmergeMe::addNumber(std::string token)
{
    // Only accept positive integers (optionally prefixed with +)
    if (!std::regex_match(token, std::regex("[+]?[0-9]+")))
	{
        _error = true;
        std::cerr << "Error: Invalid token '" << token << "'\n";
        return;
    }

    // Convert string to long to check for overflow
    long value = std::strtol(token.c_str(), NULL, 10);
    if (value > INT32_MAX)
	{
        _error = true;
        std::cerr << "Error: Value too large: " << token << "\n";
        return;
    }

    // Add valid number to both containers
    _vector.push_back((int)value);
    _deque.push_back((int)value);
}

// ------------------------------
// Output Utilities
// ------------------------------

// Print input values before sorting
void PmergeMe::printBefore(char **argv)
{
    std::cout << "Before:\t";
    for (int i = 1; argv[i]; ++i)
        std::cout << argv[i] << " ";
    std::cout << std::endl;
}

// Print the sorted vector
void PmergeMe::printAfter()
{
    std::cout << "After:\t";
    for (size_t i = 0; i < _vector.size(); ++i)
        std::cout << _vector[i] << " ";
    std::cout << std::endl;
}

// Print timing for both vector and deque sorting
void PmergeMe::printTiming()
{
	std::cout << std::fixed << std::setprecision(6);
    std::cout << "Time to process a range of " << _vector.size() << " elements with std::vector : " << _vector_time << " s\n";
    std::cout << "Time to process a range of " << _deque.size() << " elements with std::deque : " << _deque_time << " s\n";
}

// ------------------------------
// Jacobsthal Sequence Generation
// ------------------------------

// Jacobsthal numbers: J(n) = J(n-1) + 2 * J(n-2), with J(0) = 0, J(1) = 1
// This function generates the sequence until it exceeds the size of the pair list
std::vector<int> PmergeMe::createJacobsthalSequenceVector(size_t size)
{
    std::vector<int> seq;

    // The Jacobsthal sequence is defined recursively:
    // J(0) = 0, J(1) = 1
    // J(n) = J(n - 1) + 2 * J(n - 2)
    //
    // In the context of Ford-Johnson sorting, we skip J(0) = 0
    // and start from J(1) = 1, J(2) = 3 since 0 is not a valid insertion index.

    seq.push_back(1); // J(1)
    seq.push_back(3); // J(2)

    // We continue generating terms of the Jacobsthal sequence
    // until the current term exceeds the number of available pairs.
    //
    // Each number in the Jacobsthal sequence determines a *batch* of
    // 'min' elements (b-values) to insert during the binary insertion phase.
    //
    // The idea is to stagger insertions in such a way that the sorted chain
    // has grown enough to reduce comparisons when the next element is inserted.

    while (seq.back() < (int)size)
    {
        // Get the next term using the recurrence relation:
        // J(n) = J(n-1) + 2 * J(n-2)
        int next = seq.back() + 2 * seq[seq.size() - 2];
        seq.push_back(next);
    }

    // Once the last term in the sequence is greater than or equal to the number
    // of elements we need to insert, we stop. The sequence is ready.
    return (seq);
}

std::deque<int> PmergeMe::createJacobsthalSequenceDeque(size_t size)
{
    std::deque<int> seq;
    seq.push_back(1);
    seq.push_back(3);
    while (seq.back() < (int)size)
        seq.push_back(seq.back() + 2 * seq[seq.size() - 2]);
    return (seq);
}

// ------------------------------
// Pair Value Lookup
// ------------------------------

// Finds the secondary (smaller) value from a pair based on the larger (first) value
int PmergeMe::findPairValueVector(int a, std::vector<std::pair<int, int>>& pairs)
{
    for (size_t i = 0; i < pairs.size(); ++i)
        if (pairs[i].first == a) return pairs[i].second;
    return (-1); // Not found
}

int PmergeMe::findPairValueDeque(int a, std::deque<std::pair<int, int>>& pairs)
{
    for (size_t i = 0; i < pairs.size(); ++i)
        if (pairs[i].first == a) return pairs[i].second;
    return (-1); // Not found
}

// ------------------------------
// Binary Insertion using Jacobsthal Order
// ------------------------------

// This function inserts the "smaller" values (b values) from the pair list into the main chain
// The order of insertion is determined by the Jacobsthal sequence for optimal comparisons
void PmergeMe::binaryInsertVector(std::vector<int>& main, std::vector<std::pair<int, int>>& pairs)
{
    // Generate the Jacobsthal sequence up to the number of (max, min) pairs.
    // This sequence determines the *order* in which we insert the 'min' elements for optimal comparison efficiency.
    std::vector<int> seq = createJacobsthalSequenceVector(pairs.size());

    // We create a copy of the current main chain (which only has the 'max' values).
    // This is important because we'll be inserting into `main` during this function,
    // and we want to preserve the original ordering/indexes of 'a' values to find their corresponding 'b'.
    std::vector<int> main_copy = main;

    // Step 1: Insert the first 'b' value (the one paired with the first 'a') at the front of the sorted chain.
    // This is a special-case pre-insertion step that always happens before the Jacobsthal loop.
    int b1 = findPairValueVector(main_copy[0], pairs);
    if (b1 != -1)
        main.insert(main.begin(), b1); // Insert before all 'a' values

    // If we only have one pair, we inserted both 'a' and 'b' already, so we're done.
    if (pairs.size() == 1)
        return;

    // Step 2: Iterate through the Jacobsthal sequence.
    // For each segment between two sequence values, we insert values in **reverse** order.
    // This ordering reduces the number of comparisons needed in the worst case.
    for (size_t i = 1; i < seq.size(); ++i)
    {
        size_t upper = seq[i];     // Current Jacobsthal number (e.g., 3, 5, 11, ...)
        size_t lower = seq[i - 1]; // Previous Jacobsthal number

        // Step 3: Go in reverse within this interval: [upper - 1, ..., lower]
        // This staggered reverse-order insert helps balance the sorted chain and minimize comparisons.
        for (size_t j = upper; j > lower; --j)
        {
            // Skip if the index is out of bounds (can happen when Jacobsthal exceeds pair size)
            if (j > pairs.size()) continue;

            // Step 4: Find the corresponding 'a' value and look up its 'b' pair
            int a = main_copy[j - 1];         // Get the 'a' from the original main copy
            int b = findPairValueVector(a, pairs);  // Find the associated 'b' value
            if (b == -1) continue;            // Safety check, skip if not found

            // Step 5: Binary insert the 'b' into the sorted main chain using lower_bound
            // lower_bound returns the first position where b can go to keep the vector sorted
            auto pos = std::lower_bound(main.begin(), main.end(), b);

            // Insert 'b' at the calculated position
            main.insert(pos, b);
        }
    }

    // After this process, all 'b' values (min elements) are inserted back into the main chain
    // in an order designed to reduce the number of comparisons thanks to the Jacobsthal sequence.
}

void PmergeMe::binaryInsertDeque(std::deque<int>& main, std::deque<std::pair<int, int>>& pairs)
{
    std::deque<int> seq = createJacobsthalSequenceDeque(pairs.size());
    std::deque<int> main_copy = main;

    int b1 = findPairValueDeque(main_copy[0], pairs);
    if (b1 != -1)
        main.insert(main.begin(), b1);

    if (pairs.size() == 1)
        return;

    for (size_t i = 1; i < seq.size(); ++i)
	{
        size_t upper = seq[i], lower = seq[i - 1];

        for (size_t j = upper; j > lower; --j)
		{
            if (j > pairs.size()) continue;

            int a = main_copy[j - 1];
            int b = findPairValueDeque(a, pairs);
            if (b == -1) continue;

            auto pos = std::lower_bound(main.begin(), main.end(), b);
            main.insert(pos, b);
        }
    }
}

// ------------------------------
// Ford-Johnson Recursive Sort
// ------------------------------

// Sorts the input vector using the merge-insert Ford-Johnson algorithm
// Step 1: Pair elements into (max, min) and collect them into a list
// Step 2: Recursively sort the max elements (main chain)
// Step 3: Use Jacobsthal order to insert min elements back into the main chain
// Step 4: Insert leftover value if input size was odd
void PmergeMe::fordJohnsonSortVector(std::vector<int>& vec)
{
    // Base case: if the input vector contains 0 or 1 elements, it is already sorted
    if (vec.size() <= 1)
        return;

    // Step 1: Pair the input values into (max, min) pairs
    // -----------------------------------------------
    // For every pair of adjacent elements, make a pair where the larger value comes first.
    // This ensures that we always collect the maximum into the "main" chain.
    std::vector<std::pair<int, int>> pairs;

    for (size_t i = 0; i + 1 < vec.size(); i += 2)
    {
        if (vec[i] < vec[i + 1])
            std::swap(vec[i], vec[i + 1]); // Ensure first element is the larger one
        pairs.push_back(std::make_pair(vec[i], vec[i + 1])); // (max, min)
    }

    // Step 2: Handle leftover element if input size is odd
    // -----------------------------------------------------
    // If there's an odd number of elements, the last one doesn't belong to a pair.
    // We'll store it separately and insert it at the end of the sort.
    bool hasLeftover = vec.size() % 2 != 0;
    int leftover = hasLeftover ? vec.back() : 0;

    // Step 3: Create the "main" chain from the maximum values of each pair
    // ---------------------------------------------------------------------
    // The main chain will recursively be sorted, and the "min" elements (the second
    // part of each pair) will be inserted back later in optimal order.
    std::vector<int> main;
    for (size_t i = 0; i < pairs.size(); ++i)
        main.push_back(pairs[i].first);

    // Step 4: Recursively sort the main chain
    // ----------------------------------------
    // This is where the Ford-Johnson merge-insert sort becomes recursive.
    // We sort the main chain first, which contains the larger half of each pair.
    fordJohnsonSortVector(main);

    // Step 5: Insert the "min" elements (b-values) back into the main chain
    // ----------------------------------------------------------------------
    // We use the Jacobsthal sequence to decide the optimal order for insertion
    // to minimize the number of comparisons.
    binaryInsertVector(main, pairs);

    // Step 6: Insert the leftover element if it exists
    // -------------------------------------------------
    // Finally, insert the leftover element (if present) into the sorted chain
    // using binary search for optimal position.
    if (hasLeftover)
    {
        auto pos = std::lower_bound(main.begin(), main.end(), leftover);
        main.insert(pos, leftover);
    }

    // Step 7: Update the original vector with the sorted result
    // ----------------------------------------------------------
    vec = main;
}

void PmergeMe::fordJohnsonSortDeque(std::deque<int>& deq)
{
    if (deq.size() <= 1) return;

    std::deque<std::pair<int, int>> pairs;

    for (size_t i = 0; i + 1 < deq.size(); i += 2)
	{
        if (deq[i] < deq[i + 1]) std::swap(deq[i], deq[i + 1]);
        pairs.push_back(std::make_pair(deq[i], deq[i + 1]));
    }

    bool hasLeftover = deq.size() % 2 != 0;
    int leftover = hasLeftover ? deq.back() : 0;

    std::deque<int> main;
    for (size_t i = 0; i < pairs.size(); ++i)
        main.push_back(pairs[i].first);

    fordJohnsonSortDeque(main);
    binaryInsertDeque(main, pairs);

    if (hasLeftover)
	{
        auto pos = std::lower_bound(main.begin(), main.end(), leftover);
        main.insert(pos, leftover);
    }

    deq = main;
}

// ------------------------------
// Public Entry Points (Timing)
// ------------------------------

// Measures and stores execution time for vector sort
void PmergeMe::sortVector()
{
    auto start = std::chrono::high_resolution_clock::now();
    fordJohnsonSortVector(_vector);
    auto end = std::chrono::high_resolution_clock::now();
    _vector_time = std::chrono::duration<double>(end - start).count();
}

// Measures and stores execution time for deque sort
void PmergeMe::sortDeque()
{
    auto start = std::chrono::high_resolution_clock::now();
    fordJohnsonSortDeque(_deque);
    auto end = std::chrono::high_resolution_clock::now();
    _deque_time = std::chrono::duration<double>(end - start).count();
}