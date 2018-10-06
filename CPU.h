#include "Player.h"

#ifndef CPU_H
#define CPU_H
// Derived class of Player for AI
class CPU: public Player
{
	public:
		virtual void move(char spaces[], bool isAdjacent[][16], Player& opponent);
		virtual void removePiece(char spaces[], bool isAdjacent[][16], Player& opponent);
	private:
		void placeNewPiece(char spaces[], bool isAdjacent[][16], Player& opponent);
		void moveOneOfThreePieces(char spaces[], Player& opponent, bool isAdjacent[][16]);
		void movePieceToAdjacent(char spaces[], Player& opponent, bool isAdjacent[][16]);
		void checkOpponentMills(char spaces[], Player& opponent, bool mills[]); // Assumes that all opponent pieces are in a mill
		void checkPartialMills(char spaces[], Player& opponent, bool forSelf);
		// This function examines the partial mills of the CPU if forSelf is true, and examines the player's
		// partial mills if forSelf is false. The status of the partial mills is stored in the partialMills
		// array.

		bool pieceIsInPartialMill(char piece, int millNumber);

		char findEmptySpotInPartialMill(char spaces[], int millNumber);
		// This function assumes that the partial mill has a status of 1 and returns the character representing
		// the empty spot in the mill. It can be used to examine the mill of either the CPU or the player.

		char determinePieceByPartialMills(char spaces[], Player& opponent, bool mills[]);
		// A partial mill is two pieces that need a third to form a mill
		int partialMills[possibleMills]; // 0 if no partial mill, 1 if empty space, -1 if occupied by opponent
		// The array can be used to determine either the player's or CPU's mills
};
#endif 