CFLAGS=-Wall -Wextra -std=c99 -Werror

SRC_DIR=src
TEST_DIR=test

OBJ=$(SRC_DIR)/cards.o \
	$(SRC_DIR)/entity.o \
	$(SRC_DIR)/board.o \
	$(SRC_DIR)/utils.o \
	$(SRC_DIR)/deck.o \
	$(SRC_DIR)/game.o
TEST_OBJ=$(OBJ) $(TEST_DIR)/assert.o
TEST_PARAMS=$(TEST_OBJ) -o test-1 && ./test-1 && rm test-1

.PHONY: all
all: code

.PHONY: code
code: $(SRC_DIR)/main.c $(OBJ)
	gcc $(CFLAGS) $(SRC_DIR)/main.c $(OBJ) -o project

.PHONY: clean
clean:
	rm -f project test-1

.PHONY: exec
exec: code
	./project -s `date +%s%N`
	rm project

.PHONY: test
test: test-entity test-cards test-game test-board test-deck

.PHONY: test-entity
test-entity: $(TEST_DIR)/test_entity.c $(TEST_OBJ)
	gcc $(CFLAGS) -g $(TEST_DIR)/test_entity.c $(TEST_PARAMS)

.PHONY: test-cards
test-cards: $(TEST_DIR)/test_cards.c $(TEST_OBJ)
	gcc $(CFLAGS) -g $(TEST_DIR)/test_cards.c $(TEST_PARAMS)

.PHONY: test-game
test-game: $(TEST_DIR)/test_game.c $(TEST_OBJ)
	gcc $(CFLAGS) -g $(TEST_DIR)/test_game.c $(TEST_PARAMS)

.PHONY: test-board
test-board: $(TEST_DIR)/test_board.c $(TEST_OBJ)
	gcc $(CFLAGS) -g $(TEST_DIR)/test_board.c $(TEST_PARAMS)

.PHONY: test-deck
test-deck: $(TEST_DIR)/test_deck.c $(TEST_OBJ)
	gcc $(CFLAGS) -g $(TEST_DIR)/test_deck.c $(TEST_PARAMS)
