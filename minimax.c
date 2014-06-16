#include <stdio.h>
#include <stdint.h>

// constants for win condition checking
// _ _ _ _
// _ _ _ _
// _ _ _ _
#define ROW0 0x000F
// _ _ _ _
#define ROW1 0x00F0
#define ROW2 0x0F00
#define ROW3 0xF000
#define COL0 0x1111
#define COL1 0x2222
#define COL2 0x4444
#define COL3 0x8888
#define DIA0 0x1248
#define DIA1 0x8421
#define is_legal(a) (!(bboards[0] & a) && !(bboards[1] & a))

// ANSI escape sequences

#define ANSI_RESET "\x1B[0m"
#define ANSI_BLACK "\x1B[30m"
#define ANSI_RED "\x1B[31m"
#define ANSI_GREEN "\x1B[32m"
#define ANSI_YELLOW "\x1B[33m"
#define ANSI_BLUE "\x1B[34m"
#define ANSI_PURPLE "\x1B[35m"
#define ANSI_CYAN "\x1B[36m"
#define ANSI_WHITE "\x1B[37m"
#define ANSI_CLEAR_SCRN "\x1B[2J"
#define ANSI_MOVE_UP_ONE "\x1B[1A"
#define ANSI_MOVE_DOWN_ONE "\x1B[1B"
#define ANSI_CLEAR_LINE "\x1B[2K"
#define ANSI_BG_BLACK "\x1B[40m"
#define ANSI_SAVE_CURSOR "\x1B[s"
#define ANSI_RESTORE_CURSOR "\x1B[u"
#define ANSI_CLEAR_BELOW "\x1B[J"
#define ANSI_HIDE_CURSOR "\x1B[?25l"
#define ANSI_SHOW_CURSOR "\x1B[?25h"

#define move_cursor(a, b) printf("\x1B[%d;%dH", a, b)
#define move_up() printf(ANSI_MOVE_UP_ONE)
#define move_down() printf(ANSI_MOVE_DOWN_ONE)
#define clear_line() printf(ANSI_CLEAR_LINE)
#define hide_cursor() printf(ANSI_HIDE_CURSOR)
#define show_cursor() printf(ANSI_SHOW_CURSOR)

#define BOARD_ROW 1
#define BOARD_COL 1
#define ANALYSIS_ROW 1
#define ANALYSIS_COL 50
#define PLY_ROW 18
// 0 is white, 1 is black
uint16_t white = 0x0000;
uint16_t black = 0x0000;

uint16_t bboards[2] = {0};
uint16_t wins[] = {ROW0, ROW1, ROW2, ROW3, COL0, COL1, COL2, COL3, DIA0, DIA1};
//char disp[16] = {[0 ... 15] = '_'};

// returns 1 if white move wins, 0 if tie, -1 if black wins
//int minimax(uint16_t, int, int);
int max(uint16_t, int);
int min(uint16_t, int);
int check_win(int);
void init_board(int, int);
void update_board(uint16_t, int);
void update_analysis(uint16_t, int);
void undo_analysis(uint16_t, int);
uint16_t best_move(int);

int (*minimax[])(uint16_t, int) = {min, max};

int main(){
    //printf("%s", ANSI_WHITE); 
    //init_board
    //hide_cursor();
    init_board(BOARD_ROW, BOARD_COL);
    init_board(ANALYSIS_ROW, ANALYSIS_COL);
    move_cursor(20, 0);
    printf("Pick a color: 0(white), 1(black): ");
    int usrcolor;
    scanf("%d", &usrcolor);
    int to_move = 0;
    move_cursor(20, 0);
    //printf("usr_color: %d\n" ,usrcolor);
    while(!check_win(to_move)){
        move_cursor(20, 0);
        //printf("to_move: %d\n", to_move);
        //printf("usrcolor: %d\n", usrcolor);
        //display_board();
        if(usrcolor == to_move){
            move_cursor(20, 0);
            clear_line();
            //printf("user move\n");
            uint16_t move;
            printf("Pick a space to play: ");
            //printf("&uscolor: %p, &move: %p\n", &move);
            scanf("%hd", &move);
            move = 1 << move;
            //printf("move: %d\n", move);
            update_board(move, to_move);
            update_analysis(move, to_move);
        } else {
            //move_cursor(20, 0);
            //printf("computer move\n");
            //printf("%x, %x\n", bboards[0], bboards[1]);
            uint16_t move = best_move(to_move);
            //printf("%x, %x\n", bboards[0], bboards[1]);
            //printf("is_legal(%d): %d\n", move, is_legal(move));
            //printf("bestmove: %x\n", move);
            update_board(move, to_move); 
            update_analysis(move, to_move);
        }
        to_move ^= 1;
    }
    //printf("check_win: %d", check_win(to_move));
    //display_board();
}
void init_board(int row, int col) {
    for(int i = 0; i < 4; i++){
        move_cursor(row + i, col);
        printf("_ _ _ _\n");
    }
}


void update_board(uint16_t move, int side){
    bboards[side] |= move; 
    int index = __builtin_popcount(((uint32_t)move << 1) - 1) - 1;
    int row = 3 - (index / 4) + BOARD_ROW;
    int col = 2 * (index & 3) + BOARD_COL;
    move_cursor(row, col);
    putchar(side ? 'O' : 'X');
}

void update_analysis(uint16_t move, int side){
    bboards[side] |= move; 
    int index = __builtin_popcount(((uint32_t)move << 1) - 1) - 1;
    int row = 3 - (index / 4) + ANALYSIS_ROW;
    int col = 2 * (index & 3) + ANALYSIS_COL;
    move_cursor(row, col);
    putchar(side ? 'O' : 'X');
}

void undo_analysis(uint16_t move, int side){
    bboards[side] ^= move;
    int index = __builtin_popcount(((uint32_t)move << 1) - 1) - 1;
    int row = 3 - (index / 4) + ANALYSIS_ROW;
    int col = 2 * (index & 3) + ANALYSIS_COL;
    move_cursor(row, col);
    putchar('_');
}
// for max: score > best
// for min: -score > -best

uint16_t best_move(int side) {
    // returns best move, otherwise 0 if no legal moves exist
    hide_cursor();
    uint16_t best_move = 0;
    int best_score = 2 * side - 1;
    // calculate this so we don't have to branch =D
    // if side is 0 (white), we use a factor of (1)
    // if side is 1 (black), we negate (-1)
    int branch = -2 * side + 1;
    // 2 * side - 1 = score 
    uint16_t move = 1;
    for(int i = 0; i < 16; i++){
        //printf("considering move : 0x%X\n", move);
        if(is_legal(move)){
            int score = minimax[side](move, 0);
            //printf("finite recursion\n");
            if(score * branch > best_score * branch){
                best_score = score;
                best_move = move; 
            }
        }
        move <<= 1;
    }
    show_cursor();
    return best_move;
}
// evaluate white's move
// returns the evaluation of the strongest
// opponent reply

void update_ply(int ply){
    move_cursor(PLY_ROW, 0); 
    clear_line();
    printf("Ply: %d", ply);
}
int min(uint16_t move, int ply) {
    //printf("ply: %d\n", ply);
    //bboards[0] |= move;
    update_ply(ply);
    update_analysis(move, 0);
    int min_score = 2;
    if(check_win(0)){
        //bboards[0] ^= move;
        undo_analysis(move, 0);
        return 1;
    }
    //uint16_t next_move = 1;
    for(int i = 0; i < 16; i++) {
        //printf("max: %d\n", i);
        if(is_legal(1 << i)){
            int score = max(1 << i, ply + 1);
            if(score < min_score)
                min_score = score;
        }
        //next_move <<= 1;
    }
    //printf("whew\n");
    //bboards[0] ^= move;
    undo_analysis(move, 0);
    if(min_score != 2)
        return min_score;
    else
        return 0;
}

// evaluate black's move
// returns the evaluation of the strongest
// opponent reply
int max(uint16_t move, int ply) {
    //bboards[1] |= move;
    update_ply(ply);
    update_analysis(move, 1);
    int max_score = -2;
    if(check_win(1)){
        //bboards[1] ^= move;
        undo_analysis(move, 1);
        return -1;
    }
    //uint16_t next_move = 1;
    for(int i = 0; i < 16; i++) {
        //printf("min: %d\n", i);
        if(is_legal(1 << i)){
            int score = min(1 << i, ply + 1);
            if(score > max_score)
                max_score = score;
        }
        //next_move <<= 1;
    }
    //printf("whew\n");
    //bboards[1] ^= move;
    undo_analysis(move, 1);
    if(max_score != -2)
        return max_score;
    else
        return 0;
}

int check_win(int side){
    //printf("sizeof(wins): %d\n", sizeof(wins));
    for(int i = 0; i < sizeof(wins) / sizeof(uint16_t); i++){
        if((bboards[side] & wins[i]) == wins[i]){
            //printf("bboards[side]: %x\n", bboards[side]);
            //printf("wins[i]: %x\n", wins[i]);
            //printf("bboards[side] & wins[i]: %x\n",  bboards[side] & wins[i]);
            return 1;
        }
    }
    return 0;
}

/*
int minimax(uint16_t move, int side, int depth) {
    //printf("depth: %d\n", depth);
    //printf("minimax\n");
    int eval;
    switch(side){
        case 0:
            //white to move
            eval = -1;
            break;
        case 1:
            // black to move
            eval = 1;
            break;
    }
    // make move
    bboards[side] |= move; 
    // hopefully remove this branch
    if(check_win(side)){
        //printf("uh oh\n");
        bboards[side] ^= move;
        return side ? -1 : 1; 
    }
    // now generate all ply responses from opponent
    side ^= 1;
    uint16_t next_move = 1;
    for(int i = 0; i < 16; i++) {
        //printf("herp\n");
        //printf("new_move: %x\n", next_move);
        if((side && eval == 1) || (!side && eval == -1)){
            side ^= 1;
            bboards[side] ^= move;
            return eval;
        }
        //printf("white bit_board: %x\n", bboards[0]);
        //printf("black bit_board: %x\n", bboards[1]);
        if(is_legal(next_move)) {

            //printf("legal\n");
            int reply = minimax(next_move, side, depth + 1);
            switch(side){ // reply
                case 0: // white reply
                    if(reply < eval)
                        eval = reply;
                    break;
                case 1:
                    if(reply > eval)
                        eval = reply;
        }
                    break;
            }
        next_move <<= 1;
    }
    side ^= 1;
    bboards[side] ^= move;
    return eval;
}

uint16_t best_move(int side){
    uint16_t best;
    uint16_t move = 1;
    int eval;
    switch(side){
        case 0:
            eval = -1;
            break;
        case 1:
            eval = 1;
            break;
    }
    for(int i = 0; i < 16; i++){
        if(is_legal(move)){ // if move is legal
            if((!side && eval == 1) || (side && eval == -1)){
                printf("hi\n");
                return move;
            }
            //printf("considering move: %x\n", move);
            int tmp = minimax(move, side, 0);
            //printf("side: %d, tmp: %d, eval: %d\n", side, tmp, eval);
            //printf("\n\n");
            switch(side){
                case 0:
                    if(tmp >= eval){
                        tmp = eval;
                        best = move;       
                    }
                    break;
                case 1:
                    if(tmp <= eval){
                        tmp = eval;
                        best = move;
                    }
                    break;
            } 
        }
        move <<= 1;
    }
    printf("herp\n");
    return best;
}
*/
