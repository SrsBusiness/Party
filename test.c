#include <stdio.h>
#include "minicurses.h"
#include "bitboard.h"
#include "search.h"
#include "debug.h"

board_state promotion = {WHITE, {{AFILE & RANK1, 0, 0, 0, 0, AFILE & RANK7, (AFILE & RANK1 | AFILE & RANK7), SENTINAL}, {HFILE & RANK8, 0, BFILE & RANK8, 0, 0, 0, (HFILE & RANK8) | (BFILE & RANK8), SENTINAL}}, {{0, 0}, {0, 0}, {0, 0}}};

board_state en_passant = {WHITE, {{AFILE & RANK1, 0, 0, 0, 0, AFILE & RANK2, (AFILE & RANK1 | AFILE & RANK2), SENTINAL}, {HFILE & RANK8, 0, 0, 0, 0, BFILE & RANK4, (HFILE & RANK8) | (BFILE & RANK4), SENTINAL}}, {{0, 0}, {0, 0}, {0, 0}}};


int main() {
    //printf("%lu\n", sizeof(move));
    hide_cursor();
    clear_all();
    board_state b;
    b = en_passant;
    //board_init(&b);
    //board_state b = {WHITE, {{AFILE & RANK1, 0, 0, 0, 0, AFILE & RANK7, (AFILE & RANK1 | AFILE & RANK7), SENTINAL}, {HFILE & RANK8, 0, BFILE & RANK8, 0, 0, 0, (HFILE & RANK8) | (BFILE & RANK8), SENTINAL}}, {{0, 0}, {0, 0}, {0, 0}}};
    dumb_search(&b, -10, 10, 4);
}

