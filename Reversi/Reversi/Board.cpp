#include "Board.h"

Board::Board() :boardArr(8, std::vector<Chess*>(8, nullptr))
{
	//initialize checkboard
	//checkBoardTexture.loadFromFile(CHECKBOARD);
	//checkBoard.setTexture(checkBoardTexture);
	//checkBoard.setPosition(sf::Vector2f(35.0625f, 58.05f));
	//checkBoard.setScale(sf::Vector2f(CHECKBOARD_SCALE_SIZE, CHECKBOARD_SCALE_SIZE));

	//initialize chess
	Chess* black[2];
	black[0] = new Chess(Team::Black);
	black[1] = new Chess(Team::Black);
	black[0]->setPosition({ 3,4 });
	black[1]->setPosition({ 4,3 });

	Chess* white[2];
	white[0] = new Chess(Team::White);
	white[1] = new Chess(Team::White);
	white[0]->setPosition({ 3,3 });
	white[1]->setPosition({ 4,4 });

	//initialize boardArr
	boardArr[3][4] = black[0];
	boardArr[4][3] = black[1];
	boardArr[3][3] = white[0];
	boardArr[4][4] = white[1];
}

Board::Board(const Board& rhs) :boardArr(8, std::vector<Chess*>(8, nullptr))
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (rhs.boardArr[j][i] != nullptr) {
				this->boardArr[j][i] = new Chess(*rhs.boardArr[j][i]);
			}
		}
	}
	this->checkBoardTexture = rhs.checkBoardTexture;
	this->checkBoard = rhs.checkBoard;
}

Board::~Board()
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (boardArr[j][i] != nullptr)
				delete boardArr[j][i];
		}
	}
}

Board& Board::operator=(const Board& rhs)
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (rhs.boardArr[j][i] != nullptr) {
				this->boardArr[j][i] = new Chess(*rhs.boardArr[j][i]);
			}
		}
	}
	this->checkBoardTexture = rhs.checkBoardTexture;
	this->checkBoard = rhs.checkBoard;
	return *this;
}