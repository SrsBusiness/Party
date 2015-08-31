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
#define NO_PIECE    -1

#define SENTINAL 0x060438fAA905AA1Bul

#define BITBOARD_BLACKOUT 0xFFFFFFFFFFFFFFFFul

// File Masks
#define BITBOARD_AFILE       0x0101010101010101ul
#define BITBOARD_BFILE       0x0202020202020202ul
#define BITBOARD_CFILE       0x0404040404040404ul
#define BITBOARD_DFILE       0x0808080808080808ul
#define BITBOARD_EFILE       0x1010101010101010ul
#define BITBOARD_FFILE       0x2020202020202020ul
#define BITBOARD_GFILE       0x4040404040404040ul
#define BITBOARD_HFILE       0x8080808080808080ul

#define FILE_SET_AFILE       0x01
#define FILE_SET_BFILE       0x02
#define FILE_SET_CFILE       0x04
#define FILE_SET_DFILE       0x08
#define FILE_SET_EFILE       0x10
#define FILE_SET_FFILE       0x20
#define FILE_SET_GFILE       0x40
#define FILE_SET_HFILE       0x80

// Rank Masks
#define BITBOARD_RANK1       0x00000000000000FFul
#define BITBOARD_RANK2       0x000000000000FF00ul
#define BITBOARD_RANK3       0x0000000000FF0000ul
#define BITBOARD_RANK4       0x00000000FF000000ul
#define BITBOARD_RANK5       0x000000FF00000000ul
#define BITBOARD_RANK6       0x0000FF0000000000ul
#define BITBOARD_RANK7       0x00FF000000000000ul
#define BITBOARD_RANK8       0xFF00000000000000ul

// Diagonals from bottom left to top right
#define BITBOARD_DIAG_A7B8        0x0201000000000000ul
#define BITBOARD_DIAG_A6C8        0x0402010000000000ul
#define BITBOARD_DIAG_A5D8        0x0804020100000000ul
#define BITBOARD_DIAG_A4E8        0x1008040201000000ul
#define BITBOARD_DIAG_A3F8        0x2010080402010000ul
#define BITBOARD_DIAG_A2G8        0x4020100804020100ul
#define BITBOARD_DIAG_A1H8        0x8040201008040201ul
#define BITBOARD_DIAG_B1H7        0x0080402010080402ul
#define BITBOARD_DIAG_C1H6        0x0000804020100804ul
#define BITBOARD_DIAG_D1H5        0x0000008040201008ul
#define BITBOARD_DIAG_E1H4        0x0000000080402010ul
#define BITBOARD_DIAG_F1H3        0x0000000000804020ul
#define BITBOARD_DIAG_G1H2        0x0000000000008040ul

// Diagonals from top left to bottom right
#define BITBOARD_DIAG_A2B1        0x0000000000000102ul
#define BITBOARD_DIAG_A3C1        0x0000000000010204ul
#define BITBOARD_DIAG_A4D1        0x0000000001020408ul
#define BITBOARD_DIAG_A5E1        0x0000000102040810ul
#define BITBOARD_DIAG_A6F1        0x0000010204081020ul
#define BITBOARD_DIAG_A7G1        0x0001020408102040ul
#define BITBOARD_DIAG_A8H1        0x0102040810204080ul
#define BITBOARD_DIAG_B8H2        0x0204081020408000ul
#define BITBOARD_DIAG_C8H3        0x0408102040800000ul
#define BITBOARD_DIAG_D8H4        0x0810204080000000ul
#define BITBOARD_DIAG_E8H5        0x1020408000000000ul
#define BITBOARD_DIAG_F8H6        0x2040800000000000ul
#define BITBOARD_DIAG_G8H7        0x4080000000000000ul

// Light/Dark square masks
#define BITBOARD_LIGHT       0x55AA55AA55AA55AAul
#define BITBOARD_DARK        0xAA55AA55AA55AA55ul

// Starting Squares of pieces
#define BITBOARD_KING        0x1000000000000010ul
#define BITBOARD_QUEEN       0x0800000000000008ul
#define BITBOARD_BISHOP      0x2400000000000024ul
#define BITBOARD_KNIGHT      0x4200000000000042ul
#define BITBOARD_ROOK        0x8100000000000081ul
#define BITBOARD_PAWN        0x00FF00000000FF00ul
#define BITBOARD_WHITE       0x000000000000FFFFul
#define BITBOARD_BLACK       0xFFFF000000000000ul

#define BITBOARD_A1     (BITBOARD_AFILE & BITBOARD_RANK1)
#define BITBOARD_A2     (BITBOARD_AFILE & BITBOARD_RANK2)
#define BITBOARD_A3     (BITBOARD_AFILE & BITBOARD_RANK3)
#define BITBOARD_A4     (BITBOARD_AFILE & BITBOARD_RANK4)
#define BITBOARD_A5     (BITBOARD_AFILE & BITBOARD_RANK5)
#define BITBOARD_A6     (BITBOARD_AFILE & BITBOARD_RANK6)
#define BITBOARD_A7     (BITBOARD_AFILE & BITBOARD_RANK7)
#define BITBOARD_A8     (BITBOARD_AFILE & BITBOARD_RANK8)
#define BITBOARD_B1     (BITBOARD_BFILE & BITBOARD_RANK1)
#define BITBOARD_B2     (BITBOARD_BFILE & BITBOARD_RANK2)
#define BITBOARD_B3     (BITBOARD_BFILE & BITBOARD_RANK3)
#define BITBOARD_B4     (BITBOARD_BFILE & BITBOARD_RANK4)
#define BITBOARD_B5     (BITBOARD_BFILE & BITBOARD_RANK5)
#define BITBOARD_B6     (BITBOARD_BFILE & BITBOARD_RANK6)
#define BITBOARD_B7     (BITBOARD_BFILE & BITBOARD_RANK7)
#define BITBOARD_B8     (BITBOARD_BFILE & BITBOARD_RANK8)
#define BITBOARD_C1     (BITBOARD_CFILE & BITBOARD_RANK1)
#define BITBOARD_C2     (BITBOARD_CFILE & BITBOARD_RANK2)
#define BITBOARD_C3     (BITBOARD_CFILE & BITBOARD_RANK3)
#define BITBOARD_C4     (BITBOARD_CFILE & BITBOARD_RANK4)
#define BITBOARD_C5     (BITBOARD_CFILE & BITBOARD_RANK5)
#define BITBOARD_C6     (BITBOARD_CFILE & BITBOARD_RANK6)
#define BITBOARD_C7     (BITBOARD_CFILE & BITBOARD_RANK7)
#define BITBOARD_C8     (BITBOARD_CFILE & BITBOARD_RANK8)
#define BITBOARD_D1     (BITBOARD_DFILE & BITBOARD_RANK1)
#define BITBOARD_D2     (BITBOARD_DFILE & BITBOARD_RANK2)
#define BITBOARD_D3     (BITBOARD_DFILE & BITBOARD_RANK3)
#define BITBOARD_D4     (BITBOARD_DFILE & BITBOARD_RANK4)
#define BITBOARD_D5     (BITBOARD_DFILE & BITBOARD_RANK5)
#define BITBOARD_D6     (BITBOARD_DFILE & BITBOARD_RANK6)
#define BITBOARD_D7     (BITBOARD_DFILE & BITBOARD_RANK7)
#define BITBOARD_D8     (BITBOARD_DFILE & BITBOARD_RANK8)
#define BITBOARD_E1     (BITBOARD_EFILE & BITBOARD_RANK1)
#define BITBOARD_E2     (BITBOARD_EFILE & BITBOARD_RANK2)
#define BITBOARD_E3     (BITBOARD_EFILE & BITBOARD_RANK3)
#define BITBOARD_E4     (BITBOARD_EFILE & BITBOARD_RANK4)
#define BITBOARD_E5     (BITBOARD_EFILE & BITBOARD_RANK5)
#define BITBOARD_E6     (BITBOARD_EFILE & BITBOARD_RANK6)
#define BITBOARD_E7     (BITBOARD_EFILE & BITBOARD_RANK7)
#define BITBOARD_E8     (BITBOARD_EFILE & BITBOARD_RANK8)
#define BITBOARD_F1     (BITBOARD_FFILE & BITBOARD_RANK1)
#define BITBOARD_F2     (BITBOARD_FFILE & BITBOARD_RANK2)
#define BITBOARD_F3     (BITBOARD_FFILE & BITBOARD_RANK3)
#define BITBOARD_F4     (BITBOARD_FFILE & BITBOARD_RANK4)
#define BITBOARD_F5     (BITBOARD_FFILE & BITBOARD_RANK5)
#define BITBOARD_F6     (BITBOARD_FFILE & BITBOARD_RANK6)
#define BITBOARD_F7     (BITBOARD_FFILE & BITBOARD_RANK7)
#define BITBOARD_F8     (BITBOARD_FFILE & BITBOARD_RANK8)
#define BITBOARD_G1     (BITBOARD_GFILE & BITBOARD_RANK1)
#define BITBOARD_G2     (BITBOARD_GFILE & BITBOARD_RANK2)
#define BITBOARD_G3     (BITBOARD_GFILE & BITBOARD_RANK3)
#define BITBOARD_G4     (BITBOARD_GFILE & BITBOARD_RANK4)
#define BITBOARD_G5     (BITBOARD_GFILE & BITBOARD_RANK5)
#define BITBOARD_G6     (BITBOARD_GFILE & BITBOARD_RANK6)
#define BITBOARD_G7     (BITBOARD_GFILE & BITBOARD_RANK7)
#define BITBOARD_G8     (BITBOARD_GFILE & BITBOARD_RANK8)
#define BITBOARD_H1     (BITBOARD_HFILE & BITBOARD_RANK1)
#define BITBOARD_H2     (BITBOARD_HFILE & BITBOARD_RANK2)
#define BITBOARD_H3     (BITBOARD_HFILE & BITBOARD_RANK3)
#define BITBOARD_H4     (BITBOARD_HFILE & BITBOARD_RANK4)
#define BITBOARD_H5     (BITBOARD_HFILE & BITBOARD_RANK5)
#define BITBOARD_H6     (BITBOARD_HFILE & BITBOARD_RANK6)
#define BITBOARD_H7     (BITBOARD_HFILE & BITBOARD_RANK7)
#define BITBOARD_H8     (BITBOARD_HFILE & BITBOARD_RANK8)

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
#define east_one(x) (((x) << 1) & ~BITBOARD_AFILE)
#define west_one(x) (((x) >> 1) & ~BITBOARD_HFILE)
#define ne_one(x)   (((x) << 9) & ~BITBOARD_AFILE)
#define nw_one(x)   (((x) << 7) & ~BITBOARD_HFILE)
#define se_one(x)   (((x) >> 7) & ~BITBOARD_AFILE)
#define sw_one(x)   (((x) >> 9) & ~BITBOARD_HFILE)

// knights

//#define bishop_attacks(x) (noEaOccl(x) | soEaOccl(x) | noWeOccl(x) | soWeOccl(x))
//#define rook_attacks(x) (nortOccl(x) | eastOccl(x) | soutOccl(x) | westOccl(x))
//#define queen_attacks(x) (bishop_attacks(x) | rook_attacks(x))

// square of passed pawns

extern const uint64_t square_pp[64];

// for black's pov, just vertical flip the index, index into the table, then
// vertical flip the bitboard.


#define fliph(x)        fliph0(x)
uint64_t fliph0 (uint64_t x);
uint64_t fliph1 (uint64_t x);
uint64_t fliph2 (uint64_t x);


uint64_t flipld(uint64_t);
uint64_t flipdd(uint64_t);
uint64_t rotate_pi(uint64_t);
uint64_t rotate_clockwise(uint64_t);
uint64_t rotate_anticlockwise(uint64_t);

uint64_t north_span(uint64_t);
uint64_t south_span(uint64_t);
uint64_t east_span(uint64_t);
uint64_t west_span(uint64_t);
uint64_t ne_span(uint64_t);
uint64_t nw_span(uint64_t);
uint64_t se_span(uint64_t);
uint64_t sw_span(uint64_t);

#define w_front_span(x) (north_span(x))
#define w_rear_span(x)  (south_span(x))
#define b_front_span(x) (south_span(x))
#define b_rear_span(x)  (north_span(x))

uint64_t north_fill(uint64_t);
uint64_t south_fill(uint64_t);
uint64_t east_fill(uint64_t);
uint64_t west_fill(uint64_t);
uint64_t ne_fill(uint64_t);
uint64_t nw_fill(uint64_t);
uint64_t se_fill(uint64_t);
uint64_t sw_fill(uint64_t);

#define w_front_fill(x) (north_fill(x))
#define w_rear_fill(x)  (south_fill(x))
#define b_front_fill(x) (south_fill(x))
#define b_rear_fill(x)  (north_fill(x))

#define file_fill(x)    (north_fill(x) | south_fill(x))
uint64_t south_occluded_fill(uint64_t, uint64_t);
uint64_t north_occluded_fill(uint64_t, uint64_t);
uint64_t east_occluded_fill(uint64_t, uint64_t);
uint64_t ne_occluded_fill(uint64_t, uint64_t);
uint64_t se_occluded_fill(uint64_t, uint64_t);
uint64_t west_occluded_fill(uint64_t, uint64_t);
uint64_t sw_occluded_fill(uint64_t, uint64_t);
uint64_t nw_occluded_fill(uint64_t, uint64_t);

// iterate through all pieces in a uint64_t
// uint64_t b;
// while(b){
//     int index = __builtin_ctz(b);
//     ...
//     b &= b - 1;
//
// }

void display_uint64_t(uint64_t, int, int);
void display_text(char *, int, int);

struct magic_entry {
    const uint64_t *attack_table; /* lookup table of attack sets */
    uint64_t mask;    /* occupancy mask */
    uint64_t magic;   /* magic number */
    int shift;  /* how much to shift by */
};

struct board_flags {
    int castle_q[2];    /* 0 if either king or queen rook has moved, 1 otherwise */
    int castle_k[2];    /* 0 if either king or king rook has moved, 1 otherwise */
    uint8_t en_passant[2];  /* bitboard of pawns that have just double pushed */
};

struct board_state {
    //char board88[64];
    int turn;   /* 0 if white, 1 if black */
    /* bitboards */
    uint64_t bb[2][7];
    struct board_flags flags;
};

/* 
 *  Mailbox works as follows.
 *  Most significant bit refers to color
 *  Other bits determine piece
 **/

struct move {
    int p_mover, s_mover, t_mover;
    uint64_t primary, secondary, tertiary;
    struct board_flags flags;
};

#define all_pieces(x) ((x)->bb[0][ALL] | (x)->bb[1][ALL])

#define pieces(x, y) ((x).kings[y] | (x).queens[y] | (x).bishops[y] |\
        (x).knights[y] | (x).rooks[y] | (x).pawns[y] |)

#define copy_flags(m, board) do{(m)->en_passant_pre[0] = (board)->en_passant[0];\
        (m)->en_passant_pre[1] = (board)->en_passant[1];\
        (m)->castle_q_pre[0] = (board)->castle_q[0];\
        (m)->castle_q_pre[1] = (board)->castle_q[1];\
        (m)->castle_k_pre[0] = (board)->castle_k[0];\
        (m)->castle_k_pre[1] = (board)->castle_k[1];} while(0)
#endif
