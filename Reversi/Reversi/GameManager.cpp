#include "GameManager.h"

//*********************************************************************
// 
// Function
// 
//*********************************************************************




//*********************************************************************
// 
// GameManager Class
// 
//*********************************************************************

GameManager::GameManager()
{
	m_currentPlayer = Team::Black;
	m_board = new Board();
	m_viewer = new Viewer(this);
	backgroundMusic.openFromFile(MUSIC_PATH);
}

GameManager::~GameManager()
{
	if (m_board)
		delete m_board;
	if (m_viewer)
		delete m_viewer;
}

void GameManager::run()
{
	backgroundMusic.setLoop(true);
	backgroundMusic.play();
	backgroundMusic.setVolume(50);
	while (true)
	{
		switch (this->menu())
		{
		case 0:
			return;
		case 1:
			this->onePlayerGame(InGameState::start);
			break;
		case 2:
			this->twoPlayerGame(InGameState::start);
			break;
		default:
			break;
		}
	}
}

int GameManager::menu()
{
	while (m_viewer->windowIsOpen())
	{
		m_viewer->update();
		m_viewer->clear();
		if (m_viewer->mouseClick(sf::Mouse::Left)) {
			switch (m_viewer->showMenu())
			{
			case 0:
				m_viewer->close();
				return 0;
				break;
			case 1:
				return 1;
				break;
			case 2:
				return 2;
				break;
			default:
				break;
			}
		}
		else {
			m_viewer->showMenu();
		}
		m_viewer->display();
	}
	return 0;
}

void GameManager::onePlayerGame(InGameState state)
{
	
	std::vector<Coord> canPlacePosCoord, flips;
	Coord coordChoiseToPlace;
	Team teamWin = Team::None, AITeam = Team::White;
	const sf::Time aiWait = sf::seconds(1), flipTime = sf::milliseconds(500);
	sf::Clock aiClock, flipClock;
	aiClock.restart();

	while (m_viewer->windowIsOpen())
	{
		switch (state)
		{
			//======================================================start====================================================	**start a new game
		case InGameState::start:
			srand(time(NULL));
			m_file.close();
			m_board->newBoard();
			m_currentPlayer = Team::Black;
			aiClock.restart();
			state = InGameState::canPlace;
			break;

			//====================================================canPlace==================================================	**load coord can place chess
		case InGameState::canPlace:
			canPlacePosCoord = m_board->coordCanPlace(m_currentPlayer);
			if (!canPlacePosCoord.size())
				state = InGameState::oneSideWin;
			else
				state = InGameState::selectChess;
			break;

			//====================================================selectChess================================================	**wait for press a object
		case InGameState::selectChess:
			if (m_currentPlayer == AITeam)
			{
				if(aiClock.getElapsedTime() >= aiWait)
				{
					aiClock.restart();
					int size = canPlacePosCoord.size();
					int i = rand() % size;
					coordChoiseToPlace = canPlacePosCoord[i];
					state = InGameState::choiceMove;
					break;
				}
			}
			else
			{
				aiClock.restart();
				if (m_viewer->mouseClick(sf::Mouse::Left))
				{
					auto pos = m_viewer->getMousePosition();
					for (auto& coord : canPlacePosCoord) {
						if (m_viewer->isChoiceToMove(pos, coord)) {
							coordChoiseToPlace = coord;
							state = InGameState::choiceMove;
							break;
						}
					}
				}
			}
			break;

			//====================================================choiceMove================================================	**move the chess
		case InGameState::choiceMove:
			flips = m_board->placeChess(m_currentPlayer, coordChoiseToPlace);
			canPlacePosCoord.clear();
			flipClock.restart();
			state = InGameState::flipChess;
			break;

			//====================================================flipChess================================================	**flip the chess
		case InGameState::flipChess:
			if(flipClock.getElapsedTime() >= flipTime)
				state = InGameState::oneSideWin;
			break;

			//==================================================oneSideWin==================================================	**decide to play anthor game
		case InGameState::oneSideWin:
			//normal win
			teamWin = m_currentPlayer;
			if (m_board->oneSideIsWin(teamWin)) {
				if (this->endGame(teamWin))
					//chose for a new game
					state = InGameState::start;
				else
					//quit
					return;
				continue;
			}
			else
			{
				if (m_currentPlayer == Team::White)
					m_currentPlayer = Team::Black;
				else
					m_currentPlayer = Team::White;
				state = InGameState::canPlace;
			}
			break;
		}

		//update
		switch (m_viewer->update())
		{
		case 0:
			break;
		case 1:
			std::cout << "windows close\n";
			return;
			break;
		case 2:
			std::cout << "pause\n";

			switch (this->pause())
			{
			case 0:
				std::cout << "windows close\n";
				return;
				break;
			case 1:
				std::cout << "continue\n";
				break;
			case 2:
				std::cout << "back to menu\n";
				return;
				break;
			}
			break;
		}

		//draw
		m_viewer->clear();
		m_viewer->drawBoard();
		if(state == InGameState::flipChess)
			m_viewer->drawChess((float)(flipClock.getElapsedTime() / flipTime), flips);
		else
			m_viewer->drawChess();
		m_viewer->drawCurrentPlayer(m_currentPlayer);
		m_viewer->drawCanMovePos(canPlacePosCoord);

		m_viewer->display();
	}
}

void GameManager::twoPlayerGame(InGameState state)
{
	std::vector<Coord> canPlacePosCoord, flips;
	Coord coordChoiseToPlace;
	Team teamWin = Team::None;
	const sf::Time flipTime = sf::milliseconds(500);
	sf::Clock flipClock;

	while (m_viewer->windowIsOpen())
	{
		switch (state)
		{
			//======================================================start====================================================	**start a new game
		case InGameState::start:
			m_file.close();
			m_board->newBoard();
			m_currentPlayer = Team::Black;
			state = InGameState::canPlace;
			break;

			//====================================================canPlace==================================================	**load coord can place chess
		case InGameState::canPlace:
			canPlacePosCoord = m_board->coordCanPlace(m_currentPlayer);
			if (!canPlacePosCoord.size())
				state = InGameState::oneSideWin;
			else
				state = InGameState::selectChess;
			break;

			//====================================================selectChess================================================	**wait for press a object
		case InGameState::selectChess:
			if (m_viewer->mouseClick(sf::Mouse::Left))
			{
				auto pos = m_viewer->getMousePosition();
				for (auto& coord : canPlacePosCoord) {
					if (m_viewer->isChoiceToMove(pos, coord)) {
						coordChoiseToPlace = coord;
						state = InGameState::choiceMove;
						break;
					}
				}
			}
			break;

			//====================================================choiceMove================================================	**move the chess
		case InGameState::choiceMove:
			flips = m_board->placeChess(m_currentPlayer, coordChoiseToPlace);
			canPlacePosCoord.clear();
			flipClock.restart(); 
			state = InGameState::flipChess;
			break;

			//====================================================flipChess================================================	**flip the chess
		case InGameState::flipChess:
			if (flipClock.getElapsedTime() >= flipTime)
				state = InGameState::oneSideWin;
			break;

			//==================================================oneSideWin==================================================	**decide to play anthor game
		case InGameState::oneSideWin:
			//normal win
			teamWin = m_currentPlayer;
			if (m_board->oneSideIsWin(teamWin)) {
				if (this->endGame(teamWin))
					state = InGameState::start;
				else
					return;
				continue;
			}
			else
			{
				if (m_currentPlayer == Team::White)
					m_currentPlayer = Team::Black;
				else
					m_currentPlayer = Team::White;
				state = InGameState::canPlace;
			}
			break;
		}

		//update
		switch (m_viewer->update())
		{
		case 0:
			break;
		case 1:
			std::cout << "windows close\n";
			return;
			break;
		case 2:
			std::cout << "pause\n";

			switch (this->pause())
			{
			case 0:
				std::cout << "windows close\n";
				return;
				break;
			case 1:
				std::cout << "continue\n";
				break;
			case 2:
				std::cout << "back to menu\n";
				return;
				break;
			}
			break;
		}

		//draw
		m_viewer->clear();
		m_viewer->drawCurrentPlayer(m_currentPlayer);
		m_viewer->drawBoard();
		if (state == InGameState::flipChess)
			m_viewer->drawChess((float)(flipClock.getElapsedTime() / flipTime), flips);
		else
			m_viewer->drawChess();
		m_viewer->drawCanMovePos(canPlacePosCoord);

		m_viewer->display();
	}
}

bool GameManager::endGame(Team team)
{
	while (m_viewer->windowIsOpen()) {
		//update
		switch (m_viewer->update())
		{
		case 0:
			break;
		case 1:
			return false;
			break;
		case 2:
			return false;
			break;
		default:
			break;
		}

		m_viewer->clear();
		m_viewer->drawCurrentPlayer(m_currentPlayer);
		m_viewer->drawBoard();
		m_viewer->drawChess();
		if (m_viewer->mouseClick(sf::Mouse::Left)) {
			switch (m_viewer->showWinner(team))
			{
			case -1:
				break;
			case 0:
				return false;
			case 1:
				return true;
			default:
				break;
			}
		}
		else
			m_viewer->showWinner(team);
		m_viewer->display();
	}
	return false;
}

int GameManager::pause()
{
	while (m_viewer->windowIsOpen())
	{

		//update
		switch (m_viewer->update())
		{
		case 0:
			break;
		case 1:
			return 0;
			break;
		case 2:
			return 1;
			break;
		default:
			break;
		}

		//draw
		m_viewer->clear();
		m_viewer->drawCurrentPlayer(m_currentPlayer);
		m_viewer->drawBoard();
		m_viewer->drawChess();
		if (m_viewer->mouseClick(sf::Mouse::Left)) {
			switch (m_viewer->showPause())
			{
			case 0:
				return 1;
				break;
			case 1:
				return 2;
				break;
			default:
				break;
			}
		}
		else
			m_viewer->showPause();

		m_viewer->display();
	}
	return 0;
}
