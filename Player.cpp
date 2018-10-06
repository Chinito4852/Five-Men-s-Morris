#include <iostream>
#include "Player.h"

Player::Player()
{
	lost = false;
	piece = ' ';
	piecesLeft = 5;
	piecesOnHand = 5;
	lastTurnMill = 0;
	for (int i = 0; i < possibleMills; i++)
	{
		isMillOccupied[i] = false;
	}
	for (int i = 0; i < spacesOnBoard; i++)
	{
		pieceOnBoard[i] = false;
	}
}

void Player::move(char spaces[], bool isAdjacent[][16], Player& opponent) // unfinished
{
	/*
	If (piecesOnHand > 0)
	{
		while (the player has not placed a new piece on the board)
		{
			prompt player to place piece on board
			If (the chosen spot is occupied)
				repeat loop 
		}
		piecesOnHand--
	}
	Else
	{
		If (pieces == 3)
		{
			while (player has not chosen valid piece to move)
			{
				prompt player to choose piece to move
				If (player chooses invalid piece)
					repeat loop
			}
		}
		Else
		{
			while (player has not chosen valid piece to move)
			{
				prompt player to choose piece to move
				If (player chooses piece with no adjacent empty spots or invalid piece)
					repeat loop
			}
		}
		
		If (player moved piece from a used mill)
		{
			the mill is now available and can be valid
		}
		
		If (player created a valid mill)
		{
			while (player has not chosen a valid piece to remove)
			{
				prompt player to remove any opponent piece
				If (player does not choose opponent piece)
					repeat loop
			}
		}
	}
	*/
	return;
}

void Player::removePiece(char spaces[], Player& opponent)
{
	/*
	If (all of the opponent's pieces are in a mill)
	{
		player may remove any piece
	}
	Else
	{
		player must remove a piece not in a mill
	}
	*/
	return;
}

bool Player::madeMill() 
{
	bool hasNewMill = false; // bool to determine if the player has recently created a new mill
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
	
	Letters corrspond to their hexadecimal values in the array. There are eight possible mills.
	A player has a mill when they have a row of three pieces. A mill is valid if it was created on 
	the current turn. If a valid mill is discovered, the isMillOccupied array will be updated so that
	it cannot be considered a valid mill on any following turn until one of the pieces in that mill
	has moved.
	*/
	
	// First, check if the any of the player's pieces in a mill have moved
	if (isMillOccupied[0] && (!pieceOnBoard[0] || !pieceOnBoard[1] || !pieceOnBoard[2]))
	{
		// If the mill was occupied, but one of its pieces have been moved
		isMillOccupied[0] = false;
	}
	if (isMillOccupied[1] && (!pieceOnBoard[0] || !pieceOnBoard[6] || !pieceOnBoard[13]))
	{
		isMillOccupied[1] = false;
	}
	if (isMillOccupied[2] && (!pieceOnBoard[13] || !pieceOnBoard[14] || !pieceOnBoard[15]))
	{
		isMillOccupied[2] = false;
	}
	if (isMillOccupied[3] && (!pieceOnBoard[2] || !pieceOnBoard[9] || !pieceOnBoard[15]))
	{
		isMillOccupied[3] = false;
	}
	if (isMillOccupied[4] && (!pieceOnBoard[3] || !pieceOnBoard[4] || !pieceOnBoard[5]))
	{
		isMillOccupied[4] = false;
	}
	if (isMillOccupied[5] && (!pieceOnBoard[3] || !pieceOnBoard[7] || !pieceOnBoard[10]))
	{
		isMillOccupied[5] = false;
	}
	if (isMillOccupied[6] && (!pieceOnBoard[10] || !pieceOnBoard[11] || !pieceOnBoard[12]))
	{
		isMillOccupied[6] = false;
	}
	if (isMillOccupied[7] && (!pieceOnBoard[5] || !pieceOnBoard[8] || !pieceOnBoard[12]))
	{
		isMillOccupied[7] = false;
	}

	// Check if a new mill was created
	if (pieceOnBoard[0] && pieceOnBoard[1] && pieceOnBoard[2] && !isMillOccupied[0])
	{
		// If three pieces form a mill that was not occupied previously, then a new mill is made
		isMillOccupied[0] = true;
		hasNewMill = true;
	}
	else if (pieceOnBoard[0] && pieceOnBoard[6] && pieceOnBoard[13] && !isMillOccupied[1])
	{
		isMillOccupied[1] = true;
		hasNewMill = true;
	}
	else if (pieceOnBoard[13] && pieceOnBoard[14] && pieceOnBoard[15] && !isMillOccupied[2])
	{
		isMillOccupied[2] = true;
		hasNewMill = true;
	}
	else if (pieceOnBoard[2] && pieceOnBoard[9] && pieceOnBoard[15] && !isMillOccupied[3])
	{
		isMillOccupied[3] = true;
		hasNewMill = true;
	}
	else if (pieceOnBoard[3] && pieceOnBoard[4] && pieceOnBoard[5] && !isMillOccupied[4])
	{
		isMillOccupied[4] = true;
		hasNewMill = true;
	}
	else if (pieceOnBoard[3] && pieceOnBoard[7] && pieceOnBoard[10] && !isMillOccupied[5])
	{
		isMillOccupied[5] = true;
		hasNewMill = true;
	}
	else if (pieceOnBoard[10] && pieceOnBoard[11] && pieceOnBoard[12] && !isMillOccupied[6])
	{
		isMillOccupied[6] = true;
		hasNewMill = true;
	}
	else if (pieceOnBoard[5] && pieceOnBoard[8] && pieceOnBoard[12] && !isMillOccupied[7])
	{
		isMillOccupied[7] = true;
		hasNewMill = true;
	}

	return hasNewMill;
	
}

int Player::indexOfSpot(char spot)
{
	if (spot == 'F')
		return 15;
	else if (spot == 'E')
		return 14;
	else if (spot == 'D')
		return 13;
	else if (spot == 'C')
		return 12;
	else if (spot == 'B')
		return 11;
	else if (spot == 'A')
		return 10;
	else if (spot == '9')
		return 9;
	else if (spot == '8')
		return 8;
	else if (spot == '7')
		return 7;
	else if (spot == '6')
		return 6;
	else if (spot == '5')
		return 5;
	else if (spot == '4')
		return 4;
	else if (spot == '3')
		return 3;
	else if (spot == '2')
		return 2;
	else if (spot == '1')
		return 1;
	else if (spot == '0')
		return 0;
}

// Return the space's char corresponding to it's index in the array of board spaces
char Player::charOfIndex(int index)
{
	if (index == 15)
		return 'F';
	else if (index == 14)
		return 'E';
	else if (index == 13)
		return 'D';
	else if (index == 12)
		return 'C';
	else if (index == 11)
		return 'B';
	else if (index == 10)
		return 'A';
	else if (index == 9)
		return '9';
	else if (index == 8)
		return '8';
	else if (index == 7)
		return '7';
	else if (index == 6)
		return '6';
	else if (index == 5)
		return '5';
	else if (index == 4)
		return '4';
	else if (index == 3)
		return '3';
	else if (index == 2)
		return '2';
	else if (index == 1)
		return '1';
	else if (index == 0)
		return '0';
}

// Check if the spot is a space on the board
bool Player::spotIsInRange(char spot)
{
	if (spot == '0' || spot == '1' || spot == '2' || spot == '3'
		|| spot == '4' || spot == '5' || spot == '6' || spot == '7'
		|| spot == '8' || spot == '9' || spot == 'A' || spot == 'B'
		|| spot == 'C' || spot == 'D' || spot == 'E' || spot == 'F')
		return true;
	else
		return false;
}

void Player::piecesInMills(char spaces[], Player& opponent, std::list<char>& piecesInMill)
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

	if (spaces[0] == opponent.piece && spaces[1] == opponent.piece && spaces[2] == opponent.piece)
	{
		piecesInMill.push_back('0');
		piecesInMill.push_back('1');
		piecesInMill.push_back('2');
	}
	if (spaces[0] == opponent.piece && spaces[6] == opponent.piece && spaces[13] == opponent.piece)
	{
		piecesInMill.push_back('0');
		piecesInMill.push_back('6');
		piecesInMill.push_back('D');
	}
	if (spaces[13] == opponent.piece && spaces[14] == opponent.piece && spaces[15] == opponent.piece)
	{
		piecesInMill.push_back('D');
		piecesInMill.push_back('E');
		piecesInMill.push_back('F');
	}
	if (spaces[2] == opponent.piece && spaces[9] == opponent.piece && spaces[15] == opponent.piece)
	{
		piecesInMill.push_back('2');
		piecesInMill.push_back('9');
		piecesInMill.push_back('F');
	}
	if (spaces[3] == opponent.piece && spaces[4] == opponent.piece && spaces[5] == opponent.piece)
	{
		piecesInMill.push_back('3');
		piecesInMill.push_back('4');
		piecesInMill.push_back('5');
	}
	if (spaces[3] == opponent.piece && spaces[7] == opponent.piece && spaces[10] == opponent.piece)
	{
		piecesInMill.push_back('3');
		piecesInMill.push_back('7');
		piecesInMill.push_back('A');
	}
	if (spaces[10] == opponent.piece && spaces[11] == opponent.piece && spaces[12] == opponent.piece)
	{
		piecesInMill.push_back('A');
		piecesInMill.push_back('B');
		piecesInMill.push_back('C');
	}
	if (spaces[5] == opponent.piece && spaces[8] == opponent.piece && spaces[12] == opponent.piece)
	{
		piecesInMill.push_back('5');
		piecesInMill.push_back('8');
		piecesInMill.push_back('C');
	}
	// Remove duplicate elements and sort in ascending order
	piecesInMill.unique();
	piecesInMill.sort();
	return;
}

void Player::printBoard(char spaces[])
{
	/*
	0-------1-------2
	|       |       |
	|   3---4---5   |
	|   |       |   |
	6---7       8---9
	|   |       |   |
	|   A---B---C   |
	|       |       |
	D-------E-------F
	*/

	// If the space is not occupied, the position character will be shown instead
	char boardSpaces[16];
	for (int i = 0; i < 16; i++)
	{
		if (spaces[i] == 'E')
			boardSpaces[i] = charOfIndex(i);
		else
			boardSpaces[i] = spaces[i];
	}

	for (int i = 0; i < 16; i++)
	{
		if (boardSpaces[i] != 'X' && boardSpaces[i] != 'O')
			boardSpaces[i] = tolower(boardSpaces[i]);
	}

	std::cout << boardSpaces[0] << "-------" << boardSpaces[1] << "-------" << boardSpaces[2] << std::endl;
	std::cout << "|       |       |" << std::endl;
	std::cout << "|   " << boardSpaces[3] << "---" << boardSpaces[4] << "---" << boardSpaces[5] << "   |" << std::endl;
	std::cout << "|   |       |   |" << std::endl;
	std::cout << boardSpaces[6] << "---" << boardSpaces[7] << "       " << boardSpaces[8] << "---" << boardSpaces[9] << std::endl;
	std::cout << "|   |       |   |" << std::endl;
	std::cout << "|   " << boardSpaces[10] << "---" << boardSpaces[11] << "---" << boardSpaces[12] << "   |" << std::endl;
	std::cout << "|       |       |" << std::endl;
	std::cout << boardSpaces[13] << "-------" << boardSpaces[14] << "-------" << boardSpaces[15] << std::endl;
	return;
}


