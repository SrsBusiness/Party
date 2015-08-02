#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>
 
#define WHITE 0
#define BLACK 1

#define KING    0
#define QUEEN   1
#define BISHOP  2
#define KNIGHT  3
#define ROOK    4
#define PAWN    5
#define ALL     6
#define NO_PIECE    7

#define SENTINAL 0x060438fAA905AA1Bul

#define BLACKOUT 0xFFFFFFFFFFFFFFFFul

// File Masks
#define AFILE       0x0101010101010101ul
#define BFILE       0x0202020202020202ul
#define CFILE       0x0404040404040404ul
#define DFILE       0x0808080808080808ul
#define EFILE       0x1010101010101010ul
#define FFILE       0x2020202020202020ul
#define GFILE       0x4040404040404040ul
#define HFILE       0x8080808080808080ul

// Rank Masks
#define RANK1       0x00000000000000FFul
#define RANK2       0x000000000000FF00ul
#define RANK3       0x0000000000FF0000ul
#define RANK4       0x00000000FF000000ul
#define RANK5       0x000000FF00000000ul
#define RANK6       0x0000FF0000000000ul
#define RANK7       0x00FF000000000000ul
#define RANK8       0xFF00000000000000ul

// Diagonals from bottom left to top right
#define A7B8        0x0201000000000000ul
#define A6C8        0x0402010000000000ul
#define A5D8        0x0804020100000000ul
#define A4E8        0x1008040201000000ul
#define A3F8        0x2010080402010000ul
#define A2G8        0x4020100804020100ul
#define A1H8        0x8040201008040201ul
#define B1H7        0x0080402010080402ul
#define C1H6        0x0000804020100804ul
#define D1H5        0x0000008040201008ul
#define E1H4        0x0000000080402010ul
#define F1H3        0x0000000000804020ul
#define G1H2        0x0000000000008040ul

// Diagonals from top left to bottom right
#define A2B1        0x0000000000000102ul
#define A3C1        0x0000000000010204ul
#define A4D1        0x0000000001020408ul
#define A5E1        0x0000000102040810ul
#define A6F1        0x0000010204081020ul
#define A7G1        0x0001020408102040ul
#define A8H1        0x0102040810204080ul
#define B8H2        0x0204081020408000ul
#define C8H3        0x0408102040800000ul
#define D8H4        0x0810204080000000ul
#define E8H5        0x1020408000000000ul
#define F8H6        0x2040800000000000ul
#define G8H7        0x4080000000000000ul

// Light/Dark square masks
#define LIGHT       0x55AA55AA55AA55AAul
#define DARK        0xAA55AA55AA55AA55ul

// Starting Squares of pieces
#define BB_KING        0x1000000000000010ul
#define BB_QUEEN       0x0800000000000008ul
#define BB_BISHOP      0x2400000000000024ul
#define BB_KNIGHT      0x4200000000000042ul
#define BB_ROOK        0x8100000000000081ul
#define BB_PAWN        0x00FF00000000FF00ul
#define BB_WHITE       0x000000000000FFFFul
#define BB_BLACK       0xFFFF000000000000ul


// macro functions on bitboards

/* bitscan: 
 * lsb returns index of least significant bit 
 * msb returns index of most significant bit
 **/
#define lsb(board) (__builtin_ctzll(board))
#define msb(board) (63 - __builtin_clzll(board))
#define popcnt(board) (__builtin_popcountll(board))
//#define bit_scan(board) (__builtin_ffsll(board))

// flips the bytes of the word
#define flipv(board)    (__builtin_bswap64(board))

// returns the word rotated, hopefully compiler optimized
#define rolll(x, y) (((x) << (y)) | ((uint64_t)(x) >> (8 * sizeof(uint64_t) - (y))))
#define rorll(x, y) (((uint64_t)(x) >> (y)) | ((x) << (8 * sizeof(uint64_t) - (y))))

// directional shifts by one square
// kings/pawns
#define north_one(x)    ((x) << 8)
#define south_one(x)    ((x) >> 8)
#define east_one(x) (((x) << 1) & ~AFILE)
#define west_one(x) (((x) >> 1) & ~HFILE)
#define ne_one(x)   (((x) << 9) & ~AFILE)
#define nw_one(x)   (((x) << 7) & ~HFILE)
#define se_one(x)   (((x) >> 7) & ~AFILE)
#define sw_one(x)   (((x) >> 9) & ~HFILE)


// knights


//#define bishop_attacks(x) (noEaOccl(x) | soEaOccl(x) | noWeOccl(x) | soWeOccl(x))
//#define rook_attacks(x) (nortOccl(x) | eastOccl(x) | soutOccl(x) | westOccl(x))
//#define queen_attacks(x) (bishop_attacks(x) | rook_attacks(x))



typedef uint64_t bboard;
// square of passed pawns

extern const bboard square_pp[64];

// for black's pov, just vertical flip the index, index into the table, then
// vertical flip the bitboard.


#define fliph(x)        fliph0(x)
bboard fliph0 (bboard x);
bboard fliph1 (bboard x);
bboard fliph2 (bboard x);


bboard flipld(bboard);
bboard flipdd(bboard);
bboard rotate_pi(bboard);
bboard rotate_clockwise(bboard);
bboard rotate_anticlockwise(bboard);

bboard north_span(bboard);
bboard south_span(bboard);
bboard east_span(bboard);
bboard west_span(bboard);
bboard ne_span(bboard);
bboard nw_span(bboard);
bboard se_span(bboard);
bboard sw_span(bboard);

#define w_front_span(x) (north_span(x))
#define w_rear_span(x)  (south_span(x))
#define b_front_span(x) (south_span(x))
#define b_rear_span(x)  (north_span(x))

bboard north_fill(bboard);
bboard south_fill(bboard);
bboard east_fill(bboard);
bboard west_fill(bboard);
bboard ne_fill(bboard);
bboard nw_fill(bboard);
bboard se_fill(bboard);
bboard sw_fill(bboard);

#define w_front_fill(x) (north_fill(x))
#define w_rear_fill(x)  (south_fill(x))
#define b_front_fill(x) (south_fill(x))
#define b_rear_fill(x)  (north_fill(x))

#define file_fill(x)    (north_fill(x) | south_fill(x))
bboard south_occluded_fill(bboard, bboard);
bboard north_occluded_fill(bboard, bboard);
bboard east_occluded_fill(bboard, bboard);
bboard ne_occluded_fill(bboard, bboard);
bboard se_occluded_fill(bboard, bboard);
bboard west_occluded_fill(bboard, bboard);
bboard sw_occluded_fill(bboard, bboard);
bboard nw_occluded_fill(bboard, bboard);

// iterate through all pieces in a bboard
// bboard b;
// while(b){
//     int index = __builtin_ctz(b);
//     ...
//     b &= b - 1;
//
// }

void display_bboard(bboard, int, int);
void display_text(char *, int, int);

typedef struct _magic_entry {
    const bboard *attack_table; /* lookup table of attack sets */
    bboard mask;    /* occupancy mask */
    bboard magic;   /* magic number */
    int shift;  /* how much to shift by */
} magic_entry;

typedef struct _board_flags {
    int castle_q[2];    /* 0 if either king or queen rook has moved, 1 otherwise */
    int castle_k[2];    /* 0 if either king or king rook has moved, 1 otherwise */
    bboard en_passant[2];  /* bitboard of pawns that have just double pushed */
} board_flags;

typedef struct _board_state {
    //char board88[64];
    int turn;   /* 0 if white, 1 if black */
    /* bitboards */
    bboard bb[2][8];
    board_flags flags;
} board_state;

/* 
 *  Mailbox works as follows.
 *  Most significant bit refers to color
 *  Other bits determine piece
 **/

typedef struct _move {
    int p_mover, s_mover;
    bboard primary;     /* xor with primary moving piece */
    bboard secondary;   /* xor with secondary piece */
    bboard capture;     /* xor with captive */
    board_flags flags;
} move;

#define all_pieces(x) ((x)->bb[WHITE][KING] | (x)->bb[BLACK][KING] | (x)->bb[WHITE][QUEEN] |\
        (x)->bb[BLACK][QUEEN] | (x)->bb[WHITE][BISHOP] | (x)->bb[BLACK][BISHOP] | (x)->bb[WHITE][KNIGHT] |\
        (x)->bb[BLACK][KNIGHT] | (x)->bb[WHITE][ROOK] | (x)->bb[BLACK][ROOK] | (x)->bb[WHITE][PAWN] |\
        (x)->bb[BLACK][PAWN])

#define pieces(x, y) ((x).kings[y] | (x).queens[y] | (x).bishops[y] |\
        (x).knights[y] | (x).rooks[y] | (x).pawns[y] |)

#define copy_flags(m, board) do{(m)->en_passant_pre[0] = (board)->en_passant[0];\
        (m)->en_passant_pre[1] = (board)->en_passant[1];\
        (m)->castle_q_pre[0] = (board)->castle_q[0];\
        (m)->castle_q_pre[1] = (board)->castle_q[1];\
        (m)->castle_k_pre[0] = (board)->castle_k[0];\
        (m)->castle_k_pre[1] = (board)->castle_k[1];} while(0)



#endif
