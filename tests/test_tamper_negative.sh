#!/usr/bin/env bash
set -euo pipefail

PLAINTEXT="0001001000110100010101100111100010011010101111001101111011110001"
KEY="0001001100110100010101110111100110011011101111001101111111110001"

g++ -std=c++17 -Wall -Wextra -pedantic des.cpp -o des_test
CIPHERTEXT=$(printf "1\n%s\n%s\n" "$PLAINTEXT" "$KEY" | ./des_test | grep -Eo '[01]{64,}' | tail -n 1)
FIRST=${CIPHERTEXT:0:1}
if [[ "$FIRST" == "0" ]]; then
  TAMPERED="1${CIPHERTEXT:1}"
else
  TAMPERED="0${CIPHERTEXT:1}"
fi
DECRYPTED=$(printf "2\n%s\n%s\n" "$TAMPERED" "$KEY" | ./des_test | grep -Eo '[01]{64,}' | tail -n 1)

if [[ "$DECRYPTED" == "$PLAINTEXT" ]]; then
  echo "[FAIL] Tampered ciphertext decrypted to the original plaintext"
  rm -f des_test
  exit 1
fi

echo "[PASS] Tampered ciphertext does not decrypt to the original plaintext."
rm -f des_test
