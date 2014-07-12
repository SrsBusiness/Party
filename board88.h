#define invalid_square_88(a) (a & 0x88)
#define diff88(a, b) (a - b + 0x77)

extern unsigned char board_88[128];
const unsigned char dist_man[240] = {14, 13, 12, 11, 10, 9, 8, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            13, 12, 11, 10, 9, 8, 7, 6,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            12, 11, 10, 9, 8, 7, 6, 5,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            11, 10, 9, 8, 7, 6, 5, 4,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            10, 9, 8, 7, 6, 5, 4, 3,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            9, 8, 7, 6, 5, 4, 3, 2,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            8, 7, 6, 5, 4, 3, 2, 1,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 6, 5, 4, 3, 2, 1, 0,
                            1, 2, 3, 4, 5, 6, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            1, 2, 3, 4, 5, 6, 7, 8,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            2, 3, 4, 5, 6, 7, 8, 9,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            3, 4, 5, 6, 7, 8, 9, 10,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            4, 5, 6, 7, 8, 9, 10, 11,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            5, 6, 7, 8, 9, 10, 11, 12,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            6, 7, 8, 9, 10, 11, 12, 13,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 8, 9, 10, 11, 12, 13, 14, -1
                            };
const unsigned char dist[240] = {7, 7, 7, 7, 7, 7, 7, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 6, 6, 6, 6, 6, 6, 6,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 6, 5, 5, 5, 5, 5, 5,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 6, 5, 4, 4, 4, 4, 4,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 6, 5, 4, 3, 3, 3, 3,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 6, 5, 4, 3, 2, 2, 2,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 6, 5, 4, 3, 2, 1, 1,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 6, 5, 4, 3, 2, 1,
                            0, 1, 2, 3, 4, 5, 6, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            1, 1, 2, 3, 4, 5, 6, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            2, 2, 2, 3, 4, 5, 6, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            3, 3, 3, 3, 4, 5, 6, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            4, 4, 4, 4, 4, 5, 6, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            5, 5, 5, 5, 5, 5, 6, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            6, 6, 6, 6, 6, 6, 6, 7,
                            -1, -1, -1, -1, -1, -1, -1, -1,
                            7, 7, 7, 7, 7, 7, 7, 7, -1
                            };
