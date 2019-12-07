#include "../src/cards.h"
#include "../src/entity.h"
#include "assert.h"

void test_deal_hand() {
	struct entity _player;
	struct entity * player = &_player;
	set_up_one_player_with_hand_and_deck(player);

	deal_hand(player);

  int haveHand = 1;
  for (size_t i = 0; i < HAND_SIZE; i++) {
    if (player->hand[i] == TEST) {
      haveHand = 0;
    }
  }

	assert(haveHand, "deal_hand doesn't have the expected behaviour.");

	tear_down_one_player_with_hand_and_deck(player);
}

void test_select_card() {
	struct entity _player;
	struct entity * player = &_player;
	set_up_one_player_with_hand_and_deck(player);

	assert(select_card(player) != -1, "select_card doesn't have the expected behaviour.");

	deal_hand(player);

	assert(select_card(player) == -1, "select_card doesn't have the expected behaviour.");

	tear_down_one_player_with_hand_and_deck(player);
}

int main()
{
	test_deal_hand();
	test_select_card();

  return 0;
}
