#include "assert.h"

void assert(int bool, char * message)
{
  if (!bool)
  {
    printf("Assert error: %s\n", message);
    fflush(stdout);
    exit(1);
  }
}

void test(struct entity * caster, struct entity * target, struct game_data * game_data) {
  (void) caster;
	(void) target;
	(void) game_data;
}
const struct card _TEST = { "TEST", test, 0 };
const struct card * TEST = &_TEST;

const struct card _TEST2 = { "TEST2", test, 1 };
const struct card * TEST2 = &_TEST2;

void set_up_one_player_with_hand_and_deck(struct entity * player) {
	player->life = 50;
	player->mana = 0;
	player->mana_regen = 1;
	sprintf(player->name, "Rémi");
	for (int i = 0; i < HAND_SIZE; ++i)
	{
		player->hand[i] = TEST;
	}
  player->deck = malloc(sizeof(struct deck));
  player->deck->index = 0;
	for (int i = 0; i < DECK_SIZE; ++i)
	{
		player->deck->cards[i] = TEST2;
	}
}

void tear_down_one_player_with_hand_and_deck(struct entity * player) {
  free(player->deck);
}

void set_up_many_players(struct game_data * game_data) {
  for (int i = 0; i < game_data->n_players; ++i)
  {
    game_data->entities[i] = malloc(sizeof(struct entity));
  }
}

void tear_down_many_players(struct game_data * game_data) {
  for (int i = 0; i < game_data->n_players + game_data->n_friends; i++) {
    free(game_data->entities[i]);
  }
}
