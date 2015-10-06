#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"

extern uint64_t nodes_visited;

int dumb_search(struct board_state *, int, int, int);
int32_t min(struct board_state *, int32_t alpha, int32_t beta, int depth_left);
int32_t max(struct board_state *, int32_t alpha, int32_t beta, int depth_left);
void search(struct board_state *board, int depth, struct move *best_move);
#endif
