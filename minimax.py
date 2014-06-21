#white_win = [['X', 'X', 'X', '_', '_', '_', '_', '_', '_'],['_', '_', '_', 'X', 'X', 'X', '_', '_', '_'], ['_', '_', '_', '_', '_', '_', 'X', 'X', 'X'], ['X', '_', '_', 'X', '_', '_', 'X', '_', '_'], ['_', 'X', '_', '_', 'X', '_' ,'_', 'X', '_'], ['_', '_', 'X', '_', '_', 'X', '_', '_', 'X'], ['X', '_', '_', '_', 'X', '_', '_', '_', 'X'], ['_', '_', 'X', '_', 'X', '_', 'X', '_', '_']]
#black_win = [['O', 'O', 'O', '_', '_', '_', '_', '_', '_'],['_', '_', '_', 'O', 'O', 'O', '_', '_', '_'], ['_', '_', '_', '_', '_', '_', 'O', 'O', 'O'], ['O', '_', '_', 'O', '_', '_', 'O', '_', '_'], ['_', 'O', '_', '_', 'O', '_' ,'_', 'O', '_'], ['_', '_', 'O', '_', '_', 'O', '_', '_', 'O'], ['O', '_', '_', '_', 'O', '_', '_', '_', 'O'], ['_', '_', 'O', '_', 'O', '_', 'O', '_', '_']]
win = [[0, 1, 2, 4], [4, 5, 6, 7], [8, 9, 10, 11], [12, 13, 14, 15], [0, 4, 8, 12], [1, 5, 9, 13], [2, 6, 10, 14], [3, 7, 11, 15], [0, 5, 10, 15], [3, 6, 9, 12]]
board = ['_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_']

#win = [[0, 1, 2], [3, 4, 5], [6, 7, 8], [0, 3, 6], [1, 4, 7], [2, 5, 8], [0, 4, 8], [2, 4, 6]]
#board = ['_', '_', '_', '_', '_', '_', '_', '_', '_']

white_move = True
board_row = 1
board_col = 1
analysis_row = 12
analysis_col = 1
dim = 4

table = {}

def check_win(piece):
    global win
    global board
    for line in win:
        if line_complete(line, piece):
            return True
    return False

def line_complete(line, piece):
    global board
    for i in range(len(line)):
        if board[line[i]] != piece:
            return False
    return True


#def check_white_win():
#    global white_win
#    global board
#    for win in white_win:
#        if win == board:
#            return True
#    return False

def best_move():
    global white_move
    hide_cursor()
    #if white_move and check_win('O'):
    #    table[tuple(board)] = -1
    #    return -1
    #elif (not white_move) and check_win('X'):
    #    table[tuple(board)] = 1
    #    return -1
    piece = 'X' if white_move else 'O'
    best = -2
    next_move = -1
    for i in range(dim * dim):
        if board[i] != '_':
            continue
        if best == 1:
            return next_move
        move(i, piece)
        #update_board(analysis_row, analysis_col, i, piece)
        score = -1 * negamax()
        #print(score)
        if score > best:
            best = score
            next_move = i
        unmove(i)
        #update_board(analysis_row, analysis_col, i, '_')
    show_cursor()
    return next_move

def negamax():
    global white_move
    global table
    if white_move and check_win('O'):
        table[tuple(board)] = -1;
        return -1
    elif (not white_move) and check_win('X'):
        table[tuple(board)] = -1;
        return -1
    best = -2
    piece = 'X' if white_move else 'O'
    for i in range(dim * dim):
        if board[i] != '_':
            continue
        if best == 1:
            return best
        move(i, piece)
        #update_board(analysis_row, analysis_col, i, piece)
        if tuple(board) not in table:
            #move_cursor(20, 20)
            #clear_line()
            #print("miss")
            score = -1 * negamax()
        else:
            #move_cursor(20, 20)
            #clear_line()
            #print("hit")
            score = -1 * table[tuple(board)]
        if score > best:
            best = score
        unmove(i);
        #update_board(analysis_row, analysis_col, i, '_')
    if best != -2:
        table[tuple(board)] = 0
        return best
    else:
        return 0


def move(index, piece):
    global white_move
    global board
    white_move = not white_move
    board[index] = piece

def unmove(index):
    global white_move
    global board
    white_move = not white_move
    board[index] = '_'

def init_board(row, col):
    for i in range(dim * dim):
        update_board(row, col, i, '_')

def update_board(row, col, index, piece):
    #move(index, piece);
    #print("row: " + ((index / 3) + row) + " col: " + (2 * (index % 3) + col))
    move_cursor((dim - 1) - (index / dim) + row, 2 * (index % dim) + col)
    print(piece)

def move_cursor(row, col):
    print("\033[{:d};{:d}H".format(row, col)),

def clear_line():
    print("\x1B[2K")

def reset_terminal():
    print("\x1B[2J")

def hide_cursor():
    print("\x1B[?25l")

def show_cursor():
    print("\x1B[?25h")

def test():
    for i in range(dim * dim):
        update_board(board_row, board_col, i, i)

def check_board():
    move_cursor(0, 20)
    print("board: "),
    print(board)

def main():
    global white_move
    global board_row
    global board_col
    global table
    reset_terminal()
    #test()
    #raw_input()
    init_board(board_row, board_col)
    init_board(analysis_row, analysis_col)
    game_over = False
    move_cursor(30, 0)
    usr_white = int(raw_input("Choose color: white(0), black(1): ")) == 0
    usr_piece = 'X' if usr_white else 'O'
    ai_piece = 'X' if not usr_white else 'O'
    while not game_over:
        check_board()
        if usr_white == white_move:
            move_cursor(30, 0)
            clear_line()
            index = int(raw_input("Choose index to place piece: "))
            move(index, usr_piece)
            update_board(board_row, board_col, index, usr_piece)
            #update_board(analysis_row, analysis_col, index, usr_piece)
        else:
            index = best_move()
            if index == -1:
                break
            move(index, ai_piece)
            update_board(board_row, board_col, index, ai_piece)
            #update_board(analysis_row, analysis_col, index, ai_piece)
        if check_win('O') or check_win('X'):
            game_over = True
    print(table)
    print(len(table))

if __name__ == "__main__":
    main()

