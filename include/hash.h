#ifndef HASH_H
#define HASH_H

extern uint64_t ZOBRIST_piece_keys[2][6][64];
extern uint64_t ZOBRIST_castle_q_keys[2];
extern uint64_t ZOBRIST_castle_k_keys[2];
extern uint64_t ZOBRIST_enpassant_keys[8];
extern uint64_t ZOBRIST_turn;
extern uint64_t ZOBRIST_table_size;
extern int32_t *ZOBRIST_transposition_table;

uint64_t rand_int64();
void ZOBRIST_prng_init(int);
void ZOBRIST_rng_init();
void print_literal_keys();

#endif
