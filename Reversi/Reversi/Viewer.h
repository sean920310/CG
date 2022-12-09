#pragma once
#include "GameHeader.h"
#include "Board.h"
#include "GameManager.h"

class GameManager;

typedef struct vec2
{
	float x, y;
	vec2() :x(0), y(0)
	{
	}

	vec2(int x, int y) :x(x), y(y)
	{
	}

	vec2(const Coord& rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
	}

	vec2& operator=(const Coord& rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
		return *this;
	}

	bool operator==(const Coord& rhs) {
		return (this->x == rhs.x && this->y == rhs.y);
	}
}vec2;

class Viewer
{
	friend class GameManager;
private:
	static int SCREEN_WIDTH;
	static int SCREEN_HEIGHT;
	static int BOARD_WIDTH;
	static int BOARD_HEIGHT;

	GameManager* m_game;

	sf::RenderWindow* window;
	sf::VideoMode videoMode;
	sf::Event ev;
	sf::Font font;
	sf::Font timeFont;
	bool clickLock = false;
public:
	Viewer(GameManager* game);
	~Viewer();

	bool windowIsOpen();
	int pollevent();
	sf::Vector2i getMousePosition();
	bool mouseClick(sf::Mouse::Button button);
	void close();
	//0:not event 1:close 2:esc 
	int update();
	void clear();
	//0:exit game 1:start new game 2:select a file
	int showMenu();
	//-1:no select 0:no 1:yes
	int showWinner(Team team);
	//-1:no select 0:continue 1:to menu
	int showPause();
	void drawBoard();
	void drawChess();
	void drawCanMovePos(std::vector<Coord> coords);
	void drawCurrentPlayer(Team team);
	void display();

public:
	bool isChoiceToMove(sf::Vector2i mouseCoord, Coord coord); 
	vec2 coordToVec2(const Coord& coord);

public:
	static const char	 MENU_EXIT;
	static const char	 MENU_START;
	static const char	 MENU_FILE;
	static const char	 PAUSE_EXIT;
	static const char	 PAUSE_CONTINUE;
	static const char	 PAUSE_MENU;
};
