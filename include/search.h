#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"

int dumb_search(struct board_state *, int, int, int);
int32_t min(struct board_state *, int32_t alpha, int32_t beta, int depth_left);
int32_t max(struct board_state *, int32_t alpha, int32_t beta, int depth_left);
void search(struct board_state *board, int turn, int depth, struct move *best_move);
#endif
