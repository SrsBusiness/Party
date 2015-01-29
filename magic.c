#define __USE_GNU

#include <stdio.h>
#include "bitboard.h"
#include "minicurses.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

bboard rook_occupancy_masks[64];
bboard bishop_occupancy_masks[64];
char rook_magic_shifts[64];
char bishop_magic_shifts[64];

void generate_rook_occupancy_masks(bboard *, char *);
void generate_bishop_occupancy_masks(bboard *, char *);

bboard number_to_occupancy(int, bboard);
bboard generate_attack_set_rook(bboard, bboard);
bboard generate_magic_rook(int, int);
void handler(int);
 
bboard current, magic;
uint32_t least;

uint64_t rand64() {
    uint64_t result = rand() | ((uint64_t)rand() << 32);
}

sighandler_t Signal(int signum, sighandler_t handler) {
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    sigaction(signum, &action, &old_action);
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    return (old_action.sa_handler);
}


int main() {
    //bboard occupancy = 0x000101010101017E;
    //bboard index = 0xFFF0000000000000;
    //bboard magic = index / occupancy;
    //bboard test = 0x000101010101017E;
    //bboard test_index = (test * magic) & index;
    //printf("%llX\n", magic);
    //printf("%llX\n", index);
    //printf("%llX\n", test_index);
    int urand = open("/dev/urandom", O_RDONLY);
    int seed;
    read(urand, &seed, sizeof(seed));
    srand(seed);
    Signal(SIGINT, handler);
    printf("Registered signal handler\n");
    generate_rook_occupancy_masks(rook_occupancy_masks, rook_magic_shifts);
    generate_bishop_occupancy_masks(bishop_occupancy_masks, bishop_magic_shifts);
    int i;
    clear_all();
    hide_cursor();
    bboard magic[64];
    generate_magic_rook(27, 10);
    show_cursor();
}

void generate_rook_occupancy_masks(bboard *occupancy, char *shifts) {
    int i;
    bboard mask;
    for (i = 0; i < 64; i++) {
        mask = (bboard)1 << i;
        occupancy[i] = (north_span(mask) & ~RANK8) | 
            (south_span(mask) & ~RANK1) | 
            (east_span(mask) & ~HFILE) | 
            (west_span(mask) & ~AFILE);
        shifts[i] = 64 - __builtin_popcountll(occupancy[i]);
    }
}

void generate_bishop_occupancy_masks(bboard *occupancy, char *shifts) {
    bboard exclude = 0x007E7E7E7E7E7E00;
    bboard mask;
    int i;
    for (i = 0; i < 64; i++) {
        mask = (bboard)1 << i;
        occupancy[i] = (ne_span(mask) | nw_span(mask) | 
                se_span(mask) | sw_span(mask)) & exclude;
        shifts[i] = 64 - __builtin_popcountll(occupancy[i]);
    }
}

bboard number_to_occupancy(int num, bboard mask) {
    bboard lsb, result = 0;
    int num_bits = __builtin_popcount(num);
    int num_mask_bits = __builtin_popcountll(mask);
    assert(num_bits <= num_mask_bits);
    int i;
    while (num) {
        lsb = mask & -mask;
        if(num & 1)
            result |= lsb;
        mask ^= lsb;
        num >>= 1;
    }
    return result;
}

bboard generate_attack_set_rook(bboard gen, bboard occupancy) {
    return (north_occluded_fill(gen, ~north_one(occupancy)) |
        south_occluded_fill(gen, ~south_one(occupancy)) |
        east_occluded_fill(gen, ~east_one(occupancy)) |
        west_occluded_fill(gen, ~west_one(occupancy))) ^ gen;
}

bboard generate_magic_rook(int index, int attack_bits) {
    magic = 0xFFFFFFFFFFFFFFFF;
    least = 0xFFFFFFFF;
    bboard *used = malloc(sizeof(bboard) << attack_bits);
    bboard occupancy_mask = rook_occupancy_masks[index];
    int fail;
    int unique;
    int num_occupancy_bits = 
            __builtin_popcountll(occupancy_mask);
    int max_occupancies = (bboard)1 << num_occupancy_bits;
    //move_cursor(29, 1);
    //printf("Max occupancies: %d\n", max_occupancies);
    while(1) {
        current = rand64() & rand64() & rand64();
        fail = unique = 0;
        memset(used, 0xFF, sizeof(bboard) << attack_bits);
                int j;
        for (j = 0; j < max_occupancies && !fail; j++) {
            bboard occupancy = number_to_occupancy(j, occupancy_mask);
            //move_cursor(1, 1);
            //clear_line();
            //printf("current: %llX occupancy: %d\n", current, j);
            //display_bboard(occupancy, 2, 1);
            bboard attack = generate_attack_set_rook((bboard)1 << index, 
                    occupancy);
            //display_bboard(attack, 12, 1);
            uint32_t hash = (occupancy * current) >> (64 - attack_bits);
            if (used[hash] == 0xFFFFFFFFFFFFFFFF) {
                unique++;
                used[hash] = attack;
            } else if (used[hash] != attack) {
                fail = 1;
            }
        }
        if (!fail && unique < least) {
            magic = current;
            least = unique;
        }
    }
    return magic;
}

void handler(int sig) {
    move_cursor(30, 1);
    char str[100];
    sprintf(str, "Current: 0x%llX, magic: 0x%llX, least: %lu\n", current, magic, least);
    write(1, str, strlen(str));
}


