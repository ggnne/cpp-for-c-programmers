# Implement Monte Carlo Hex Game

The player should be able to interact with the program and choose its “color” with blue (or X) going first and red (or O) going second. The program should have a convenient interface for entering a move, displaying the board, and then making its own move. The program should determine when the game is over and announce the winner. 

This program will evaluate a position using a Monte Carlo selection of moves until the board is filled up. Then using work of Homework 4 you determine who won.  The program takes turns. It inputs the human (or machine opponent if playing against another program) move. When it is the “AI”’s turn, it is to evaluate all legal available next moves and select a “best” move.  Each legal move will be evaluated using  ~1000 or more trials. Each trial winds the game forward by randomly selecting successive moves until there is a winner. The trial is counted as a win or loss. The ratio: wins/trials are the AI’s metric for picking which next move to make.

A simple board display would be to have an 11 x 11 printout with B, R, or a blank (or X, O, b) in each position. A simple way to input a move would be to have the player enter an (i,j) coordinate corresponding to a currently empty hexagon and have the program check that this is legal, and if not, ask for another choice.

You may want to combine this approach with the min-max algorithm (or the more efficient alpha-beta), as described in the videos.
