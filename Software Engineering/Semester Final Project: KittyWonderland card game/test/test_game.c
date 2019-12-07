#include <sys/stat.h>

#include "../src/game.h"
#include "../src/cards.h"
#include "../src/entity.h"
#include "assert.h"

void set_up(struct game_data * game_data, const struct card * card) {
  for (int i = 0; i < game_data->n_players; ++i)
  {
    game_data->entities[i] = malloc(sizeof(struct entity));
    game_data->entities[i]->type = PLAYER_TYPE;
    game_data->entities[i]->life = 50;
    game_data->entities[i]->mana = 0;
    game_data->entities[i]->mana_regen = 1;
    sprintf(game_data->entities[i]->name, "player%d", i);

    for (int j = 0; j < HAND_SIZE; ++j)
  	{
  		game_data->entities[i]->hand[j] = card;
  	}
    game_data->entities[i]->deck = malloc(sizeof(struct deck));
  	for (int j = 0; j < DECK_SIZE; ++j)
  	{
  		game_data->entities[i]->deck->cards[j] = TEST2;
  	}
  }
}

void tear_down(struct game_data * game_data) {
  for (int i = 0; i < game_data->n_players; i++) {
    free(game_data->entities[i]->deck);
    free(game_data->entities[i]);
  }
}

void test_play_turn() {
  struct game_data _game_data;
  struct game_data * game_data = &_game_data;
  game_data->n_players = 4;
  size_t nbOfChangedCards;

  set_up(game_data, TEST);

  play_turn(game_data->entities[0], game_data);
  nbOfChangedCards = 0;
  for (size_t i = 0; i < HAND_SIZE; i++) {
    if (game_data->entities[0]->hand[i] != TEST) {
      nbOfChangedCards++;
    }
  }
  assert(nbOfChangedCards == 1, "play_turn() doesn't have the expected behaviour.");

  set_up(game_data, TEST2);
  play_turn(game_data->entities[0], game_data);
  nbOfChangedCards = 0;
  for (size_t i = 0; i < HAND_SIZE; i++) {
    if (game_data->entities[0]->hand[i] != TEST2) {
      nbOfChangedCards++;
    }
  }
  assert(nbOfChangedCards == 0, "play_turn() doesn't have the expected behaviour.");

  tear_down(game_data);
}

void test_die() {
  struct game_data _game_data;
  struct game_data * game_data = &_game_data;
  game_data->n_players = 4;
  game_data->n_friends = 0;
  set_up(game_data, TEST);

  int initial_n_players = game_data->n_players;
  die(0, game_data);
  assert(game_data->n_players == initial_n_players - 1, "die() doesn't have the expected behaviour.");
  assert(game_data->entities[0]->name[6] == game_data->entities[initial_n_players - 1]->name[6], "die() doesn't have the expected behaviour.");
  for (int i = 1; i < game_data->n_players; i++) {
    assert(game_data->entities[i]->name[6] == '0' + i, "die() shouldn't change not last game_data->entities.");
  }

  tear_down(game_data);
}

void test_purge_the_dead() {
	struct game_data _game_data;
	struct game_data * game_data = &_game_data;
  game_data->n_players = 4;

  set_up(game_data, TEST);

  for (int i = 0; i < game_data->n_players; i++) {
    game_data->entities[i]->life = 0;
  }
  purge_the_dead(game_data);
  assert(game_data->n_players == 0, "purge_the_dead() should leave game_data->entities[] empty.");

  tear_down(game_data);
}

void test_generate_mana() {
	struct game_data _game_data;
	struct game_data * game_data = &_game_data;
  game_data->n_players = 4;

  set_up(game_data, TEST);

  generate_mana(game_data);
  for (int i = 0; i < game_data->n_players; i++) {
    assert(game_data->entities[i]->mana == 1, "generate_mana() doesn't have the expected behaviour.");
  }

  tear_down(game_data);
}

int main()
{
  test_play_turn();
  test_die();
  test_purge_the_dead();
  test_generate_mana();

  return 0;
}
