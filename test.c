#include <stdio.h>
#include "minicurses.h"
#include "bitboard.h"
#include "search.h"
#include "debug.h"


int main() {
    clear_all();
    board_state b;
    board_init(&b);
    dumb_search(&b, -10, 10, 4);
}

