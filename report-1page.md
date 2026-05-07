# FIT4012 Lab 4 - DES / TripleDES Report

## Mục tiêu

Mục tiêu của bài lab là hoàn thiện chương trình DES trong C++ và mở rộng để hỗ trợ các chế độ chạy theo yêu cầu: DES encrypt, DES decrypt, TripleDES encrypt và TripleDES decrypt. Chương trình nhận dữ liệu nhị phân từ `stdin`, xử lý theo block 64 bit và in kết quả cuối cùng dưới dạng chuỗi bit.

## Thiết kế chương trình

Chương trình được viết trong `des.cpp`. Các thành phần chính gồm:

- Hàm chuyển đổi nhị phân/thập phân và hàm XOR chuỗi bit.
- Bảng Initial Permutation và Inverse Initial Permutation của DES.
- Lớp `KeyGenerator` sinh 16 round keys từ khóa 64 bit bằng PC-1, dịch trái C/D, sau đó dùng PC-2.
- Lớp `DES` thực hiện 16 vòng Feistel, gồm expansion 32 bit thành 48 bit, XOR với round key, S-box substitution, permutation P và hoán đổi left/right.
- Các hàm helper cho DES multi-block, DES decrypt bằng round keys đảo ngược, TripleDES encrypt/decrypt.

## Input và output

Chương trình nhận mode ở dòng đầu:

- `1`: DES encrypt.
- `2`: DES decrypt.
- `3`: TripleDES encrypt.
- `4`: TripleDES decrypt.

Với DES, chương trình nhận dữ liệu và một khóa 64 bit. Với TripleDES, chương trình nhận dữ liệu và ba khóa 64 bit `K1`, `K2`, `K3`. Output cuối cùng là một chuỗi nhị phân để CI có thể tách và kiểm tra.

## Padding và xử lý nhiều block

DES xử lý block 64 bit. Nếu dữ liệu dài hơn 64 bit, chương trình chia dữ liệu thành nhiều block 64 bit và xử lý từng block tuần tự. Nếu block cuối chưa đủ 64 bit, chương trình thêm bit `0` vào cuối cho đủ block. Cách zero padding này đơn giản và phù hợp cho bài học nhập môn. Tuy nhiên, nó không phải cơ chế padding an toàn cho hệ thống thực tế vì có thể khó phân biệt bit `0` thật với bit `0` được thêm vào khi padding.

## TripleDES

TripleDES encrypt được triển khai theo đúng contract của bài:

```text
E(K3, D(K2, E(K1, P)))
```

TripleDES decrypt thực hiện theo thứ tự ngược lại:

```text
D(K1, E(K2, D(K3, C)))
```

Các hàm TripleDES dùng lại cùng lớp `DES` bằng cách gọi encrypt/decrypt block theo đúng thứ tự khóa.

## Kiểm thử

Repo có 5 test shell script:

1. `test_des_sample.sh`: kiểm tra ciphertext DES mẫu.
2. `test_encrypt_decrypt_roundtrip.sh`: kiểm tra mã hóa rồi giải mã trả về plaintext ban đầu.
3. `test_multiblock_padding.sh`: kiểm tra dữ liệu dài hơn 64 bit và zero padding.
4. `test_tamper_negative.sh`: flip 1 bit ciphertext và kiểm tra kết quả giải mã khác plaintext ban đầu.
5. `test_wrong_key_negative.sh`: decrypt bằng key sai và kiểm tra không khôi phục plaintext ban đầu.

Kết quả chạy test được lưu trong `logs/test-output.txt` để làm minh chứng nộp bài.

## Kết luận

Chương trình hoàn thành các yêu cầu chính của lab: DES encrypt/decrypt, TripleDES encrypt/decrypt, nhập dữ liệu từ `stdin`, xử lý multi-block, zero padding và có test dương/tamper/wrong-key. Chương trình chỉ dùng cho mục đích học tập DES và không được xem là công cụ bảo mật production-ready.
