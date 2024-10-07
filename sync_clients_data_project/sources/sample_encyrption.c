#include "sample_encyrption.h"


void encryptData(char *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        data[i] ^= ENCRYPTION_KEY;
    }
}

void decryptData(char *data, size_t length) {
    // XOR encryption is symmetric, so we can use the same function
    encryptData(data, length);
}