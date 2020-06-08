ifeq ($(shell uname), Darwin)
CC = gcc-10
# CC = icc
else
CC = gcc
endif

# OPTFLAGS = -g3
OPTFLAGS = -Ofast -DNDEBUG -mavx -mavx2 -fpeephole -m64 -march=native -mtune=native -mms-bitfields -ftree-vect-loop-version -fsched-spec-load-dangerous -fomit-frame-pointer
CFLAGS = $(OPTFLAGS) -std=c99 -Wno-implicit-int -Wno-unused-result -Wno-switch -Wno-pointer-arith -fopt-info -Wall -Wextra

CORES = $(shell getconf _NPROCESSORS_ONLN)
all:; $(MAKE) -j$(CORES) _all
_all: sudoku_check sudoku_solver sudoku_ui sudoku_advanced sudoku_generator

sudoku_check: sudoku_check.o checker_sd.o checker_io.o
sudoku_solver: sudoku_solver.o solver.o solver_sd.o solver_io.o
sudoku_advanced: sudoku_advanced.c algx.c
	$(CC) $(CFLAGS) -fwhole-program sudoku_advanced.c -o $@
sudoku_generator:sudoku_generator.c algx.c
	$(CC) $(CFLAGS) sudoku_generator.c -lbsd -o $@
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
