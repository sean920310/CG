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

#define FONT_PATH "./Asset/Font/GenSenRounded-M.ttc"

enum class Team
{
	None = 0,
	Black,
	White
};

enum class InGameState
{
	start,
	canPlace,
	selectChess,
	pressChess,
	choiceMove,
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

	Coord operator-(const Coord& rhs)
	{
		Coord newCoord(this->x - rhs.x, this->y - rhs.y);
		if (newCoord.x < 0)
			newCoord.x = 0;
		if (newCoord.y < 0)
			newCoord.y = 0;
		return newCoord;
	}

	Coord operator+(const Coord& rhs)
	{
		Coord newCoord(this->x + rhs.x, this->y + rhs.y);
		if (newCoord.x > 7)
			newCoord.x = 7;
		if (newCoord.y > 7)
			newCoord.y = 7;
		return newCoord;
	}
}Coord;
