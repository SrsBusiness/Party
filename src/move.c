#include "move.h"
#include "hash.h"

/*              ******** Incremental Move Updates ********
 *
 *  There are several different kinds of moves, and our incremental update
 *  scheme must account for all of them.
 *
 *  The majority of moves will just update 1 piece:
 *      Normal Moves
 *
 *  There are a few types of moves that update 2 pieces:
 *      Captures (including en passant)
 *      Castling
 *      Promotion
 *
 *  Finally there is the rare case of updating 3 pieces:
 *      Promotion + Capture
 *
 *  Thus, each move will have an primary, a secondary, and a tertiary mover
 *  _________________________________________________________________________
 * | Move Type            | Primary   | Secondary        | Tertiary          |
 * |______________________|___________|__________________|___________________|
 * | Normal Move          | Mover     | -None-           | -None-            |
 * | Captures             | Captor    | -None-           | Captive           |
 * | Castling             | King      | Rook             | -None-            |
 * | En passant           | Captor    | -None            | Captive Pawn      |
 * | Promotion            | Pawn      | Promotion Piece  | -None-            |
 * | Promotion + Capture  | Pawn      | Promotion Piece  | Captive           |
 * |______________________|___________|__________________|___________________|
 *
 * For each mover, we will have an index and a bitboard.
 * 
 * Incremental make and unmake looks something like this:
 * bb[primary]      ^= primary_move;
 * bb[secondary]    ^= secondary_move;
 * bb[tertiary]     ^= tertiary_move;
 *
 * Local copies of irreversible state such as ep_flags, castling rights, etc.
 * will be created during make to be restored in unmake
 *
 * Challenges/Potential Pitfalls:
 *
 *      1.  Captures - When a capture is performed, we need to know what piece
 *          resides on a given square (namely the capture square). However, 
 *          bitboards do not naturally lend themselves to answering this
 *          question. We could iterate through all the pieces and see if they
 *          occupy said square, or we could keep a mailbox representation of
 *          the board that must also be updated incrementally, but we would
 *          then only need to index into an array.
 *
 *      2.  En Passant - Capture square != Captive Square. This is an
 *          important consideration when updating redundant union sets.
 *
 *      3.  State Flags - Obviously these flags need to be modified and
 *          restored during make and unmake. Restoring the flags is merely
 *          a matter of saving a local copy of the flags before make is
 *          performed. However, a considerable amount of logic is required to
 *          properly maintain these flags. This block of logic can either be
 *          implemented in move generation functions or it can be in make.
 *          There are advantages and disadvantages to both. If it is in make,
 *          the logic is more centralized (ease of maintenance), and it is
 *          a more intuitive design. However there may be a duplication
 *          of logic. Consider a function that is capable of generating en
 *          passant captures. The function must have logic to see that an en
 *          passant is possible, which may mean that it parses the state flags,
 *          in which case it may as well compute the new state flags.
 *
 *
 * In conjuction with the incremental bitboard updates, we will need to keep
 * track of incremental updates to the Zobrist hash key as well.
 * 
 **/

void bb_make(board_state *board, move *m) {
    /* primary mover */
    board->bb[board->turn][m->p_mover] ^= m->primary;
    board->bb[board->turn][ALL] ^= m->primary;

    /* secondary mover*/
    if (m->s_mover != NO_PIECE) {
        board->bb[board->turn][m->s_mover] ^= m->secondary;
        board->bb[board->turn][ALL] ^= m->secondary;
    }
    
    /* tertiary mover */
    if(m->t_mover != NO_PIECE) {
        board->bb[!board->turn][m->t_mover] ^= m->tertiary;
        board->bb[!board->turn][ALL] ^= m->tertiary;
    }
}

void make(board_state *board, move *m) {
    /* modify bitboards */
    bb_make(board, m);
    board->turn = !board->turn;
}

/* unused */
void unmake(board_state *board, move *m) {
    board->turn = !board->turn;
    bb_make(board, m);
}
