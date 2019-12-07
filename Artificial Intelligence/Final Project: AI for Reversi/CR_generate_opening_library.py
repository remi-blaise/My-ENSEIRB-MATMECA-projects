from Reversi import Board
import CR_ai
import sys
import argparse
import json
from random import choice
from time import time

version = "1"

'''

We want to write a file on the following format:

# For black
[
  first_black_move, {
    white_move1: [ second_black_move, { ... } ],
    white_move2: [ second_black_move, { ... } ],
    ...
  }
]

# For white
{
  black_move1: [ first_white_move, { ... } ],
  black_move2: [ first_white_move, { ... } ],
  ...
}

or a mix:

# For both
{
  best: black_move1,
  black_move1: {
    best: white_move1,
    white_move1: { ... },
    white_move2: { ... }
  },
  black_move2: { ... },
  ...
}

'''

total_visited_nodes = 0
total_calculated_nodes = 0
visited_nodes = 0
calculated_nodes = 0
is_calculation_finished = False

def hash(move):
    return "%s,%s" % (move[1], move[2])

def progress_pre_count(board, progress_deepness=3):
    if progress_deepness == 1:
        return len(board.legal_moves())

    count = 0

    for move in board.legal_moves():
        board.push(move)
        count += progress_pre_count(board, progress_deepness - 1)
        board.pop()

    return count

progress_count = progress_pre_count(Board(10))
progress_visited = 0

def progress_bar(name, value, endvalue, bar_length = 50):
    percent = float(value) / endvalue
    arrow = '-' * int(round(percent*bar_length) - 1) + '>'
    spaces = ' ' * (bar_length - len(arrow))
    sys.stdout.write("\r{0}: [{1}]{2}%".format(name, arrow + spaces, int(round(percent*100))))
    sys.stdout.flush()
    if value == endvalue:
        sys.stdout.write('\n')

def alphabeta(board, storage, storage_deepness, exploration_deepness, is_maxising = True, alpha = float('-inf'), beta = float('+inf'), progress_deepness = 3):
    global optimize_against_alphabeta
    global visited_nodes, calculated_nodes
    global progress_count, progress_visited
    visited_nodes += 1

    if storage and 'calculated' in storage:
        return storage['calculated']

    legal_moves = board.legal_moves()
    if len(legal_moves) == 0 or exploration_deepness == 0:
        # h = board.heuristique(board)
        h = CR_ai.compose([ CR_ai.basic_heuristique, (CR_ai.forcing_basic_heuristique, lambda x: 2*x), (CR_ai.impregnable_heuristique, lambda x: 50*x), CR_ai.pos_heuristique ])(board, board._nextPlayer)
        calculated_nodes += 1
        return h, h

    value = float('-inf' if is_maxising else 'inf')
    moves = []

    for move in legal_moves:
        board.push(move)
        move_hash = hash(move)
        child_storage = None
        if 1 < storage_deepness:
            if not move_hash in storage:
                storage[move_hash] = {}
            child_storage = storage[move_hash]
        opponent_value, alphaOrBeta = alphabeta(board, child_storage, storage_deepness - 1, exploration_deepness - 1, not is_maxising, alpha, beta, progress_deepness - 1)
        board.pop()

        if is_maxising:
            alpha = max(alpha, alphaOrBeta)
        else:
            beta = min(beta, alphaOrBeta)

        if optimize_against_alphabeta and alpha >= beta:
            break

        if (opponent_value >= value if is_maxising else opponent_value <= value) :
            moves.append(move)
            value = opponent_value

    calculated = [ value, (alpha if is_maxising else beta) ]

    if 0 < storage_deepness and len(moves):
        storage['best'] = hash(choice(moves))
        storage['calculated'] = calculated

    if progress_deepness == 0:
        progress_visited += 1
        progress_bar("Computing", progress_visited, progress_count)

    return calculated

# Parse args
parser = argparse.ArgumentParser()
parser.add_argument("storage_deepness", type=int)
parser.add_argument("--exploration_deepness", type=int, default=None)
parser.add_argument("--restrict_to_alphabeta", help="/!\ Optimize for strong players (carefull!) /!\\", action="store_true")
args = parser.parse_args()
storage_deepness, exploration_deepness, optimize_against_alphabeta = args.storage_deepness, args.exploration_deepness, args.restrict_to_alphabeta
if exploration_deepness is None:
    exploration_deepness = storage_deepness

# Read data
reading_start_time = time()
filename = 'opening_libraries/openings%s-%d-%d-v%s.data' % ('-ab' if optimize_against_alphabeta else '', storage_deepness, exploration_deepness, version)
try:
    with open(filename) as file:
        last_data = json.load(file)
except FileNotFoundError:
    last_data = False
reading_end_time = time()

# Compute
board = Board(10)
tree = {}
if last_data:
    tree = last_data['tree']
    progress_visited = last_data['progress_visited']
    total_visited_nodes = last_data['total_visited_nodes']
    total_calculated_nodes = last_data['total_calculated_nodes']
executing_start_time = time()
try:
    print("\nAppuies sur CTRL-C pour mettre en pause !")
    alphabeta(board, tree, storage_deepness, exploration_deepness)
    is_calculation_finished = True
except KeyboardInterrupt:
    print("")
executing_end_time = time()
total_visited_nodes += visited_nodes
total_calculated_nodes += calculated_nodes

# Write data
writing_start_time = time()

with open(filename, 'w') as file:
    json.dump({
        "version": version,
        "is_calculation_finished": is_calculation_finished,
        "progress_visited": progress_visited,
        "total_visited_nodes": total_visited_nodes,
        "total_calculated_nodes": total_calculated_nodes,
        "storage_deepness": storage_deepness,
        "exploration_deepness": exploration_deepness,
        "optimize_against_alphabeta": optimize_against_alphabeta,
        "tree": tree
    }, file, indent=2, sort_keys=True)

def clean_tree(node = tree):
    if 'calculated' in node:
        del node['calculated']
    if 'best' in node:
        node['b'] = node.pop('best')
    for index in node:
        if index in ('best', 'b', 'calculated'):
            continue
        clean_tree(node[index])

if is_calculation_finished:
    clean_tree()
    with open(filename + '.min', 'w') as file:
        json.dump(tree, file, indent=None, separators=(',', ':'), sort_keys=True)

writing_end_time = time()

print("")
print("Total visited nodes:", total_visited_nodes)
print("Total calculated nodes:", total_calculated_nodes)
if total_visited_nodes != visited_nodes:
    print("Visited nodes:", visited_nodes)
if total_calculated_nodes != calculated_nodes:
    print("Calculated nodes:", calculated_nodes)
print("")
print("Reading time: ", reading_end_time - reading_start_time)
print("Execution time: ", executing_end_time - executing_start_time)
print("Writing time: ", writing_end_time - writing_start_time)
print("")
