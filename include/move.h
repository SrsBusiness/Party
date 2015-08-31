#ifndef MOVE_H
#define MOVE_H

#include "bitboard.h"
#include "pqueue.h"

extern const struct board_state BOARD_INITIAL_STATE;

void board_init(struct board_state *);
void make(struct board_state *, struct move *);
void unmake(struct board_state *, struct move *, struct board_flags *);

#endif
