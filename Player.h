#include <list>
// Base class for Human and CPU classes
#ifndef PLAYER_H
#define PLAYER_H
class Player 
{
	public:
		Player();
		virtual void move(char spaces[], bool isAdjacent[][16], Player& opponent);
		virtual void removePiece(char spaces[], Player& opponent);
		void printBoard(char spaces[]);
		bool madeMill();
		int piecesLeft;   // Each player begins with 5 and loses when they reach 2
		int piecesOnHand; // The player cannot move any pieces on the board until piecesOnHand is 0
		char piece;       // 'X' or 'O'
		int lastTurnMill; // Integer to represent the mill formation from last turn.
		                  // It is 0 if they did not create a mill in the previous turn.
		enum {possibleMills = 8};
		enum {spacesOnBoard = 16};
		bool isMillOccupied[possibleMills];     // Array to determine which possible mills the player occupies
		bool pieceOnBoard[spacesOnBoard];       // Array to determine if a space on the board contains one of the player's pieces
		bool lost;                              // true if the player has lost the game (has two pieces or cannot move)
	protected:
		int indexOfSpot(char spot);     // Returns the array index representation of a spot on the board ('1' to 1, 'A' to 10, etc.)
		char charOfIndex(int index);    // Returns the char representation of a spot on the board (1 to '1', 11 to 'B', etc.)
		bool spotIsInRange(char spot);  // Determines if a spot is in range of the board ('9' is in range, but 'G' is not)
		void piecesInMills(char spaces[], Player& opponent, std::list<char>& piecesInMill);
};
#endif