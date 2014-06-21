
black_win = ["OOO______", "___OOO___", "______OOO", "O__O__O__", "_O__O__O_", "__O__O__O", "O___O___O", "__O_O_O__"]
white_win = ["XXX______", "___XXX___", "______XXX", "X__X__X__", "_X__X__X_", "__X__X__X", "X___X___X", "__X_X_X__"]
board = "_________"
white_move = True
board_row = 1
board_col = 1
analysis_row = 1
analysis_col = 20

def check_black_win():
    for win in black_win:
        if win == board:
            return True
    return False

def check_white_win():
    for win in white_win:
        if win == board:
            return True
    return False

def bestmove():
    piece = 'X' if white_move else 'O'
    best = -2
    next_move = -1
    for i in range(16):
        if best == 1:
            return next_move
        move(i, piece)
        score = -1 * negamax()
        if score > best:
            best = score
            next_move = i
        unmove(i)
    return next_move

def negamax():
    if white_move and check_white_win():
        return 1
    elif (not white_move) and check_black_win():
        return 1
    best = -2
    piece = 'X' if white_move else 'O'
    for i in range(16):
        if best == 1:
            return best
        move(i, piece)
        score = -1 * negamax()
        if score > best:
            best = score
        unmove(i);
    if best != -2:
        return best
    else:
        return 0


def move(index, piece):
    white_move = not white_move
    board[index] = piece

def unmove(index):
    white_move = not white_move
    board[index] = '_'

def init_board(row, col):
    for i in range(3):
        move_cursor(row + i, col)
        print("_ _ _")

def update_board(row, col, index, piece):
    move(index, piece);
    move_cursor(2 - (index / 3) + row, 2 * (index % 3) + col)
    print(piece,)

def move_cursor(row, col):
    print("\x1B[{:d};{:d}H".format(row, col))

def reset_terminal():
    print("\x1b[2J")

def main():
    reset_terminal()
    init_board(board_row, board_col)
    game_over = False
    usr_white = int(raw_input("Choose color: white(0), black(1)")) == 0
    usr_piece = 'X' if usr_white else 'O'
    ai_piece = 'X' if not usr_white else 'O'
    while not game_over:
        if usr_white == white_move:
            index = int(raw_input("Choose index to place piece."))
            update_board(board_row, board_col, index, usr_piece)
        else:
            index = best_move()
            update_board(board_row, board_col, index, ai_piece)

if __name__ == "__main__":
    main()

