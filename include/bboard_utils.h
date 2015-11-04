#ifndef BBOARD_UTILS_H
#define BBOARD_UTILS_H

#include "minicurses.h"
#include "bitboard.h"
/* for debugging purposes */
void display_bboard(uint64_t b, int row, int col);

void display_text(char *string, int row, int col);

#define BKING   "\u265A"
#define BQUEEN  "\u265B"
#define BROOK   "\u265C"
#define BBISHOP "\u265D"
#define BKNIGHT "\u265E"
#define BPAWN   "\u265F"
void print_board(struct board_state *board, int row, int col);
void serialize_board(struct board_state *b);
#endif
