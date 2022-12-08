#include "GameManager.h"

//*********************************************************************
// 
// Function
// 
//*********************************************************************

//mouse postion is on coord that choice to move
bool isChoiceToMove(sf::Vector2i mouseCoord, Coord coord) {

	float x, y;
	x = CHECKBOARD_SCALE_SIZE * (coord.x * 233.75 + 34) + 35.0625 - CHESS_SCALE_SIZE * (606.0 / 2);
	y = CHECKBOARD_SCALE_SIZE * (coord.y * 233.75 + 34) + 58.05 - CHESS_SCALE_SIZE * (606.0 / 2);
	sf::IntRect circle(x, y, 60, 60);
	return circle.contains(mouseCoord);
}


//*********************************************************************
// 
// GameManager Class
// 
//*********************************************************************

GameManager::GameManager()
{
	m_currentPlayer = Team::Black;
	m_board = new Board();
	m_viewer = new Viewer();
}

GameManager::~GameManager()
{
	if (m_board)
		delete m_board;
	if (m_viewer)
		delete m_viewer;
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

void GameManager::inGame(InGameState state)
{
	std::vector<std::vector<Team>> boardArr;
	std::vector<Coord> canPlacePosCoord;
	Coord coordChoiseChess, coordChoiseToPlace;
	bool isCheck = false;
	Team teamCheck = Team::None, teamWin = Team::None;

	while (m_viewer->windowIsOpen())
	{
		switch (state)
		{
			//======================================================start====================================================	**start a new game
		case InGameState::start:
			m_file.close();
			m_board->newBoard();
			boardArr = m_board->getBoardArr();
			m_currentPlayer = Team::Black;
			isCheck = false;
			state = InGameState::canPlace;
			break;

			//====================================================inputFile==================================================	**start a game with input file
		case InGameState::inputFile:
			state = InGameState::oneSideWin;
			break;

			//====================================================canPlace==================================================	**load coord can place chess
		case InGameState::canPlace:
			canPlacePosCoord = m_board->coordCanPlace(m_currentPlayer);
			state = InGameState::selectChess;
			break;

			//====================================================selectChess================================================	**wait for press a object
		case InGameState::selectChess:
			for (auto& coord : canPlacePosCoord) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && isChoiceToMove(m_viewer->getMousePosition(), coord)) {
					coordChoiseToPlace = coord;
					state = InGameState::choiceMove;
					break;
				}
			}
			state = InGameState::selectChess;
			break;

			//====================================================choiceMove================================================	**move the chess
		case InGameState::choiceMove:
			//board.getChess(coordChoiseChess)->move(board,coordChoiseToPlace);
			//this->logFile(coordChoiseChess, coordChoiseToPlace);
			m_board->placeChess(coordChoiseToPlace);
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

	//	//update
	//	switch (viewer.update())
	//	{
	//	case 0:
	//		break;
	//	case 1:
	//		std::cout << "windows close\n";
	//		return;
	//		break;
	//	case 2:
	//		std::cout << "pause\n";

	//		switch (this->pause())
	//		{
	//		case 0:
	//			std::cout << "windows close\n";
	//			return;
	//			break;
	//		case 1:
	//			std::cout << "continue\n";
	//			clock.restart();
	//			break;
	//		case 2:
	//			std::cout << "back to menu\n";
	//			return;
	//			break;
	//		}
	//		break;
	//	}

	//	//draw
	//	viewer.clear();
	//	auto boardSprites = this->board.getAllSprite();
	//	viewer.drawSprite(boardSprites);
	//	viewer.drawCanMovePos(canPlacePosCoord);
	//	viewer.drawRightSideObject(currentPlayer);
	//	viewer.drawTime(redTime, blackTime);

	//	//surrender
	//	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
	//		switch (viewer.showSurrender(true, currentPlayer))
	//		{
	//		case 0:
	//			if (this->confirmSurrender()) {
	//				teamWin = Team::Red;
	//				oneSurrender = true;
	//				state = InGameState::oneSideWin;
	//			}
	//			break;
	//		case 1:
	//			if (this->confirmSurrender()) {
	//				teamWin = Team::Black;
	//				oneSurrender = true;
	//				state = InGameState::oneSideWin;
	//			}
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//	else
	//		viewer.showSurrender(true, currentPlayer);

	//	if (isCheck)
	//		viewer.showCheck(teamCheck);

		m_viewer->display();
	}
}

bool GameManager::endGame(Team team)
{
	//TODO: show winner and chose what to do next
	return false;
}
