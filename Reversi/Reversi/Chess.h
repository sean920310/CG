#pragma once
#include "GameHeader.h"
class Board;
#include "Board.h"

class Chess
{
protected:
	Team team;
	sf::Texture chessTexture;
	sf::Sprite chess;
	Coord coord;
	bool isSelect = false;
public:
	//constructor
	Chess(const Team& team = Team::Black);
	Chess(const Chess& rhs);

	//function
	void setPosition(Coord coord);
	//void showSelect(sf::Vector2i mouseCoord, Team team);
	//sf::Sprite getSprite();
	//Coord getCoord();
	//Team getTeam();
	//Characters getCharacter();
	//bool isChoice();
	////*check if the move will be check
	//void removeWillCheckCoord(const Board& board, std::vector<Coord>& allCoord);

	////move function
	//virtual std::vector<Coord> coordCanMove(const Board& board) = 0;
	//virtual bool moveable(const Board& board, Coord toCoord) = 0;
};

