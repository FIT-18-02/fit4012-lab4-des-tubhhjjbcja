#!/usr/bin/env bash
set -euo pipefail

EXPECTED="0111111010111111010001001001001100100011111110101111101011111000"
PLAINTEXT="0001001000110100010101100111100010011010101111001101111011110001"
KEY="0001001100110100010101110111100110011011101111001101111111110001"

g++ -std=c++17 -Wall -Wextra -pedantic des.cpp -o des_test
OUTPUT=$(printf "1\n%s\n%s\n" "$PLAINTEXT" "$KEY" | ./des_test)
RESULT=$(printf "%s\n" "$OUTPUT" | grep -Eo '[01]{64,}' | tail -n 1)

if [[ "$RESULT" != "$EXPECTED" ]]; then
  echo "[FAIL] DES sample encrypt output is wrong"
  echo "Expected: $EXPECTED"
  echo "Actual:   $RESULT"
  rm -f des_test
  exit 1
fi

echo "[PASS] DES sample encrypt produced the expected ciphertext."
rm -f des_test
