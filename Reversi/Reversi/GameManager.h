#pragma once
#include "GameHeader.h"
#include "Board.h"
#include "Viewer.h"


class GameManager
{
private:
	std::vector<Chess*> onBoard;
	Team currentPlayer;		//1:White , 2:Black
	sf::Time timeLimit;
	sf::Time whiteTime;
	sf::Time blackTime;
	sf::Clock clock;
	Board board;
	Viewer viewer;
	std::vector<sf::Sprite> sprites;
	std::fstream file;
	std::wstring filePath;

public:
	GameManager();

	//0:exit game 1:start new game 2:select a file
	int menu();
	void inGame(InGameState state);
	//bool endGame(Team team);
	////0:windows close 1:continue 2:to menu
	//int pause();
	//bool confirmSurrender();
	//void readFile();
	//void logFile(Coord from, Coord to);
	
};

