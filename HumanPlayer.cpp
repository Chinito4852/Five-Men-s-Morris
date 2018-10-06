#include "HumanPlayer.h"
#include <iostream>
#include <cctype>    // Manipulate chars
#include <list>      // List is used to store moveable pieces
#include <algorithm> // List search functions
#include <limits>    // numeric_limits

void HumanPlayer::move(char spaces[], bool isAdjacent[][16], Player& opponent)
{
	printBoard(spaces);

	// The player adds a piece to board if they still have pieces to add
	if (piecesOnHand > 0)
		placeNewPiece(spaces);
	else
	{
		// The player can move any piece to any empty space if they have 3 pieces left
		if (piecesLeft == 3)
			moveOneOfThreePieces(spaces);
		else
		{
			// The player moves a piece to an adjacent empty space if they have more than 3 pieces left
			movePieceToAdjacent(spaces, isAdjacent);
		}
	}

	// The player removes one of the opponent's pieces if they made a mill
	if (madeMill())
		removePiece(spaces, opponent);

	return;
}

void HumanPlayer::removePiece(char spaces[], Player& opponent)
{
	printBoard(spaces);
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

	// The opponent may only remove pieces that are in a mill if all of the opponent's pieces
	// are in a mill.
	bool choseValidPiece = false;

	// Prompt the player to choose a piece to remove
	while (!choseValidPiece)
	{
		std::cout << "Choose one of the opponent's pieces to remove (0-F): ";
		std::cin.get(pieceToRemove);
		pieceToRemove = toupper(pieceToRemove);

		// Flush cin to read only the first character typed in
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Check if the player chose a valid piece to remove
		std::list<char>::iterator listIterator;  // Iterates through the list of opponent pieces
		std::list<char>::iterator listIterator2; // Iterates through the list of ooponent pieces that are in mills
		listIterator = find(opponentPieces.begin(), opponentPieces.end(), pieceToRemove);
		listIterator2 = find(piecesInMill.begin(), piecesInMill.end(), pieceToRemove);

		if ((opponentPieces.size() != piecesInMill.size()) && (listIterator2 != piecesInMill.end()))
		{
			// If the player chose a piece that is in a mill and not all of the opponent's pieces are in a mill
			std::cout << "You cannot choose a piece that is in mill unless all" << std::endl;
			std::cout << "of the opponent's pieces are in a mill." << std::endl;
		}
		else if (listIterator == opponentPieces.end()) // If the player does not choose one of the opponent's pieces
		{
			std::cout << pieceToRemove << " does not contain a piece you can remove." << std::endl;
		}
		else
		{
			choseValidPiece = true;
		}
	}

	// Remove the opponent's piece
	spaces[indexOfSpot(pieceToRemove)] = 'E';
	opponent.piecesLeft--;
	opponent.pieceOnBoard[indexOfSpot(pieceToRemove)] = false;
	if (opponent.piecesLeft == 2)
		opponent.lost = true;

	return;
}

void HumanPlayer::placeNewPiece(char spaces[])
{
	char spot = ' '; // Variable to store the space the player wants to add a piece to
	bool placedPiece = false;
	while (!placedPiece)
	{
		// Display valid spaces
		std::cout << "Place a piece on the board (0-F): " << std::endl;
		std::cin.get(spot);
		spot = toupper(spot);
		

		// Flush cin to read only the first character typed in
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Check if the player chose an open spot
		if (!spotIsInRange(spot))
		{
			std::cout << spot << " is not a space on the board" << std::endl << std::endl;
		}
		else if (spaces[indexOfSpot(spot)] != 'E')
		{
			std::cout << spot << " is not an open space." << std::endl << std::endl;
		}
		else
		{
			std::cout << std::endl;
			placedPiece = true;
			// Update the board with the new piece
			spaces[indexOfSpot(spot)] = piece;
			pieceOnBoard[indexOfSpot(spot)] = true;
		}
	}
	piecesOnHand--;
	return;
}

void HumanPlayer::moveOneOfThreePieces(char spaces[])
{
	std::cout << "You may move any of your pieces to any empty space." << std::endl;
	// Variables to monitor player's progression
	bool movedPiece = false;
	bool choseValidPiece = false;
	bool choseValidSpot = false;
	bool cancelMove = false;

	// Variables to store player's piece to move and the spot to move to
	char pieceToMove = ' ';
	char spotToMove = ' ';

	while (!movedPiece)
	{
		// Prompt the player to choose a piece until they choose a valid piece
		while (!choseValidPiece)
		{
			// Determine which spaces contain the player's pieces
			std::list<char> moveablePieces;
			for (int i = 0; i < 16; i++)
			{
				if (spaces[i] == piece)
					moveablePieces.push_back(charOfIndex(i));
			}

			// Display which spaces have the player's pieces
			std::list<char>::iterator listIterator;
			listIterator = moveablePieces.begin();
			std::cout << "Your pieces are located at ";
			for (int i = 0; i < 3; i++)
			{
				std::cout << *listIterator;
				if (i == 0)
					std::cout << ", ";
				else if (i == 1)
					std::cout << ", and ";
				else if (i == 2)
					std::cout << "." << std::endl << std::endl;
				listIterator++;
			}

			// Prompt the player to choose a piece
			std::cout << "Choose one of your pieces to move (0-F): ";
			std::cin.get(pieceToMove);
			pieceToMove = toupper(pieceToMove);

			// Flush cin to read only the first character typed in
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			// Check if the player chose a valid piece by searching for the player's choice in the list of 
			// moveable pieces. listIterator will point to the end of the list if it does not contain the
			// piece being searched for.
			listIterator = find(moveablePieces.begin(), moveablePieces.end(), pieceToMove);

			if (listIterator == moveablePieces.end()) // If the player did not choose a valid piece
				std::cout << pieceToMove << " does not contain of your moveable pieces" << std::endl;
			else
				choseValidPiece = true;
		}
				
		// Prompt the player to place the piece at a valid spot until they choose one or cancel
		while (!(cancelMove) && !(choseValidSpot))
		{
			// Prompt the player to choose a spot to move or cancel
			std::cout << "Choose any spot to move in (0-F) or type X to choose a different piece: ";
			std::cin.get(spotToMove);
			spotToMove = toupper(spotToMove);
			
			// Flush cin to read only the first character typed in
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			
			if (spotToMove == 'X') // If the player cancels
			{
				cancelMove = true;
			}
			else if (!spotIsInRange(spotToMove)) // If the chosen spot is part of the board
			{
				std::cout << spotToMove << " is not a space on the board." << std::endl;
			}
			else if (spaces[indexOfSpot(spotToMove)] != 'E') // If the chosen spot is not empty
			{
				std::cout << "You cannot move your piece there." << std::endl;
			}
			else // The player has chosen to move a valid piece to a valid spot
			{
				choseValidSpot = true;
				// Move the pieces
				spaces[indexOfSpot(pieceToMove)] = 'E';
				spaces[indexOfSpot(spotToMove)] = piece;
				pieceOnBoard[indexOfSpot(pieceToMove)] = false;
				pieceOnBoard[indexOfSpot(spotToMove)] = true;
			}
		}
		
		// If the player has chosen a valid piece and a spot to move it to
		if (choseValidSpot && choseValidPiece && (!cancelMove))
			movedPiece = true;
		else // The loop repeats
		{
			choseValidSpot = false;
			choseValidPiece = false;
			cancelMove = false;
			char pieceToMove = ' ';
			char spotToMove = ' ';
		}
	}
	return;
}

void HumanPlayer::movePieceToAdjacent(char spaces[], bool isAdjacent[][16])
{
	// Determine which spaces contain pieces that can move and store them into a list
	// A piece can move if it has at least one empty adjacent space.
	std::list<char> moveablePieces;
	
	for (int i = 0; i < 16; i++) // For every spot i on the board
	{
		if (spaces[i] == piece) // If the space contains one of the player's pieces
		{
			for (int j = 0; j < 16; j++) // For every other possible space j
			{
				if ((isAdjacent[i][j]) && (spaces[j] == 'E')) // If i and j are adjacent and j is an empty spot
				{
					// Duplicate entries will occur if a piece has multiple empty adjacent spaces
					moveablePieces.push_back(charOfIndex(i));
				}
			}
		}
	}
	// Remove duplicate entries
	moveablePieces.unique();
	
	if (moveablePieces.empty()) // If the player cannot move any pieces, then they lose
	{
		std::cout << "You cannot move any of your pieces." << std::endl;
		system("pause");
		lost = true;
		return;
	}
	
	// Variables to monitor player's progression
	bool movedPiece = false;
	bool choseValidPiece = false;
	bool choseValidSpot = false;
	bool cancelMove = false;
	
	// Variables to store player's piece to move and the spot to move to
	char pieceToMove = ' ';
	char spotToMove = ' ';
	
	while (!movedPiece)
	{
		while (!choseValidPiece)
		{
			std::cout << "You can move a piece to an adjacent empty spot." << std::endl;
			// Display which pieces can be moved
			std::list<char>::iterator listIterator;
			listIterator = moveablePieces.begin();
			std::cout << "Your moveable piece(s) are located at ";
			if (moveablePieces.size() > 2) // If the player has at least 3 moveable pieces
			{
				for (int i = 0; i < moveablePieces.size(); i++)
				{
					std::cout << *listIterator;
					if (i < moveablePieces.size() - 2)
						std::cout << ", ";
					else if (i == moveablePieces.size() - 2)
						std::cout << ", and ";
					else if (i == moveablePieces.size() - 1)
						std::cout << ". " << std::endl << std::endl;
					listIterator++;
				}
			}
			else if (moveablePieces.size() == 2)
			{
				std::cout << *listIterator << " and ";
				listIterator++;
				std::cout << *listIterator << "." << std::endl << std::endl;
			}
			else if (moveablePieces.size() == 1)
			{
				std::cout << *listIterator << "." << std::endl << std::endl;
			}

			// Prompt the player to choose a piece
			std::cout << "Choose one of your pieces to move (0-F): ";
			std::cin.get(pieceToMove);
			pieceToMove = toupper(pieceToMove);
			
			// Flush cin to read only the first character typed in
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			
			// Check if the player chose a valid piece by searching for the player's choice in the list of 
			// moveable pieces. listIterator will point to the end of the list if it does not contain the
			// piece being searched for.
			listIterator = find(moveablePieces.begin(), moveablePieces.end(), pieceToMove);
			
			if (listIterator == moveablePieces.end()) // If the chosen spot does not have a piece
			{
				std::cout << pieceToMove << " does not contain one of your moveable pieces." << std::endl;
			}
			else
			{
				choseValidPiece = true;
			}
		}
		
		while ((!choseValidSpot) && (!cancelMove))
		{
			// Determine which spaces the chosen piece can move to (there will be at least one)
			std::list<char> availableSpaces;
			for (int i = 0; i < 16; i++)
			{
				if ((isAdjacent[indexOfSpot(pieceToMove)][i]) && (spaces[i] == 'E'))
				{
					availableSpaces.push_back(charOfIndex(i));
				}
			}

			// Prompt the player to move their piece
			if (availableSpaces.size() >= 1)
			std::cout << "Choose an adjacent spot or enter X to choose a different piece: " << std::endl;
			std::cin.get(spotToMove);
			spotToMove = toupper(spotToMove);
			
			// Flush cin to read only the first character typed in
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			
			
			// Check if the player chose a valid spot to move in (assuming they did not cancel)
			std::list<char>::iterator listIterator;
			listIterator = find(availableSpaces.begin(), availableSpaces.end(), spotToMove);
			
			// Process the player's action
			if (spotToMove == 'X') // If the player cancels
			{
				cancelMove = true;
			}
			else if (listIterator == availableSpaces.end()) // The player chooses an invalid spot
			{
				std::cout << "You cannot move your piece there." << std::endl;
			}
			else // The player chooses a valid spot
			{
				choseValidSpot = true;
				// Update board
				spaces[indexOfSpot(pieceToMove)] = 'E';
				spaces[indexOfSpot(spotToMove)] = piece;
				pieceOnBoard[indexOfSpot(pieceToMove)] = false;
				pieceOnBoard[indexOfSpot(spotToMove)] = true;
			}
		}
		
		// If the player has chosen a valid piece and a spot to move it to
		if (choseValidSpot && choseValidPiece && (!cancelMove))
			movedPiece = true;
		else // The loop repeats
		{
			choseValidSpot = false;
			choseValidPiece = false;
			cancelMove = false;
			char pieceToMove = ' ';
			char spotToMove = ' ';
		}
	}
	return;
}

