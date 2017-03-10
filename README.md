# sdsolver

## Author

Researched by me.

## Contents

* Checker
* Basic (stupid) solver
* Advanced (backtrack) solver
* Command-line interface for composing puzzles

## Capabilities

* Checker: none, it's useless
* Basic solver:

	- Possible values stored in bits
	- Recursion heuristics for least possibilities on a cell
	
* Advanced solver:

	- Algorithm X
	- Arrays instead of DLX
	- Forward heuristics: minimal number of solutions for a constraint
	- Another heuristics: maximum number of deadends for the constraint

* Puzzle composer:

	- ncurses + advanced solver
	- Arrows
	- Setting/unsetting values
	- Validation
	- Completion (of a particular cell)
	- Exit

## Note

CS2002-W07Practical-C2. This is not the whole submission. Researched and coded in 14 days.