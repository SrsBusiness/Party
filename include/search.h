#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"

#define NORMAL_MOVE 0
#define CHECKMATE   1
#define STALEMATE   2

extern uint64_t nodes_visited;

int dumb_search(struct board_state *, int, int, int);
int board_status(struct board_state *);
int32_t min(struct board_state *, int32_t alpha, int32_t beta, int depth_left);
int32_t max(struct board_state *, int32_t alpha, int32_t beta, int depth_left);
int32_t quiescent_min(struct board_state *, int32_t alpha, int32_t beta);
int32_t quiescent_max(struct board_state *, int32_t alpha, int32_t beta);
int search(struct board_state *board, int depth, struct move *best_move);
int id_search(struct board_state *board, int depth, struct move *best_move);
#endif
