#include "minicurses.h"
/* for debugging purposes */
void display_bboard(bboard b, int row, int col) {
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
void print_board(board_state *board, int row, int col) {
    int i, j;
    int color, piece, file, rank;
    char *graph;
    /* bitboards */
    move_cursor(1, 60);
    printf("%d %d", board->flags.castle_q[WHITE],
            board->flags.castle_k[WHITE]);
    move_cursor(2, 60);
    printf("%d %d", board->flags.castle_q[BLACK],
            board->flags.castle_k[BLACK]);
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
                move_cursor(7 - (square / 8) + row, ((square & 7) << 1) + col);
                printf("%s ", piece_chars[j]);
                current ^= (current & -current);
            }
        }
    }
    reset_attr();
    display_bboard(board->flags.en_passant[0], 1, 21);
    display_bboard(board->flags.en_passant[1], 1, 41);
    for(i = WHITE; i <= BLACK; i++) {
        for(j = KING; j <= ALL; j++) {
            display_bboard(board->bb[i][j], 10 + 10 * i, 20 * j + 1);
        }
    }
    display_bboard(all_pieces(board), 10 + WHITE * 10, 20 * NO_PIECE + 1);
    display_bboard(all_pieces(board), 10 + BLACK * 10, 20 * NO_PIECE + 1);

    //assert((board->bb[WHITE][KING] |
    //            board->bb[WHITE][QUEEN] |
    //            board->bb[WHITE][BISHOP] |
    //            board->bb[WHITE][KNIGHT] |
    //            board->bb[WHITE][ROOK] |
    //            board->bb[WHITE][PAWN]
    //            ) == board->bb[WHITE][6]);
    //assert((board->bb[BLACK][KING] |
    //            board->bb[BLACK][QUEEN] |
    //            board->bb[BLACK][BISHOP] |
    //            board->bb[BLACK][KNIGHT] |
    //            board->bb[BLACK][ROOK] |
    //            board->bb[BLACK][PAWN]
    //            ) == board->bb[BLACK][6]);
    reset_attr();
}
