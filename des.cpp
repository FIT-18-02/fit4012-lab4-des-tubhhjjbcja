#include <algorithm>
#include <bitset>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

string convert_decimal_to_binary(int decimal) {
    return bitset<4>(decimal).to_string();
}

int convert_binary_to_decimal(const string& binary) {
    return stoi(binary, nullptr, 2);
}

string Xor(const string& a, const string& b) {
    if (a.size() != b.size()) {
        throw invalid_argument("XOR inputs must have the same length");
    }

    string result;
    result.reserve(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        result += (a[i] != b[i]) ? '1' : '0';
    }
    return result;
}

bool is_binary_string(const string& value) {
    return !value.empty() && all_of(value.begin(), value.end(), [](char ch) {
        return ch == '0' || ch == '1';
    });
}

void validate_binary(const string& value, const string& name) {
    if (!is_binary_string(value)) {
        throw invalid_argument(name + " must be a non-empty binary string containing only 0 and 1");
    }
}

void validate_key(const string& key, const string& name) {
    validate_binary(key, name);
    if (key.size() != 64) {
        throw invalid_argument(name + " must be exactly 64 bits");
    }
}

string pad_zero_right(string input, size_t block_size = 64) {
    validate_binary(input, "input");
    size_t remainder = input.size() % block_size;
    if (remainder != 0) {
        input.append(block_size - remainder, '0');
    }
    return input;
}

string initial_permutation(const string& input) {
    const int initial_permutation_table[64] = {
        58,50,42,34,26,18,10,2,
        60,52,44,36,28,20,12,4,
        62,54,46,38,30,22,14,6,
        64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9,1,
        59,51,43,35,27,19,11,3,
        61,53,45,37,29,21,13,5,
        63,55,47,39,31,23,15,7
    };

    if (input.size() != 64) {
        throw invalid_argument("DES block must be exactly 64 bits before initial permutation");
    }

    string permuted;
    permuted.reserve(64);
    for (int index : initial_permutation_table) {
        permuted += input[index - 1];
    }
    return permuted;
}

string inverse_initial_permutation(const string& input) {
    const int inverse_permutation_table[64] = {
        40,8,48,16,56,24,64,32,
        39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30,
        37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28,
        35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26,
        33,1,41,9,49,17,57,25
    };

    if (input.size() != 64) {
        throw invalid_argument("DES block must be exactly 64 bits before inverse permutation");
    }

    string permuted;
    permuted.reserve(64);
    for (int index : inverse_permutation_table) {
        permuted += input[index - 1];
    }
    return permuted;
}

class KeyGenerator {
private:
    string key;
    vector<string> roundKeys;

    const int pc1[56] = {
        57,49,41,33,25,17,9,
        1,58,50,42,34,26,18,
        10,2,59,51,43,35,27,
        19,11,3,60,52,44,36,
        63,55,47,39,31,23,15,
        7,62,54,46,38,30,22,
        14,6,61,53,45,37,29,
        21,13,5,28,20,12,4
    };

    const int pc2[48] = {
        14,17,11,24,1,5,
        3,28,15,6,21,10,
        23,19,12,4,26,8,
        16,7,27,20,13,2,
        41,52,31,37,47,55,
        30,40,51,45,33,48,
        44,49,39,56,34,53,
        46,42,50,36,29,32
    };

    string shift_left(const string& key_chunk, int shifts) const {
        return key_chunk.substr(shifts) + key_chunk.substr(0, shifts);
    }

public:
    explicit KeyGenerator(const string& input_key) : key(input_key) {
        validate_key(key, "key");
    }

    void generateRoundKeys() {
        roundKeys.clear();

        string permutedKey;
        permutedKey.reserve(56);
        for (int index : pc1) {
            permutedKey += key[index - 1];
        }

        string left = permutedKey.substr(0, 28);
        string right = permutedKey.substr(28, 28);

        for (int i = 0; i < 16; ++i) {
            int shifts = (i == 0 || i == 1 || i == 8 || i == 15) ? 1 : 2;
            left = shift_left(left, shifts);
            right = shift_left(right, shifts);

            string combinedKey = left + right;
            string roundKey;
            roundKey.reserve(48);
            for (int index : pc2) {
                roundKey += combinedKey[index - 1];
            }

            roundKeys.push_back(roundKey);
        }
    }

    const vector<string>& getRoundKeys() const {
        return roundKeys;
    }
};

class DES {
private:
    const int expansion_table[48] = {
        32,1,2,3,4,5,4,5,
        6,7,8,9,8,9,10,11,
        12,13,12,13,14,15,16,17,
        16,17,18,19,20,21,20,21,
        22,23,24,25,24,25,26,27,
        28,29,28,29,30,31,32,1
    };

    const int permutation_tab[32] = {
        16,7,20,21,29,12,28,17,
        1,15,23,26,5,18,31,10,
        2,8,24,14,32,27,3,9,
        19,13,30,6,22,11,4,25
    };

    const int substitution_boxes[8][4][16] = {{
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    }, {
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    }, {
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    }, {
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    }, {
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    }, {
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    }, {
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    }, {
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }};

    vector<string> round_keys;

    string process_block(const string& input, vector<string> keys) const {
        if (input.size() != 64) {
            throw invalid_argument("DES process block input must be exactly 64 bits");
        }

        string perm = initial_permutation(input);
        string left = perm.substr(0, 32);
        string right = perm.substr(32, 32);

        for (int i = 0; i < 16; i++) {
            string right_expanded;
            right_expanded.reserve(48);
            for (int index : expansion_table) {
                right_expanded += right[index - 1];
            }

            string xored = Xor(keys[i], right_expanded);

            string substituted;
            substituted.reserve(32);
            for (int j = 0; j < 8; j++) {
                string row_bits = xored.substr(j * 6, 1) + xored.substr(j * 6 + 5, 1);
                int row = convert_binary_to_decimal(row_bits);

                string col_bits = xored.substr(j * 6 + 1, 4);
                int col = convert_binary_to_decimal(col_bits);

                int val = substitution_boxes[j][row][col];
                substituted += convert_decimal_to_binary(val);
            }

            string permuted;
            permuted.reserve(32);
            for (int index : permutation_tab) {
                permuted += substituted[index - 1];
            }

            string new_right = Xor(permuted, left);
            left = right;
            right = new_right;
        }

        string combined_text = right + left;
        return inverse_initial_permutation(combined_text);
    }

public:
    explicit DES(const vector<string>& keys) : round_keys(keys) {
        if (round_keys.size() != 16) {
            throw invalid_argument("DES requires exactly 16 round keys");
        }
    }

    string encrypt_block(const string& input) const {
        return process_block(input, round_keys);
    }

    string decrypt_block(const string& input) const {
        vector<string> reversed_keys = round_keys;
        reverse(reversed_keys.begin(), reversed_keys.end());
        return process_block(input, reversed_keys);
    }
};

vector<string> make_round_keys(const string& key) {
    KeyGenerator keygen(key);
    keygen.generateRoundKeys();
    return keygen.getRoundKeys();
}

string des_encrypt_blocks(const string& plaintext, const string& key) {
    validate_binary(plaintext, "plaintext");
    validate_key(key, "key");

    string padded = pad_zero_right(plaintext);
    DES des(make_round_keys(key));

    string output;
    output.reserve(padded.size());
    for (size_t i = 0; i < padded.size(); i += 64) {
        output += des.encrypt_block(padded.substr(i, 64));
    }
    return output;
}

string des_decrypt_blocks(const string& ciphertext, const string& key) {
    validate_binary(ciphertext, "ciphertext");
    validate_key(key, "key");

    string padded = pad_zero_right(ciphertext);
    DES des(make_round_keys(key));

    string output;
    output.reserve(padded.size());
    for (size_t i = 0; i < padded.size(); i += 64) {
        output += des.decrypt_block(padded.substr(i, 64));
    }
    return output;
}

string triple_des_encrypt_block(const string& plaintext, const string& key1, const string& key2, const string& key3) {
    if (plaintext.size() != 64) {
        throw invalid_argument("TripleDES input block must be exactly 64 bits");
    }

    DES des1(make_round_keys(key1));
    DES des2(make_round_keys(key2));
    DES des3(make_round_keys(key3));

    string step1 = des1.encrypt_block(plaintext);  // E(K1, P)
    string step2 = des2.decrypt_block(step1);      // D(K2, ...)
    return des3.encrypt_block(step2);              // E(K3, ...)
}

string triple_des_decrypt_block(const string& ciphertext, const string& key1, const string& key2, const string& key3) {
    if (ciphertext.size() != 64) {
        throw invalid_argument("TripleDES input block must be exactly 64 bits");
    }

    DES des1(make_round_keys(key1));
    DES des2(make_round_keys(key2));
    DES des3(make_round_keys(key3));

    string step1 = des3.decrypt_block(ciphertext); // D(K3, C)
    string step2 = des2.encrypt_block(step1);      // E(K2, ...)
    return des1.decrypt_block(step2);              // D(K1, ...)
}

string triple_des_encrypt_blocks(const string& plaintext, const string& key1, const string& key2, const string& key3) {
    validate_binary(plaintext, "plaintext");
    validate_key(key1, "K1");
    validate_key(key2, "K2");
    validate_key(key3, "K3");

    string padded = pad_zero_right(plaintext);
    string output;
    output.reserve(padded.size());
    for (size_t i = 0; i < padded.size(); i += 64) {
        output += triple_des_encrypt_block(padded.substr(i, 64), key1, key2, key3);
    }
    return output;
}

string triple_des_decrypt_blocks(const string& ciphertext, const string& key1, const string& key2, const string& key3) {
    validate_binary(ciphertext, "ciphertext");
    validate_key(key1, "K1");
    validate_key(key2, "K2");
    validate_key(key3, "K3");

    string padded = pad_zero_right(ciphertext);
    string output;
    output.reserve(padded.size());
    for (size_t i = 0; i < padded.size(); i += 64) {
        output += triple_des_decrypt_block(padded.substr(i, 64), key1, key2, key3);
    }
    return output;
}

void run_sample_program() {
    string plaintext = "0001001000110100010101100111100010011010101111001101111011110001";
    string key = "0001001100110100010101110111100110011011101111001101111111110001";
    cout << "Ciphertext: " << des_encrypt_blocks(plaintext, key) << endl;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    try {
        int mode = 0;
        if (!(cin >> mode)) {
            run_sample_program();
            return 0;
        }

        string data;
        string key1;
        string key2;
        string key3;

        switch (mode) {
            case 1:
                cin >> data >> key1;
                cout << des_encrypt_blocks(data, key1) << endl;
                break;
            case 2:
                cin >> data >> key1;
                cout << des_decrypt_blocks(data, key1) << endl;
                break;
            case 3:
                cin >> data >> key1 >> key2 >> key3;
                cout << triple_des_encrypt_blocks(data, key1, key2, key3) << endl;
                break;
            case 4:
                cin >> data >> key1 >> key2 >> key3;
                cout << triple_des_decrypt_blocks(data, key1, key2, key3) << endl;
                break;
            default:
                cerr << "Invalid mode. Use 1=DES encrypt, 2=DES decrypt, 3=TripleDES encrypt, 4=TripleDES decrypt." << endl;
                return 1;
        }
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        return 1;
    }

    return 0;
}
