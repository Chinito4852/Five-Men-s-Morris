#include <iostream>
#include <cctype>
#include "HumanPlayer.h"
#include "CPU.h"

int main()
{
	
	// Create an array to represent spaces on board
	char spaces[16];
	for (int i = 0; i < 16; i++)
	{
		// 'E' signifies an empty spaces
		spaces[i] = 'E';
	}
	
	// Create an adjacency matrix to represent adjacent spaces on the board
	
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
	
	bool adjacencyMatrix[16][16];
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			adjacencyMatrix[i][j] = false;
		}
	}
	adjacencyMatrix[0][1] = true;
	adjacencyMatrix[0][6] = true;
	adjacencyMatrix[1][0] = true;
	adjacencyMatrix[1][2] = true;
	adjacencyMatrix[1][4] = true;
	adjacencyMatrix[2][1] = true;
	adjacencyMatrix[2][9] = true;
	adjacencyMatrix[3][4] = true;
	adjacencyMatrix[3][7] = true;
	adjacencyMatrix[4][1] = true;
	adjacencyMatrix[4][3] = true;
	adjacencyMatrix[4][5] = true;
	adjacencyMatrix[5][4] = true;
	adjacencyMatrix[5][8] = true;
	adjacencyMatrix[6][0] = true;
	adjacencyMatrix[6][7] = true;
	adjacencyMatrix[6][13] = true;
	adjacencyMatrix[7][3] = true;
	adjacencyMatrix[7][6] = true;
	adjacencyMatrix[7][10] = true;
	adjacencyMatrix[8][5] = true;
	adjacencyMatrix[8][9] = true;
	adjacencyMatrix[8][12] = true;
	adjacencyMatrix[9][2] = true;
	adjacencyMatrix[9][8] = true;
	adjacencyMatrix[9][15] = true;
	adjacencyMatrix[10][7] = true;
	adjacencyMatrix[10][11] = true;
	adjacencyMatrix[11][10] = true;
	adjacencyMatrix[11][12] = true;
	adjacencyMatrix[11][14] = true;
	adjacencyMatrix[12][8] = true;
	adjacencyMatrix[12][11] = true;
	adjacencyMatrix[13][6] = true;
	adjacencyMatrix[13][14] = true;
	adjacencyMatrix[14][11] = true;
	adjacencyMatrix[14][13] = true;
	adjacencyMatrix[14][15] = true;
	adjacencyMatrix[15][9] = true;
	adjacencyMatrix[15][14] = true;
	
	// Prompt the player to choose to play against another player or the AI
	char choice = ' ';
	std::cout << "Press 1 to play against a second player or press 2 to play against an AI: ";
	std::cin.get(choice);
	// Flush cin to read only the first character typed in
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	while (choice != '1' && choice != '2')
	{
		std::cout << choice << " is not a valid choice" << std::endl;
		std::cout << "Press 1 to play against a second player or press 2 to play against an AI: ";
		std::cin.get(choice);
		// Flush cin to read only the first character typed in
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	// Create human and CPU players (and player pointers)
	HumanPlayer player1;
	if (choice == '1')
	{
		HumanPlayer player2;
		// Assign pieces (and pointers) to players
		Player* xPlayer;
		Player* oPlayer;

		std::cout << "Player 1, choose X or O (X will go first): ";
		std::cin.get(choice);
		choice = toupper(choice);
		// Flush cin to read only the first character typed in
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		while (choice != 'X' && choice != 'O')
		{
			std::cout << choice << " is not a valid choice" << std::endl;
			std::cout << "Player 1, choose X or O (X will go first): ";
			std::cin.get(choice);
			choice = toupper(choice);
			// Flush cin to read only the first character typed in
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		if (choice == 'X')
		{
			player1.piece = 'X';
			player2.piece = 'O';
			xPlayer = &player1;
			oPlayer = &player2;
		}
		else
		{
			player1.piece = 'O';
			player2.piece = 'X';
			xPlayer = &player2;
			oPlayer = &player1;
		}


		// Players take turns until one loses
		int turnCount = 1;
		while ((!player1.lost) && (!player2.lost))
		{
			if (turnCount % 2 != 0)
			{
				if (player1.piece == 'X')
					std::cout << "Player 1's turn" << std::endl;
				else
					std::cout << "Player 2's turn" << std::endl;
				(*xPlayer).move(spaces, adjacencyMatrix, *oPlayer);
			}
			else
			{
				if (player1.piece == 'O')
					std::cout << "Player 1's turn" << std::endl;
				else
					std::cout << "Player 2's turn" << std::endl;
				(*oPlayer).move(spaces, adjacencyMatrix, *xPlayer);
			}
			system("cls");
			turnCount++;
		}

		player1.printBoard(spaces);
		if (player1.lost)
			std::cout << "Player 2 wins." << std::endl;
		else
			std::cout << "Player 1 wins." << std::endl;
	}
	else
	{
		CPU player2;
		// Assign pieces (and pointers) to players
		Player* xPlayer;
		Player* oPlayer;

		std::cout << "Player 1, choose X or O (X will go first): ";
		std::cin.get(choice);
		choice = toupper(choice);
		// Flush cin to read only the first character typed in
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		while (choice != 'X' && choice != 'O')
		{
			std::cout << choice << " is not a valid choice" << std::endl;
			std::cout << "Player 1, choose X or O (X will go first): ";
			std::cin.get(choice);
			choice = toupper(choice);
			// Flush cin to read only the first character typed in
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		if (choice == 'X')
		{
			player1.piece = 'X';
			player2.piece = 'O';
			xPlayer = &player1;
			oPlayer = &player2;
		}
		else
		{
			player1.piece = 'O';
			player2.piece = 'X';
			xPlayer = &player2;
			oPlayer = &player1;
		}


		// Players take turns until one loses
		int turnCount = 1;
		while ((!player1.lost) && (!player2.lost))
		{
			if (turnCount % 2 != 0)
			{
				if (player1.piece == 'X')
					std::cout << "Player 1's turn" << std::endl;
				else
					std::cout << "Player 2's turn" << std::endl;
				(*xPlayer).move(spaces, adjacencyMatrix, *oPlayer);
			}
			else
			{
				if (player1.piece == 'O')
					std::cout << "Player 1's turn" << std::endl;
				else
					std::cout << "Player 2's turn" << std::endl;
				(*oPlayer).move(spaces, adjacencyMatrix, *xPlayer);
			}
			system("cls");
			turnCount++;
		}

		player1.printBoard(spaces);
		if (player1.lost)
			std::cout << "Player 2 wins." << std::endl;
		else
			std::cout << "Player 1 wins." << std::endl;
	}
	
	
	system("pause");
	return 0;
}




