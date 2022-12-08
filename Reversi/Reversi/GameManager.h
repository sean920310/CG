#pragma once
#include "GameHeader.h"
#include "Board.h"
#include "Viewer.h"

class Viewer;

class GameManager
{
private:
	Team m_currentPlayer;		//1:White , 2:Black
	Board* m_board = nullptr;
	Viewer* m_viewer = nullptr;
	std::vector<sf::Sprite> m_sprites;
	std::fstream m_file;
	std::wstring m_filePath;

public:
	GameManager();
	~GameManager();

	//0:exit game 1:start new game 2:select a file
	int menu();
	void inGame(InGameState state);
	bool endGame(Team team);
	////0:windows close 1:continue 2:to menu
	//int pause();
	//bool confirmSurrender();
	//void readFile();
	//void logFile(Coord from, Coord to);
};

