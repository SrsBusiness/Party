#ifndef MINICURSES_H
#define MINICURSES_H

#include <stdio.h>

#define ANSI_RESET "\x1B[0m"
#define ANSI_FG_BLACK "\x1B[30m"
#define ANSI_FG_RED "\x1B[31m"
#define ANSI_FG_GREEN "\x1B[32m"
#define ANSI_FG_YELLOW "\x1B[33m"
#define ANSI_FG_BLUE "\x1B[34m"
#define ANSI_FG_PURPLE "\x1B[35m"
#define ANSI_FG_CYAN "\x1B[36m"
#define ANSI_FG_WHITE "\x1B[37m"

#define ANSI_BG_BLACK "\x1B[40m"
#define ANSI_BG_RED "\x1B[41m"
#define ANSI_BG_GREEN "\x1B[42m"
#define ANSI_BG_YELLOW "\x1B[43m"
#define ANSI_BG_BLUE "\x1B[44m"
#define ANSI_BG_PURPLE "\x1B[45m"
#define ANSI_BG_CYAN "\x1B[46m"
#define ANSI_BG_WHITE "\x1B[47m"


#define ANSI_CLEAR_SCRN "\x1B[2J"
#define ANSI_MOVE_UP "\x1B[%dA"
#define ANSI_MOVE_DOWN "\x1B[%dB"
#define ANSI_MOVE_FORWARD "\x1B[%dC"
#define ANSI_MOVE_BACK "\x1B[%dD"
#define ANSI_CLEAR_LINE "\x1B[2K"
#define ANSI_BG_BLACK "\x1B[40m"
#define ANSI_SAVE_CURSOR "\x1B[s"
#define ANSI_RESTORE_CURSOR "\x1B[u"
#define ANSI_CLEAR_BELOW "\x1B[J"
#define ANSI_HIDE_CURSOR "\x1B[?25l"
#define ANSI_SHOW_CURSOR "\x1B[?25h"

// These macros are not intended to be used as first class
// citizens. Doing so will result in undefined behavior.
// Only the side-effect of these functions are significant.

#define move_cursor(a, b) printf("\x1B[%d;%dH", a, b)
#define move_up(a) printf(ANSI_MOVE_UP, a)
#define move_down(a) printf(ANSI_MOVE_DOWN, a)
#define move_forward(a) printf(ANSI_MOVE_FORWARD, a)
#define move_back(a) printf(ANSI_MOVE_BACK, a)
#define clear_line() printf(ANSI_CLEAR_LINE)
#define hide_cursor() printf(ANSI_HIDE_CURSOR)
#define show_cursor() printf(ANSI_SHOW_CURSOR)
#define clear_all() printf(ANSI_CLEAR_SCRN)

#define black_font() printf(ANSI_FG_BLACK)
#define red_font() printf(ANSI_FG_RED)
#define green_font() printf(ANSI_FG_GREEN)
#define yellow_font() printf(ANSI_FG_YELLOW)
#define blue_font() printf(ANSI_FG_BLUE)
#define purple_font() printf(ANSI_FG_PURPLE)
#define cyan_font() printf(ANSI_FG_CYAN)
#define white_font() printf(ANSI_FG_WHITE)

#define black_bg() printf(ANSI_BG_BLACK)
#define red_bg() printf(ANSI_BG_RED)
#define green_bg() printf(ANSI_BG_GREEN)
#define yellow_bg() printf(ANSI_BG_YELLOW)
#define blue_bg() printf(ANSI_BG_BLUE)
#define purple_bg() printf(ANSI_BG_PURPLE)
#define cyan_bg() printf(ANSI_BG_CYAN)
#define white_bg() printf(ANSI_BG_WHITE)


#define save_cursor() printf(ANSI_SAVE_CURSOR)
#define restore_cursor() printf(ANSI_RESTORE_CURSOR)
#define clear_below() printf(ANSI_CLEAR_BELOW)

#define reset_attr() printf(ANSI_RESET)

#endif
