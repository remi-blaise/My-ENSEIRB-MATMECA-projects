#include "utils.h"

int randint(int a, int b) {
  return rand() % (b - a) + a;
}

void display_array(int len, char t[]) {
  int i = len;
  printf("\t");
  while (i){
    printf("%d", t[len - i]);
    i = i - 1;
  }
  printf("\n");
}

void display_board(int board_size, char (*board) [board_size]) {
  for (int i = 0; i < board_size; i++) {
    display_array(board_size, board[i]);
  }
}

void mod(int * a, int b) {
  if (b <= *a) {
    *a -= b;
  }
  if (*a < 0) {
    *a += b;
  }
}

int min(int a, int b) {
  return a < b ? a : b;
}

int max(int a, int b) {
  return a > b ? a : b;
}
