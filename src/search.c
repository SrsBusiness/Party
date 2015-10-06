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
        return compute_material(board);
    }
    int32_t final_score;
    struct priority_queue move_list;
    priority_queue_init(&move_list, 64); /* Arbitrary capacity */
    generate_moves_black(board, &move_list);
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
    set_transposition_score(old_hash, final_score);
    return final_score;
}

/* White is trying to maximize the score */
int32_t max(struct board_state *board, int32_t alpha, int32_t beta, int depth_left) {
    nodes_visited++;
    if (depth_left == 0) {
        return compute_material(board);
    }
    int32_t final_score;
    struct priority_queue move_list;
    priority_queue_init(&move_list, 64);
    generate_moves_white(board, &move_list);
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
    set_transposition_score(old_hash, final_score);
    return final_score;
}

void search(struct board_state *board, int depth, struct move *best_move) {
    int32_t min_max;
    struct priority_queue move_list;
    struct board_flags save = board->flags;
    uint64_t old_hash = board->hash;
    struct move *m;
    priority_queue_init(&move_list, 64);
    nodes_visited = 0;
    switch(board->turn) {
        case WHITE:
            min_max = -INT32_MAX;
            generate_moves_white(board, &move_list);

            while ((m = priority_queue_pop(&move_list)) != NULL) {
                make(board, m);
                int32_t score = min(board, -INT32_MAX, INT32_MAX, depth - 1);
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
            generate_moves_black(board, &move_list);
            while ((m = priority_queue_pop(&move_list)) != NULL) {
                make(board, m);
                int32_t score = max(board, -INT32_MAX, INT32_MAX, depth - 1);
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
}
