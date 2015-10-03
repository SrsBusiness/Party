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
//#include "bboard_utils.h"
//
/* alpha is lower-bound, beta upper-bound */
/* alpha is the best that black can do, but that is dependent on white's
 * responses i.e. 
 *
 * beta is the reverse */

/* Black is trying to minimize the score */
int32_t min(struct board_state *board, int32_t alpha, int32_t beta, int depth_left) {
    if (depth_left == 0) {
        return compute_material(board);
    }
    int32_t final_score;
    struct priority_queue move_list;
    priority_queue_init(&move_list, 64); /* Arbitrary capacity */
    generate_moves_black(board, &move_list);
    struct move *m;
    struct board_flags save = board->flags;
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        make(board, m);
        int32_t score = max(board, alpha, beta, depth_left - 1);
        unmake(board, m);
        free(m);
        board->flags = save;
        /* If this score is lower/equal to alpha, then this position is already
         * at least as good as the best black can hope for, so we can abandon
         * this branch
         **/
        if (score <= alpha) {
            final_score = alpha;
            goto CLEANUP;
        }
        /* If this score is lower than the upper bound, then we need to update
         * the best white can hope for
         **/
        if (score < beta) {
            beta = score;
        }
    }
    final_score = beta;
CLEANUP:
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        free(m);
    }
    priority_queue_destroy(&move_list);
    return final_score;
}

/* White is trying to maximize the score */
int32_t max(struct board_state *board, int32_t alpha, int32_t beta, int depth_left) {
    if (depth_left == 0) {
        return compute_material(board);
    }
    int32_t final_score;
    struct priority_queue move_list;
    priority_queue_init(&move_list, 64);
    generate_moves_white(board, &move_list);
    struct move *m;
    struct board_flags save = board->flags;
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        make(board, m);
        int32_t score = min(board, alpha, beta, depth_left - 1);
        unmake(board, m);
        free(m);
        board->flags = save;
        if (score >= beta) {
            final_score = beta;
            goto CLEANUP;
        }
        if (score > alpha) {
            alpha = score;
        }
    }
    final_score = alpha;
CLEANUP:
    while ((m = priority_queue_pop(&move_list)) != NULL) {
        free(m);
    }
    priority_queue_destroy(&move_list);
    return final_score;
}

void search(struct board_state *board, int turn, int depth, struct move *best_move) {
    int32_t min_max;
    struct priority_queue move_list;
    struct board_flags save = board->flags;
    struct move *m;
    priority_queue_init(&move_list, 64);
    switch(turn) {
        case WHITE:
            min_max = -INT32_MAX;
            generate_moves_white(board, &move_list);
            while ((m = priority_queue_pop(&move_list)) != NULL) {
                make(board, m);
                int32_t score = min(board, -INT32_MAX, INT32_MAX, depth);
                if (score > min_max) {
                    min_max = score;
                    *best_move = *m;
                }
                unmake(board, m);
                free(m);
                board->flags = save;
            }
            break;
        case BLACK:
            min_max = INT32_MAX; /* for symmetry */
            generate_moves_black(board, &move_list);
            while ((m = priority_queue_pop(&move_list)) != NULL) {
                make(board, m);
                int32_t score = max(board, -INT32_MAX, INT32_MAX, depth);
                if (score < min_max) {
                    min_max = score;
                    *best_move = *m;
                }
                unmake(board, m);
                free(m);
                board->flags = save;
            }
            break;
    }
    priority_queue_destroy(&move_list);
}
