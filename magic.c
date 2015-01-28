#include <stdio.h>
#include "bitboard.h"
#include "minicurses.h"

bboard rook_occupancy_masks[64];
bboard bishop_occupancy_masks[64];
char rook_magic_shifts[64];
char bishop_magic_shifts[64];

void generate_rook_occupancy_masks(bboard *, char *);
void generate_bishop_occupancy_masks(bboard *, char *);

int main() {
    //bboard occupancy = 0x000101010101017E;
    //bboard index = 0xFFF0000000000000;
    //bboard magic = index / occupancy;
    //bboard test = 0x000101010101017E;
    //bboard test_index = (test * magic) & index;
    //printf("%llX\n", magic);
    //printf("%llX\n", index);
    //printf("%llX\n", test_index);
    generate_rook_occupancy_masks(rook_occupancy_masks, rook_magic_shifts);
    generate_bishop_occupancy_masks(bishop_occupancy_masks, bishop_magic_shifts);
    int i;
    //clear_all();
    //hide_cursor();
    bboard mask;
    for (i = 0; i < 64; i++) {
        /* bishop */
        //display_bboard((bboard)1 << i, 1, 1);
        //display_bboard(bishop_occupancy_masks[i], 1, 20);
        
        /* rook */
        //display_bboard((bboard)1 << i, 10, 1);
        //display_bboard(rook_occupancy_masks[i], 10, 20);
        //getchar();

        //printf("0x%llX,\n", rook_occupancy_masks[i]);
        //printf("0x%llX,\n", bishop_occupancy_masks[i]);
        //printf("%d,\n", rook_magic_shifts[i]);
        printf("%d,\n", bishop_magic_shifts[i]);
        
    }
    //show_cursor();
}

void generate_rook_occupancy_masks(bboard *occupancy, char *shifts) {
    int i;
    bboard mask;
    for (i = 0; i < 64; i++) {
        mask = (bboard)1 << i;
        occupancy[i] = (north_span(mask) & ~RANK8) | 
            (south_span(mask) & ~RANK1) | 
            (east_span(mask) & ~HFILE) | 
            (west_span(mask) & ~AFILE);
        shifts[i] = 64 - __builtin_popcountll(occupancy[i]);
    }
}

void generate_bishop_occupancy_masks(bboard *occupancy, char *shifts) {
    bboard exclude = 0x007E7E7E7E7E7E00;
    bboard mask;
    int i;
    for (i = 0; i < 64; i++) {
        mask = (bboard)1 << i;
        occupancy[i] = (ne_span(mask) | nw_span(mask) | 
                se_span(mask) | sw_span(mask)) & exclude;
        shifts[i] = 64 - __builtin_popcountll(occupancy[i]);
    }
}
