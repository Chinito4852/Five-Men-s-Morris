#include "Player.h"

#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H
// Derived class of Player for human user
class HumanPlayer: public Player
{
	public:
		virtual void move(char spaces[], bool isAdjacent[][16], Player& opponent);
		virtual void removePiece(char spaces[], Player& opponent);
	private:
		void placeNewPiece(char spaces[]);
		void moveOneOfThreePieces(char spaces[]);
		void movePieceToAdjacent(char spaces[], bool isAdjacent[][16]);
};
#endif