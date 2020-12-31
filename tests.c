#include "sha256.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "test_vectors.h"

void test_various_lengths(void){
    int i;
    char hex[SHA256_HEX_SIZE];

    for (i = 0; i < 256; i++){
        sha256_hex(data[i], i, hex);

        assert(0 == strcmp(hex, hashes[i]));

        printf("Test for input of length %i passed.\n", i);
    }
}

void test_bytes(void){
    const char *text = "Hello, World!";
    uint8_t bytes[SHA256_BYTES_SIZE];
    uint8_t expected_bytes[SHA256_BYTES_SIZE] = {
        223, 253, 96, 33, 187, 43, 213, 176,
        175, 103, 98, 144, 128, 158, 195, 165,
        49, 145, 221, 129, 199, 247, 10, 75,
        40, 104, 138, 54, 33, 130, 152, 111
    };

    sha256_bytes(text, strlen(text), bytes);

    assert(0 == memcmp(bytes, expected_bytes, SHA256_BYTES_SIZE));

    printf("Byte test passed\n");
}

void test_8gb(void){
    int i;
    uint8_t data[256];
    sha256 sha;
    char hex[SHA256_HEX_SIZE];
    const char *expected_hex = "73ce5d97029dbd1784eba6e3027ff0a1f080a3872a057b07decee9234107fb47";

    printf("Running test with 8 GB of data. This might take a few minutes.\n");

    for (i = 0; i < 256; i++) data[i] = i;

    sha256_init(&sha);
    for (i = 0; i < 32 * 1024 * 1024; i++){
        sha256_append(&sha, data, sizeof(data));
    }
    sha256_finalize_hex(&sha, hex);

    assert(0 == strcmp(hex, expected_hex));
}

void test_file(const char *path){
    struct sha256 sha;
    char hex[SHA256_HEX_SIZE];
    char buf[4096];
    FILE *fp;
    const char *expected_hex = "6b0382b16279f26ff69014300541967a356a666eb0b91b422f6862f6b7dad17e";

    sha256_init(&sha);

    fp = fopen(path, "rb");
    while (1){
        size_t n = fread(buf, 1, sizeof(buf), fp);

        if (n <= 0) break;

        sha256_append(&sha, buf, n);
    }
    fclose(fp);

    sha256_finalize_hex(&sha, hex);

    assert(0 == strcmp(hex, expected_hex));

    printf("File test passed\n");
}

void store4_le(uint8_t *dst, uint32_t x){
    int i;
    for (i = 0; i < 4; i++){
        *dst++ = x & 0xff;
        x >>= 8;
    }
}

uint32_t char2uint(char c){
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'z') return c - 'a' + 10;
    if ('A' <= c && c <= 'Z') return c - 'A' + 10;
    return 0;
}

void hex2bytes(uint8_t *dst, const char *hex){
    int i;
    assert(strlen(hex) == 64);
    for (i = 31; i >= 0; i--, hex += 2){
        dst[i] = (char2uint(hex[0]) << 4) | char2uint(hex[1]);
    }
}

void bytes2hex(char *hex, const uint8_t *bytes){
    int i;
    for (i = 31; i >= 0; i--){
        *hex++ = "0123456789abcdef"[(bytes[i] >> 4) & 0xf];
        *hex++ = "0123456789abcdef"[(bytes[i] >> 0) & 0xf];
    }
    *hex++ = '\0';
}

void test_bitcoin(void){
    /* Verify bitcoin blockchain block number 663535.
     *
     * https://btc.com/0000000000000000000491fee5448371ad848da280adac9199b77703781956ec
     * https://blockstream.info/block/0000000000000000000491fee5448371ad848da280adac9199b77703781956ec
     * https://www.blockchain.com/btc/block/0000000000000000000491fee5448371ad848da280adac9199b77703781956ec
     */
    const char *prev_block = "0000000000000000000ac83a166fa063f61a97d45ff4e50e0cfe5b91caf49856";
    const char *merkle_root = "0f80378655cd3ec33090ca4ef3f9f6f0f7395276d8d8be9eab07dae9580d8763";
    char *expected = "0000000000000000000491fee5448371ad848da280adac9199b77703781956ec";

    uint32_t version = 0x2fffe000;
    /* Make sure that you are in the correct timezone when parsing the date. */
    uint32_t time = 0x5feb7be2;
    uint32_t bits = 0x170f2217;
    uint32_t nonce = 0x5313fe5b;

    char hex[SHA256_HEX_SIZE];
    uint8_t buf[4 + 32 + 32 + 4 + 4 + 4];
    uint8_t *ptr = buf;

    store4_le(ptr, version);
    ptr += 4;
    hex2bytes(ptr, prev_block);
    ptr += SHA256_BYTES_SIZE;
    hex2bytes(ptr, merkle_root);
    ptr += SHA256_BYTES_SIZE;
    store4_le(ptr, time);
    ptr += 4;
    store4_le(ptr, bits);
    ptr += 4;
    store4_le(ptr, nonce);

    sha256_bytes(buf, sizeof(buf), buf);
    sha256_bytes(buf, SHA256_BYTES_SIZE, buf);

    bytes2hex(hex, buf);

    assert(0 == strcmp(hex, expected));

    printf("Bitcoin test passed\n");
}

int main(void){
    test_various_lengths();
    test_bytes();
    test_file("LICENSE");
    test_bitcoin();
    test_8gb();

    return 0;
}
