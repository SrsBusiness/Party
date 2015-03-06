#include <stdio.h>
#include "minicurses.h"
#include "bitboard.h"
#include "search.h"
#include "debug.h"


int main() {
    //printf("%lu\n", sizeof(move));
    hide_cursor();
    clear_all();
    //board_state b;
    //board_init(&b);
    board_state b = {WHITE, {{AFILE & RANK1, 0, 0, 0, 0, AFILE & RANK7, (AFILE & RANK1 | AFILE & RANK7), SENTINAL}, {HFILE & RANK8, 0, 0, 0, 0, 0, HFILE & RANK8, SENTINAL}}, {{0, 0}, {0, 0}, {0, 0}}};
    dumb_search(&b, -10, 10, 20);
}

