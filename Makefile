ifeq ($(shell uname), Darwin)
CC = gcc-7
else
CC = gcc
endif

# OPTFLAGS = -g3
OPTFLAGS = -Ofast -DNDEBUG -fpeephole -m64 -march=native
CFLAGS = $(OPTFLAGS) -std=c99 -Wno-implicit-int -Wno-unused-result -Wno-switch -Wno-pointer-arith

CORES = $(shell getconf _NPROCESSORS_ONLN)
all:; $(MAKE) -j$(CORES) _all
_all: sudoku_check sudoku_solver sudoku_ui sudoku_advanced sudoku_generator

sudoku_check: sudoku_check.o checker_sd.o checker_io.o
sudoku_solver: sudoku_solver.o solver.o solver_sd.o solver_io.o
sudoku_advanced: sudoku_advanced.o algx.o
sudoku_generator:sudoku_generator.c algx.c
# note: do not redirect the file into ui executable, as it will stop reading the
# keys after receiving EOF.
sudoku_ui: sudoku_ui.c algx.o
	$(CC) $(CFLAGS) $(shell pkg-config --libs --cflags ncurses ncursesw) sudoku_ui.c algx.o -o $@

# run all 501.5k tests
test:
	rm -rvf res
	./test_all

clean:
	rm -vf *.o sudoku_check sudoku_solver sudoku_advanced sudoku_ui sudoku_generator
	rm -rvf *.dSYM
