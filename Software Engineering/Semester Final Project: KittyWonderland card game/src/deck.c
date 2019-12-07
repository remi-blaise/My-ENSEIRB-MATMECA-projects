#include "deck.h"

void deal_deck(struct deck * deck) {
  deck->cards[0] = HELL; // To avoid infinite games
  for (int i = 1; i < DECK_SIZE; ++i) {
    deck->cards[i] = draw_random_card();
  }
  deck->index = 0;
  shuffle_deck(deck);
}

void shuffle_deck(struct deck * deck) {
  for(int i = 0; i < DECK_SIZE; i++) {
    int j = randint(0, DECK_SIZE);
    const struct card * tmp = deck->cards[i];
    deck->cards[i] = deck->cards[j];
    deck->cards[j] = tmp;
  }
}

const struct card * draw_card(struct deck * deck) {
  if (deck->index == DECK_SIZE) {
    shuffle_deck(deck);
    deck->index = 0;
  }
  return deck->cards[deck->index++];
}
