[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/B72A5Yyp)
# FIT4012 - Lab 4: DES / TripleDES

Repo này chứa chương trình C++ minh họa mã hóa DES và TripleDES cho Lab 4 của FIT4012. Chương trình nhận chuỗi bit nhị phân từ `stdin`, xử lý theo từng block 64 bit, hỗ trợ mã hóa/giải mã DES và TripleDES.

## 1. Cấu trúc repo

```text
.
├── .github/
│   ├── grading/
│   ├── scripts/
│   └── workflows/
├── logs/
│   ├── .gitkeep
│   ├── README.md
│   └── test-output.txt
├── scripts/
│   └── run_sample.sh
├── tests/
│   ├── test_des_sample.sh
│   ├── test_encrypt_decrypt_roundtrip.sh
│   ├── test_multiblock_padding.sh
│   ├── test_tamper_negative.sh
│   └── test_wrong_key_negative.sh
├── .gitignore
├── CMakeLists.txt
├── Makefile
├── README.md
├── des.cpp
└── report-1page.md
```

## 2. Cách chạy chương trình

### Cách 1: Dùng Makefile

```bash
make
./des
```

### Cách 2: Biên dịch trực tiếp

```bash
g++ -std=c++17 -Wall -Wextra -pedantic des.cpp -o des
./des
```

### Cách 3: Dùng CMake

```bash
cmake -S . -B build
cmake --build build
./build/des
```

## 3. Input / Đầu vào

Chương trình nhận dữ liệu từ `stdin`. Dòng đầu tiên là mode chạy:

```text
1 = DES encrypt
2 = DES decrypt
3 = TripleDES encrypt
4 = TripleDES decrypt
```

### Mode 1: DES encrypt

Nhập lần lượt:

```text
1
plaintext_nhi_phan
key_64_bit
```

Ví dụ:

```bash
printf "1\n0001001000110100010101100111100010011010101111001101111011110001\n0001001100110100010101110111100110011011101111001101111111110001\n" | ./des
```

### Mode 2: DES decrypt

Nhập lần lượt:

```text
2
ciphertext_nhi_phan
key_64_bit
```

### Mode 3: TripleDES encrypt

Nhập lần lượt:

```text
3
plaintext_nhi_phan
K1_64_bit
K2_64_bit
K3_64_bit
```

TripleDES encrypt dùng thứ tự:

```text
E(K3, D(K2, E(K1, P)))
```

### Mode 4: TripleDES decrypt

Nhập lần lượt:

```text
4
ciphertext_nhi_phan
K1_64_bit
K2_64_bit
K3_64_bit
```

TripleDES decrypt dùng thứ tự ngược lại:

```text
D(K1, E(K2, D(K3, C)))
```

## 4. Output / Đầu ra

Kết quả cuối cùng được in ra dưới dạng một chuỗi nhị phân. Với mode encrypt, output là ciphertext. Với mode decrypt, output là plaintext đã giải mã.

Chương trình không in round keys trong chế độ chạy chính để output dễ kiểm thử tự động. Nếu chạy chương trình mà không nhập mode, chương trình in ciphertext mẫu để tương thích với test sample ban đầu.

## 5. Padding đang dùng

Chương trình dùng zero padding theo block 64 bit:

- Nếu plaintext hoặc ciphertext đã có độ dài chia hết cho 64, chương trình xử lý trực tiếp.
- Nếu block cuối thiếu bit, chương trình thêm ký tự `0` vào cuối cho đủ 64 bit.
- Nếu plaintext dài hơn 64 bit, chương trình chia thành nhiều block 64 bit và mã hóa tuần tự từng block.
- Khi decrypt, chương trình trả về plaintext sau giải mã, bao gồm cả các bit `0` đã được padding nếu có.

Hạn chế của zero padding là không thể luôn phân biệt được bit `0` nào là dữ liệu thật và bit `0` nào là padding. Vì vậy cách này chỉ phù hợp cho bài học nhập môn về block cipher, không nên dùng như thiết kế bảo mật hoàn chỉnh trong thực tế.

## 6. Tests bắt buộc

Repo có 5 test shell script:

- `tests/test_des_sample.sh`: kiểm tra DES sample encrypt.
- `tests/test_encrypt_decrypt_roundtrip.sh`: kiểm tra decrypt(encrypt(plaintext)) trả về plaintext ban đầu.
- `tests/test_multiblock_padding.sh`: kiểm tra plaintext dài hơn 64 bit và zero padding.
- `tests/test_tamper_negative.sh`: flip 1 bit ciphertext và kiểm tra không khôi phục plaintext ban đầu.
- `tests/test_wrong_key_negative.sh`: decrypt bằng key sai và kiểm tra không khôi phục plaintext ban đầu.

Chạy toàn bộ test bằng:

```bash
make test
```

Hoặc chạy từng test:

```bash
bash tests/test_des_sample.sh
bash tests/test_encrypt_decrypt_roundtrip.sh
bash tests/test_multiblock_padding.sh
bash tests/test_tamper_negative.sh
bash tests/test_wrong_key_negative.sh
```

## 7. Logs / Minh chứng

Thư mục `logs/` chứa minh chứng chạy chương trình và test. File `logs/test-output.txt` ghi lại output thực tế khi chạy `make test`.

## 8. Ethics & Safe use

- Chỉ chạy và kiểm thử trên dữ liệu học tập hoặc dữ liệu giả lập.
- Không dùng repo này để tấn công hoặc can thiệp hệ thống thật.
- Không trình bày chương trình là công cụ bảo mật sẵn sàng cho môi trường production.
- Nếu tham khảo mã, tài liệu, công cụ hoặc AI, cần ghi nguồn rõ ràng.
- Khi cộng tác nhóm, cần trung thực học thuật và mô tả đúng phần việc của mình.
- Việc kiểm thử chỉ phục vụ học DES / TripleDES ở mức nhập môn.

## 9. Checklist nộp bài

Trước khi nộp, repo đã có:

- `des.cpp` hoàn thiện.
- `README.md` hoàn chỉnh.
- `report-1page.md` hoàn chỉnh.
- `tests/` với 5 test.
- Negative test cho `tamper` và `wrong key`.
- `logs/` có file minh chứng thật.
- Không còn placeholder trong source, README, report hoặc test.

## 10. CI và submission contract

Chương trình tuân thủ contract nhập/xuất để GitHub Actions có thể kiểm tra tự động:

- Mode 1: DES encrypt, có multi-block và zero padding.
- Mode 2: DES decrypt, dùng round keys đảo ngược.
- Mode 3: TripleDES encrypt theo chuỗi `E(K3, D(K2, E(K1, P)))`.
- Mode 4: TripleDES decrypt theo chuỗi ngược lại.
- Kết quả cuối cùng luôn xuất hiện dưới dạng chuỗi nhị phân dài hợp lệ.
