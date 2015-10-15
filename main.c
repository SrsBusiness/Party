#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bitboard.h"
#include "search.h"
#include "minicurses.h"
#include "hash.h"
#include "move.h"
#include "bboard_utils.h"
#include "eval.h"

#ifdef UNIT_TEST
#include "unit_tests.h"
#endif

const uint64_t files[] = {
    BITBOARD_AFILE,
    BITBOARD_BFILE,
    BITBOARD_CFILE,
    BITBOARD_DFILE,
    BITBOARD_EFILE,
    BITBOARD_FFILE,
    BITBOARD_GFILE,
    BITBOARD_HFILE
};

const uint64_t ranks[] = {
    BITBOARD_RANK1,
    BITBOARD_RANK2,
    BITBOARD_RANK3,
    BITBOARD_RANK4,
    BITBOARD_RANK5,
    BITBOARD_RANK6,
    BITBOARD_RANK7,
    BITBOARD_RANK8
};


uint64_t algebraic_to_bitboard(const char *move);

int find_matching_move(struct board_state *board, uint64_t user_move,
        struct move *m);

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <depth>\n", argv[0]);
        return 1;
    }
    
    int depth = atoi(argv[1]);
    
    ZOBRIST_transposition_init(256);
    struct board_state pos = BOARD_INITIAL_STATE;
    struct board_state empty;
    memset(&empty, 0, sizeof(struct board_state));
    struct move best_move, user_move;
    char user_input[6] = {0};
    clear_all();
    while (1) {
        print_board(&pos, 1, 1);
        move_cursor(12, 1);
        printf("%lu nodes visited\n", nodes_visited);
        printf("score: %d\n", compute_material(&pos));
        reset_attr();
        //print_transposition_table(12, 1);
        uint64_t bb = 0;
        do {
            fgets(user_input, sizeof(user_input), stdin);
            user_input[4] = 0;
        } while ((bb = algebraic_to_bitboard(user_input)) == 0 ||
                find_matching_move(&pos, bb, &user_move) == 0);
        //sleep(1);
        make(&pos, &user_move);
        clear_all();
        print_board(&pos, 1, 1);
        reset_attr();
        search(&pos, depth, &best_move);
        make(&pos, &best_move);
    }
}

uint64_t algebraic_to_bitboard(const char *move) {
    size_t len = strlen(move);
    if (len != 4)
        return 0ul;
    if (move[0] < 'a' || move[0] > 'h' ||
            move[2] < 'a' || move[2] > 'h' ||
            move[1] < '1' || move[1] > '8' ||
            move[3] < '1' || move[3] > '8') 
        return 0ul;
    return (files[move[0] - 'a'] & ranks[move[1] - '1']) |
        (files[move[2] - 'a'] & ranks[move[3] - '1']);
}

int find_matching_move(struct board_state *board, uint64_t user_move,
        struct move *m) {
    int success = 0;
    struct priority_queue move_list;
    priority_queue_init(&move_list, 64);
    switch(board->turn) {
        case WHITE:
            generate_moves_white(board, &move_list);
            break;
        case BLACK:
            generate_moves_black(board, &move_list);
            break;
    }
    struct move *next;
    while ((next = priority_queue_pop(&move_list)) != NULL) {
        if (next->primary == user_move) {
            *m = *next;
            success = 1;
        }
        free(next);
    }
    priority_queue_destroy(&move_list);
    return success;
}
