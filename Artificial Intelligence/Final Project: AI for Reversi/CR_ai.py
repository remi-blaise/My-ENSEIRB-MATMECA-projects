from random import randint, choice
from time import time
from copy import deepcopy
import json
import sys

debug = False

##############################################################################
#### EXPLORATION ALGORITHMS ##################################################
##############################################################################

class ExplorationException(Exception):
    pass

def alphabeta_explorer(board, max_time, max_deepness, heuristique):
    global debug
    player = board._nextPlayer

    def print_debug(string, deepness):
        if debug:
            print((max_deepness - deepness) * ' ' + string)

    def alphabeta(deepness, alpha, beta):
        if debug:
            debugArray = [ '(deepness %d)' % deepness ]

        if max_time < time():
            raise ExplorationException()

        legal_moves = board.legal_moves()

        if board.is_game_over() or deepness <= 0:
            score = heuristique(board, player)
            print_debug("HEURISTIQUE " + str(score), deepness)
            if debug:
                debugArray.append(score)
            return score, None, debugArray if debug else None

        if deepness % 2 != max_deepness % 2:
            value = float('+inf')

            for move in legal_moves:
                board.push(move)
                print_debug("PUSH move %s" % move, deepness)
                # print(board)
                opponent_value, _, dA = alphabeta(deepness - 1, alpha, beta)
                value = min(value, opponent_value)
                if debug:
                    debugArray.append(dA)
                print_debug("POP", deepness)
                board.pop()

                if value <= alpha:
                    print_debug("COUPURE ALPHA", deepness)
                    print_debug("VALUE is %s with %s" % (move, value), deepness)
                    if debug:
                        debugArray.append(value)
                    return value, move, debugArray if debug else None

                beta = min(beta, value)
        else:
            value = float('-inf')

            for move in legal_moves:
                board.push(move)
                print_debug("PUSH move %s" % move, deepness)
                # print(board)
                opponent_value, _, dA = alphabeta(deepness - 1, alpha, beta)
                value = max(value, opponent_value)
                if debug:
                    debugArray.append(dA)
                print_debug("POP", deepness)
                board.pop()

                if beta <= value:
                    print_debug("COUPURE BETA", deepness)
                    print_debug("VALUE is %s with %s" % (move, value), deepness)
                    if debug:
                        debugArray.append(value)
                    return value, move, debugArray if debug else None

                alpha = max(alpha, value)

        print_debug("VALUE is %s with %s" % (move, value), deepness)
        if debug:
            debugArray.append(value)
        return value, move, debugArray if debug else None

    value, move, dA = alphabeta(max_deepness, float('-inf'), float('+inf'))
    return move, dA

def deepen(board, max_time, explorer, heuristique):
    move = choice(board.legal_moves())
    deepness = 5
    max_deepness = 96 - len(board._stack)
    is_black = board._nextPlayer == board._BLACK

    try:
        while deepness <= max_deepness:
            move, dA = explorer(deepcopy(board), max_time, deepness, heuristique)
            deepness += 1
    except ExplorationException:
        pass
    finally:
        print("(deepness %d)" % deepness)
        sys.stderr.write("(deepness %d)" % deepness)
        sys.stderr.flush()

    return move

##############################################################################
#### HEURISTIQUES ############################################################
##############################################################################

# An heuristique has to be positive if given player win, negative elsewise

def coeff(b):
    return 1 if b else -1

def compose(heuristiques):
    '''Take a list of [ heuristiques or (heuristique, normalizer) ] and return the compound sum heuristique.'''
    def compound_heuristique(board, player):
        def term(arg):
            if type(arg) == tuple:
                heuristique, normalizer = arg
            else:
                heuristique, normalizer = arg, lambda x: x
            return normalizer(heuristique(board, player))
        return sum(map(term, heuristiques))
    return compound_heuristique

def basic_heuristique(board, player):
    if player is board._WHITE:
        return board._nbWHITE - board._nbBLACK
    return board._nbBLACK - board._nbWHITE

def basic_progressive_heuristique(board, player):
    return (len(board._stack) - 80) * board.heuristique(player)

def victory_heuristique(board, player):
    if board.is_game_over():
        score = basic_heuristique(board, player)
        return 0 if score == 0 else score * float('+inf')
    return 0

def forcing_basic_heuristique(board, player):
    return coeff(player == board._nextPlayer) * len(board._stack)

def forcing_compound_heuristique(board, player):
    _sum = 0
    legal_moves = board.legal_moves()
    for move in legal_moves:
        board.push(move)
        _sum += forcing_basic_heuristique(board, player)
        board.pop()
    return forcing_basic_heuristique(board, player) + _sum / len(legal_moves)

#POS HEURISTICS
def cornerValue(board, value, player):
    opponent = (board._WHITE if player == board._BLACK else board._BLACK)
    def eval(cases_min, cases_max):
        value = 0
        for case in cases_max:
            if board._board[case[0]][case[1]] == player:
                value += 500
            elif board._board[case[0]][case[1]] == opponent:
                value -= 500
        for case in cases_min:
            if board._board[case[0]][case[1]] == player:
                value -= 500
            elif board._board[case[0]][case[1]] == opponent:
                value += 500

        return value
    toMin = [(1,0), (0,1), (0,8), (1,9), (8,0), (9,1), (8,9), (9,8)]
    toMax = [(0,0), (1,1), (0,9), (1,8), (9,0), (8,1), (9,9), (8,8)]
    return eval(toMin,toMax)

def bordValue(board, value, player):
    opponent = (board._WHITE if player == board._BLACK else board._BLACK)

    def eval(cases_max):
        value = 0
        for case in cases_max:
            if board._board[case[0]][case[1]] == player:
                value += (50 if len(board._stack) > 30 else 3)
            elif board._board[case[0]][case[1]] == opponent:
                value -= (50 if len(board._stack) > 30 else 3)
        return value

    toMax = []
    for i in range(0,10):
        for j in range(0,10):
            if i == 0 or j == 0 or i == 9 or j == 9:
                toMax.append((i,j))
    return eval(toMax)


def centerValue(board, value,player):
    opponent = (board._WHITE if player == board._BLACK else board._BLACK)
    value = 0
    for case in [(4,4), (5,4), (4,5), (5,5)]:
        x, y = case
        if board._board[x][y] == player:
            value += (50 if len(board._stack) < 20 else 10)
        elif board._board[x][y] == opponent:
            value -= (50 if len(board._stack) < 20 else 10)
    return value


def pos_heuristique(board, player):
    value = 0
    value = cornerValue(board, value, player)
    value = centerValue(board, value, player)
    value = bordValue(board,value,player)
    if len(board._stack) > 30:
        value += board.heuristique(player)
    else:
        value -= board.heuristique(player)
    return value

# Initial value: 50 on tier1, 10 elsewise
def center_value_heuristique(board, player):
    opponent = (board._WHITE if player == board._BLACK else board._BLACK)
    value = 0
    for case in [(4,4), (5,4), (4,5), (5,5)]:
        x, y = case
        if board._board[x][y] == player:
            value += 1
        elif board._board[x][y] == opponent:
            value -= 1
    return value

class ImpregnableBoard:
    '''Take count of impregnable pieces of a single player.

    Definition: Corners are impregnable. A piece is imprenable if a neighbour is imprenable.
    Note that this definition is not the most complete but the easiest to compute.
    '''

    def __init__(self, boardsize, player):
        self._boardsize = boardsize
        self._player = player
        self._board = []
        self._stack = []
        self.count = 0

        for x in range(self._boardsize):
            self._board.append([ False ] * self._boardsize)

    def _is_impregnable(self, x, y):
        horizontal_impregnability = x == 0 or x == self._boardsize - 1
        vertical_impregnability = y == 0 or y == self._boardsize - 1
        diagonal_impregnability = horizontal_impregnability or vertical_impregnability

        horizontal_impregnable = horizontal_impregnability or self._board[x - 1][y] or self._board[x + 1][y]
        if not horizontal_impregnable:
            return False

        vertical_impregnable = vertical_impregnability or self._board[x][y - 1] or self._board[x][y + 1]
        if not vertical_impregnable:
            return False

        first_diagonal_impregnable = diagonal_impregnability or self._board[x - 1][y - 1] or self._board[x + 1][y + 1]
        if not first_diagonal_impregnable:
            return False

        second_diagonal_impregnable = diagonal_impregnability or self._board[x - 1][y + 1] or self._board[x + 1][y - 1]
        if not second_diagonal_impregnable:
            return False

        return True

    def _update(self, board, x0, y0):
        '''Recursively update board, stack and count'''

        if board._board[x0][y0] != self._player or self._board[x0][y0] or not self._is_impregnable(x0, y0):
            return

        # Is impregnable!
        self._board[x0][y0] = True
        self._stack[-1][1].append((x0, y0))
        self.count += 1

        # Update neighbourhood
        for x1 in range(x0 - 1, x0 + 2):
            for y1 in range(y0 - 1, y0 + 2):
                if (x1, y1) != (x0, y0) and 0 <= x1 and x1 < self._boardsize and 0 <= y1 and y1 < self._boardsize:
                    self._update(board, x1, y1)

    def _append(self, board, move):
        '''Append player move to board'''

        player, x, y = move
        assert player == self._player

        # Memorize move and contribution to count
        self._stack.append([ move, [] ])

        # Update impregnability
        self._update(board, x, y)

    def _pop(self):
        _, toFlip = self._stack.pop()
        for x0, y0 in toFlip:
            self._board[x0][y0] = False
        self.count -= len(toFlip)

    def compute(self, board):
        '''Compute count of impregnable pieces.

        To call after the push of each self._player move.
        '''
        global debug

        if debug:
            sys.stderr.write("PLAYER %s\n" % self._player)
            sys.stderr.write("BOARD STACK %s\n" % board._stack)
            sys.stderr.write("IMPREGNABLE STACK %s\n" % self._stack)
            sys.stderr.flush()

        # Check what moves are still valid
        board_stack_index = -1 % len(board._stack) % 2
        nb_of_valid = 0
        for move, _ in self._stack:
            if board_stack_index < len(board._stack) and move == board._stack[board_stack_index][0]:
                nb_of_valid += 1
                board_stack_index += 2
            else:
                break

        # Pop next ones
        for i in range(len(self._stack) - nb_of_valid):
            self._pop()

        if debug:
            sys.stderr.write("AFTER POP %s\n" % self._stack)
            sys.stderr.flush()

        # Append each new move
        while board_stack_index < len(board._stack):
            self._append(board, board._stack[board_stack_index][0])
            board_stack_index += 2

        if debug:
            sys.stderr.write("AFTER APPEND %s\n" % self._stack)
            sys.stderr.flush()

impregnable_boards = [ None, ImpregnableBoard(10, 1), ImpregnableBoard(10, 2) ]

def impregnable_heuristique(board, player):
    global impregnable_boards, debug

    impregnable_boards[board._flip(board._nextPlayer)].compute(board)

    if debug:
        sys.stderr.write("\n")

    move = board._stack[-1][0]
    board.pop()
    impregnable_boards[board._flip(board._nextPlayer)].compute(board)
    board.push(move)

    if debug:
        sys.stderr.write("\n")

    black_count = impregnable_boards[1].count
    white_count = impregnable_boards[2].count
    score = coeff(player == board._BLACK) * (impregnable_boards[1].count - impregnable_boards[2].count)
    if debug:
        sys.stderr.write("SCORES: %s (black=%s, white=%s)\n" % (score, black_count, white_count))
    return score


def pos_vic_heuristique(board, player):
    if len(board._stack) < 44:
        value = pos_heuristique(board, player)
    else:
        value = victory_heuristique(board, player)
    return value

##############################################################################
#### PLAYERS #################################################################
##############################################################################

def tier(a, b, c, d = None, d_moves = 10):
    def player(board, deadline):
        n = len(board._stack)
        if d and (96 - d_moves) <= n:
            return d(board, deadline)
        if n < 32:
            return a(board, deadline)
        if n < 64:
            return b(board, deadline)
        return c(board, deadline)
    return player

def random_player(board, deadline):
    return choice(board.legal_moves())

def basic_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer, basic_heuristique)

basic_tier1_player = tier(basic_player, random_player, random_player)
basic_tier2_player = tier(random_player, basic_player, random_player)
basic_tier3_player = tier(random_player, random_player, basic_player)

def basic_semi_player(board, deadline):
    if len(board._stack) < 50:
        return random_player(board, deadline)
    return basic_player(board, deadline)

def basic_quasi_player(board, deadline):
    if len(board._stack) < 80:
        return random_player(board, deadline)
    return basic_player(board, deadline)

def basic_progressive_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer, basic_progressive_heuristique)

def victory_player(board, deadline):
    # if len(board._stack) < 80:
    #     print('============ USING RANDOM ============')
    #     return random_player(board, deadline)
    print('============ USING VICTORY ============')
    return deepen(board, deadline, alphabeta_explorer, victory_heuristique)

def forcing_basic_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer, forcing_basic_heuristique)

def forcing_compound_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer, forcing_compound_heuristique)

forcing_tier1_player = tier(forcing_compound_player, random_player, random_player)
forcing_tier2_player = tier(random_player, forcing_compound_player, random_player)
forcing_tier3_player = tier(random_player, random_player, forcing_compound_player)

# Formerly forcing_basic_player
def forcing_basic_vic_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer,
        compose([ victory_heuristique, forcing_basic_heuristique ])
    )

# Formerly forcing_compound_player
def forcing_vic_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer,
        compose([ victory_heuristique, forcing_compound_heuristique ])
    )

def bf_vic_player(alpha = 1):
    def player(board, deadline):
        return deepen(board, deadline, alphabeta_explorer,
            compose([ victory_heuristique, basic_heuristique, (forcing_compound_heuristique, lambda x: alpha*x) ])
        )
    return player

bf1_vic_player = bf_vic_player()
bf2_vic_player = bf_vic_player(2)
bf25_vic_player = bf_vic_player(2.5)
bf275_vic_player = bf_vic_player(2.75)
bf3_vic_player = bf_vic_player(3)
bf325_vic_player = bf_vic_player(3.25)
bf35_vic_player = bf_vic_player(3.5)
bf4_vic_player = bf_vic_player(4)

bf325_tier1_vic_player = tier(bf325_vic_player, random_player, random_player)
bf325_tier2_vic_player = tier(random_player, bf325_vic_player, random_player)

bf305_tier3_vic_player = tier(random_player, random_player, bf_vic_player(3.05))
bf315_tier3_vic_player = tier(random_player, random_player, bf_vic_player(3.15))
bf325_tier3_vic_player = tier(random_player, random_player, bf325_vic_player)
bf335_tier3_vic_player = tier(random_player, random_player, bf_vic_player(3.35))

def impregnable_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer, impregnable_heuristique)

# Formerly impregnable_player
def impregnable_vic_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer,
        compose([ victory_heuristique, impregnable_heuristique ])
    )

impregnable_tier1_player = tier(impregnable_player, random_player, random_player)
impregnable_tier2_player = tier(random_player, impregnable_player, random_player)
impregnable_tier3_player = tier(random_player, random_player, impregnable_player)

def pos_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer, pos_heuristique)

pos_tier1_player = tier(pos_player, random_player, random_player)
pos_tier2_player = tier(random_player, pos_player, random_player)
pos_tier3_player = tier(random_player, random_player, pos_player)

def pos_vic_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer, pos_vic_heuristique)


def fp_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer,
        compose([ pos_heuristique, impregnable_heuristique ])
    )

def fp2_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer,
        compose([ center_value_heuristique, (impregnable_heuristique, lambda x: 10*x) ])
    )

fp2_tier1_player = tier(fp2_player, random_player, random_player)
fp2_tier2_player = tier(random_player, fp2_player, random_player)
fp2_tier3_player = tier(random_player, random_player, fp2_player)

bfi_315_30_player = lambda board, deadline: deepen(board, deadline, alphabeta_explorer, compose([
    basic_heuristique,
    (forcing_compound_heuristique, lambda x: 3.15*x),
    (impregnable_heuristique, lambda x: 30*x)
]))
bfi_315_50_player = lambda board, deadline: deepen(board, deadline, alphabeta_explorer, compose([
    basic_heuristique,
    (forcing_compound_heuristique, lambda x: 3.15*x),
    (impregnable_heuristique, lambda x: 50*x)
]))

bfi_315_30_tier3_vic_player = tier(random_player, random_player, bfi_315_30_player, victory_player)
bfi_315_30_tier3_vic_12_player = tier(random_player, random_player, bfi_315_30_player, victory_player, 12)
bfi_315_50_tier3_vic_player = tier(random_player, random_player, bfi_315_50_player, victory_player)

def bip2_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer,
        compose([ (center_value_heuristique,lambda x: 500*x), (impregnable_heuristique, lambda x: 500*x), basic_heuristique])
    )

bip_tier2_player = tier(random_player, bip2_player, random_player)


def bi_player(board, deadline):
    return deepen(board, deadline, alphabeta_explorer,
        compose([ (impregnable_heuristique, lambda x: 500*x), basic_heuristique])
    )

bi_tier2_player = tier(random_player, bi_player, random_player)

tree = None
def opening_init(filename):
    global tree
    with open(filename) as file:
        tree = json.load(file)
opening_init('opening_libraries/openings-7-7-v1.data.min')

def opening_player(next_player = random_player):
    def player(board, deadline, deepness = 7):
        if deepness - 2 < len(board._stack):
            return next_player(board, deadline)

        global tree
        node = tree
        def hash(move):
            return "%s,%s" % (move[1], move[2])

        for move, _ in board._stack:
            try:
                node = node[hash(move)]
            except KeyError:
                print(hash(move))
                print('!!! MOVE NOT IN OPENING !!!')
                return next_player(board, deadline)

        r = [ board._nextPlayer ] + list(map(int, node['b'].split(',')))
        return r
    return player
opening_random_player = opening_player()

final_player = tier(opening_player(basic_player), forcing_compound_player, bfi_315_30_player, victory_player, 10)


##############################################################################
#### TESTS ###################################################################
##############################################################################

if __name__ == '__main__':
    debug = True

    from Reversi import Board
    import json
    board = Board(10)

    # board.push([1, 2, 4])
    # move, dA = alphabeta_explorer(board, float('+inf'), 3, basic_heuristique)
    # print(json.dumps(dA, indent=4))
    # print("RESULT IS " + str(move))

    while not board.is_game_over():
        board.push(choice(board.legal_moves()))
    impregnable_heuristique(board, board._BLACK)
    print(board)
    board.pop()
    print(board)
