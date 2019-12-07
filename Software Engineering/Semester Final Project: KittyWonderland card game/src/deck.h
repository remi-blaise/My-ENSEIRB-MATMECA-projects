#ifndef __DECK__
#define __DECK__

#define DECK_SIZE 50

#include "utils.h"
#include "cards.h"

struct deck {
  const struct card * cards[DECK_SIZE];
  int index;
};

void shuffle_deck(struct deck *);
void deal_deck(struct deck *);
const struct card * draw_card(struct deck *);

#endif
