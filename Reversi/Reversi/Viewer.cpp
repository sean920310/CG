#include "Viewer.h"

int Viewer::SCREEN_HEIGHT = 768;
int Viewer::SCREEN_WIDTH = 1280;

Viewer::Viewer() : ev()
{
	this->videoMode.height = SCREEN_HEIGHT;
	this->videoMode.width = SCREEN_WIDTH;

	this->window = new sf::RenderWindow(this->videoMode, L"Reversi 黑白棋", sf::Style::Close | sf::Style::Titlebar);
	this->window->setFramerateLimit(60);


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
	bool click = false;
	while (sf::Mouse::isButtonPressed(button))
	{
		click = true;
	}
	return click;
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
	sf::Text startText(L"開始遊戲", font);
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

	sf::Text fileText(L"讀取資料", font);
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

void Viewer::showCheck(Team team)
{
	if (team == Team::White) {
		sf::Text redCheck(L"紅方將軍!!", font);
		redCheck.setCharacterSize(40);
		//redCheck.setStyle(sf::Text::Bold);
		redCheck.setFillColor(sf::Color::Black);
		redCheck.setPosition(700, 384 + 60);
		this->window->draw(redCheck);
	}
	else
	{

		sf::Text blackCheck(L"黑方將軍!!", font);
		blackCheck.setCharacterSize(40);
		//blackCheck.setStyle(sf::Text::Bold);
		blackCheck.setFillColor(sf::Color::Black);
		blackCheck.setPosition(700, 0 + 60);
		this->window->draw(blackCheck);
	}

}

void Viewer::showWinner(Team team)
{
	//show who win
	sf::Text win;
	if (team == Team::White)
		win.setString(L"紅方獲勝!!");
	else
		win.setString(L"黑方獲勝!!");
	win.setFont(font);
	win.setCharacterSize(80);
	win.setStyle(sf::Text::Bold);
	win.setFillColor(sf::Color::Black);
	win.setPosition(700, 100);
	this->window->draw(win);

}

int Viewer::showOneMoreGame()
{
	int choice = -1;
	//show one more game
	sf::Text oneMore(L"再來一局?", font);
	oneMore.setCharacterSize(50);
	oneMore.setFillColor(sf::Color::Black);
	oneMore.setPosition(700, 300);
	this->window->draw(oneMore);

	//show botton
	sf::Text yesText(L"是", font);
	sf::Text noText(L"否", font);

	yesText.setCharacterSize(40);
	yesText.setFillColor(sf::Color::Black);
	yesText.setOrigin(20, 20);
	yesText.setPosition(750, 420);

	noText.setCharacterSize(40);
	noText.setFillColor(sf::Color::Black);
	noText.setOrigin(20, 20);
	noText.setPosition(870, 420);

	sf::RectangleShape yesBtn(sf::Vector2f(100, 60));
	sf::RectangleShape noBtn(sf::Vector2f(100, 60));
	yesBtn.setOrigin(50, 30);
	noBtn.setOrigin(50, 30);
	yesBtn.setPosition(750, 430);
	noBtn.setPosition(870, 430);
	yesBtn.setOutlineThickness(1);
	noBtn.setOutlineThickness(1);
	yesBtn.setOutlineColor(sf::Color(0, 0, 0, 255));
	noBtn.setOutlineColor(sf::Color(0, 0, 0, 255));
	yesBtn.setFillColor(sf::Color(0, 0, 0, 50));
	noBtn.setFillColor(sf::Color(0, 0, 0, 50));

	sf::FloatRect yesRect(yesBtn.getPosition() - yesBtn.getOrigin(), yesBtn.getSize());
	sf::FloatRect noRect(noBtn.getPosition() - noBtn.getOrigin(), noBtn.getSize());

	if (yesRect.contains(sf::Vector2f(this->getMousePosition()))) {
		yesBtn.scale(1.05, 1.05);
		yesText.scale(1.05, 1.05);
		choice = 1;
	}
	else {
		yesBtn.scale(1, 1);
		yesText.scale(1, 1);
	}

	if (noRect.contains(sf::Vector2f(this->getMousePosition()))) {
		noBtn.scale(1.05, 1.05);
		noText.scale(1.05, 1.05);
		choice = 0;
	}
	else {
		noBtn.scale(1, 1);
		noText.scale(1, 1);
	}

	this->window->draw(yesBtn);
	this->window->draw(noBtn);
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

int Viewer::showSurrender(bool canSelect, Team team)
{
	int choice = -1;
	sf::Text red(L"投降", font);
	red.setCharacterSize(40);
	red.setFillColor(sf::Color::Black);
	red.setOrigin(40, 20);
	red.setPosition(1050, 414);

	sf::RectangleShape redBtn(sf::Vector2f(100, 60));
	redBtn.setOrigin(50, 30);
	redBtn.setPosition(1050, 424);
	redBtn.setOutlineThickness(1);
	redBtn.setOutlineColor(sf::Color(0, 0, 0, 255));
	redBtn.setFillColor(sf::Color(0, 0, 0, 50));

	sf::Text black(L"投降", font);
	black.setCharacterSize(40);
	black.setFillColor(sf::Color::Black);
	black.setOrigin(40, 20);
	black.setPosition(1050, 30);

	sf::RectangleShape blackBtn(sf::Vector2f(100, 60));
	blackBtn.setOrigin(50, 30);
	blackBtn.setPosition(1050, 40);
	blackBtn.setOutlineThickness(1);
	blackBtn.setOutlineColor(sf::Color(0, 0, 0, 255));
	blackBtn.setFillColor(sf::Color(0, 0, 0, 50));

	sf::FloatRect redRect(redBtn.getPosition() - redBtn.getOrigin(), redBtn.getSize());
	sf::FloatRect blackRect(blackBtn.getPosition() - blackBtn.getOrigin(), blackBtn.getSize());

	if (redRect.contains(sf::Vector2f(this->getMousePosition())) && canSelect && team == Team::White) {
		redBtn.scale(1.05, 1.05);
		red.scale(1.05, 1.05);
		choice = 1;
	}
	else {
		redBtn.scale(1, 1);
		red.scale(1, 1);
	}

	if (blackRect.contains(sf::Vector2f(this->getMousePosition())) && canSelect && team == Team::Black) {
		blackBtn.scale(1.05, 1.05);
		black.scale(1.05, 1.05);
		choice = 0;
	}
	else {
		blackBtn.scale(1, 1);
		black.scale(1, 1);
	}
	if (team == Team::White) {
		black.setFillColor(sf::Color::Color(0, 0, 0, 127));
		this->window->draw(blackBtn);
		this->window->draw(black);
		this->window->draw(redBtn);
		this->window->draw(red);
	}
	else
	{
		red.setFillColor(sf::Color::Color(0, 0, 0, 127));
		this->window->draw(redBtn);
		this->window->draw(red);
		this->window->draw(blackBtn);
		this->window->draw(black);

	}
	return choice;
}

int Viewer::showConfirmSurrender()
{
	int choice = -1;
	sf::RectangleShape background(sf::Vector2f(1280, 576));
	background.setFillColor(sf::Color(0, 0, 0, 127));
	background.setPosition(0, 96);

	sf::Text confirm(L"確定要投降嗎？", font);
	confirm.setCharacterSize(80);
	confirm.setFillColor(sf::Color::White);
	confirm.setOrigin(240, 40);
	confirm.setPosition(640, 190);

	sf::Text yes(L"是", font);
	yes.setCharacterSize(50);
	yes.setFillColor(sf::Color::White);
	yes.setOrigin(25, 25);
	yes.setPosition(640, 350);

	sf::Text no(L"否", font);
	no.setCharacterSize(50);
	no.setFillColor(sf::Color::White);
	no.setOrigin(25, 25);
	no.setPosition(640, 450);

	if (yes.getGlobalBounds().contains(sf::Vector2f(this->getMousePosition())))
	{
		yes.setScale(1.2, 1.2);
		choice = 1;
	}
	else
	{
		yes.setScale(1, 1);
	}
	if (no.getGlobalBounds().contains(sf::Vector2f(this->getMousePosition())))
	{
		no.setScale(1.2, 1.2);
		choice = 0;
	}
	else
	{
		no.setScale(1, 1);
	}

	this->window->draw(background);
	this->window->draw(confirm);
	this->window->draw(yes);
	this->window->draw(no);

	return choice;
}

void Viewer::drawTime(sf::Time redTime, sf::Time blackTime)
{

	std::string redMin, redSec, blackMin, blackSec;
	redMin = std::to_string(int(redTime.asSeconds()) / 60);
	redSec = std::to_string(int(redTime.asSeconds()) % 60);
	blackMin = std::to_string(int(blackTime.asSeconds()) / 60);
	blackSec = std::to_string(int(blackTime.asSeconds()) % 60);
	if (int(redTime.asSeconds()) / 60 < 10) {
		redMin = "0" + redMin;
	}
	if (int(redTime.asSeconds()) % 60 < 10) {
		redSec = "0" + redSec;
	}
	if (int(blackTime.asSeconds()) / 60 < 10) {
		blackMin = "0" + blackMin;
	}
	if (int(blackTime.asSeconds()) % 60 < 10) {
		blackSec = "0" + blackSec;
	}

	sf::Text red(redMin + ":" + redSec, timeFont);
	red.setCharacterSize(60);
	red.setFillColor(sf::Color::White);
	red.setOutlineThickness(2);
	red.setOutlineColor(sf::Color::Black);
	red.setPosition(1000, 464);


	sf::Text black(blackMin + ":" + blackSec, timeFont);
	black.setCharacterSize(60);
	black.setFillColor(sf::Color::White);
	black.setOutlineThickness(2);
	black.setOutlineColor(sf::Color::Black);
	black.setPosition(1000, 80);

	this->window->draw(red);
	this->window->draw(black);

}

void Viewer::drawRightSideObject(Team team)
{
	//draw text
	sf::Text red(L"紅方", font);
	red.setCharacterSize(50);
	red.setStyle(sf::Text::Bold);
	red.setFillColor(sf::Color::Red);

	sf::Text black(red);
	black.setString(L"黑方");
	black.setFillColor(sf::Color::Black);
	black.setPosition(700, 0);
	red.setPosition(700, 384);
	this->window->draw(red);
	this->window->draw(black);

	//draw current player
	sf::CircleShape showCurrentTeam(15.f);
	showCurrentTeam.setFillColor(sf::Color::Red);
	showCurrentTeam.setOutlineThickness(1);
	showCurrentTeam.setOutlineColor(sf::Color::Black);
	if (team == Team::White) {
		showCurrentTeam.setPosition(660, 404);
	}
	else
	{
		showCurrentTeam.setPosition(660, 20);
	}
	this->window->draw(showCurrentTeam);


}

void Viewer::drawCanMovePos(std::vector<Coord> coords)
{
	for (auto coord : coords) {
		sf::CircleShape circle(30.3);
		float x, y;
		x = CHECKBOARD_SCALE_SIZE * (coord.x * 233.75 + 34) + 35.0625 - CHESS_SCALE_SIZE * (606.0 / 2);
		y = CHECKBOARD_SCALE_SIZE * (coord.y * 233.75 + 34) + 58.05 - CHESS_SCALE_SIZE * (606.0 / 2);
		circle.setPosition(x, y);
		circle.setFillColor(sf::Color(255, 0, 0, 127));
		this->window->draw(circle);
	}
}

void Viewer::drawSprite(std::vector<sf::Sprite> sprites)
{
	//draw
	for (const auto& sprite : sprites)
		this->window->draw(sprite);

}

void Viewer::display()
{

	this->window->display();
}