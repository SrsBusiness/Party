#include "debug.h"
#include "minicurses.h"
#include "assert.h"

#define BKING   "\u265A"
#define BQUEEN  "\u265B"
#define BROOK   "\u265C"
#define BBISHOP "\u265D"
#define BKNIGHT "\u265E"
#define BPAWN   "\u265F"



static char *piece_chars[6] = {
    BKING,
    BQUEEN,
    BROOK,
    BBISHOP,
    BKNIGHT,
    BPAWN
};


void print_board(board_state *board, int row, int col) {
    int i, j;
    int color, piece, file, rank;
    char *graph;
    uint8_t *board88 = board->board88;
    /* mailbox board */
    move_cursor(1, 1);
    printf("Mailbox:");
    for(i = 0; i < 64; i++) {
        if (board88[i] == NO_PIECE)
            graph = " ";
        else {
            piece = board88[i] & 0x0F;
            color = !!(0x80 & board88[i]);
            graph = piece_chars[piece];
        }
        file = i % 8;
        rank = i / 8;
        if((rank + file) % 2)
            cyan_bg();
        else
            purple_bg();
        move_cursor(7 - (i / 8) + row + 1, ((i & 7) << 1) + col);
        if (color)
            black_font();
        else
            white_font();
        printf("%s ", graph);
    }

    /* bitboards */
    move_cursor(11, 1);
    reset_attr();
    printf("Bitboards:");
    for(i = 0; i < 64; i++) {
        file = i % 8;
        rank = i / 8;
        if((rank + file) % 2)
            cyan_bg();
        else
            purple_bg();
        move_cursor(7 - (i / 8) + row + 11, ((i & 7) << 1) + col);
        printf("  ");
    }
    bboard current;
     
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
                move_cursor(7 - (square / 8) + row + 11, ((square & 7) << 1) + col);
                printf("%s ", piece_chars[j]);
                current ^= (current & -current);
            }
        }
    }
    assert((board->bb[WHITE][KING] |
                board->bb[WHITE][QUEEN] |
                board->bb[WHITE][BISHOP] |
                board->bb[WHITE][KNIGHT] |
                board->bb[WHITE][ROOK] |
                board->bb[WHITE][PAWN]
                ) == board->bb[WHITE][6]);
    assert((board->bb[BLACK][KING] |
                board->bb[BLACK][QUEEN] |
                board->bb[BLACK][BISHOP] |
                board->bb[BLACK][KNIGHT] |
                board->bb[BLACK][ROOK] |
                board->bb[BLACK][PAWN]
                ) == board->bb[BLACK][6]);
    reset_attr();
}
