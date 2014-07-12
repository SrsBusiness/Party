#include <stdint.h>
#include "board88.h"
#include "bitboard.h"
#include <stdio.h>
// macros for board88

unsigned char board_88[128];
// table of Manhattan distances. To index into the table, use
// the diff88 macro.
//const unsigned char dist_man[240];
// Table of actual square distances. The indexing scheme
// is the same as above

int main(){
    for(int i = 0; i < 240; i++)
        printf("%d", dist[i]);
}

