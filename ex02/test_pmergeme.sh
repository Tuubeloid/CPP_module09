#!/bin/bash

# Path to your executable
EXEC=./PmergeMe

# Color helpers
GREEN="\\033[1;32m"
RED="\\033[1;31m"
NC="\\033[0m" # No Color

# Valid test cases
VALID_TESTS=(
    "3 5 9 7 4"
    "1"
    "1 2 3 4 5 6"
    "9 8 7 6 5"
    "+10 +5 +3"
    "$(shuf -i 1-1000 -n 10)"
)

# Invalid test cases
INVALID_TESTS=(
    "-1 2"
    "2147483648"
    "abc 1"
    "3 5 5 5 2 3"
    "3.14"
    "1e4"
    ""
)

run_valid_tests() {
    echo -e "${GREEN}Running VALID test cases...${NC}"
    for i in "${!VALID_TESTS[@]}"; do
        echo -e "\nTest $((i+1)): ${VALID_TESTS[i]}"
        $EXEC ${VALID_TESTS[i]}
    done
}

run_invalid_tests() {
    echo -e "\n${RED}Running INVALID test cases...${NC}"
    for i in "${!INVALID_TESTS[@]}"; do
        echo -e "\nTest $((i+1)): ${INVALID_TESTS[i]}"
        $EXEC ${INVALID_TESTS[i]}
    done
}

run_stress_test() {
    echo -e "\n${GREEN}Running STRESS test with 3000 random numbers...${NC}"
    ARGS=$(shuf -i 1-100000 -n 3000 | tr '\\n' ' ')
    $EXEC $ARGS
}

# Run all tests
run_valid_tests
run_invalid_tests
run_stress_test