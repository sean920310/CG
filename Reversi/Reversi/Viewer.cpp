#include "Viewer.h"

int Viewer::SCREEN_HEIGHT = 768;
int Viewer::SCREEN_WIDTH = 1280;
int Viewer::BOARD_HEIGHT = 700;
int Viewer::BOARD_WIDTH = 700;

vec2 Viewer::coordToVec2(const Coord& coord)
{
	float dx = BOARD_WIDTH / 8.0f;
	float dy = BOARD_HEIGHT / 8.0f;

	vec2 result;
	result.x = (SCREEN_WIDTH / 2.0f) + (coord.x - 4) * dx + (dx / 2.0f);
	result.y = (SCREEN_HEIGHT / 2.0f) + (coord.y - 4) * dy + (dy / 2.0f);
	return result;
}

//mouse postion is on coord that choice to move
bool Viewer::isChoiceToMove(sf::Vector2i mouseCoord, Coord coord)
{
	vec2 pos = coordToVec2(coord);

	pos.x -= BOARD_WIDTH / 16.0f;
	pos.y -= BOARD_HEIGHT / 16.0f;

	sf::IntRect circle(pos.x, pos.y, BOARD_WIDTH / 8.0f, BOARD_WIDTH / 8.0f);

	return circle.contains(mouseCoord);
}



Viewer::Viewer(GameManager* game) : ev()
{
	this->videoMode.height = SCREEN_HEIGHT;
	this->videoMode.width = SCREEN_WIDTH;

	this->window = new sf::RenderWindow(this->videoMode, L"Reversi 黑白棋", sf::Style::Close | sf::Style::Titlebar);
	this->window->setFramerateLimit(60);

	m_game = game;

	//timeFont.loadFromFile(TIMEFONT_PATH);
	font.loadFromFile(FONT_PATH);
}

Viewer::~Viewer()
{
	delete this->window;
}

bool Viewer::windowIsOpen()
{
	return this->window->isOpen();
}

int Viewer::pollevent()
{
	while (this->window->pollEvent(ev))
	{
		switch (ev.type)
		{
		case sf::Event::Closed:
			window->close();
			return 1;
			break;
		case sf::Event::KeyReleased:
			if (ev.key.code == sf::Keyboard::Escape) {
				return 2;
			}
			break;
		default:
			break;
		}
	}
	return 0;
}

sf::Vector2i Viewer::getMousePosition()
{
	return sf::Mouse::getPosition(*this->window);
}

bool Viewer::mouseClick(sf::Mouse::Button button)
{
	bool press = false;
	while (sf::Mouse::isButtonPressed(button))
	{
		press = true;
	}
	return press;
}

void Viewer::close()
{
	this->window->close();
}

int Viewer::update()
{
	return pollevent();
}

void Viewer::clear()
{
	this->window->clear(sf::Color::Color(30,30,30));
}

int Viewer::showMenu()	//0:exit game 1:start new game 2:select a file
{
	int choice = -1;

	//title
	sf::Text title(L"黑白棋", font);
	title.setCharacterSize(120);
	title.setStyle(sf::Text::Bold);
	title.setFillColor(sf::Color::White);
	title.setOrigin(180, 60);
	title.setPosition(640, 160);
	this->window->draw(title);

	//button
	sf::Text startText(L"單人遊戲", font);
	startText.setCharacterSize(50);
	startText.setFillColor(sf::Color::Black);
	startText.setOrigin(100, 20);
	startText.setPosition(640, 380);
	sf::RectangleShape startBtn(sf::Vector2f(260, 70));
	startBtn.setOrigin(130, 35);
	startBtn.setPosition(640, 390);
	startBtn.setOutlineThickness(1);
	startBtn.setOutlineColor(sf::Color(0, 0, 0, 255));
	startBtn.setFillColor(sf::Color(150, 150, 150, 255));

	sf::Text fileText(L"雙人遊戲", font);
	fileText.setCharacterSize(50);
	fileText.setFillColor(sf::Color::Black);
	fileText.setOrigin(100, 20);
	fileText.setPosition(640, 480);
	sf::RectangleShape fileBtn(sf::Vector2f(260, 70));
	fileBtn.setOrigin(130, 35);
	fileBtn.setPosition(640, 490);
	fileBtn.setOutlineThickness(1);
	fileBtn.setOutlineColor(sf::Color(0, 0, 0, 255));
	fileBtn.setFillColor(sf::Color(150, 150, 150, 255));

	sf::Text exitText(L"離開遊戲", font);
	exitText.setCharacterSize(50);
	exitText.setFillColor(sf::Color::Black);
	exitText.setOrigin(100, 20);
	exitText.setPosition(640, 580);
	sf::RectangleShape exitBtn(sf::Vector2f(260, 70));
	exitBtn.setOrigin(130, 35);
	exitBtn.setPosition(640, 590);
	exitBtn.setOutlineThickness(1);
	exitBtn.setOutlineColor(sf::Color(0, 0, 0, 255));
	exitBtn.setFillColor(sf::Color(150, 150, 150, 255));

	sf::FloatRect startRect(startBtn.getPosition() - startBtn.getOrigin(), startBtn.getSize());
	sf::FloatRect fileRect(fileBtn.getPosition() - fileBtn.getOrigin(), fileBtn.getSize());
	sf::FloatRect exitRect(exitBtn.getPosition() - exitBtn.getOrigin(), exitBtn.getSize());

	if (startRect.contains(sf::Vector2f(this->getMousePosition())))
	{
		startBtn.scale(1.05, 1.05);
		startText.scale(1.05, 1.05);
		choice = 1;
	}
	else
	{
		startBtn.scale(1, 1);
		startText.scale(1, 1);
	}

	if (fileRect.contains(sf::Vector2f(this->getMousePosition())))
	{
		fileBtn.scale(1.05, 1.05);
		fileText.scale(1.05, 1.05);
		choice = 2;
	}
	else
	{
		fileBtn.scale(1, 1);
		fileText.scale(1, 1);
	}

	if (exitRect.contains(sf::Vector2f(this->getMousePosition())))
	{
		exitBtn.scale(1.05, 1.05);
		exitText.scale(1.05, 1.05);
		choice = 0;
	}
	else
	{
		exitBtn.scale(1, 1);
		exitText.scale(1, 1);
	}


	this->window->draw(startBtn);
	this->window->draw(startText);
	this->window->draw(fileBtn);
	this->window->draw(fileText);
	this->window->draw(exitBtn);
	this->window->draw(exitText);

	return choice;
}

int Viewer::showWinner(Team team)
{
	sf::RectangleShape background(sf::Vector2f(1280, 576));
	background.setFillColor(sf::Color(0, 0, 0, 127));
	background.setPosition(0, 96);


	//show who win
	sf::Text win;
	if (team == Team::White)
		win.setString(L"白方獲勝!!");
	else
		win.setString(L"黑方獲勝!!");
	win.setFont(font);
	win.setCharacterSize(80);
	win.setStyle(sf::Text::Bold);
	win.setFillColor(sf::Color::White);
	win.setOrigin(80 * 4 / 2, 40);
	win.setPosition(SCREEN_WIDTH / 2, 190);

	int choice = -1;
	//show one more game
	sf::Text oneMore(L"再來一局?", font);
	oneMore.setCharacterSize(50);
	oneMore.setFillColor(sf::Color::White);
	oneMore.setOrigin(50 * 4 / 2, 25);
	oneMore.setPosition(SCREEN_WIDTH / 2, 350);

	//show botton
	sf::Text yesText(L"是", font);
	sf::Text noText(L"否", font);

	yesText.setCharacterSize(40);
	yesText.setFillColor(sf::Color::White);
	yesText.setOrigin(20, 20);
	yesText.setPosition(SCREEN_WIDTH / 2 - 100, 470);

	noText.setCharacterSize(40);
	noText.setFillColor(sf::Color::White);
	noText.setOrigin(20, 20);
	noText.setPosition(SCREEN_WIDTH / 2 + 100, 470);

	if (yesText.getGlobalBounds().contains(sf::Vector2f(this->getMousePosition())))
	{
		yesText.setScale(1.1, 1.1);
		choice = 1;
	}
	else {
		yesText.scale(1, 1);
	}

	if (noText.getGlobalBounds().contains(sf::Vector2f(this->getMousePosition()))) {
		noText.scale(1.1, 1.1);
		choice = 0;
	}
	else {
		noText.scale(1, 1);
	}
	this->window->draw(background);
	this->window->draw(win);

	this->window->draw(oneMore);
	this->window->draw(yesText);
	this->window->draw(noText);
	return choice;
}

int Viewer::showPause()
{
	int choice = -1;
	sf::RectangleShape background(sf::Vector2f(1280, 576));
	background.setFillColor(sf::Color(0, 0, 0, 127));
	background.setPosition(0, 96);

	sf::Text pause(L"暫停", font);
	pause.setCharacterSize(80);
	pause.setFillColor(sf::Color::White);
	pause.setPosition(560, 150);

	sf::Text continu(L"繼續遊戲", font);
	continu.setCharacterSize(50);
	continu.setFillColor(sf::Color::White);
	continu.setOrigin(100, 25);
	continu.setPosition(640, 350);

	sf::Text toMenu(L"回到主選單", font);
	toMenu.setCharacterSize(50);
	toMenu.setFillColor(sf::Color::White);
	toMenu.setOrigin(125, 25);
	toMenu.setPosition(640, 450);

	if (continu.getGlobalBounds().contains(sf::Vector2f(this->getMousePosition())))
	{
		continu.setScale(1.1, 1.1);
		choice = 0;
	}
	else
	{
		continu.setScale(1, 1);
	}
	if (toMenu.getGlobalBounds().contains(sf::Vector2f(this->getMousePosition())))
	{
		toMenu.setScale(1.1, 1.1);
		choice = 1;
	}
	else
	{
		toMenu.setScale(1, 1);
	}
	this->window->draw(background);
	this->window->draw(pause);
	this->window->draw(continu);
	this->window->draw(toMenu);
	return choice;
}

void Viewer::drawCanMovePos(std::vector<Coord> coords)
{
	for (const Coord& coord : coords) {
		vec2 pos = coordToVec2(coord);
		sf::CircleShape circle(BOARD_WIDTH / 32.0f );
		circle.setOrigin(BOARD_WIDTH / 32.0f, BOARD_WIDTH / 32.0f);
		circle.setPosition(pos.x, pos.y);
		circle.setFillColor(sf::Color(0, 0, 0, 100));

		this->window->draw(circle);
	}
}

void Viewer::drawCurrentPlayer(Team team)
{
	sf::Text black(L"黑棋", font);
	black.setCharacterSize(50);
	black.setFillColor(sf::Color::White);
	black.setPosition(100, 50);

	sf::Text white(L"白棋", font);
	white.setCharacterSize(50);
	white.setFillColor(sf::Color::White);
	white.setPosition(1100, 50);

	sf::CircleShape current(10);
	current.setFillColor(sf::Color::Red);
	if (team == Team::Black)
		current.setPosition(50, 70);
	else
		current.setPosition(1050, 70);


	int blackNum = m_game->m_board->getChessNum(Team::Black);
	int whiteNum = m_game->m_board->getChessNum(Team::White);
	sf::Text blackNumText(std::to_string(blackNum), font);
	blackNumText.setCharacterSize(40);
	blackNumText.setFillColor(sf::Color::White);
	blackNumText.setPosition(100, 120);

	sf::Text whiteNumText(std::to_string(whiteNum), font);
	whiteNumText.setCharacterSize(40);
	whiteNumText.setFillColor(sf::Color::White);
	whiteNumText.setPosition(1100, 120);

	this->window->draw(black);
	this->window->draw(white);
	this->window->draw(blackNumText);
	this->window->draw(whiteNumText);
	this->window->draw(current);
}

void Viewer::drawBoard()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			vec2 pos = coordToVec2(Coord(i, j));
			sf::RectangleShape box(sf::Vector2f(BOARD_WIDTH / 8.0f, BOARD_HEIGHT / 8.0f));
			box.setOrigin(BOARD_WIDTH / 16.0f, BOARD_HEIGHT / 16.0f);
			box.setPosition(pos.x, pos.y);
			box.setOutlineThickness(2);
			box.setOutlineColor(sf::Color::Black);
			box.setFillColor(sf::Color(0, 150, 0, 255));
			this->window->draw(box);
		}
	}
}

void Viewer::drawChess()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Team team = this->m_game->m_board->getChess(Coord(i, j));
			if (team == Team::None)
				continue;

			vec2 pos = coordToVec2(Coord(i, j));
			sf::CircleShape chess(BOARD_WIDTH / 16.0f - 5.0f);
			chess.setOrigin(BOARD_WIDTH / 16.0f - 5.0f, BOARD_WIDTH / 16.0f - 5.0f);
			chess.setPosition(pos.x, pos.y);
			chess.setOutlineThickness(1);
			chess.setOutlineColor(sf::Color::Black);
			if(team == Team::White)
				chess.setFillColor(sf::Color(255, 255, 255, 255));
			else
				chess.setFillColor(sf::Color(0, 0, 0, 255));

			this->window->draw(chess);
		}
	}
}

void Viewer::drawChess(float t, std::vector<Coord> flips)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Team team = this->m_game->m_board->getChess(Coord(i, j));
			if (team == Team::None)
				continue;

			vec2 pos = coordToVec2(Coord(i, j));
			sf::CircleShape chess(BOARD_WIDTH / 16.0f - 5.0f);
			chess.setOrigin(BOARD_WIDTH / 16.0f - 5.0f, BOARD_WIDTH / 16.0f - 5.0f);
			chess.setPosition(pos.x, pos.y);
			chess.setOutlineThickness(1);
			chess.setOutlineColor(sf::Color::Black);

			bool flip = false;
			for (int m = 0; m < flips.size(); m++)
			{
				if (flips[m] == Coord(i, j)) flip = true;
			}

			if (team == Team::White)
				chess.setFillColor(sf::Color(255, 255, 255, 255));
			else
				chess.setFillColor(sf::Color(0, 0, 0, 255));
			
			if (flip)
			{
				float deg = t * 180;
				if (deg < 90.0f)
				{
					if (team != Team::White)
						chess.setFillColor(sf::Color(255, 255, 255, 255));
					else
						chess.setFillColor(sf::Color(0, 0, 0, 255));
				}
				//std::cout << deg << std::endl;
				chess.scale(1.0f - sin(deg * 3.1415926 / 180), 1.0f);
			}

			this->window->draw(chess);
		}
	}
}

void Viewer::display()
{

	this->window->display();
}