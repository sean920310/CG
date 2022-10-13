#include "Chess.h"

Chess::Chess(const Team& team) :coord({ 0,0 })
{
	this->team = team;
	if (team == Team::Black)
		chessTexture.loadFromFile(CHESS_BLACK);
	else
		chessTexture.loadFromFile(CHESS_WHITE);
	chess.setTexture(chessTexture);
	chess.setScale(sf::Vector2f(CHESS_SCALE_SIZE, CHESS_SCALE_SIZE));
}

Chess::Chess(const Chess& rhs)
{
	this->team = rhs.team;
	this->chessTexture = rhs.chessTexture;
	this->chess = rhs.chess;
	this->coord = rhs.coord;
}

void Chess::setPosition(Coord coord)
{
	this->coord = coord;
	float x, y;
	x = CHECKBOARD_SCALE_SIZE * (coord.x * 233.75 + 34) + 35.0625 - CHESS_SCALE_SIZE * (606.0 / 2);
	y = CHECKBOARD_SCALE_SIZE * (coord.y * 233.75 + 34) + 58.05 - CHESS_SCALE_SIZE * (606.0 / 2);
	chess.setPosition(sf::Vector2f(x, y));
}
