#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#define WIN32_LEAN_AND_MEAN
#include <shobjidl.h> 
#include <windows.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

#define CHESS_BLACK "./Asset/Image/Chess/black.png"
#define CHESS_WHITE "./Asset/Image/Chess/white.png"

#define FONT_PATH "./Asset/Font/GenSenRounded-M.ttc"
#define TIMEFONT_PATH "./Asset/Font/digital-7 (mono).ttf"

#define CHESS_SCALE_SIZE 0.1f
#define CHESS_CHOISE_SCALE_SIZE 0.108f
#define CHECKBOARD_SCALE_SIZE 0.3f

#define TIMELIMIT 60


enum class Team
{
	None,
	Black,
	White
};

enum class InGameState
{
	start,
	inputFile,
	selectChess,
	pressChess,
	choiceMove,
	isCheck,
	oneSideWin
};

typedef struct Coord {
	int x;
	int y;

	Coord() :x(0), y(0)
	{
	}

	Coord(int x, int y) :x(x), y(y)
	{
	}

	Coord(const Coord& rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
	}

	Coord& operator=(const Coord& rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
		return *this;
	}

	bool operator==(const Coord& rhs) {
		return (this->x == rhs.x && this->y == rhs.y);
	}
}Coord;
