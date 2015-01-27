#include <stdio.h>
#include "bitboard.h"
#include "minicurses.h"

bboard rook_occupancy_masks[64];
bboard bishop_occupancy_masks[64];

void generate_rook_occupancy_masks(bboard *);

int main() {
    //bboard occupancy = 0x000101010101017E;
    //bboard index = 0xFFF0000000000000;
    //bboard magic = index / occupancy;
    //bboard test = 0x000101010101017E;
    //bboard test_index = (test * magic) & index;
    //printf("%llX\n", magic);
    //printf("%llX\n", index);
    //printf("%llX\n", test_index);
    generate_rook_occupancy_masks(rook_occupancy_masks);
    int i;
    clear_all();
    hide_cursor();
    for (i = 0; i < 64; i++) {
        display_bboard((bboard)1 << i, 1, 1);
        display_bboard(rook_occupancy_masks[i], 1, 20);
        getchar();
    }
    show_cursor();
}

void generate_rook_occupancy_masks(bboard *occupancy) {
    bboard exclude = 0x007E7E7E7E7E7E00;
    int i;
    bboard mask;
    for (i = 0; i < 64; i++) {
        mask = (bboard)1 << i;
        occupancy[i] = (north_span(mask) & ~RANK8) | 
            (south_span(mask) & ~RANK1) | 
            (east_span(mask) & ~HFILE) | 
            (west_span(mask) & ~AFILE);
    }
}
