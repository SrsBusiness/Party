#white_win = [['X', 'X', 'X', '_', '_', '_', '_', '_', '_'],['_', '_', '_', 'X', 'X', 'X', '_', '_', '_'], ['_', '_', '_', '_', '_', '_', 'X', 'X', 'X'], ['X', '_', '_', 'X', '_', '_', 'X', '_', '_'], ['_', 'X', '_', '_', 'X', '_' ,'_', 'X', '_'], ['_', '_', 'X', '_', '_', 'X', '_', '_', 'X'], ['X', '_', '_', '_', 'X', '_', '_', '_', 'X'], ['_', '_', 'X', '_', 'X', '_', 'X', '_', '_']]
#black_win = [['O', 'O', 'O', '_', '_', '_', '_', '_', '_'],['_', '_', '_', 'O', 'O', 'O', '_', '_', '_'], ['_', '_', '_', '_', '_', '_', 'O', 'O', 'O'], ['O', '_', '_', 'O', '_', '_', 'O', '_', '_'], ['_', 'O', '_', '_', 'O', '_' ,'_', 'O', '_'], ['_', '_', 'O', '_', '_', 'O', '_', '_', 'O'], ['O', '_', '_', '_', 'O', '_', '_', '_', 'O'], ['_', '_', 'O', '_', 'O', '_', 'O', '_', '_']]
win = [[0, 1, 2, 3], [4, 5, 6, 7], [8, 9, 10, 11], [12, 13, 14, 15], [0, 4, 8, 12], [1, 5, 9, 13], [2, 6, 10, 14], [3, 7, 11, 15], [0, 5, 10, 15], [3, 6, 9, 12]]
board = ['_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_', '_']

#win = [[0, 1, 2], [3, 4, 5], [6, 7, 8], [0, 3, 6], [1, 4, 7], [2, 5, 8], [0, 4, 8], [2, 4, 6]]
#board = ['_', '_', '_', '_', '_', '_', '_', '_', '_']

white_move = True
board_row = 1
board_col = 1
analysis_row = 12
analysis_col = 1
dim = 4
total_nodes = 0
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
    #best = -2
    alpha = -2
    beta = 2
    next_move = -1
    for i in range(dim * dim):
        if board[i] != '_':
            continue
        if alpha == 1:
            return next_move
        move(i, piece)
        #update_board(analysis_row, analysis_col, i, piece)
        if tuple(board) in table:
            score = -1 * table[tuple(board)]
        else:
            score = -1 * negamax(-1 * beta, -1 * alpha)
        #print(score)
        if score > alpha:
            alpha = score
            next_move = i
        unmove(i)
        #update_board(analysis_row, analysis_col, i, '_')
    show_cursor()
    return next_move

#alpha-beta
#evaluate all moves for white
#make move for white (move1)
#   now evaluate position
#   try all black's responses
#   we find all of them result in even game
#   now eval of 0 is lower bound
#
#try next move for white (move2)
#   now evaluate position
#   try all black's responses
#   response #1 results in black with significant advantage
#   therefore we can discard this subtree and return
#
#try next move for white (move3)
#   try all black's reponses
#   move1 results in slight advantage for white
#   try move2
#       move1 -> even game
#       move2 -> winning advantage for white
#       we can therefore discard this subtree 
#       since the subtree of black's move1 led to 
#       only a slight advantage
#
#alpha will represent lower bound (lowest white will settle for)
#beta will represent upper bound (highest black will settle for)
#
#because of negamax, the lowest white will settle for becomes
#the negative of the highest black will settle for, and vice versa
#
#therefore what was now alpha becomes -beta for the next negamax call
#and vice versa
#
#the condition on whether to abandon the current subtree was whether
#the current score <= alpha (from white's point of view). With the negation i
#this becomes score >= beta. Let's say white was first to move. When 
#evaluating black's responses, the evaluations for black are the negation of 
#those for white. Therefore -score(black) = score(white). 
#Also beta(black) = -alpha(white). The relation score(white) <= alpha(white) 
#must fail. therefore the equivalent relation from black's point of view is 
#-score(black) <= -beta(black), which is equivalent to 
#score(black) >= beta(black). Therefore the comparison to be made in
#negamax is score >= beta. Luckiy this is symmetric because of the negation 
#used in negamax
#
#Now what about the beta score (white's pov), or beta(white)? We need to 
#update what black is willing to tolerate. This is the same as -alpha(black).
#This is simply let's assume that we are analysing white's responses to 
#black's previous move. Then alpha(black) is simply the best score for black
#so far, which, negated, becomes beta for white. Symmetrically, this applies
#to beta for black as well.
#
#
#
#
#
#
#
#
#
def negamax(alpha, beta):
    #alpha is the current lower bound
    #if score is lower than alpha, then we need to leave    
    global white_move
    global table
    global total_nodes
    #if tuple(board) in table:
    #    return table[tuple(board)]
    total_nodes += 1
    if white_move and check_win('O'):
        #table[tuple(board)] = -1;
        return -1
    elif (not white_move) and check_win('X'):
        #table[tuple(board)] = -1;
        return -1
    #best = -2
    tie = True
    piece = 'X' if white_move else 'O'
    for i in range(dim * dim):
        if board[i] != '_': # checking move legality
            continue
        else:
            tie = False
        if alpha == 1: # if a win was found, look no further!
            #table[tuple(board)] = alpha
            return alpha
        move(i, piece)
        #update_board(analysis_row, analysis_col, i, piece)
        if tuple(board) in table:
            score = -1 * table[tuple(board)]
        else:
            score = -1 * negamax(-1 * beta, -1 * alpha)
            table[tuple(board)] = -1 * score
        if score >= beta:
            unmove(i)
            return beta
        if score > alpha:
            alpha = score
        unmove(i);
        #update_board(analysis_row, analysis_col, i, '_')
    if not tie:
        #table[tuple(board)] = alpha
        return alpha
    else:
        #print("tie")
        #table[tuple(board)] = 0
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
    move_cursor((dim - 1) - int(index / dim) + row, 2 * (index % dim) + col)
    print(piece)

def move_cursor(row, col):
    print("\033[{:d};{:d}H".format(row, col), end="")

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
    print("board: ", end="")
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
    usr_white = int(input("Choose color: white(0), black(1): ")) == 0
    usr_piece = 'X' if usr_white else 'O'
    ai_piece = 'X' if not usr_white else 'O'
    move_count = 0
    while not game_over and move_count < dim * dim:
        check_board()
        if usr_white == white_move:
            move_cursor(30, 0)
            clear_line()
            index = int(input("Choose index to place piece: "))
            move(index, usr_piece)
            update_board(board_row, board_col, index, usr_piece)
            #update_board(analysis_row, analysis_col, index, usr_piece)
        else:
            index = best_move()
            move(index, ai_piece)
            update_board(board_row, board_col, index, ai_piece)
            #update_board(analysis_row, analysis_col, index, ai_piece)
        if check_win('O') or check_win('X'):
            game_over = True
        move_count += 1
    print(move_count, " moves")
    #print(table)
    print("positions: ", str(len(table)))
    print("nodes: ", str(total_nodes))

if __name__ == "__main__":
    main()

