#!/usr/bin/env bash
set -euo pipefail

PLAINTEXT="0001001000110100010101100111100010011010101111001101111011110001"
CORRECT_KEY="0001001100110100010101110111100110011011101111001101111111110001"
WRONG_KEY="1111000011110000111100001111000011110000111100001111000011110000"

g++ -std=c++17 -Wall -Wextra -pedantic des.cpp -o des_test
CIPHERTEXT=$(printf "1\n%s\n%s\n" "$PLAINTEXT" "$CORRECT_KEY" | ./des_test | grep -Eo '[01]{64,}' | tail -n 1)
DECRYPTED=$(printf "2\n%s\n%s\n" "$CIPHERTEXT" "$WRONG_KEY" | ./des_test | grep -Eo '[01]{64,}' | tail -n 1)

if [[ "$DECRYPTED" == "$PLAINTEXT" ]]; then
  echo "[FAIL] Wrong key unexpectedly recovered the original plaintext"
  rm -f des_test
  exit 1
fi

echo "[PASS] Decryption with the wrong key does not recover the original plaintext."
rm -f des_test
