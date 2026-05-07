#!/usr/bin/env bash
set -euo pipefail

PLAINTEXT="1010101111001101111001101010101011110000111100001111000011110000"
KEY="0001001100110100010101110111100110011011101111001101111111110001"

g++ -std=c++17 -Wall -Wextra -pedantic des.cpp -o des_test
CIPHERTEXT=$(printf "1\n%s\n%s\n" "$PLAINTEXT" "$KEY" | ./des_test | grep -Eo '[01]{64,}' | tail -n 1)
DECRYPTED=$(printf "2\n%s\n%s\n" "$CIPHERTEXT" "$KEY" | ./des_test | grep -Eo '[01]{64,}' | tail -n 1)

if [[ "$DECRYPTED" != "$PLAINTEXT" ]]; then
  echo "[FAIL] decrypt(encrypt(plaintext)) did not return the original plaintext"
  echo "Plaintext: $PLAINTEXT"
  echo "Decrypted: $DECRYPTED"
  rm -f des_test
  exit 1
fi

echo "[PASS] DES encrypt/decrypt round-trip returned the original plaintext."
rm -f des_test
