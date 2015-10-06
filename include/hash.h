#ifndef HASH_H
#define HASH_H

#include "bitboard.h"

#define ZOBRIST_initial_hash 0x5dd3819c464372f0

struct transposition_entry {
    int32_t score;
};

extern uint64_t ZOBRIST_piece_keys[2][6][64];
extern uint64_t ZOBRIST_castle_q_keys[2];
extern uint64_t ZOBRIST_castle_k_keys[2];
extern uint64_t ZOBRIST_en_passant_keys[8];
extern uint64_t ZOBRIST_turn;
extern uint64_t ZOBRIST_table_size;
extern struct transposition_entry *ZOBRIST_transposition_table;

uint64_t rand_int64();
void ZOBRIST_prng_init(int);
void ZOBRIST_rng_init();
void print_literal_keys();
uint64_t ZOBRIST_hash(const struct board_state *board);
uint64_t ZOBRIST_incremental_update(const struct board_state *board, const struct move *m);
void ZOBRIST_transposition_init(uint64_t size);
void set_transposition_score(uint64_t hash, int32_t score);
int32_t get_transposition_score(uint64_t hash);
void print_transposition_table(int row, int col);
#endif
