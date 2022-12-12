#pragma once
#include "GameHeader.h"

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
	std::vector<std::vector<Team>> m_boardArr;
	int m_blackNum = 0, m_whiteNum = 0;

public:
	Board();
	Board(const Board& rhs);
	Board& operator=(const Board& rhs);
	void newBoard();
	Team getChess(Coord coord) const;
	std::vector<std::vector<Team>> getBoardArr();
	int getChessNum(Team team);
	std::vector<Coord> coordCanPlace(Team currentTeam);
	std::vector<Coord> placeChess(Team currentTeam, Coord coord);
	bool oneSideIsWin(Team& team);
};
