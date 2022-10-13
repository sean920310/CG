#pragma once
#include "GameHeader.h"

class Viewer
{
private:
	sf::RenderWindow* window;
	sf::VideoMode videoMode;
	sf::Event ev;
	sf::Font font;
	sf::Font timeFont;
	bool clickLock = false;
public:
	Viewer();
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
	void showCheck(Team team);
	void showWinner(Team team);
	//-1:no select 0:no 1:yes
	int showOneMoreGame();
	//-1:no select 0:continue 1:to menu
	int showPause();
	//-1:no select 0:Black 1:Red
	int showSurrender(bool canSelect, Team team);
	//-1:no select 0:no 1:yes
	int showConfirmSurrender();
	void drawTime(sf::Time redTime, sf::Time blackTime);
	void drawRightSideObject(Team team);
	void drawCanMovePos(std::vector<Coord> coords);
	void drawSprite(std::vector<sf::Sprite> sprites);
	void display();
public:
	static const char	 MENU_EXIT;
	static const char	 MENU_START;
	static const char	 MENU_FILE;
	static const char	 PAUSE_EXIT;
	static const char	 PAUSE_CONTINUE;
	static const char	 PAUSE_MENU;
};
