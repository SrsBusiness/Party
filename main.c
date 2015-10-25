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

const char *bb_to_square[] = {
    "a1",
    "b1",
    "c1",
    "d1",
    "e1",
    "f1",
    "g1",
    "h1",
    "a2",
    "b2",
    "c2",
    "d2",
    "e2",
    "f2",
    "g2",
    "h2",
    "a3",
    "b3",
    "c3",
    "d3",
    "e3",
    "f3",
    "g3",
    "h3",
    "a4",
    "b4",
    "c4",
    "d4",
    "e4",
    "f4",
    "g4",
    "h4",
    "a5",
    "b5",
    "c5",
    "d5",
    "e5",
    "f5",
    "g5",
    "h5",
    "a6",
    "b6",
    "c6",
    "d6",
    "e6",
    "f6",
    "g6",
    "h6",
    "a7",
    "b7",
    "c7",
    "d7",
    "e7",
    "f7",
    "g7",
    "h7",
    "a8",
    "b8",
    "c8",
    "d8",
    "e8",
    "f8",
    "g8",
    "h8"
};

uint64_t algebraic_to_bitboard(const char *move, uint64_t *source);
void move_to_algebraic(const struct move *m, char *str);

int find_matching_move(struct board_state *board, uint64_t user_move,
        uint64_t src, struct move *m);

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <depth> <log>\n", argv[0]);
        return 1;
    }
    
    int depth = atoi(argv[1]);
    char *file_path = argv[2];
    FILE *f = fopen(file_path, "w+");
    
    ZOBRIST_transposition_init(256);
    struct board_state pos = BOARD_INITIAL_STATE;
    struct board_state empty;
    memset(&empty, 0, sizeof(struct board_state));
    struct move best_move, user_move;
    char user_input[6] = {0};
    char move_str[6] = {0};
    clear_all();
    int turn_count = 1;
    while (1) {
        print_board(&pos, 1, 1);
        move_cursor(12, 1);
        printf("%lu nodes visited\n", nodes_visited);
        printf("score: %d\n", compute_material(&pos));
        int status = board_status(&pos);
        switch(status) {
            case CHECKMATE:
                printf("Checkmate\n");
                return 0;
                break;
            case STALEMATE:
                printf("Stalemate\n");
                return 0;
                break;
        }
        reset_attr();
        //print_transposition_table(12, 1);
        uint64_t bb = 0;
        uint64_t source;
        do {
            fgets(user_input, sizeof(user_input), stdin);
            user_input[4] = 0;
        } while ((bb = algebraic_to_bitboard(user_input, &source)) == 0 ||
                find_matching_move(&pos, bb, source, &user_move) == 0);
        //sleep(1);
        make(&pos, &user_move);
        move_to_algebraic(&user_move, move_str);
        fprintf(f, "%d. %s ", turn_count, move_str);
        clear_all();
        print_board(&pos, 1, 1);
        reset_attr();
        status = search(&pos, depth, &best_move);
        move_cursor(12, 1);
        switch(status) {
            case CHECKMATE:
                printf("Checkmate\n");
                fprintf(f, "\n");
                return 0;
                break;
            case STALEMATE:
                printf("Stalemate\n");
                fprintf(f, "\n");
                return 0;
                break;
            case NORMAL_MOVE:
                make(&pos, &best_move);
                move_to_algebraic(&best_move, move_str);
                fprintf(f, "%s\n", move_str);
                break;
        }
        fflush(f);
        turn_count++;
    }
    fclose(f);
}

uint64_t algebraic_to_bitboard(const char *move, uint64_t *source) {
    size_t len = strlen(move);
    if (len != 4)
        return 0ul;
    if (move[0] < 'a' || move[0] > 'h' ||
            move[2] < 'a' || move[2] > 'h' ||
            move[1] < '1' || move[1] > '8' ||
            move[3] < '1' || move[3] > '8') 
        return 0ul;
    uint64_t src = files[move[0] - 'a'] & ranks[move[1] - '1'];
    uint64_t dest = files[move[2] - 'a'] & ranks[move[3] - '1'];
    *source = src;
    return src | dest;
}

void move_to_algebraic(const struct move *m, char *str) {
    uint64_t src = m->primary_src;
    uint64_t dest = m->primary_src ^ m->primary;
    snprintf(str, 6, "%s%s", bb_to_square[lsb(src)], bb_to_square[lsb(dest)]);
}

int find_matching_move(struct board_state *board, uint64_t user_move,
        uint64_t src, struct move *m) {
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
        if (next->primary == user_move && next->primary_src == src) {
            *m = *next;
            success = 1;
        }
        free(next);
    }
    priority_queue_destroy(&move_list);
    return success;
}
