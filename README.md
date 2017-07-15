# sdsolve

## Author

Researched and written by Kirill Rodriguez.

## Contents

* Checker
* Basic (stupid) solver
* Advanced (backtrack) solver
* Command-line interface for composing puzzles
* Basic sudoku problem generator

## Usage

### Compiling

	make

### Running

#### Checker/solver

```
$ ./sudoku_checker # ./sudoku_solver # ./sudoku_advanced
3
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
```

#### UI

```
$ ./sudoku_ui
3
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
```

![ui_run](./scshts/uirun.png)

* Arrows to navigate
* **ENTER** to evaluate status
* **ESC** to leave

```
  3
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
```

![ui_run](./scshts/uivalid.png)

* Numbers to set the value
* **TAB** to complete the cell for a unique solution

```
  3
    0  0  0  0  0  0  0  1  2
    0  0  0  0  0  0  0  0  3
    0  0  2  3  0  0  4  0  0
    0  0  1  8  0  0  0  0  5
    0  6  0  0  7  0  8  0  0
    0  0  0  0  0  9  0  0  0
    0  0  8  5  0  0  0  0  0
    9  0  0  0  4  0  5  0  0
    4  7  0  0  0  6  0  0  0
```

#### Generator

```
$ time ( ./sudoku_generator 5 )
  5
  0  1  0  3 16  0  0  0 13 24  0  0 14 17 10  0 20  0  0 22 21  2 19  5  0
  0  0  0  0  0 18  0  0  0  3  0  0 13  1  0  0 17  0  0  0  0 15  0  0 12
  0  0  8  0  0  9  0 23  0  0 22  0  0 21 20  0 16  0  0  5  0  1 14  0 10
  9  0  0 15 21  0  2  0 22  0  0  0  6  4  0  0  0  0  1 11  0  8  0 18  0
  0  7  5  0  0 21 14  8 19  1  0  0  0 15 16  0  0 23  4  6  0 11 20  0 13
  0  0 25 23 14  0 12  0  0  2  0  0  0  0 18 16 19  0  5  0 15 20  0 13  0
 16  0 15  0  0  0  0 17  5  0 19 11  0 20  0  0  0  8  7 13  0  0  0  0  0
  8  5  1 19  0  0  0  0  0 23 13  7 17  0  0  0 15  9  2 10  0  0 24 16 21
 11  0 10  0  7 16  0  0 14  0 24  0  0  9 15  0  4 22 12  0  0 17  0  0 23
 17  0 22  0 24  0 10 15  4  0  0 14  8  0  0  3 11  0  0  0  0  0  6  0 18
  6 16  0  0  0  0  0  0  0 17  0  0  1  0  0  0  0  5 11  0  0  0  0  4 15
  0 14  0  0  9  0  0  0 15  6 10  5 24  0  0 23  0  0 25  7 19 21  0  8  0
 13 25  0  0  0  0 19  0  0 11 16  0  9  0  0  0 12 14  0 15 23  0 18  0  1
 15  0  4  7  0  0  0  1  0  0  0 25 20 23  0 24 10 16  0  2  0  0  0  0 22
 20  0  2 10  0  5 23  0  0  7  0 21 15 22  0  0  0  0  0 19 25  0  0 17 24
 19  0 20  9 15  6  3 18 17  0  0 16  0 11  0  0  0  0  0  1  0  0  7 25  0
  0 11 14  0  0 15  0  4  0  0  0  9  0  0 17  0  0  0 19  8  1 18  0  0  0
  0 24 17  0  0  0  0  0  0 21  0 10  0  0  7  0 22  0  9  4  0  0  0 14  0
  0  0 12  0 22  0  8  0  7  0  0  0 21  0  1  0  0 11  0  0  0  0  0  0  0
  0 21  0  0  0  0  0  9  0  0 18  0  4  8  0  0  0 15  0 24  0  3  0 22  0
 18  4 19  0  0  0  0  0  0 22  0  0 23 14 12  0  0  2  0 21  7 25 10  0 20
 25  8 16  1 23 14  4  0  2 10 21 18  0  0  9 22  0 19  0  0  0  0  3 15  0
  2 22  0 20  0 17  0  0 12  9  4 15 16  0  0  0  0  0  0 18  8 23  0 11 14
  0  0  0 14 11  0 18  0 20  0  0  0 10  2 13  0  0  0 23 12 16  0  0 19  0
 10  0  9  0 12  0  6 11  0 19  0  0  7  3  0  0 25 13  0  0 18  0  1  2  0
( ./sudoku_generator 5; )  682.79s user 0.38s system 99% cpu 11:26.01 total
```

Now, we will check how hard it is for the advanced solver to finish:

```
$ time ( ./sudoku_advanced < input.txt )
  4  1 18  3 16 12 15  6 13 24 11 23 14 17 10  7 20 25  8 22 21  2 19  5  9
 14  6 11 22 20 18  7 16 10  3  5  2 13  1  8 19 17 21 24  9  4 15 25 23 12
 24  2  8 25 19  9 17 23 11  4 22 12 18 21 20 15 16  3 13  5  6  1 14  7 10
  9 23 13 15 21 25  2  5 22 20  7 19  6  4 24 12 14 10  1 11 17  8 16 18  3
 12  7  5 17 10 21 14  8 19  1  9  3 25 15 16  2 18 23  4  6 22 11 20 24 13
 21  3 25 23 14 11 12 24  9  2  1  4 22 10 18 16 19  6  5 17 15 20  8 13  7
 16  9 15  6  2  1 21 17  5 18 19 11  3 20 23 14 24  8  7 13 10 22  4 12 25
  8  5  1 19  4 22 20  3  6 23 13  7 17 12 25 18 15  9  2 10 11 14 24 16 21
 11 20 10 18  7 16 13 19 14  8 24  6  5  9 15 21  4 22 12 25  3 17  2  1 23
 17 13 22 12 24  7 10 15  4 25  2 14  8 16 21  3 11  1 20 23  5 19  6  9 18
  6 16 23  8 18  2 24 22 25 17 14 13  1  7 19 20 21  5 11  3  9 10 12  4 15
 22 14  3 11  9 20 16 12 15  6 10  5 24 18  4 23  1 17 25  7 19 21 13  8  2
 13 25 21 24  5  4 19 10  3 11 16 17  9  6  2  8 12 14 22 15 23  7 18 20  1
 15 19  4  7 17  8  9  1 21 13 12 25 20 23 11 24 10 16 18  2 14  6  5  3 22
 20 12  2 10  1  5 23 14 18  7  8 21 15 22  3  9 13  4  6 19 25 16 11 17 24
 19 10 20  9 15  6  3 18 17 14 23 16  2 11 22 13  5 12 21  1 24  4  7 25  8
  3 11 14  2 25 15 22  4 24  5  6  9 12 13 17 10  7 20 19  8  1 18 23 21 16
  1 24 17 16  8 13 25  2 23 21  3 10 19  5  7  6 22 18  9  4 20 12 15 14 11
 23 18 12  4 22 19  8 20  7 16 15 24 21 25  1 17  2 11  3 14 13  5  9 10  6
  5 21  7 13  6 10 11  9  1 12 18 20  4  8 14 25 23 15 16 24  2  3 17 22 19
 18  4 19  5  3 24  1 13 16 22 17  8 23 14 12 11  9  2 15 21  7 25 10  6 20
 25  8 16  1 23 14  4  7  2 10 21 18 11 24  9 22  6 19 17 20 12 13  3 15  5
  2 22 24 20 13 17  5 25 12  9  4 15 16 19  6  1  3  7 10 18  8 23 21 11 14
  7 17  6 14 11  3 18 21 20 15 25  1 10  2 13  5  8 24 23 12 16  9 22 19  4
 10 15  9 21 12 23  6 11  8 19 20 22  7  3  5  4 25 13 14 16 18 24  1  2 17
( ./sudoku_advanced < input.txt; )  11.04s user 0.02s system 99% cpu 11.077 total
```

### Testing

### Checker, solvers

	./test_all

### Generator

```
$ for i in {1..10000}; do ./sudoku_generator 3 | time ( ./sudoku_solver >/dev/null ); done 2>&1 | cut -d ' ' -f 13 | sort -n --reverse | head
0.018
0.018
0.016
0.016
0.016
0.015
0.015
0.015
0.015
0.015
```

```
$ for i in {1..10}; do ./sudoku_generator 4 | time ( ./sudoku_advanced >/dev/null ); done 2>&1 | cut -d ' ' -f 13 | sort -n --reverse | head
0.324
0.211
0.196
0.189
0.178
0.144
0.132
0.122
0.119
0.077
```

## Capabilities

* Checker: none, it's useless
* Basic solver:

	- Possible values stored in bits
	- Recursion heuristics for least possibilities on a cell

* Advanced solver:

	- Algorithm X
	- Arrays instead of sparse matrices
	- Forward heuristics: minimal number of solutions for a constraint
	- Error heuristics: maximum number of deadends for the constraint

* Puzzle composer:

	- ncurses + advanced solver
	- Arrows
	- Setting/unsetting values
	- Validation
	- Completion (of a particular cell)
	- Exit

* Puzzle generator:

	- Puts random values into sudoku, solves it, and unsets random values
	- Uses advanced solver
	- Uses `clock(3)` to evaluate the difficulty.

## Note

CS2002-W07Practical-C2. This is not the whole submission. Researched and coded in 14 days.
