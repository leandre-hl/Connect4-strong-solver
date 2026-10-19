#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "game.h"

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

uint64_t* transposition_table_init(int size);
void transposition_table_free(uint64_t* tt);
int transposition_table_index(uint64_t key, int size);

void transposition_table_put(uint64_t key, uint8_t val, uint64_t* tt, int size);
uint8_t transposition_table_get(uint64_t key, uint64_t* tt, int size);

#endif