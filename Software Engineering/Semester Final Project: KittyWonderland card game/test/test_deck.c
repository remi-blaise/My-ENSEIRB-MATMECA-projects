#include "../src/deck.h"
#include "../src/entity.h"
#include "assert.h"

void test_deal_deck() {
	struct entity _player;
	struct entity * player = &_player;

	set_up_one_player_with_hand_and_deck(player);

	deal_deck(player->deck);

  int haveDeck = 1;
  for (size_t i = 0; i < DECK_SIZE; i++) {
    if (player->deck->cards[i] == TEST2) {
      haveDeck = 0;
    }
  }

	assert(haveDeck, "deal_deck doesn't have the expected behaviour.");

	tear_down_one_player_with_hand_and_deck(player);
}

void test_draw_card() {
	struct entity _player;
	struct entity * player = &_player;

	set_up_one_player_with_hand_and_deck(player);

	assert(draw_card(player->deck) == TEST2, "draw_card doesn't have the expected behaviour.");

	tear_down_one_player_with_hand_and_deck(player);
}

int main()
{
  test_deal_deck();
	test_draw_card();

  return 0;
}
