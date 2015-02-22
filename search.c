#include "pawn.h"
#include "knight.h"
#include "rook.h"
#include "bishop.h"
#include "search.h"
#include "minicurses.h"
#include "debug.h"
#include <string.h>

void generate_pawn_moves(board_state *, move *, int *);


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



void board_init(board_state *board) {
    board->turn = 0; 
    board->castle_q[0] = board->castle_q[1] = 
        board->castle_k[0] = board->castle_k[1] = 1;
    board->en_passant[0] = board->en_passant[1] = 0;
    board->bb[WHITE][KING] = BB_KING & BB_WHITE;
    board->bb[WHITE][QUEEN] = BB_QUEEN & BB_WHITE;
    board->bb[WHITE][BISHOP] = BB_BISHOP & BB_WHITE;
    board->bb[WHITE][KNIGHT] = BB_KNIGHT & BB_WHITE;
    board->bb[WHITE][ROOK] = BB_ROOK & BB_WHITE;
    board->bb[WHITE][PAWN] = BB_PAWN & BB_WHITE;
    board->bb[BLACK][KING] = BB_KING & BB_BLACK;
    board->bb[BLACK][QUEEN] = BB_QUEEN & BB_BLACK;
    board->bb[BLACK][BISHOP] = BB_BISHOP & BB_BLACK;
    board->bb[BLACK][KNIGHT] = BB_KNIGHT & BB_BLACK;
    board->bb[BLACK][ROOK] = BB_ROOK & BB_BLACK;
    board->bb[BLACK][PAWN] = BB_PAWN & BB_BLACK;
    board->bb[WHITE][6] = BB_WHITE;
    board->bb[BLACK][6] = BB_BLACK;
    memcpy(board->board88, start_pos, sizeof(start_pos));
}

void bb_make(board_state *board, move *m, int turn) {
    board->bb[board->turn][m->mover] ^= m->target;
    board->bb[board->turn][6] ^= m->target;
    /* switch better than if-else? */
    switch (m->captive) {
        case NO_PIECE: /* if captive is -1, then there is no capture */
            break;
        default:
            board->bb[!board->turn][m->captive] ^= m->capture;
            board->bb[!board->turn][m->captive] ^= m->capture;
            break;
    }
}

void make(board_state *board, move *m) {
    /* modify mailbox */
    board->board88[m->dest] = board->board88[m->src];
    board->board88[m->src] = NO_PIECE;
    /* modify bitboards */
    bb_make(board, m, board->turn);
    board->turn = !board->turn;
}

void unmake(board_state *board, move *m) {
    board->turn = !board->turn;
    /* modify mailbox */
    board->board88[m->src] = board->board88[m->dest];
    board->board88[m->dest] = m->captive | (board->turn << 7);
    /* modify bitboards */
    bb_make(board, m, board->turn);
}

/* dumb because it considers every possible branch */
int dumb_search(board_state *board, int alpha, int beta, int depth) {
    if (depth <= 0) {
        /* return quiescent search */
        return 0;
    }

    int turn = board->turn;

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
    
    generate_pawn_moves(board, candidates, &move_index);

    uint8_t en_passant[2];
    int castle_q[2];
    int castle_k[2];

    
    /* iterate through each move */
    int i;
    for(i = 0; i < move_index; i++) {
        make(board, &candidates[i]);
        print_board(board, 1, 1);
        move_cursor(28, 1);
        clear_line();
        display_text("Make", 28, 1);
        move_cursor(30, 1);
        printf("i: %d, depth: %d, src: %d, dest: %d\n", i, depth, candidates[i].src, candidates[i].dest);
        display_text("Pawns", 31, 1);
        display_bboard(board->bb[WHITE][PAWN], 32, 1);
        display_bboard(board->bb[BLACK][PAWN], 32, 20);
        display_text("Target", 31, 39);
        display_bboard(candidates[i].target, 32, 39);
        getchar();
        
        score = -dumb_search(board, -beta, -alpha, depth - 1);
        if(score >= beta) {
            unmake(board, &candidates[i]);
            print_board(board, 1, 1);
            move_cursor(30, 1);
            printf("i: %d, depth: %d, src: %d, dest: %d\n", i, depth, candidates[i].src, candidates[i].dest);
            display_text("Pawns", 31, 1);
            display_bboard(board->bb[WHITE][PAWN], 32, 1);
            display_bboard(board->bb[BLACK][PAWN], 32, 20);
            display_text("Target", 31, 39);
            display_bboard(candidates[i].target, 32, 39);
            getchar();

            return beta;
        }
        if(score > alpha)
            alpha = score;


        unmake(board, &candidates[i]);
        print_board(board, 1, 1);
        move_cursor(28, 1);
        clear_line();
        display_text("Unmake", 28, 1);
        move_cursor(30, 1);
        printf("i: %d, depth: %d, src: %d, dest: %d\n", i, depth, candidates[i].src, candidates[i].dest);
        display_text("Pawns", 31, 1);
        display_bboard(board->bb[WHITE][PAWN], 32, 1);
        display_bboard(board->bb[BLACK][PAWN], 32, 20);
        display_text("Target", 31, 39);
        display_bboard(candidates[i].target, 32, 39);
        getchar();
        board->en_passant[0] = en_passant[0];
        board->en_passant[1] = en_passant[1];
        board->castle_q[0] = castle_q[0];
        board->castle_q[1] = castle_q[1];
        board->castle_k[0] = castle_k[0];
        board->castle_k[1] = castle_k[1];
    }
    /* make move */
    //board->bb[turn][PAWN] ^= piece;
    //score = -search(board, -beta, -alpha, depth - 1, !turn);
    //if (score >= beta)
    //    return beta;
    //if (score > alpha)
    //    alpha = score;

    /*
     *  for each move
     *      make move
     *      score = -search(-beta, -alpha, depth - 1, !turn);
     *      if (score >= beta) {
     *          return beta;
     *      }
     *      if (score > alpha) {
     *          alpha = score;
     *      }
     *      return alpha
     *
     */
    return alpha;
}

void generate_pawn_moves(board_state *board, move *candidates,
        int *move_index) {
    int turn = board->turn;

    /* if captured piece is not -1, then that piece needs to be
     * restored during an unmake 
     **/
    int captured_piece = -1;
    int src, dest;

    bboard pieces;
    bboard piece;
    //bboard target;
    /* Find candidate moves */ 
    /* Pawns */
    /* single pushes */
    pieces = pawns_able_2_push(board->bb[turn][PAWN], 
            ~all_pieces(board), turn);
    while(pieces) {
        /* get current location of a piece */
        piece = pieces & -pieces;
                src = lsb(piece);
        pieces ^= piece;
        /* or it with target location */
        bboard tmp = single_push_targets(piece, 
                ~all_pieces(board), turn);
        dest = lsb(tmp);
        piece |= tmp;
        //display_bboard(piece, 32, 1);
        //display_bboard(pieces, 32, 20);
        //getchar();

        /* Add it to candidates list */
        candidates[(*move_index)++] = (move){{0, 0}, {board->castle_q[0],
            board->castle_q[1]}, {board->castle_k[0], board->castle_k[1]}, 
            PAWN, NO_PIECE, src, dest, piece, 0,};
    }

    ///* double pushes */
    //pieces = pawns_able_2_dbl_push(board->bb[turn][PAWN],
    //        ~all_pieces(board), turn);
    //while(pieces) {
    //    /* get current location of a piece */
    //    piece = pieces & -pieces;
    //    /* or it with target location */
    //    piece |= double_push_targets(board->bb[turn][PAWN], 
    //            ~all_pieces(board), turn);
    //    /* en passant mask */
    //    uint8_t ep_mask = south_fill(piece);
    //    //candidates[move_index++] = (move){PAWN, NO_PIECE, piece, 0};
    //    move *m = candidates + *move_index;
    //    m->en_passant[turn] = ep_mask;
    //    m->en_passant[!turn] = 0;
    //    m->castle_q[0] = board->castle_q[0];
    //    m->castle_q[1] = board->castle_q[1];
    //    m->castle_k[0] = board->castle_k[0];
    //    m->castle_k[1] = board->castle_k[1];
    //    //m->mover = PAWN; m->captive = NO_PIECE; m->
    //    pieces ^= piece;
    //}

    ///* captures */
    //pieces = pawns_able_2_dbl_push(board->bb[turn][PAWN],
    //        ~all_pieces(board), turn);
    //while(pieces) {
    //    ;
    //}
}
