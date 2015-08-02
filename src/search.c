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

static board_flags initial_flags = {{0, 0}, {1, 1}, {1, 1}};
static bboard initial_bb[2][8] = {{
    BB_KING & BB_WHITE,
    BB_QUEEN & BB_WHITE,
    BB_BISHOP & BB_WHITE,
    BB_KNIGHT & BB_WHITE,
    BB_ROOK & BB_WHITE,
    BB_PAWN & BB_WHITE,
    BB_WHITE,
    SENTINAL,
}, {
    BB_KING & BB_BLACK,
    BB_QUEEN & BB_BLACK,
    BB_BISHOP & BB_BLACK,
    BB_KNIGHT & BB_BLACK,
    BB_ROOK & BB_BLACK,
    BB_PAWN & BB_BLACK,
    BB_BLACK,
    SENTINAL,
}};

const board_state initial_state = {
    0,
    {{
        BB_KING & BB_WHITE,
        BB_QUEEN & BB_WHITE,
        BB_BISHOP & BB_WHITE,
        BB_KNIGHT & BB_WHITE,
        BB_ROOK & BB_WHITE,
        BB_PAWN & BB_WHITE,
        BB_WHITE
    }, {
        BB_KING & BB_BLACK,
        BB_QUEEN & BB_BLACK,
        BB_BISHOP & BB_BLACK,
        BB_KNIGHT & BB_BLACK,
        BB_ROOK & BB_BLACK,
        BB_PAWN & BB_BLACK,
        BB_BLACK
    }},
    {{1, 1}, {1, 1}, {0, 0}}

};

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
    *board = initial_state;
}

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
    
    generate_pawn_moves(board, candidates, &move_index);

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

void generate_pawn_moves(board_state *board, move *candidates,
        int *move_index) {
    
    generate_double_pushes(board, candidates, move_index);
    generate_single_pushes(board, candidates, move_index); 
    //printf("%d\n", *move_index);
    generate_pawn_captures(board, candidates, move_index);
    generate_en_passant(board, candidates, move_index);
    //printf("%d\n", *move_index);
    //while(1);
    ///* captures */
    //pieces = pawns_able_to_double_push(board->bb[turn][PAWN],
    //        ~all_pieces(board), turn);
    //while(pieces) {
    //    ;
    //}
}

void generate_single_pushes(board_state *board, move *candidates, 
        int *move_index) {

    int turn = board->turn;
    /* if captured piece is not -1, then that piece needs to be
     * restored during an unmake 
     **/
    bboard pieces, piece, target;
    //bboard target;
    /* Find candidate moves */ 
    /* Pawns */
    /* single pushes */
    pieces = pawns_able_to_push(board->bb[turn][PAWN], 
            ~all_pieces(board), turn);
    while(pieces) {
        /* get current location of a piece */
        piece = pieces & -pieces;
        pieces ^= piece;
        target = single_push_targets(piece, 
                ~all_pieces(board), turn);

        if((target & RANK1) | (target & RANK8)) {
            /* promotion */
            bboard b;
            for(b = QUEEN; b <= ROOK; b++) {
                move *m = &candidates[*move_index];
                
                m->flags = (board_flags){{0, 0}, 
                    {board->flags.castle_q[0], board->flags.castle_q[1]}, 
                    {board->flags.castle_k[0], board->flags.castle_k[1]},
                };

                m->p_mover = PAWN;
                m->s_mover = b;
                m->primary = piece;
                m->secondary = target;
                m->capture = BLACKOUT;
                (*move_index)++;
        
            }
            continue;
        }
        piece |= target;
        /* Add it to candidates list */
        move *m = &candidates[*move_index];

        m->flags = (board_flags){ 
            {board->flags.castle_q[0], board->flags.castle_q[1]}, 
            {board->flags.castle_k[0], board->flags.castle_k[1]},
            {0, 0},
        };

        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->primary = piece;
        m->secondary = SENTINAL;
        m->capture = BLACKOUT;
        (*move_index)++;
    }
}

void generate_double_pushes(board_state *board, move *candidates, 
        int *move_index) {

    int turn = board->turn;

    bboard pieces, piece, target;

    /* double pushes */
    pieces = pawns_able_to_double_push(board->bb[turn][PAWN],
            ~all_pieces(board), turn);
    while(pieces) {
        /* get current location of a piece */
        piece = pieces & -pieces;
        pieces ^= piece;
        /* or it with target location */
        target = double_push_targets(piece, 
                ~all_pieces(board), turn);
        
        /* Add it to candidates list */
        move *m = &candidates[*move_index];

        m->flags = board->flags; 
        m->flags.en_passant[turn] = target;
        m->flags.en_passant[!turn] = 0;

        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->primary = piece | target;
        m->secondary = SENTINAL;
        m->capture = BLACKOUT;
        (*move_index)++;

    }
}

void generate_pawn_captures(board_state *board, move *candidates, 
        int *move_index) {

    int turn = board->turn;
    /* if captured piece is not -1, then that piece needs to be
     * restored during an unmake 
     **/
    bboard pieces, piece, targets, target;
    //bboard target;
    /* Find candidate moves */ 
    /* Pawns */
    /* single pushes */
    //bboard anus = ~all_pieces(board);
    //pieces = pawns_able_to_capture_any(board->bb[turn][PAWN], 
    //        ~all_pieces(board), turn);
    pieces = pawns_able_to_capture_any(board->bb[turn][PAWN], 
            board->bb[!turn][ALL], turn);

    while(pieces) {
        /* get current location of a piece */
        piece = pieces & -pieces;
        pieces ^= piece;
        targets = pawn_capture_targets(piece, 
                board->bb[!turn][ALL], turn);
        while(targets) {
            target = targets & -targets;
            targets ^= target;
            if((target & RANK1) | (target & RANK8)) {
                /* promotion */
                bboard b;
                for(b = QUEEN; b <= ROOK; b++) {
                    move *m = &candidates[*move_index];
                    
                    m->flags = (board_flags){ 
                        {board->flags.castle_q[0], board->flags.castle_q[1]}, 
                        {board->flags.castle_k[0], board->flags.castle_k[1]},
                        {0, 0},
                    };
                    m->p_mover = PAWN;
                    m->s_mover = b;
                    m->primary = piece;
                    m->secondary = target;
                    m->capture = ~target;
                    (*move_index)++;
                }
                continue;
            }
            /* Add it to candidates list */
            move *m = &candidates[*move_index];

            m->flags = (board_flags){
                {board->flags.castle_q[0], board->flags.castle_q[1]}, 
                {board->flags.castle_k[0], board->flags.castle_k[1]},
                {0, 0},
            };

            m->p_mover = PAWN;
            m->s_mover = NO_PIECE;
            m->primary = piece | target;
            m->secondary = SENTINAL;
            m->capture = ~target;
            (*move_index)++;
        }
    }
}

void generate_en_passant(board_state *board, move *candidates, 
        int *move_index) {

    int turn = board->turn;

    bboard target, pieces, piece, ep;

    ep = board->flags.en_passant[!turn];

    pieces = (east_one(ep) | west_one(ep)) &
        board->bb[turn][PAWN];
    
    switch(turn) {
        case WHITE:
            target = north_one(ep);
            break;
        case BLACK:
            target = south_one(ep);
            break;
    }
    while(pieces) {
        piece = pieces & -pieces;
        pieces ^= piece;
        move *m = &candidates[*move_index];
        m->flags = (board_flags){
            {board->flags.castle_q[0], board->flags.castle_q[1]}, 
            {board->flags.castle_k[0], board->flags.castle_k[1]},
            {0, 0},
        };
        m->p_mover = PAWN;
        m->s_mover = NO_PIECE;
        m->primary = piece | target;
        m->secondary = SENTINAL;
        m->capture = ~ep;
        (*move_index)++;
    }
}
