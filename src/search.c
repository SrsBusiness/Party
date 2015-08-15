#include <string.h>
#include <assert.h>
#include "pawn.h"
#include "knight.h"
#include "rook.h"
#include "bishop.h"
#include "search.h"
#include "move.h"
#include "eval.h"
#include "bboard_utils.h"

void generate_pawn_moves(board_state *, move *, int *);
void generate_single_pushes(board_state *, move *, int *);
void generate_double_pushes(board_state *, move *, int *);
void generate_pawn_captures(board_state *, move *, int *);
void generate_en_passant(board_state *, move *, int *);


static const uint8_t start_pos[64] = {
    ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK,
    PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, 
    NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, 
    NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE,
    NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, 
    NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, NO_PIECE, 
    PAWN | 0x80, PAWN | 0x80, PAWN | 0x80, PAWN | 0x80, PAWN | 0x80, PAWN | 0x80, PAWN | 0x80, PAWN | 0x80, 
    ROOK | 0x80, KNIGHT | 0x80, BISHOP | 0x80, QUEEN | 0x80, KING | 0x80, BISHOP | 0x80, KNIGHT | 0x80, ROOK | 0x80,
};

/* dumb because it considers every possible branch */
int dumb_search(board_state *board, int alpha, int beta, int depth) {
    if (depth <= 0) {
        /* return quiescent search */
        return compute_material(board);
    }

    /* Upperbound on number of candidate moves is 137, which is probably
     * unattainable. In case of bugs, check for buffer overflows with
     * candidates
     **/
    move candidates[128]; int move_index = 0;
    int score; 

    /*
     *  Moves can modify state data about castling and en passant.
     *  Unmaking those moves needs to restore this data. It needs to be saved.
     **/
    
    //generate_pawn_moves(board, candidates, &move_index);

    board_state altered;
    
    /* iterate through each move */
    int i;
    for(i = 0; i < move_index; i++) {
        altered = *board;
        make(&altered, &candidates[i]);
        
        #ifdef  DEBUG
        print_board(&altered, 1, 1);
        move_cursor(28, 1);
        clear_line();
        display_text("Make", 28, 1);
        move_cursor(30, 1);
        clear_line();
        printf("i: %d, depth: %d", i, depth);
        display_text("Pawns", 31, 1);
        display_bboard(altered.bb[WHITE][PAWN], 32, 1);
        display_bboard(altered.bb[BLACK][PAWN], 32, 20);
        display_text("Primary", 31, 39);
        display_bboard(candidates[i].primary, 32, 39);
        getchar();
        #endif
        
        score = -dumb_search(&altered, -beta, -alpha, depth - 1);

        if(score > alpha)
            alpha = score;

        #ifdef  DEBUG
        print_board(board, 1, 1);
        move_cursor(28, 1);
        clear_line();
        display_text("Unmake", 28, 1);
        move_cursor(30, 1);
        clear_line();
        printf("i: %d, depth: %d", i, depth);
        display_text("Pawns", 31, 1);
        display_bboard(board->bb[WHITE][PAWN], 32, 1);
        display_bboard(board->bb[BLACK][PAWN], 32, 20);
        display_text("Primary", 31, 39);
        display_bboard(candidates[i].primary, 32, 39);
        getchar();
        #endif
    }
    return alpha;
}
