#include "CPU.h"
#include <iostream>
#include <list>
#include <algorithm>
#include <cctype>
#include <ctime>

void CPU::move(char spaces[], bool isAdjacent[][16], Player& opponent)
{
	printBoard(spaces);
	// The player adds a piece to board if they still have pieces to add
	if (piecesOnHand > 0)
		placeNewPiece(spaces, isAdjacent, opponent);
	else
	{
		// The player can move any piece to any empty space if they have 3 pieces left
		if (piecesLeft == 3)
			moveOneOfThreePieces(spaces, opponent, isAdjacent);
		else
		{
			// The player moves a piece to an adjacent empty space if they have more than 3 pieces left
			movePieceToAdjacent(spaces, opponent, isAdjacent);
		}
	}

	// The player removes one of the opponent's pieces if they made a mill
	if (madeMill())
	{
		std::cout << "CPU has made a mill" << std::endl;
		removePiece(spaces, isAdjacent, opponent);
	}
	return;
}

void CPU::removePiece(char spaces[], bool isAdjacent[][16], Player& opponent)
{
	char pieceToRemove = ' ';
	// Determine which spaces have the opponent's pieces and store them in a list
	std::list<char> opponentPieces;
	for (int i = 0; i < 16; i++)
	{
		if (spaces[i] == opponent.piece)
			opponentPieces.push_back(charOfIndex(i));
	}

	// Determine which of the opponent's pieces are in a mill
	std::list<char> piecesInMill;
	piecesInMills(spaces, opponent, piecesInMill);

	
	if (opponentPieces.size() == 3) // If there are only three pieces, the game is won when any of them are removed
		pieceToRemove = opponentPieces.front();
	else if (opponentPieces.size() == piecesInMill.size()) // If all pieces are in a mill (and there are more than three)
	{
		// If all the pieces are in a mill and there are more than three, there must be 5 pieces left that form 2 mills 

		// Determine which mills are being occupied
		bool mills[8];
		for (int i = 0; i < 8; i++)
		{
			mills[i] = false;
		}
		checkOpponentMills(spaces, opponent, mills);
			
		/*
		0-------1-------2   // 012 is mill 0
		|       |       |   // 06D is mill 1
		|   3---4---5   |   // DEF is mill 2
		|   |       |   |   // 29F is mill 3
		6---7       8---9   // 345 is mill 4
		|   |       |   |   // 37A is mill 5
		|   A---B---C   |   // ABC is mill 6
		|       |       |   // 58C is mill 7
		D-------E-------F
		*/

		// Remove one of the outer pieces of the mills (if mills 0 and 1 are occupied, then
		// either the piece at 2 or D should be removed). There is no priority as to which of
		// the two pieces should be removed.

		if (mills[0] && mills[1])
			pieceToRemove = '2';
		else if (mills[1] && mills[2])
			pieceToRemove = '0';
		else if (mills[2] && mills[3])
			pieceToRemove = '2';
		else if (mills[0] && mills[3])
			pieceToRemove = 'F';
		else if (mills[4] && mills[5])
			pieceToRemove = 'A';
		else if (mills[5] && mills[6])
			pieceToRemove = '3';
		else if (mills[6] && mills[7])
			pieceToRemove = '5';
		else if (mills[4] && mills[7])
			pieceToRemove = 'C';
	}
	else // If not all of the pieces are in a mill (and there are more than three)
	{
		if (piecesInMill.size() == 0) // If there are no pieces that are in mills (and there are at least 4)
		{
			bool mills[8];
			for (int i = 0; i < 8; i++)
			{
				mills[i] = false;
			}
			checkOpponentMills(spaces, opponent, mills);
			// Check for any parial mills (2 pieces in a row where a mill can be formed)
			pieceToRemove = determinePieceByPartialMills(spaces, opponent, mills); // Returns ' ' if there are no partial mills occupied
			if (pieceToRemove == ' ')
				pieceToRemove = opponentPieces.front();
		}
		else // If there is a mill (there can only be one mill at most)
		{
			// Make a list of all pieces that are not in mills (pieces that can be removed)
			std::list<char>removeablePieces(opponentPieces.size() + piecesInMill.size());
			std::list<char>::iterator listIterator;
			listIterator = set_difference(opponentPieces.begin(), opponentPieces.end(), piecesInMill.begin(), piecesInMill.end(), removeablePieces.begin());
			removeablePieces.resize(opponentPieces.size() - piecesInMill.size());
			
			if (removeablePieces.size() == 1)
				pieceToRemove = removeablePieces.front();
			else // If there is a mill and more than one removeable piece, then there can only be two pieces
			{
				/*
				0-------1-------2   // 012 is mill 0
				|       |       |   // 06D is mill 1
				|   3---4---5   |   // DEF is mill 2
				|   |       |   |   // 29F is mill 3
				6---7       8---9   // 345 is mill 4
				|   |       |   |   // 37A is mill 5
				|   A---B---C   |   // ABC is mill 6
				|       |       |   // 58C is mill 7
				D-------E-------F
				*/
				// Determine which mill is being occupied
				bool mills[8];
				for (int i = 0; i < 8; i++)
				{
					mills[i] = false;
				}
				checkOpponentMills(spaces, opponent, mills);

				// Check if the opponent can create a second mill in the next turn. This is possible if a partial
				// mill is formed from a piece included in the opponent's mill and one stray piece. (A mill at 012, 
				// and a piece at 9 or F)
				pieceToRemove = determinePieceByPartialMills(spaces, opponent, mills);
				if (pieceToRemove == ' ')
					pieceToRemove = removeablePieces.front();
			}
		}
	}

	// Remove the opponent's piece
	spaces[indexOfSpot(pieceToRemove)] = 'E';
	opponent.pieceOnBoard[indexOfSpot(pieceToRemove)] = false;
	opponent.piecesLeft--;
	if (opponent.piecesLeft == 2)
		opponent.lost = true;
	printBoard(spaces);
	std::cout << "The CPU has removed your piece at " << pieceToRemove << std::endl;
	system("pause");
	return;
}

void CPU::placeNewPiece(char spaces[], bool isAdjacent[][16], Player& opponent)
{
	char spot = ' ';
	if (piecesOnHand == 5)
	{
		// Place a piece in a corner
		if (spaces[0] == 'E')
			spot = '0';
		else // If 0 is taken, then F is open
			spot = 'F';
	}
	else if (piecesOnHand == 4)
	{
		// By now, the opponent has put down one or two pieces
		if (opponent.piecesOnHand == 4) // If the opponent has put down one piece
		{
			// Place a new piece adjacent to the first
			// For every space on the board
			for (int i = 0; i < 16; i++)
			{
				// If the space contains one of the AI's pieces (space i)
				if (spaces[i] == piece)
				{
					// Search every space on the board
					for (int j = 0; j < 16; j++)
					{
						// If space j is empty and adjacent to space i
						if (isAdjacent[i][j] && spaces[j] == 'E')
						{
							// Space j is the space to place a new piece on
							spot = charOfIndex(j);
						}
					}
				}
			}

		}
		else // If the opponent has put down two pieces
		{
			// Block any partial mill
			checkPartialMills(spaces, opponent, false); // Examine opponent's mills
			for (int i = 0; i < 8; i++)
			{
				if (partialMills[i] == 1)
					spot = findEmptySpotInPartialMill(spaces, i);
			}
			// Choose any random spot if no mill could be blocked
			if (spot == ' ')
			{
				for (int i = 0; i < 16; i++)
				{
					if (spaces[i] == 'E')
						spot = charOfIndex(i);
				}
			}
		}
	}
	else if (piecesOnHand < 4) // At least 2 pieces have been placed
	{
		// Check for any partial mills that can be completed
		bool canCompleteMill = false;
		checkPartialMills(spaces, opponent, true);
		for (int i = 0; i < 8; i++)
		{
			if (partialMills[i] == 1) // if partialMills[i] == 1, it can be filled
			{
				canCompleteMill = true;
				spot = findEmptySpotInPartialMill(spaces, i);
				break;
			}
		}

		if (!canCompleteMill) // If a mill cannot be formed in this turn
		{
			// Block any partial mill
			checkPartialMills(spaces, opponent, false); // Examine opponent's mills
			for (int i = 0; i < 8; i++)
			{
				if (partialMills[i] == 1)
				{
					spot = findEmptySpotInPartialMill(spaces, i);
					break;
				}
			}
			// Choose any random spot if no mill could be blocked
			if (spot == ' ')
			{
				for (int i = 0; i < 16; i++)
				{
					if (spaces[i] == 'E')
						spot = charOfIndex(i);
				}
			}
		}
	}
	std::cout << "The AI places a piece at " << spot << std::endl;
	spaces[indexOfSpot(spot)] = piece;
	pieceOnBoard[indexOfSpot(spot)] = true;
	piecesOnHand--;
	system("pause");
	return;
}

void CPU::moveOneOfThreePieces(char spaces[], Player& opponent, bool isAdjacent[][16])
{
	char pieceToMove = ' ';
	char spotToMove = ' ';
	// Create a list of pieces that can be moved and a list of spaces that
	// can be moved to.
	std::list<char> moveablePieces;
	std::list<char> availableSpaces; // Spaces that pieces can be moved to

	for (int i = 0; i < 16; i++)
	{
		if (spaces[i] == piece)
			moveablePieces.push_back(charOfIndex(i));
		else if (spaces[i] == 'E')
			availableSpaces.push_back(charOfIndex(i));
	}

	// Check if any mills can be completed
	std::list<char>::iterator listIterator;
	char spotToFill = ' '; // Represents a spot that can complete a mill
	checkPartialMills(spaces, opponent, true);

	for (int i = 0; i < 8; i++)
	{
		if (partialMills[i] == 1) // If the partial mill can be completed
		{
			// The empty spot in the partial mill is the spot to move to
			spotToMove = findEmptySpotInPartialMill(spaces, i);
			for (listIterator = moveablePieces.begin(); listIterator != moveablePieces.end(); listIterator++)
			{
				// The piece that is not in the partial mill is the piece to move
				if (!pieceIsInPartialMill(*listIterator, i))
				{
					pieceToMove = *listIterator;
					break;
				}
			}
			if (pieceToMove != ' ')
				break;
		}
	}

	if (spotToMove == ' ')
	{
		// Check if any opponent mills can be blocked
		checkPartialMills(spaces, opponent, false);

		// Choose a spot to move to
		for (int i = 0; i < 8; i++)
		{
			if (partialMills[i] == 1)
			{
				spotToMove = findEmptySpotInPartialMill(spaces, i);
				break;
			}
		}

		if (spotToMove != ' ')
		{
			// Choose a piece to move, which is ideally not in a partial mill
			checkPartialMills(spaces, opponent, true);

			// Choose a piece to move
			for (int i = 0; i < 8; i++)
			{
				if (partialMills[i] == 1)
				{
					for (listIterator = moveablePieces.begin(); listIterator != moveablePieces.end(); listIterator++)
					{
						if (!pieceIsInPartialMill(*listIterator, i))
						{
							pieceToMove = *listIterator;
						}
					}
				}
			}

			if (pieceToMove == ' ') // Choose any piece to move
			{
				pieceToMove = moveablePieces.front();
			}
		}
	}

	if (spotToMove == ' ')
	{
		// Choose any random piece to move
		srand((int)time(0)); // Seed the random number generator
		int random = (rand() % moveablePieces.size()) - 1;
		listIterator = moveablePieces.begin();
		for (int i = 0; i < random; i++)
		{
			listIterator++;
		}
		pieceToMove = *listIterator;
		random = (rand() % availableSpaces.size()) - 1;
		listIterator = availableSpaces.begin();
		for (int i = 0; i < random; i++)
		{
			listIterator++;
		}
		spotToMove = *listIterator;
	}

	// Update the board
	std::cout << "The AI moves their piece from " << pieceToMove << " to " << spotToMove << std::endl;
	spaces[indexOfSpot(pieceToMove)] = 'E';
	spaces[indexOfSpot(spotToMove)] = piece;
	pieceOnBoard[indexOfSpot(pieceToMove)] = false;
	pieceOnBoard[indexOfSpot(spotToMove)] = true;
	system("pause");
	return;
}

void CPU::movePieceToAdjacent(char spaces[], Player& opponent, bool isAdjacent[][16])
{
	char pieceToMove = ' ';
	char spotToMove = ' ';
	// Create a list of pieces that can be moved and a list of spaces that
	// can be moved to.
	std::list<char> moveablePieces;  
	std::list<char> availableSpaces;

	// A piece is moveable if it has at least one adjacent empty spot
	for (int i = 0; i < 16; i++)
	{
		if (spaces[i] == piece) // Space i contains one of the AI's pieces
		{
			for (int j = 0; j < 16; j++)
			{
				if (isAdjacent[i][j] && (spaces[j] == 'E')) // Space j is empty and adjacent to i
				{
					moveablePieces.push_back(charOfIndex(i));  // Space i contains a moveable piece
					availableSpaces.push_back(charOfIndex(j)); // Space j is an available space
				}
			}
		}
	}
	// Remove duplicate entries
	moveablePieces.unique();
	availableSpaces.unique();

	if (moveablePieces.size() == 0) // The CPU has lost if no pieces can be moved
	{
		lost = true;
		return;
	}
	// Create an iterator to traverse through lists
	std::list<char>::iterator listIterator;

	// Complete any mill if possible
	char spotToFill = ' ';
	int millNumber = -1;
	checkPartialMills(spaces, opponent, true);
	for (int i = 0; i < 8; i++)
	{
		if (partialMills[i] == 1)
		{
			spotToFill = findEmptySpotInPartialMill(spaces, i);
			millNumber = i;
		}
	}

	if (spotToFill != ' ') // If there is a spot to be filled
	{
		// Check if the spot can be filled during this turned
		listIterator = find(availableSpaces.begin(), availableSpaces.end(), spotToFill);
		if (listIterator != availableSpaces.end()) // If the spot can be filled
		{
			for (listIterator = moveablePieces.begin(); listIterator != moveablePieces.end(); listIterator++)
			{
				if (isAdjacent[indexOfSpot(*listIterator)][indexOfSpot(spotToFill)] && !pieceIsInPartialMill(*listIterator, millNumber))
				{
					pieceToMove = *listIterator;
					spotToMove = spotToFill;
				}
			}
		}
	}
	if (spotToMove == ' ') // If spotToMove is still ' ', then no mill could be completed
	{
		// Block any opponent mill if possible
		char spotToBlock = ' ';
		checkPartialMills(spaces, opponent, false);
		for (int i = 0; i < 8; i++)
		{
			if (partialMills[i] == 1)
				spotToBlock = findEmptySpotInPartialMill(spaces, i);
		}

		if (spotToBlock != ' ') // If there is a spot that can be blocked
		{
			// Check if the spot can be blocked during this turn
			listIterator = find(availableSpaces.begin(), availableSpaces.end(), spotToBlock);
			if (listIterator != availableSpaces.end()) // If the spot can be blocked
			{
				for (listIterator = availableSpaces.begin(); listIterator != availableSpaces.end(); listIterator++)
				{
					if (isAdjacent[indexOfSpot(*listIterator)][indexOfSpot(spotToBlock)])
					{
						pieceToMove = *listIterator;
						spotToMove = spotToBlock;
					}
				}
			}
		}
	}

	if (spotToMove == ' ') // If spotToMove is still ' ', then choose any piece to move
	{
		srand((int)time(0)); // Seed the random number generator
		int random = (rand() % moveablePieces.size()) - 1;
		listIterator = moveablePieces.begin();
		for (int i = 0; i < random; i++)
		{
			listIterator++;
		}
		pieceToMove = *listIterator;
		for (listIterator = availableSpaces.begin(); listIterator != availableSpaces.end(); ++listIterator)
		{
			if (isAdjacent[indexOfSpot(pieceToMove)][indexOfSpot(*listIterator)])
			{
				spotToMove = *listIterator;
			}
		}
	}

	// Update the board
	std::cout << "The AI moves their piece from " << pieceToMove << " to " << spotToMove << std::endl;
	spaces[indexOfSpot(pieceToMove)] = 'E';
	spaces[indexOfSpot(spotToMove)] = piece;
	pieceOnBoard[indexOfSpot(pieceToMove)] = false;
	pieceOnBoard[indexOfSpot(spotToMove)] = true;
	system("pause");
	return;
}

void CPU::checkOpponentMills(char spaces[], Player& opponent, bool mills[])
{
	/*
	0-------1-------2   // 012 is mill 0      
	|       |       |   // 06D is mill 1
	|   3---4---5   |   // DEF is mill 2
	|   |       |   |   // 29F is mill 3
	6---7       8---9   // 345 is mill 4
	|   |       |   |   // 37A is mill 5
	|   A---B---C   |   // ABC is mill 6
	|       |       |   // 58C is mill 7
	D-------E-------F
	*/

	// The middle spot of a mill does not need to be checked because this function 
	// assumes that all of the opponent's pieces are in a mill

	char piece = opponent.piece;
	if (spaces[0] == piece && spaces[2] == piece)
		mills[0] = true;
	if (spaces[0] == piece && spaces[13] == piece)
		mills[1] = true;
	if (spaces[13] == piece && spaces[15] == piece)
		mills[2] = true;
	if (spaces[2] == piece && spaces[15] == piece)
		mills[3] = true;
	if (spaces[3] == piece && spaces[5] == piece)
		mills[4] = true;
	if (spaces[3] == piece && spaces[10] == piece)
		mills[5] = true;
	if (spaces[10] == piece && spaces[12] == piece)
		mills[6] = true;
	if (spaces[5] == piece && spaces[12] == piece)
		mills[7] = true;

	return;
}

void CPU::checkPartialMills(char spaces[], Player& opponent, bool forSelf)
{
	/*
	0-------1-------2   // 012 is mill 0
	|       |       |   // 06D is mill 1
	|   3---4---5   |   // DEF is mill 2
	|   |       |   |   // 29F is mill 3
	6---7       8---9   // 345 is mill 4
	|   |       |   |   // 37A is mill 5
	|   A---B---C   |   // ABC is mill 6
	|       |       |   // 58C is mill 7
	D-------E-------F
	*/	
	// Reset the status of each partial mill
	for (int i = 0; i < 8; i++)
		partialMills[i] = 0;

	char piece1 = ' ';
	char piece2 = ' ';

	if (forSelf) // If the AI's partial mills are being checked
	{
		piece1 = piece;
		piece2 = opponent.piece;
	}
	else // If the human player's partial mills are being checked
	{
		piece1 = opponent.piece;
		piece2 = piece;
	}
	
	// Check the status of mill 0
	if ((spaces[0] == 'E' && spaces[1] == piece1 && spaces[2] == piece1)
		|| (spaces[0] == piece1 && spaces[1] == 'E' && spaces[2] == piece)
		|| (spaces[0] == piece1 && spaces[1] == piece1 && spaces[2] == 'E'))
		partialMills[0] = 1;
	else if ((spaces[0] == piece2 && spaces[1] == piece1 && spaces[2] == piece1)
		|| (spaces[0] == piece1 && spaces[1] == piece2 && spaces[2] == piece1)
		|| (spaces[0] == piece1 && spaces[1] == piece1 && spaces[2] == piece2))
		partialMills[0] = -1;

	// Check the status of mill 1
	if ((spaces[0] == 'E' && spaces[6] == piece1 && spaces[13] == piece1)
		|| (spaces[0] == piece1 && spaces[6] == 'E' && spaces[13] == piece1)
		|| (spaces[0] == piece1 && spaces[6] == piece1 && spaces[13] == 'E'))
		partialMills[1] = 1;
	else if ((spaces[0] == piece2 && spaces[6] == piece1 && spaces[13] == piece1)
		|| (spaces[0] == piece1 && spaces[6] == piece2 && spaces[13] == piece1)
		|| (spaces[0] == piece1 && spaces[6] == piece1 && spaces[13] == piece2))
		partialMills[1] = -1;
	
	// Check the status of mill 2
	if ((spaces[13] == 'E' && spaces[14] == piece1 && spaces[15] == piece1)
		|| (spaces[13] == piece1 && spaces[14] == 'E' && spaces[15] == piece1)
		|| (spaces[13] == piece1 && spaces[14] == piece1 && spaces[15] == 'E'))
		partialMills[2] = 1;
	else if ((spaces[13] == piece2 && spaces[14] == piece1 && spaces[15] == piece1)
		|| (spaces[13] == piece1 && spaces[14] == piece2 && spaces[15] == piece1)
		|| (spaces[13] == piece1 && spaces[14] == piece1 && spaces[15] == piece2))
		partialMills[2] = -1;
	
	// Check the status of mill 3
	if ((spaces[2] == 'E' && spaces[9] == piece1 && spaces[15] == piece1)
		|| (spaces[2] == piece1 && spaces[9] == 'E' && spaces[15] == piece1)
		|| (spaces[2] == piece1 && spaces[9] == piece1 && spaces[15] == 'E'))
		partialMills[3] = 1;
	else if ((spaces[2] == piece2 && spaces[9] == piece1 && spaces[15] == piece1)
		|| (spaces[2] == piece1 && spaces[9] == piece2 && spaces[15] == piece1)
		|| (spaces[2] == piece1 && spaces[9] == piece1 && spaces[15] == piece2))
		partialMills[3] = -1;

	// Check the status of mill 4
	if ((spaces[3] == 'E' && spaces[4] == piece1 && spaces[5] == piece1)
		|| (spaces[3] == piece1 && spaces[4] == 'E' && spaces[5] == piece1)
		|| (spaces[3] == piece1 && spaces[4] == piece1 && spaces[5] == 'E'))
		partialMills[4] = 1;
	else if ((spaces[3] == piece2 && spaces[4] == piece1 && spaces[5] == piece1)
		|| (spaces[3] == piece1 && spaces[4] == piece2 && spaces[5] == piece1)
		|| (spaces[3] == piece1 && spaces[4] == piece1 && spaces[5] == piece2))
		partialMills[4] = -1;

	// Check the status of mill 5
	if ((spaces[3] == 'E' && spaces[7] == piece1 && spaces[10] == piece1)
		|| (spaces[3] == piece1 && spaces[7] == 'E' && spaces[10] == piece1)
		|| (spaces[3] == piece1 && spaces[7] == piece1 && spaces[10] == 'E'))
		partialMills[5] = 1;
	else if ((spaces[3] == piece2 && spaces[7] == piece1 && spaces[10] == piece1)
		|| (spaces[3] == piece1 && spaces[7] == piece2 && spaces[10] == piece1)
		|| (spaces[3] == piece1 && spaces[7] == piece1 && spaces[10] == piece2))
		partialMills[5] = -1;

	// Check the status of mill 6
	if ((spaces[10] == 'E' && spaces[11] == piece1 && spaces[12] == piece1)
		|| (spaces[10] == piece1 && spaces[11] == 'E' && spaces[12] == piece1)
		|| (spaces[10] == piece1 && spaces[11] == piece1 && spaces[12] == 'E'))
		partialMills[6] = 1;
	else if ((spaces[10] == piece2 && spaces[11] == piece1 && spaces[12] == piece1)
		|| (spaces[10] == piece1 && spaces[11] == piece2 && spaces[12] == piece1)
		|| (spaces[10] == piece1 && spaces[11] == piece1 && spaces[12] == piece2))
		partialMills[6] = -1;

	// Check the status of mill 7
	if ((spaces[5] == 'E' && spaces[8] == piece1 && spaces[12] == piece1)
		|| (spaces[5] == piece1 && spaces[8] == 'E' && spaces[12] == piece1)
		|| (spaces[5] == piece1 && spaces[8] == piece1 && spaces[12] == 'E'))
		partialMills[7] = 1;
	else if ((spaces[5] == piece2 && spaces[8] == piece1 && spaces[12] == piece1)
		|| (spaces[5] == piece1 && spaces[8] == piece2 && spaces[12] == piece1)
		|| (spaces[5] == piece1 && spaces[8] == piece1 && spaces[12] == piece2))
		partialMills[7] = -1;

	return;
}

bool CPU::pieceIsInPartialMill(char pieceToCheck, int millNumber)
{
	bool inPartialMill = false;
	if (millNumber == 0)
	{
		if (pieceToCheck == '0' || pieceToCheck == '1' || pieceToCheck == '2')
			inPartialMill = true;
	}
	else if (millNumber == 1)
	{
		if (pieceToCheck == '0' || pieceToCheck == '6' || pieceToCheck == 'D')
			inPartialMill = true;
	}
	else if (millNumber == 2)
	{
		if (pieceToCheck == 'D' || pieceToCheck == 'E' || pieceToCheck == 'F')
			inPartialMill = true;
	}
	else if (millNumber == 3)
	{
		if (pieceToCheck == '2' || pieceToCheck == '9' || pieceToCheck == 'F')
			inPartialMill = true;
	}
	else if (millNumber == 4)
	{
		if (pieceToCheck == '3' || pieceToCheck == '4' || pieceToCheck == '5')
			inPartialMill = true;
	}
	else if (millNumber == 5)
	{
		if (pieceToCheck == '3' || pieceToCheck == '7' || pieceToCheck == 'A')
			inPartialMill = true;
	}
	else if (millNumber == 6)
	{
		if (pieceToCheck == 'A' || pieceToCheck == 'B' || pieceToCheck == 'C')
			inPartialMill = true;
	}
	else if (millNumber == 7)
	{
		if (pieceToCheck == '5' || pieceToCheck == '8' || pieceToCheck == 'C')
			inPartialMill = true;
	}
	return inPartialMill;
}

char CPU::findEmptySpotInPartialMill(char spaces[], int millNumber)
{
	// millNumber must be a number from 0-7
	if (millNumber == 0)
	{
		if (spaces[0] == 'E')
			return '0';
		else if (spaces[1] == 'E')
			return '1';
		else if (spaces[2] == 'E')
			return '2';
	}
	else if (millNumber == 1)
	{
		if (spaces[0] == 'E')
			return '0';
		else if (spaces[6] == 'E')
			return '6';
		else if (spaces[13] == 'E')
			return 'D';
	}
	else if (millNumber == 2)
	{
		if (spaces[13] == 'E')
			return 'D';
		else if (spaces[14] == 'E')
			return 'E';
		else if (spaces[15] == 'E')
			return 'F';
	}
	else if (millNumber == 3)
	{
		if (spaces[2] == 'E')
			return '2';
		else if (spaces[9] == 'E')
			return '9';
		else if (spaces[15] == 'E')
			return 'F';
	}
	else if (millNumber == 4)
	{
		if (spaces[3] == 'E')
			return '3';
		else if (spaces[4] == 'E')
			return '4';
		else if (spaces[5] == 'E')
			return '5';
	}
	else if (millNumber == 5)
	{
		if (spaces[3] == 'E')
			return '3';
		else if (spaces[7] == 'E')
			return '7';
		else if (spaces[10] == 'E')
			return 'A';
	}
	else if (millNumber == 6)
	{
		if (spaces[10] == 'E')
			return 'A';
		else if (spaces[11] == 'E')
			return 'B';
		else if (spaces[12] == 'E')
			return 'C';
	}
	else if (millNumber == 7)
	{
		if (spaces[5] == 'E')
			return '5';
		else if (spaces[8] == 'E')
			return '8';
		else if (spaces[12] == 'E')
			return 'C';
	}
}

char CPU::determinePieceByPartialMills(char spaces[], Player& opponent, bool mills[])
{
	char pieceToRemove = ' ';
	if (mills[0])
	{
		if (spaces[6] == opponent.piece)
			pieceToRemove = '6';
		else if (spaces[9] == opponent.piece)
			pieceToRemove = '9';
		else
		{
			// Check if any of the CPU's partial mills are being obstructed 
			checkPartialMills(spaces, opponent, true);
			if (partialMills[2] == -1)
			{
				if (spaces[13] == opponent.piece)
					pieceToRemove = 'D';
				else if (spaces[14] == opponent.piece)
					pieceToRemove = 'E';
				else if (spaces[15] == opponent.piece)
					pieceToRemove = 'F';
			}
			else if (partialMills[5] == -1)
			{
				if (spaces[3] == opponent.piece)
					pieceToRemove = '3';
				else if (spaces[7] == opponent.piece)
					pieceToRemove = '7';
				else if (spaces[10] == opponent.piece)
					pieceToRemove = 'A';
			}
			else if (partialMills[6] == -1)
			{
				if (spaces[10] == opponent.piece)
					pieceToRemove = 'A';
				else if (spaces[11] == opponent.piece)
					pieceToRemove = 'B';
				else if (spaces[12] == opponent.piece)
					pieceToRemove = 'C';
			}
			else if (partialMills[7] == -1)
			{
				if (spaces[5] == opponent.piece)
					pieceToRemove = '5';
				else if (spaces[8] == opponent.piece)
					pieceToRemove = '8';
				else if (spaces[12] == opponent.piece)
					pieceToRemove = 'C';
			}
		}
	}
	else if (mills[1])
	{
		if (spaces[1] == opponent.piece)
			pieceToRemove = '1';
		else if (spaces[14] == opponent.piece)
			pieceToRemove = 'E';
		else
		{
			// Check if any of the CPU's partial mills are being obstructed 
			checkPartialMills(spaces, opponent, true);
			if (partialMills[3] == -1)
			{
				if (spaces[2] == opponent.piece)
					pieceToRemove = '2';
				else if (spaces[9] == opponent.piece)
					pieceToRemove = '9';
				else if (spaces[15] == opponent.piece)
					pieceToRemove = 'F';
			}
			else if (partialMills[4] == -1)
			{
				if (spaces[3] == opponent.piece)
					pieceToRemove = '3';
				else if (spaces[4] == opponent.piece)
					pieceToRemove = '4';
				else if (spaces[5] == opponent.piece)
					pieceToRemove = '5';
			}
			else if (partialMills[6] == -1)
			{
				if (spaces[10] == opponent.piece)
					pieceToRemove = 'A';
				else if (spaces[11] == opponent.piece)
					pieceToRemove = 'B';
				else if (spaces[12] == opponent.piece)
					pieceToRemove = 'C';
			}
			else if (partialMills[7] == -1)
			{
				if (spaces[5] == opponent.piece)
					pieceToRemove = '5';
				else if (spaces[8] == opponent.piece)
					pieceToRemove = '8';
				else if (spaces[12] == opponent.piece)
					pieceToRemove = 'C';
			}
		}
	}
	else if (mills[2])
	{
		if (spaces[6] == opponent.piece)
			pieceToRemove = '6';
		else if (spaces[9] == opponent.piece)
			pieceToRemove = '9';
		else
		{
			// Check if any of the CPU's partial mills are being obstructed 
			checkPartialMills(spaces, opponent, true);
			if (partialMills[0] == -1)
			{
				if (spaces[0] == opponent.piece)
					pieceToRemove = '0';
				else if (spaces[1] == opponent.piece)
					pieceToRemove = '1';
				else if (spaces[2] == opponent.piece)
					pieceToRemove = '2';
			}
			else if (partialMills[4] == -1)
			{
				if (spaces[3] == opponent.piece)
					pieceToRemove = '3';
				else if (spaces[4] == opponent.piece)
					pieceToRemove = '4';
				else if (spaces[5] == opponent.piece)
					pieceToRemove = '5';
			}
			else if (partialMills[5] == -1)
			{
				if (spaces[3] == opponent.piece)
					pieceToRemove = '3';
				else if (spaces[7] == opponent.piece)
					pieceToRemove = '7';
				else if (spaces[10] == opponent.piece)
					pieceToRemove = 'A';
			}
			else if (partialMills[7] == -1)
			{
				if (spaces[5] == opponent.piece)
					pieceToRemove = '5';
				else if (spaces[8] == opponent.piece)
					pieceToRemove = '8';
				else if (spaces[12] == opponent.piece)
					pieceToRemove = 'C';
			}
		}
	}
	else if (mills[3])
	{
		if (spaces[1] == opponent.piece)
			pieceToRemove = '1';
		else if (spaces[14] == opponent.piece)
			pieceToRemove = 'E';
		else
		{
			// Check if any of the CPU's partial mills are being obstructed 
			checkPartialMills(spaces, opponent, true);
			if (partialMills[1] == -1)
			{
				if (spaces[0] == opponent.piece)
					pieceToRemove = '0';
				else if (spaces[6] == opponent.piece)
					pieceToRemove = '6';
				else if (spaces[13] == opponent.piece)
					pieceToRemove = 'D';
			}
			else if (partialMills[4] == -1)
			{
				if (spaces[3] == opponent.piece)
					pieceToRemove = '3';
				else if (spaces[4] == opponent.piece)
					pieceToRemove = '4';
				else if (spaces[5] == opponent.piece)
					pieceToRemove = '5';
			}
			else if (partialMills[5] == -1)
			{
				if (spaces[3] == opponent.piece)
					pieceToRemove = '3';
				else if (spaces[7] == opponent.piece)
					pieceToRemove = '7';
				else if (spaces[10] == opponent.piece)
					pieceToRemove = 'A';
			}
			else if (partialMills[6] == -1)
			{
				if (spaces[10] == opponent.piece)
					pieceToRemove = 'A';
				else if (spaces[11] == opponent.piece)
					pieceToRemove = 'B';
				else if (spaces[12] == opponent.piece)
					pieceToRemove = 'C';
			}
		}
	}
	else if (mills[4])
	{
		if (spaces[7] == opponent.piece)
			pieceToRemove = '7';
		else if (spaces[8] == opponent.piece)
			pieceToRemove = '8';
		else
		{
			// Check if any of the CPU's partial mills are being obstructed 
			checkPartialMills(spaces, opponent, true);
			if (partialMills[1] == -1)
			{
				if (spaces[0] == opponent.piece)
					pieceToRemove = '0';
				else if (spaces[6] == opponent.piece)
					pieceToRemove = '6';
				else if (spaces[13] == opponent.piece)
					pieceToRemove = 'D';
			}
			else if (partialMills[2] == -1)
			{
				if (spaces[13] == opponent.piece)
					pieceToRemove = 'D';
				else if (spaces[14] == opponent.piece)
					pieceToRemove = 'E';
				else if (spaces[15] == opponent.piece)
					pieceToRemove = 'F';
			}
			else if (partialMills[3] == -1)
			{
				if (spaces[2] == opponent.piece)
					pieceToRemove = '2';
				else if (spaces[9] == opponent.piece)
					pieceToRemove = '9';
				else if (spaces[15] == opponent.piece)
					pieceToRemove = 'F';
			}
			else if (partialMills[6] == -1)
			{
				if (spaces[10] == opponent.piece)
					pieceToRemove = 'A';
				else if (spaces[11] == opponent.piece)
					pieceToRemove = 'B';
				else if (spaces[12] == opponent.piece)
					pieceToRemove = 'C';
			}
		}
	}
	else if (mills[5])
	{
		if (spaces[4] == opponent.piece)
			pieceToRemove = '4';
		else if (spaces[11] == opponent.piece)
			pieceToRemove = 'B';
		else
		{
			// Check if any of the CPU's partial mills are being obstructed 
			checkPartialMills(spaces, opponent, true);
			if (partialMills[0] == -1)
			{
				if (spaces[0] == opponent.piece)
					pieceToRemove = '0';
				else if (spaces[1] == opponent.piece)
					pieceToRemove = '1';
				else if (spaces[2] == opponent.piece)
					pieceToRemove = '2';
			}
			else if (partialMills[2] == -1)
			{
				if (spaces[13] == opponent.piece)
					pieceToRemove = 'D';
				else if (spaces[14] == opponent.piece)
					pieceToRemove = 'E';
				else if (spaces[14] == opponent.piece)
					pieceToRemove = 'F';
			}
			else if (partialMills[3] == -1)
			{
				if (spaces[0] == opponent.piece)
					pieceToRemove = '0';
				else if (spaces[9] == opponent.piece)
					pieceToRemove = '9';
				else if (spaces[15] == opponent.piece)
					pieceToRemove = 'F';
			}
			else if (partialMills[7] == -1)
			{
				if (spaces[5] == opponent.piece)
					pieceToRemove = '5';
				else if (spaces[8] == opponent.piece)
					pieceToRemove = '8';
				else if (spaces[12] == opponent.piece)
					pieceToRemove = 'C';
			}
		}
	}
	else if (mills[6])
	{
		if (spaces[7] == opponent.piece)
			pieceToRemove = '7';
		else if (spaces[8] == opponent.piece)
			pieceToRemove = '8';
		else
		{
			// Check if any of the CPU's partial mills are being obstructed 
			checkPartialMills(spaces, opponent, true);
			if (partialMills[0] == -1)
			{
				if (spaces[0] == opponent.piece)
					pieceToRemove = '0';
				else if (spaces[1] == opponent.piece)
					pieceToRemove = '1';
				else if (spaces[2] == opponent.piece)
					pieceToRemove = '2';
			}
			else if (partialMills[1] == -1)
			{
				if (spaces[0] == opponent.piece)
					pieceToRemove = '0';
				else if (spaces[6] == opponent.piece)
					pieceToRemove = '6';
				else if (spaces[13] == opponent.piece)
					pieceToRemove = 'D';
			}
			else if (partialMills[3] == -1)
			{
				if (spaces[2] == opponent.piece)
					pieceToRemove = '2';
				else if (spaces[9] == opponent.piece)
					pieceToRemove = '9';
				else if (spaces[15] == opponent.piece)
					pieceToRemove = 'F';
			}
			else if (partialMills[4] == -1)
			{
				if (spaces[3] == opponent.piece)
					pieceToRemove = '3';
				else if (spaces[4] == opponent.piece)
					pieceToRemove = '4';
				else if (spaces[5] == opponent.piece)
					pieceToRemove = '5';
			}
		}
	}
	else if (mills[7])
	{
		if (spaces[4] == opponent.piece)
			pieceToRemove = '4';
		else if (spaces[11] == opponent.piece)
			pieceToRemove = 'B';
		else
		{
			// Check if any of the CPU's partial mills are being obstructed 
			checkPartialMills(spaces, opponent, true);
			if (partialMills[0] == -1)
			{
				if (spaces[0] == opponent.piece)
					pieceToRemove = '0';
				else if (spaces[1] == opponent.piece)
					pieceToRemove = '1';
				else if (spaces[2] == opponent.piece)
					pieceToRemove = '2';
			}
			else if (partialMills[1] == -1)
			{
				if (spaces[0] == opponent.piece)
					pieceToRemove = '0';
				else if (spaces[6] == opponent.piece)
					pieceToRemove = '6';
				else if (spaces[13] == opponent.piece)
					pieceToRemove = 'D';
			}
			else if (partialMills[2] == -1)
			{
				if (spaces[13] == opponent.piece)
					pieceToRemove = 'D';
				else if (spaces[14] == opponent.piece)
					pieceToRemove = 'E';
				else if (spaces[15] == opponent.piece)
					pieceToRemove = 'F';
			}
			else if (partialMills[5] == -1)
			{
				if (spaces[3] == opponent.piece)
					pieceToRemove = '3';
				else if (spaces[7] == opponent.piece)
					pieceToRemove = '7';
				else if (spaces[10] == opponent.piece)
					pieceToRemove = 'A';
			}
		}
	}
	return pieceToRemove;
}

