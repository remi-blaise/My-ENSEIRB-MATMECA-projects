# -*- coding: utf-8 -*-

from playerInterface import PlayerInterface
from Reversi import Board
from CR_ai import final_player
from time import time

class Player(PlayerInterface):
    def __init__(self, ai_player = final_player, time = 5*60): # time = 5*60
        self._board = Board(10)
        self._mycolor = None
        self._ai_player = ai_player
        self._timePerMove = int(time / 48)

    def getPlayerName(self):
        return "Christian & Rémi - %s" % self._ai_player.__name__

    def getPlayerMove(self):
        if self._board.is_game_over():
            print("Referee asked me to play but game is over")
            return -1, -1

        color, x, y = move = self._ai_player(self._board, time() + self._timePerMove)
        self._board.push(move)

        # print("I am playing ", move)

        assert color == self._mycolor

        # print("My current board :")
        # print(self._board)

        return x, y

    def playOpponentMove(self, x,y):
        if self._board.is_game_over():
            print("Referee asked me to playOpponentMove but game is over")
            return -1, -1

        assert self._board.is_valid_move(self._opponent, x, y)
        # print("Opponent played ", (x, y))
        self._board.push([self._opponent, x, y])

    def newGame(self, color):
        self._mycolor = color
        self._opponent = 1 if color == 2 else 2

    def endGame(self, winner):
        pass

        # if self._mycolor == winner:
        #     print("I won!!!")
        # else:
        #     print("I lost :(!!")
