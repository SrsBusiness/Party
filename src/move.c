#include "move.h"

void bb_make(board_state *board, move *m, int turn) {
    /* capture */
    int i;
    for(i = QUEEN; i <= ALL; i++)
        board->bb[!board->turn][i] &= m->capture;
    
    /* primary move */
    board->bb[board->turn][m->p_mover] ^= m->primary;
    board->bb[board->turn][ALL] ^= m->primary;
    
    /* Pawn promotion and castling */
    if(m->s_mover != NO_PIECE) {
        board->bb[board->turn][m->s_mover] ^= m->secondary;
        board->bb[board->turn][ALL] ^= m->secondary;
    }
}

void make(board_state *board, move *m) {
    /* modify bitboards */
    bb_make(board, m, board->turn);
    board->turn = !board->turn;
    board->flags = m->flags;
}

/* unused */
void unmake(board_state *board, move *m) {
    board->turn = !board->turn;
    bb_make(board, m, board->turn);
    board->flags = m->flags;
}
