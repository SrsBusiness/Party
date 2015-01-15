#define invalid_square_88(a) ((a) & 0x88)
#define diff88(a, b) ((a) - (b) + 0x77)

extern unsigned char board_88[128];

// table of Manhattan distances. To index into the table, use
// the diff88 macro.
const unsigned char dist_man[240] = {14, 13, 12, 11, 10, 9, 8, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            13, 12, 11, 10, 9, 8, 7, 6,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            12, 11, 10, 9, 8, 7, 6, 5,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            11, 10, 9, 8, 7, 6, 5, 4,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            10, 9, 8, 7, 6, 5, 4, 3,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            9, 8, 7, 6, 5, 4, 3, 2,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            8, 7, 6, 5, 4, 3, 2, 1,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 6, 5, 4, 3, 2, 1, 0,
                            1, 2, 3, 4, 5, 6, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            1, 2, 3, 4, 5, 6, 7, 8,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            2, 3, 4, 5, 6, 7, 8, 9,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            3, 4, 5, 6, 7, 8, 9, 10,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            4, 5, 6, 7, 8, 9, 10, 11,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            5, 6, 7, 8, 9, 10, 11, 12,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            6, 7, 8, 9, 10, 11, 12, 13,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 8, 9, 10, 11, 12, 13, 14, 0xFF
                            };

// Table of actual square distances. The indexing scheme
// is the same as above
const unsigned char dist[240] = {7, 7, 7, 7, 7, 7, 7, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 6, 6, 6, 6, 6, 6, 6,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 6, 5, 5, 5, 5, 5, 5,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 6, 5, 4, 4, 4, 4, 4,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 6, 5, 4, 3, 3, 3, 3,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 6, 5, 4, 3, 2, 2, 2,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 6, 5, 4, 3, 2, 1, 1,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 6, 5, 4, 3, 2, 1,
                            0, 1, 2, 3, 4, 5, 6, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            1, 1, 2, 3, 4, 5, 6, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            2, 2, 2, 3, 4, 5, 6, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            3, 3, 3, 3, 4, 5, 6, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            4, 4, 4, 4, 4, 5, 6, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            5, 5, 5, 5, 5, 5, 6, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            6, 6, 6, 6, 6, 6, 6, 7,
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                            7, 7, 7, 7, 7, 7, 7, 7, 0xFF
                            };


// index 0x66 and 0x88 are special cases if one of the squares is the corner.
// instead of 2, it will be 4
const unsigned char dist_knight[240] = { 6, 5, 4, 5, 4, 5, 4, 5,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    5, 4, 5, 4, 3, 4, 3, 4,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    4, 5, 4, 3, 4, 3, 4, 3,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    5, 4, 3, 4, 3, 2, 3, 2,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    4, 3, 4, 3, 2, 3, 2, 3,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    5, 4, 3, 2, 3, 4, 1, 2,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    4, 3, 4, 3, 2, 1, 2 , 3,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    5, 4, 3, 2, 3, 2, 3,
                                    0, 3, 2, 3, 2, 3, 4, 5,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    3, 2 , 1, 2, 3, 4, 3, 4,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    2, 1, 4, 3, 2, 3, 4, 5,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    3, 2, 3, 2, 3, 4, 3, 4,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    2, 3, 2, 3, 4, 3, 4, 5,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    3, 4, 3, 4, 3, 4, 5, 4, 
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    4, 3, 4, 3, 4, 5, 4, 5,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    5, 4, 5, 4, 5, 4 ,5 ,6, 0xFF
                                    };



// 0x01 - diag (Bishop, Queen)
// 0x02 - horiz/vertic (Rook, Queen)
// 0x04 - knight
// 0x08 - king
// 0x10 - pawn

