#include "transposition_table.h"

uint64_t* transposition_table_init(int size) {
    assert(size > 0);

    uint64_t* tt = calloc(size, sizeof(*tt));
    if (!tt) {
        printf("Error: Failed to initialize transposition table\n");
        exit(EXIT_FAILURE);
    }
    return tt;
}

void transposition_table_free(uint64_t* tt) {
    free(tt);
}

int transposition_table_index(uint64_t key, int size) {
    assert(size > 0);

    return key % size;
}

void transposition_table_put(uint64_t key, uint8_t val, uint64_t* tt, int size) {
    assert (key < (1ULL << 56)); /* key stored on 56 bits */

    int i = transposition_table_index(key, size);

    uint64_t key_val = (key<<8) | val; /* 8 first bits: val; 56 last: key*/
    tt[i] = key_val;
}

uint8_t transposition_table_get(uint64_t key, uint64_t* tt, int size) {
    assert (key < (1ULL << 56)); /* key stored on 56 bits */

    int i = transposition_table_index(key, size);
    uint64_t key_val = tt[i];

    if (key_val != 0 && key_val >> 8 == key) {
        return key_val & ((1ULL << 8) - 1); /* Val stored on the first 8 bits */
    }
    return 0; /* Missing key, 0 default*/
}
