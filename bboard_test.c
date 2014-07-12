#include "bitboard.h"
#include "minicurses.h"
#include "board88.h"

#define BOARD_ROW 5
#define BOARD_COL 8

bboard bboards[52] = {WHITE, BLACK, KING, QUEEN, BISHOP, KNIGHT, ROOK, PAWN,
    AFILE, BFILE, CFILE, DFILE, EFILE, FFILE, GFILE, HFILE,
    RANK1, RANK2, RANK3, RANK4, RANK5, RANK6, RANK7, RANK8,
    A7B8,
    A6C8,
    A5D8,
    A4E8,
    A3F8,
    A2G8,
    A1H8,
    B1H7,
    C1H6,
    D1H5,
    E1H4,
    F1H3,
    G1H2,
    A2B1,
    A3C1,
    A4D1,
    A5E1,
    A6F1,
    A7G1,
    A8H1,
    B8H2,
    C8H3,
    D8H4,
    E8H5,
    F8H6,
    G8H7,
    LIGHT,
    DARK
    };

// 0x88 board
char board088[128];

// 10 x 12 board
char board120[120];



char * bboard_names[52] = {"White", "Black", "King", "Queen", "Bishop",
    "Knight", "Rook", "Pawn", "A File", "B File", "C File", "D File",
    "E File", "F File", "G File", "H FIle", "1st Rank", "2nd Rank",
    "3rd Rank", "4th Rank", "5th Rank", "6th Rank", "7th Rank",
    "8th Rank", "a7 - b8", "a6 - c8", "a5 - d8", "a4 - e8", "a3 - f8",
    "a2 - g8", "a1 - h8", "b1 - h7", "c1 - h6", "d1 - h5", "e1 - h4", 
    "f1 - h3", "g1 - h2", "a2 - b1", "a3 - c1", "a4 - d1", "a5 - e1",
    "a6 - f1", "a7 - g1", "a8 - h1", "b8 - h2", "c8 - h3", "d8 - h4",
    "e8 - h5", "f8 - h6", "g8 - h7", "Light Squares", "Dark Squares"};

void display_bboard(bboard);

int main(){
    clear_all();
    hide_cursor();
    //int i = 0;
    //while(1){
    //    move_cursor(3, 0);
    //    clear_line();
    //    printf("%s\n", bboard_names[i]);
    //    display_bboard(0);
    //    display_bboard(bboards[i]);
    //    char j = getchar();
    //    if (j == '-')
    //        i = (i + 51) % 52;
    //    else
    //        i = (i + 1) % 52;
    //}
    for(int i = 7; i >= 0; i++){
        for(int j = 7; j >= 0; j++){
            display_bboard(0);
            display_bboard(1 | (1ul << (i * 8 + j)));
            move_cursor(3, 0);
            clear_line();
            printf("Manhattan distance: %d\n", dist_man[diff88(0, (16 * i + j) )]);
            printf("%d", (16 * i + j));
            getchar();
        }
    }
}

void display_bboard(bboard b) {
    for(int i  = 0; i < 64; i++) {
        move_cursor(7 - (i / 8) + BOARD_ROW, (i & 7) + BOARD_COL);
        if((1lu << i) & b)
            putchar('#');
        else
            putchar('_');
    }
    fflush(stdin);
}
