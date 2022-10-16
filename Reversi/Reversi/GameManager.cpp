#include "GameManager.h"

GameManager::GameManager():board(),viewer()
{
	currentPlayer = Team::Black;
	//one side time limit
	timeLimit = sf::seconds(TIMELIMIT * 60);
}

int GameManager::menu()
{
	while (viewer.windowIsOpen())
	{
		viewer.update();
		viewer.clear();
		if (viewer.mouseClick(sf::Mouse::Left)) {
			switch (viewer.showMenu())
			{
			case 0:
				viewer.close();
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
			viewer.showMenu();
		}
		viewer.display();
	}
	return 0;
}

void GameManager::inGame(InGameState state)
{
	std::vector<Coord> canMovePosCoord;
	Coord coordChoiseChess, coordChoiseToMove;
	bool isCheck = false, oneSurrender = false, timeUp = false;
	Team teamCheck, teamWin;
	whiteTime = timeLimit;
	blackTime = timeLimit;
	clock.restart();

	while (viewer.windowIsOpen())
	{
	//	switch (state)
	//	{
	//		//======================================================start====================================================	**start a new game
	//	case InGameState::start:
	//		this->file.close();
	//		this->board.newBoard();
	//		onBoard = board.getAllChess();
	//		currentPlayer = Team::Red;
	//		isCheck = false;
	//		oneSurrender = false;
	//		timeUp = false;
	//		redTime = timeLimit;
	//		blackTime = timeLimit;
	//		clock.restart();
	//		state = InGameState::selectChess;
	//		break;

	//		//====================================================inputFile==================================================	**start a game with input file
	//	case InGameState::inputFile:
	//		state = InGameState::oneSideWin;
	//		break;
	//		//====================================================selectChess================================================	**wait for press a object
	//	case InGameState::selectChess:
	//		for (auto& chess : onBoard) {
	//			chess->showSelect(viewer.getMousePosition(), currentPlayer);
	//			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && chess->isChoice() && currentPlayer == chess->getTeam()) {
	//				state = InGameState::pressChess;
	//				break;
	//			}
	//		}
	//		for (auto& coord : canMovePosCoord) {
	//			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && isChoiceToMove(viewer.getMousePosition(), coord)) {
	//				coordChoiseToMove = coord;
	//				state = InGameState::choiceMove;
	//				break;
	//			}
	//		}
	//		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && state != InGameState::pressChess) {
	//			canMovePosCoord.clear();
	//		}
	//		break;

	//		//====================================================pressChess================================================	**show the coord can move to
	//	case InGameState::pressChess:
	//		for (auto& chess : onBoard) {
	//			if (chess->isChoice()) {
	//				coordChoiseChess = chess->getCoord();
	//				std::cout << chess->getCoord().x << " " << chess->getCoord().y << " is press.\n";
	//				canMovePosCoord.clear();
	//				canMovePosCoord = chess->coordCanMove(board);
	//				chess->removeWillCheckCoord(board, canMovePosCoord);
	//				state = InGameState::selectChess;
	//				break;
	//			}
	//		}
	//		break;

	//		//====================================================choiceMove================================================	**move the chess
	//	case InGameState::choiceMove:
	//		//board.getChess(coordChoiseChess)->move(board,coordChoiseToMove);
	//		this->logFile(coordChoiseChess, coordChoiseToMove);
	//		board.moveChess(coordChoiseChess, coordChoiseToMove);
	//		state = InGameState::oneSideWin;
	//		break;

	//		//====================================================isCheck===================================================	**judge if one side is check or win
	//	case InGameState::isCheck:
	//		teamCheck = currentPlayer;
	//		if (board.oneSideIsCheck(teamCheck)) {
	//			isCheck = true;
	//		}
	//		else
	//		{
	//			isCheck = false;
	//		}
	//		state = InGameState::selectChess;

	//		if (currentPlayer == Team::Red)
	//			currentPlayer = Team::Black;
	//		else
	//			currentPlayer = Team::Red;
	//		break;

	//		//==================================================oneSideWin==================================================	**decide to play anthor game
	//	case InGameState::oneSideWin:
	//		//surrender
	//		if (oneSurrender) {
	//			if (this->endGame(teamWin))
	//				state = InGameState::start;
	//			else
	//				return;
	//			continue;
	//		}

	//		//times up
	//		if (timeUp) {
	//			if (this->endGame(teamWin))
	//				state = InGameState::start;
	//			else
	//				return;
	//			continue;
	//		}

	//		//normal win
	//		teamWin = currentPlayer;
	//		if (board.oneSideIsWin(teamWin)) {
	//			if (this->endGame(teamWin))
	//				state = InGameState::start;
	//			else
	//				return;
	//			continue;
	//		}
	//		else
	//		{
	//			state = InGameState::isCheck;
	//		}
	//		break;
	//	}

	//	//clock
	//	if (currentPlayer == Team::Red)
	//	{
	//		redTime = redTime - clock.getElapsedTime();
	//	}
	//	else
	//	{
	//		blackTime = blackTime - clock.getElapsedTime();
	//	}
	//	clock.restart();

	//	if (redTime <= sf::Time::Zero) {
	//		teamWin = Team::Black;
	//		timeUp = true;
	//		state = InGameState::oneSideWin;
	//	}
	//	if (blackTime <= sf::Time::Zero) {
	//		teamWin = Team::Red;
	//		timeUp = true;
	//		state = InGameState::oneSideWin;
	//	}

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
	//	viewer.drawCanMovePos(canMovePosCoord);
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

		viewer.display();
	}
}
