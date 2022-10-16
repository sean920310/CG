#include "Board.h"

Board::Board() :boardArr(8, std::vector<Team>(8, Team::None))
{
	//initialize checkboard
	//checkBoardTexture.loadFromFile(CHECKBOARD);
	//checkBoard.setTexture(checkBoardTexture);
	//checkBoard.setPosition(sf::Vector2f(35.0625f, 58.05f));
	//checkBoard.setScale(sf::Vector2f(CHECKBOARD_SCALE_SIZE, CHECKBOARD_SCALE_SIZE));

	//initialize boardArr
	boardArr[4][3] = boardArr[3][4] = Team::Black;
	boardArr[4][4] = boardArr[3][3] = Team::White;
}

Board::Board(const Board& rhs) :boardArr(8, std::vector<Team>(8, Team::None))
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
				this->boardArr[j][i] = rhs.boardArr[j][i];
		}
	}
	this->checkBoardTexture = rhs.checkBoardTexture;
	this->checkBoard = rhs.checkBoard;
}

Board& Board::operator=(const Board& rhs)
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			this->boardArr[j][i] = rhs.boardArr[j][i];
		}
	}
	this->checkBoardTexture = rhs.checkBoardTexture;
	this->checkBoard = rhs.checkBoard;
	return *this;
}

void Board::newBoard()
{
	//Clear old board
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			this->boardArr[j][i] = Team::None;
		}
	}
	
	//initialize board
	boardArr[4][3] = boardArr[3][4] = Team::Black;
	boardArr[4][4] = boardArr[3][3] = Team::White;
}

Team Board::getChess(Coord coord) const
{
	if (coord.x > 7 || coord.y > 7)
		return Team::None;
	else
		return boardArr[coord.x][coord.y];
}