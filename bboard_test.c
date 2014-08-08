#include "bitboard.h"
#include "minicurses.h"
#include "board88.h"
#include <signal.h>
#include <stdlib.h>

#define BOARD_ROW 5
#define BOARD_COL 8
#define smiley 0x2828282882443800

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

void cleanup(){
    show_cursor();
}

void display_text(char *string, int row){
    move_cursor(row, 0);
    clear_line();
    printf("%s\n", string);
}

int main(){
    atexit(cleanup);
    clear_all();
    hide_cursor();
    int i = 0;
    bboard current;
    //*(int *)&current = rand();
    //*((int *)&current + 1) = rand();
    while(1){
        //display_text(bboard_names[i],3);
        //display_bboard(0);
        display_text("Hey, buddy", 4);
        current = smiley;
        display_bboard(current);

        getchar();
        current = flipv(current);
        display_text("Vertical Flip", 4);
        display_bboard(current);
        
        getchar();
        current = fliph(current);
        display_text("Horizontal Flip", 4);
        display_bboard(current);

        getchar();
        current = flipld(current);
        display_text("Flip about light square diag", 4);
        display_bboard(current);

        getchar();
        current = flipdd(current);
        display_text("Flip about dark square diag", 4);
        display_bboard(current);

        getchar();
        current = rotate_pi(current);
        display_text("Rotate pi radians", 4);
        display_bboard(current);

        getchar();
        current = rotate_c(current);
        display_text("Rotate pi/2 radians clockwise", 4);
        display_bboard(current);

        getchar();
        current = rotate_a(current);
        display_text("Rotate pi/2 radians anticlockwise", 4);
        display_bboard(current);
        
        getchar();
        current = north_one(current);
        display_text("Shift North", 4);
        display_bboard(current);

        getchar();
        current = south_one(current);
        display_text("Shift South", 4);
        display_bboard(current);
        
        getchar();
        current = east_one(current);
        display_text("Shift East", 4);
        display_bboard(current);
        
        getchar();
        current = west_one(current);
        display_text("Shift West", 4);
        display_bboard(current);
        
        getchar();
        current = ne_one(current);
        display_text("Shift NE", 4);
        display_bboard(current);
        
        getchar();
        current = sw_one(current);
        display_text("Shift SW", 4);
        display_bboard(current);
        
        getchar();
        current = nw_one(current);
        display_text("Shift NW", 4);
        display_bboard(current);
        
        getchar();
        current = se_one(current);
        display_text("Shift SE", 4);
        display_bboard(current);
        

        
        getchar();
        //i = (i + 1) % 52;
        //if (j == '-')
        //    i = (i + 51) % 52;
        //else
        //    i = (i + 1) % 52;
    }
    //for(int i = 7; i >= 0; i--){
    //    for(int j = 7; j >= 0; j--){
    //        display_bboard(0);
    //        display_bboard(1 | (1ul << (i * 8 + j)));
    //        move_cursor(3, 0);
    //        clear_line();
    //        printf("Distance: %d\n", dist[diff88(0, 16 * i + j)]);
    //        printf("%X", diff88(0, 16 * i + j));
    //        getchar();
    //    }
    //}
    //for(int i = 0; i < 8; i++){
    //    for(int j = 0; j < 8; j++){
    //        display_bboard(0);
    //        display_bboard(1 | (1ul << (i * 8 + j)));
    //        move_cursor(3, 0);
    //        clear_line();
    //        printf("Distance: %d\n", dist[diff88(16 * i + j, 0)]);
    //        printf("%X", diff88(16 * i + j, 0));
    //        getchar();
    //    }
    //}

    //show_cursor();
}

void display_bboard(bboard b) {
    for(int i  = 0; i < 64; i++) {
        move_cursor(7 - (i / 8) + BOARD_ROW, ((i & 7) << 1) + BOARD_COL);
        if((1lu << i) & b)
            putchar('#');
        else
            putchar('_');
    }
    fflush(stdin);
}
