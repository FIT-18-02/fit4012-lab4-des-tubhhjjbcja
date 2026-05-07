CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
TARGET := des
SRC := des.cpp

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	bash tests/test_des_sample.sh
	bash tests/test_encrypt_decrypt_roundtrip.sh
	bash tests/test_multiblock_padding.sh
	bash tests/test_tamper_negative.sh
	bash tests/test_wrong_key_negative.sh

clean:
	rm -f $(TARGET) des_test
	rm -rf build
