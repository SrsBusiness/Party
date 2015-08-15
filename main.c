#include <stdio.h>
#include <stdlib.h>
#include "bitboard.h"
#include "search.h"
#include "minicurses.h"
#include "hash.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <depth>\n", argv[0]);
        return 1;
    }
    
    int depth = atoi(argv[1]);

    board_state pos = initial_state;

    #ifdef DEBUG
    clear_line();
    #endif
    
    int eval = dumb_search(&pos, -100, 100, depth);
    //printf("%d\n", eval);
    int seed;
    FILE *f = fopen("/dev/urandom", "r");
    fread(&seed, 1, sizeof(int), f);
    ZOBRIST_prng_init(seed);
    printf("int seed = 0x%llX;\n", seed);
    print_literal_keys();
}
