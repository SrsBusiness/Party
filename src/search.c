#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "pawn.h"
#include "knight.h"
#include "rook.h"
#include "bishop.h"
#include "search.h"
#include "move.h"
#include "eval.h"
#include "hash.h"
#include "bboard_utils.h"

#ifdef UNIT_TEST
#include "unit_tests.h"
#endif



//
/* alpha is lower-bound, beta upper-bound */
/* alpha is the best that black can do, but that is dependent on white's
 * responses i.e. 
 *
 * beta is the reverse */

/* Black is trying to minimize the score */

uint64_t nodes_visited = 0;

int32_t min(struct board_state *board, int32_t alpha, int32_t beta, int depth_left) {
    nodes_visited++;
    if (depth_left == 0) {
        return quiescent_min(board, alpha, beta);
        //return compute_material(board);
    }
    int32_t final_score = beta;
    struct priority_queue move_list;
    priority_queue_init(&move_list, 64); /* Arbitrary capacity */
    int move_count = generate_moves_black(board, &move_list);

    /* Check for stalemate/checkmate */
    if (move_count == 0) {
        if (in_check(board, BLACK)) { /* checkmate */
            final_score = SCORE_MAX; 
        } else { /* stalemate */
            final_score = 0;
        }
    }
    struct move *m;
    struct board_flags save = board->flags;
    uint64_t old_hash = board->hash;
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        make(board, m);
        int32_t score = max(board, alpha, beta, depth_left - 1);
        unmake(board, m, &save, old_hash);
        free(m);
        /* If this score is lower/equal to alpha, then this position is already
         * at least as good as the best black can hope for, so we can abandon
         * this branch
         **/
        if (score <= alpha) {
            final_score = alpha;
            break;
        }
        /* If this score is lower than the upper bound, then we need to update
         * the best white can hope for
         **/
        if (score < beta) {
            beta = score;
            final_score = beta;
        }
    }
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        free(m);
    }
    priority_queue_destroy(&move_list);
    set_transposition_score(old_hash, final_score);
    return final_score;
}

/* White is trying to maximize the score */
int32_t max(struct board_state *board, int32_t alpha, int32_t beta, int depth_left) {
    nodes_visited++;
    if (depth_left == 0) {
        return quiescent_max(board, alpha, beta);
        //return compute_material(board);
    }
    int32_t final_score = alpha;
    struct priority_queue move_list;
    priority_queue_init(&move_list, 64);
    int move_count = generate_moves_white(board, &move_list);

    /* Check for stalemate/checkmate */
    if (move_count == 0) {
        if (in_check(board, WHITE)) { /* checkmate */
            final_score = SCORE_MIN; 
        } else { /* stalemate */
            final_score = 0;
        }
    }

    struct move *m;
    struct board_flags save = board->flags;
    uint64_t old_hash = board->hash;
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        make(board, m);
        int32_t score = min(board, alpha, beta, depth_left - 1);
        unmake(board, m, &save, old_hash);
        free(m);
        if (score >= beta) {
            final_score = beta;
            break;
        }
        if (score > alpha) {
            alpha = score;
            final_score = alpha;
        }
    }
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        free(m);
    }
    priority_queue_destroy(&move_list);
    set_transposition_score(old_hash, final_score);
    return final_score;
}

int32_t quiescent_min(struct board_state *board, int32_t alpha, int32_t beta) {
    //print_board(board, 1, 1);
    nodes_visited++;
    int32_t stand_pat = compute_material(board);
    //int32_t stand_pat = 0;
    if (!in_check(board, BLACK)) { /* No pruning when in check */
        if (stand_pat <= alpha) {
            return alpha;
        }
        if (stand_pat < beta) {
            beta = stand_pat;
        }
    }
    int32_t final_score = beta;
    struct priority_queue move_list;
    priority_queue_init(&move_list, 64);
    int move_count = generate_moves_black(board, &move_list);

    /* Check for stalemate/checkmate */
    if (move_count == 0) {
        if (in_check(board, BLACK)) { /* checkmate */
            final_score = SCORE_MAX; 
        } else { /* stalemate */
            final_score = 0;
        }
    }

    struct move *m;
    struct board_flags save = board->flags;
    uint64_t old_hash = board->hash;
    while ((m = priority_queue_pop(&move_list)) != NULL)  {
        /* if capture */
        if (m->t_mover != NO_PIECE) {
            make(board, m);
            int32_t score = quiescent_max(board, alpha, beta);
            unmake(board, m, &save, old_hash);
            if (score <= alpha) {
                final_score = alpha;
                free(m);
                break;
            }
            if (score < beta) {
                beta = score;
                final_score = beta;
            }
        }
        free(m);
    }
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        free(m);
    }
    priority_queue_destroy(&move_list);
    set_transposition_score(old_hash, final_score);
    return final_score;
}

int32_t quiescent_max(struct board_state *board, int32_t alpha, int32_t beta) {
    //print_board(board, 1, 1);
    nodes_visited++;
    int32_t stand_pat = compute_material(board);
    //int32_t stand_pat = 0;
    if (stand_pat >= beta) {
        return beta;
    }
    if (stand_pat > alpha) {
        alpha = stand_pat;
    }
    int32_t final_score = alpha;
    struct priority_queue move_list;
    priority_queue_init(&move_list, 64);
    int move_count = generate_moves_white(board, &move_list);

    /* Check for stalemate/checkmate */
    if (move_count == 0) {
        if (in_check(board, BLACK)) { /* checkmate */
            final_score = SCORE_MIN; 
        } else { /* stalemate */
            final_score = 0;
        }
    }
    
    struct move *m;
    struct board_flags save = board->flags;
    uint64_t old_hash = board->hash;
    while ((m = priority_queue_pop(&move_list)) != NULL)  {
        /* if capture */
        if (m->t_mover != NO_PIECE) {
            make(board, m);
            int score = quiescent_min(board, alpha, beta);
            unmake(board, m, &save, old_hash);
            if (score >= beta) {
                final_score = beta;
                free(m);
                break;
            }
            if (score > alpha) {
                alpha = score;
                final_score = alpha;
            }
        }
        free(m);
    }
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        free(m);
    }
    priority_queue_destroy(&move_list);
    set_transposition_score(old_hash, final_score);
    return final_score;
}


int search(struct board_state *board, int depth, struct move *best_move) {
    int status = NORMAL_MOVE;
    int32_t min_max;
    struct priority_queue move_list;
    struct board_flags save = board->flags;
    uint64_t old_hash = board->hash;
    struct move *m;
    priority_queue_init(&move_list, 64);
    int move_count;
    switch(board->turn) {
        case WHITE:
            min_max = -INT32_MAX;
            move_count = generate_moves_white(board, &move_list);
            if (move_count == 0) {
                if (in_check(board, WHITE)) {
                    status = CHECKMATE;
                } else {
                    status = STALEMATE;
                }
            }
            while ((m = priority_queue_pop(&move_list)) != NULL) {
                make(board, m);
                int32_t score = min(board, SCORE_MIN, SCORE_MAX, depth - 1);
                if (score > min_max) {
                    min_max = score;
                    *best_move = *m;
                }
                unmake(board, m, &save, old_hash);
                free(m);
            }
            break;
        case BLACK:
            min_max = INT32_MAX; /* for symmetry */
            move_count = generate_moves_black(board, &move_list);
            if (move_count == 0) {
                if (in_check(board, BLACK)) {
                    status = CHECKMATE;
                } else {
                    status = STALEMATE;
                }
            }
            while ((m = priority_queue_pop(&move_list)) != NULL) {
                make(board, m);
                int32_t score = max(board, SCORE_MIN, SCORE_MAX, depth - 1);
                if (score < min_max) {
                    min_max = score;
                    *best_move = *m;
                }
                unmake(board, m, &save, old_hash);
                free(m);
            }
            break;
    }
    priority_queue_destroy(&move_list);
    return status;
}

/* iterative deepening */
int id_search(struct board_state *board, int depth, struct move *best_move) {
    int status = board_status(board);
    if (status != NORMAL_MOVE) {
        return status;
    }
    nodes_visited = 0;
    for (int i = 1; i <= depth; i++) {
        search(board, i, best_move); 
    }
    return status;
}



int board_status(struct board_state *board) {
    int move_count;
    switch(board->turn) {
        case WHITE:
            move_count = generate_moves_white(board, NULL);
            break;
        case BLACK:
            move_count = generate_moves_black(board, NULL);
            break;
    }
    if (move_count == 0) {
        if (in_check(board, board->turn)) {
            return CHECKMATE;
        } else {
            return STALEMATE;
        }
    } else {
        return NORMAL_MOVE;
    }
}
