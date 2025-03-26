#!/bin/bash

PROGRAM=./RPN
TOTAL=0
PASSED=0

print_result() {
	if [ "$1" -eq 0 ]; then
		echo -e "✅ \033[1;32mPASS\033[0m: $2"
		((PASSED++))
	else
		echo -e "❌ \033[1;31mFAIL\033[0m: $2"
	fi
	((TOTAL++))
}

# --------- TEST CASES -----------

echo "=== Running RPN Tests ==="

# Valid test cases
declare -A tests
tests["8 9 * 9 - 9 - 9 - 4 - 1 +"]="42"
tests["7 7 * 7 -"]="42"
tests["1 2 * 2 / 2 * 2 4 - +"]="0"
tests["1 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 +"]="45"

# Error test cases (stderr expected)
errors=(
	""
	"1 2 + +"
	"2 a +"
	"5 -3 +"
	"2.5 2 +"
	"1 2 3 4 +"
	"9 1 - - - -"
	"+ - * /"
	"(1 + 1)"
	"8 2 * 3 / #comment"
)

for expr in "${!tests[@]}"; do
	output=$($PROGRAM "$expr" 2>/dev/null)
	expected="${tests[$expr]}"
	if [[ "$output" == "$expected" ]]; then
		print_result 0 "$expr => $output"
	else
		print_result 1 "$expr => got '$output', expected '$expected'"
	fi
done

for expr in "${errors[@]}"; do
	$PROGRAM "$expr" >/dev/null 2>tmp_error
	if [[ -s tmp_error ]]; then
		print_result 0 "Error case '$expr' properly triggered error"
	else
		print_result 1 "Error case '$expr' failed to trigger error"
	fi
done
rm -f tmp_error

# --------- FUZZ TESTS -----------

echo "=== Running Fuzz Tests (Randomized Inputs) ==="

generate_random_expr() {
	count=$((RANDOM % 5 + 3)) # 3 to 7 numbers
	stack=""
	expr=""
	for ((i=0; i<count; i++)); do
		num=$((RANDOM % 9 + 1))
		expr="$expr $num"
		stack="$stack $num"
	done
	for ((i=0; i<count-1; i++)); do
		op=$((RANDOM % 4))
		case $op in
			0) op="+" ;;
			1) op="-" ;;
			2) op="*" ;;
			3) op="/" ;;
		esac
		expr="$expr $op"
	done
	echo "$expr"
}

for ((i=0; i<10; i++)); do
	expr=$(generate_random_expr)
	output=$($PROGRAM "$expr" 2>tmp_error)
	status=$?
	if [[ "$status" -eq 0 && -z "$(cat tmp_error)" ]]; then
		print_result 0 "Fuzz test '$expr' => $output"
	else
		print_result 1 "Fuzz test '$expr' triggered error"
	fi
done
rm -f tmp_error

# --------- SUMMARY -----------

echo "=== Summary ==="
echo -e "✅ Passed: \033[1;32m$PASSED\033[0m / $TOTAL"