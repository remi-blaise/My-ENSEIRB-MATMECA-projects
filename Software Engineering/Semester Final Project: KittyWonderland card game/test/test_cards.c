#include "../src/cards.h"
#include "../src/entity.h"
#include "assert.h"

void set_up_one_player(struct entity * player) {
	player->life = 50;
	player->mana = 100;
	player->mana_regen = 2;
}

void set_up_two_players(struct entity * player1, struct entity * player2) {
	set_up_one_player(player1);
  set_up_one_player(player2);
}

void test_cards() {
	struct entity _player1;
	struct entity * player1 = &_player1;
	struct entity _player2;
	struct entity * player2 = &_player2;
	struct game_data _game_data;
	struct game_data * game_data = &_game_data;

  set_up_two_players(player1, player2);
  think(player1, player2, game_data);
  assert(player1->mana == 95 && player1->mana_regen == 3, "think doesn't have the expected behaviour.");

  set_up_two_players(player1, player2);
  razor(player1, player2, game_data);
  assert(player1->mana == 98 && player2->life == 40, "razor doesn't have the expected behaviour.");

  set_up_two_players(player1, player2);
  panacea(player1, player2, game_data);
  assert(player1->mana == 98 && player1->life == 60, "panacea doesn't have the expected behaviour.");

  set_up_two_players(player1, player2);
  steal(player1, player2, game_data);
  assert(player1->mana == 90 && player1->mana_regen == 3 && player2->mana_regen == 1, "steal doesn't have the expected behaviour.");

  set_up_two_players(player1, player2);
  player2->mana_regen = 1;
  steal(player1, player2, game_data);
  assert(player1->mana == 90 && player1->mana_regen == 3 && player2->mana_regen == 1, "steal doesn't have the expected behaviour.");

  set_up_two_players(player1, player2);
  hell(player1, player2, game_data);
  assert(player1->mana == 0 && player2->life <= -10, "hell doesn't have the expected behaviour.");
}

void test_stone() {
	struct game_data _game_data;
	struct game_data * game_data = &_game_data;
  game_data->n_players = 1;
  set_up_many_players(game_data);
  init_board(game_data);
	game_data->entities[0]->mana = 42;

	stone(game_data->entities[0], game_data->entities[0], game_data);

	int count_ones = 0;
	for (int way = -1; way <= 1; way += 2) {
    for (int axe = 0; axe <= 1; axe++) {
      count_ones += game_data->board[!axe ? game_data->entities[0]->x + way : game_data->entities[0]->x]
        [axe ? game_data->entities[0]->y + way : game_data->entities[0]->y];
    }
  }
	assert(count_ones == 1, "stone should put exactly one caramel next to player.");
	tear_down_many_players(game_data);
}

void test_puppy() {
	struct game_data _game_data;
	struct game_data * game_data = &_game_data;
  game_data->n_players = 1;
  set_up_many_players(game_data);
  init_board(game_data);
	game_data->entities[0]->mana = 42;

	puppy(game_data->entities[0], game_data->entities[0], game_data);

	assert(game_data->n_friends == 1, "puppy should summon one friend");

	int count_ones = 0;
	for (int way = -1; way <= 1; way += 2) {
    for (int axe = 0; axe <= 1; axe++) {
      count_ones += game_data->board[!axe ? game_data->entities[0]->x + way : game_data->entities[0]->x]
        [axe ? game_data->entities[0]->y + way : game_data->entities[0]->y];
    }
  }
	assert(count_ones == 1, "puppy should put exactly one puppy on the board next to player.");

	tear_down_many_players(game_data);
}

int main()
{
	test_cards();
	test_stone();
	test_puppy();

  return 0;
}
