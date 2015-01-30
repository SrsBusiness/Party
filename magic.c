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
#include "rook.h"
#include "bishop.h"

//bboard rook_occupancy_masks[64];
//bboard bishop_occupancy_masks[64];
//char rook_magic_shifts[64];
//char bishop_magic_shifts[64];

//uint64_t rook_magic_numbers[64];
//uint64_t bishop_magic_numbers[64];

void generate_rook_occupancy_masks(bboard *, char *);
void generate_bishop_occupancy_masks(bboard *, char *);

bboard number_to_occupancy(int, bboard);
bboard generate_attack_set_rook(bboard, bboard);
bboard generate_attack_set_bishop(bboard, bboard);
void generate_magic_rook(bboard *);
void generate_magic_bishop(bboard *);

void generate_rook_attack_table(int);
void generate_bishop_attack_table(int);

void rook_magic_table();

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


int main(int argc, char **argv) {
    
    if(argc < 2) {
        fprintf(stderr, "Mukund is a faggot\n");
        exit(1);
    }
    
    int index;
    sscanf(argv[1], "%d", &index);
    int urand = open("/dev/urandom", O_RDONLY);
    int seed;
    read(urand, &seed, sizeof(seed));
    srand(seed);
    //Signal(SIGINT, handler);
    //printf("Registered signal handler\n");
    //generate_rook_occupancy_masks(rook_occupancy_masks, rook_magic_shifts);
    //generate_bishop_occupancy_masks(bishop_occupancy_masks, bishop_magic_shifts);
    int i;
    bboard magic[64];
    //generate_magic_rook(magic);
    //generate_bishop_attack_table(index);
    //
    rook_magic_table();
    //for (i = 0; i < 64; i++) {
    //    printf("0x%llX,\n", magic[i]);
    //}
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

bboard generate_attack_set_bishop(bboard gen, bboard occupancy) {
    return (ne_occluded_fill(gen, ~ne_one(occupancy)) |
        nw_occluded_fill(gen, ~nw_one(occupancy)) |
        se_occluded_fill(gen, ~se_one(occupancy)) |
        sw_occluded_fill(gen, ~sw_one(occupancy))) ^ gen;
}

void generate_magic_rook(bboard *magics) {
    int fail;
    int i;
    for (i = 0; i < 64; i++) {
        bboard occupancy_mask = rook_occupancy_masks[i];
        int num_occupancy_bits = 
            __builtin_popcountll(occupancy_mask);
        int max_occupancies = (bboard)1 << num_occupancy_bits;

        bboard *used = malloc(sizeof(bboard) << num_occupancy_bits);
        do{
            current = rand64() & rand64() & rand64();
            fail = 0;
            memset(used, 0xFF, sizeof(bboard) << num_occupancy_bits);
            int j;
            for (j = 0; j < max_occupancies && !fail; j++) {
                bboard occupancy = number_to_occupancy(j, occupancy_mask);
                bboard attack = generate_attack_set_rook((bboard)1 << i, 
                        occupancy);
                uint32_t hash = (occupancy * current) >> (64 - num_occupancy_bits);
                if (used[hash] == 0xFFFFFFFFFFFFFFFF) {
                    used[hash] = attack;
                } else if (used[hash] != attack) {
                    fail = 1;
                }
            }
            if (!fail)
                magics[i] = current;
        } while (fail);
    }
}

void generate_magic_bishop(bboard *magics) {
    int fail;
    int i;
    for (i = 0; i < 64; i++) {
        bboard occupancy_mask = bishop_occupancy_masks[i];
        int num_occupancy_bits = 
            __builtin_popcountll(occupancy_mask);
        int max_occupancies = (bboard)1 << num_occupancy_bits;

        bboard *used = malloc(sizeof(bboard) << num_occupancy_bits);
        do{
            current = rand64() & rand64() & rand64();
            fail = 0;
            memset(used, 0xFF, sizeof(bboard) << num_occupancy_bits);
            int j;
            for (j = 0; j < max_occupancies && !fail; j++) {
                bboard occupancy = number_to_occupancy(j, occupancy_mask);
                bboard attack = generate_attack_set_bishop((bboard)1 << i, 
                        occupancy);
                uint64_t hash = (occupancy * current) >> (64 - num_occupancy_bits);
                if (used[hash] == 0xFFFFFFFFFFFFFFFF) {
                    used[hash] = attack;
                } else if (used[hash] != attack) {
                    fail = 1;
                }
            }
            if (!fail)
                magics[i] = current;
        } while (fail);
    }
}

void generate_rook_attack_table(int index) {
    uint64_t magic = rook_magic_numbers[index];
    bboard occupancy_mask = rook_occupancy_masks[index];    
    int num_occupancy_bits = __builtin_popcountll(occupancy_mask);
    printf("const bboard rook_%d[%d] = {\n", index, 1 << num_occupancy_bits);
    int max_occupancies = 1 << num_occupancy_bits;
    bboard *attack_table = malloc(sizeof(bboard) << num_occupancy_bits);
    int i;
    for (i = 0; i < max_occupancies; i++) {
        bboard occupancy = number_to_occupancy(i, occupancy_mask);
        attack_table[(occupancy * magic) >> (64 - num_occupancy_bits)] =
            generate_attack_set_rook((bboard)1 << index, occupancy);
    }
    for(i = 0; i < max_occupancies; i++) {
        printf("    0x%llX,\n", attack_table[i]);
    }
    printf("};\n");
}

void generate_bishop_attack_table(int index) {
    uint64_t magic = bishop_magic_numbers[index];
    bboard occupancy_mask = bishop_occupancy_masks[index];    
    int num_occupancy_bits = __builtin_popcountll(occupancy_mask);
    printf("const bboard bishop_%d[%d] = {\n", index, 1 << num_occupancy_bits);
    int max_occupancies = 1 << num_occupancy_bits;
    bboard *attack_table = malloc(sizeof(bboard) << num_occupancy_bits);
    int i;
    for (i = 0; i < max_occupancies; i++) {
        bboard occupancy = number_to_occupancy(i, occupancy_mask);
        attack_table[(occupancy * magic) >> (64 - num_occupancy_bits)] =
            generate_attack_set_bishop((bboard)1 << index, occupancy);
    }
    //clear_all();
    for(i = 0; i < max_occupancies; i++) {
        //display_text("Bishop position", 1, 1);
        //display_text("Occupancy", 1, 20);
        //display_text("Attack set", 1, 39);
        //bboard occupancy = number_to_occupancy(i, occupancy_mask);
        //bboard attack = 
        //    attack_table[(occupancy * magic) >> (64 - num_occupancy_bits)];
        //display_bboard((bboard)1 << index, 3, 1);
        //display_bboard(occupancy, 3, 20);
        //display_bboard(attack, 3, 39);
        //getchar();
        printf("    0x%llX,\n", attack_table[i]);
    }
    printf("};\n");
}

void rook_magic_table() {
    printf("const magic_entry rook_magic_table[64] = {\n");
    int i;
    for (i = 0; i < 64; i++) {
        printf("    {rook_%d, 0x%llx, 0x%llx},n", i, rook_occupancy_masks[i], 
                rook_magic_numbers[i]);
    }
    printf("};\n");
}

void handler(int sig) {
    move_cursor(30, 1);
    char str[100];
    sprintf(str, "Current: 0x%llX, magic: 0x%llX, least: %lu\n", current, magic, least);
    write(1, str, strlen(str));
}


