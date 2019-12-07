CC = gcc
CCONFIG = -Wall -Isrc -g3 -O0
CPTHREAD = $(CCONFIG) -DUSE_PTHREAD -pthread


all : compilelib pthread preemption

#compile our library
compilelib : build/01-main build/02-switch build/11-join build/12-join-main build/21-create-many build/22-create-many-recursive build/23-create-many-once build/31-switch-many build/32-switch-many-join build/51-fibonacci build/61-mutex build/62-mutex
#compile pthread lirary
pthread : build/01-main-pt build/02-switch-pt build/11-join-pt build/12-join-main-pt build/21-create-many-pt build/22-create-many-recursive-pt build/23-create-many-once-pt build/31-switch-many-pt build/32-switch-many-join-pt build/51-fibonacci-pt build/61-mutex-pt build/62-mutex-pt
#compile pthread lirary
preemption : build/01-main-prmt build/02-switch-prmt build/11-join-prmt build/12-join-main-prmt build/21-create-many-prmt build/22-create-many-recursive-prmt build/23-create-many-once-prmt build/31-switch-many-prmt build/32-switch-many-join-prmt build/51-fibonacci-prmt build/61-mutex-prmt build/62-mutex-prmt build/71-preemption-prmt

build/thread.o : src/thread.c
	$(CC) -c $< -o $@ $(CCONFIG)

build/threadprmt.o : src/thread.c
	$(CC) -c $< -o $@ $(CCONFIG) -DPREEMPTION

#-pt : output of compiling with pthread library
build/%-pt.o : tst/%.c
	$(CC) -o $@ -c $< $(CPTHREAD)

build/%-pt : build/%-pt.o
	$(CC) -o $@ $^ -lpthread

#-pt : output of compiling with pthread library
build/%-prmt.o : tst/%.c
	$(CC) -o $@ -c $<

build/%-prmt : build/%-prmt.o build/threadprmt.o
	$(CC) $^ -o $@

build/%.o: tst/%.c
	$(CC) -o $@ -c $< $(CCONFIG)

build/% : build/%.o build/thread.o
	$(CC) $^ -o $@


check:
	@echo test 01-main
	build/01-main-pt
	build/01-main
	@echo test 02-switch
	build/02-switch-pt
	build/02-switch
	@echo test 11-join
	build/11-join-pt
	build/11-join
	@echo test 12-join-main
	build/12-join-main-pt
	build/12-join-main
	@echo test 21-create-many
	build/21-create-many-pt 56
	build/21-create-many 56
	@echo test 22-create-many-recursive
	build/22-create-many-recursive-pt 56
	build/22-create-many-recursive 56
	@echo test 23-create-many-once
	build/23-create-many-once-pt 56
	build/23-create-many-once 56
	@echo test 31-switch-many
	build/31-switch-many-pt 56 45
	build/31-switch-many 56 45
	@echo test 32-switch-many-join
	build/32-switch-many-join-pt 56 45
	build/32-switch-many-join 56 45
	@echo test 51-fibonacci
	build/51-fibonacci-pt 17
	build/51-fibonacci 17
	@echo test 61-mutex
	build/61-mutex-pt 10
	build/61-mutex 10


valgrind:
	@echo test 01-main
	valgrind build/01-main
	@echo test 02-switch
	valgrind build/02-switch
	@echo test 11-join
	valgrind build/11-join
	@echo test 12-join-main
	valgrind build/12-join-main
	@echo test 21-create-many
	valgrind build/21-create-many 56
	@echo test 22-create-many-recursive
	valgrind build/22-create-many-recursive 56
	@echo test 23-create-many-once
	valgrind build/23-create-many-once 56
	@echo test 31-switch-many
	valgrind build/31-switch-many 56 45
	@echo test 32-switch-many-join
	valgrind build/32-switch-many-join 56 45
	@echo test 51-fibonacci
	valgrind build/51-fibonacci 17
	valgrind build/32-switch-many-join 56 45
	@echo test build/61-mutex
	valgrind build/61-mutex 10

.PHONY: graphs
graphs:
	echo "Cette opération prend environ six minutes..."
	./generate-data > build/graphs && ./make-graphs.py

clean:
	rm -rf build/*.o
