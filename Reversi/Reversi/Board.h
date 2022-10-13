#pragma once
#include "GameHeader.h"
class Chess;
#include "Chess.h"

//   0 1 2 3 4 5 6 7 
//	 _ _ _ _ _ _ _ _
//0	|_|_|_|_|_|_|_|_|
//1	|_|_|_|_|_|_|_|_|
//2 |_|_|_|_|_|_|_|_|
//3 |_|_|_|_|_|_|_|_|
//4 |_|_|_|_|_|_|_|_|
//5 |_|_|_|_|_|_|_|_|
//6 |_|_|_|_|_|_|_|_|
//7 |_|_|_|_|_|_|_|_|


class Board
{
private:
	std::vector<std::vector<Chess*>> boardArr;
	sf::Texture checkBoardTexture;
	sf::Sprite checkBoard;

public:
	Board();
	Board(const Board& rhs);
	~Board();
	Board& operator=(const Board& rhs);
	//void newBoard();
	//Chess* getChess(Coord coord) const;
	//std::vector<sf::Sprite> getAllSprite();
	//std::vector<Chess*> getAllChess();
	//void moveChess(const Coord& fromCoord, const Coord& toCoord);
	//bool oneSideIsCheck(Team& team);
	//bool oneSideIsWin(Team& team);
};
