#include "bboard_utils.h"
/* for debugging purposes */
void display_bboard(uint64_t b, int row, int col) {
    int i;
    for(i = 0; i < 64; i++) {
        move_cursor(7 - (i / 8) + row, ((i & 7) << 1) + col);
        if((1lu << i) & b)
            putchar('#');
        else
            putchar('_');
    }
    fflush(stdin);
}

void display_text(char *string, int row, int col){
    move_cursor(row, col);
    printf("%s\n", string);
}

#define BKING   "\u265A"
#define BQUEEN  "\u265B"
#define BROOK   "\u265C"
#define BBISHOP "\u265D"
#define BKNIGHT "\u265E"
#define BPAWN   "\u265F"
static char *piece_chars[6] = {
    BKING,
    BQUEEN,
    BBISHOP,
    BKNIGHT,
    BROOK,
    BPAWN
};
void print_board(struct board_state *board, int row, int col) {
    int i, j;
    int file, rank;
    /* bitboards */
    for(i = 0; i < 64; i++) {
        file = i % 8;
        rank = i / 8;
        if((rank + file) % 2)
            cyan_bg();
        else
            purple_bg();
        move_cursor(7 - (i / 8) + row, ((i & 7) << 1) + col);
        printf("  ");
    }
    uint64_t current;
     
    for(i = WHITE; i <= BLACK; i++) {
        for(j = KING; j <= PAWN; j++) {
            current = board->bb[i][j]; 
            while(current) {
                int square = lsb(current);
                file = square % 8;
                rank = square / 8;
                if((rank + file) % 2)
                    cyan_bg();
                else
                    purple_bg();
                if(i)
                    black_font();
                else
                    white_font();
                move_cursor(7 - (square / 8) + row, ((square & 7) << 1) + col);
                printf("%s ", piece_chars[j]);
                current ^= (current & -current);
            }
        }
    }
    move_cursor(row + 8, 0);
    reset_attr();
}
