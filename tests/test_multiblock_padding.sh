#!/usr/bin/env bash
set -euo pipefail

PLAINTEXT="1011001110001111000011111010101011110000111100001111000011110000101"
KEY="0001001100110100010101110111100110011011101111001101111111110001"
PAD_COUNT=$(( (64 - (${#PLAINTEXT} % 64)) % 64 ))
PADDED="$PLAINTEXT$(printf '0%.0s' $(seq 1 "$PAD_COUNT"))"

g++ -std=c++17 -Wall -Wextra -pedantic des.cpp -o des_test
CIPHERTEXT=$(printf "1\n%s\n%s\n" "$PLAINTEXT" "$KEY" | ./des_test | grep -Eo '[01]{64,}' | tail -n 1)
DECRYPTED=$(printf "2\n%s\n%s\n" "$CIPHERTEXT" "$KEY" | ./des_test | grep -Eo '[01]{64,}' | tail -n 1)

if (( ${#CIPHERTEXT} != 128 )); then
  echo "[FAIL] Ciphertext length should be two 64-bit blocks after zero padding"
  echo "Length: ${#CIPHERTEXT}"
  rm -f des_test
  exit 1
fi

if [[ "$DECRYPTED" != "$PADDED" ]]; then
  echo "[FAIL] Multi-block decrypt output did not match zero-padded plaintext"
  echo "Expected: $PADDED"
  echo "Actual:   $DECRYPTED"
  rm -f des_test
  exit 1
fi

echo "[PASS] Multi-block encryption and zero padding work correctly."
rm -f des_test
