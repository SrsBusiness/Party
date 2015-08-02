#include "bitboard.h"

void board_init(board_state *);
void make(board_state *, move *);
void unmake(board_state *, move *);
int dumb_search(board_state *, int, int, int);

const extern board_state initial_state;
