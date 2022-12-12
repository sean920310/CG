#include "Board.h"

Board::Board() :m_boardArr(8, std::vector<Team>(8, Team::None))
{
	//initialize m_boardArr
	m_boardArr[4][3] = m_boardArr[3][4] = Team::Black;
	m_boardArr[4][4] = m_boardArr[3][3] = Team::White;
	m_blackNum = m_whiteNum = 2;
}

Board::Board(const Board& rhs) :m_boardArr(8, std::vector<Team>(8, Team::None))
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
				this->m_boardArr[j][i] = rhs.m_boardArr[j][i];
		}
	}
}

Board& Board::operator=(const Board& rhs)
{
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			this->m_boardArr[j][i] = rhs.m_boardArr[j][i];
		}
	}
	return *this;
}

void Board::newBoard()
{
	//Clear old board
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			this->m_boardArr[j][i] = Team::None;
		}
	}
	
	//initialize board
	m_boardArr[4][3] = m_boardArr[3][4] = Team::Black;
	m_boardArr[4][4] = m_boardArr[3][3] = Team::White;
	m_blackNum = m_whiteNum = 2;

}

Team Board::getChess(Coord coord) const
{
	if (coord.x > 7 || coord.y > 7)
		return Team::None;
	else
		return m_boardArr[coord.x][coord.y];
}

std::vector<std::vector<Team>> Board::getBoardArr()
{
	return m_boardArr;
}

int Board::getChessNum(Team team)
{
	if (team == Team::White)
		return m_whiteNum;
	else if (team == Team::Black)
		return m_blackNum;
	return 0;
}

std::vector<Coord> Board::coordCanPlace(Team currentTeam)
{
	std::vector<Coord> canPlace;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (m_boardArr[i][j] == Team::None)
			{
				Team opponentTeam = (currentTeam == Team::White)? Team::Black: Team::White;
				Coord dx(1, 0), dy(0, 1),now(i,j),temp;
				bool can = false, toggle = false;

				//x+
				temp = now;
				toggle = false;
				while (temp.x != 7 && !can)
				{
					temp = temp + dx;
					if (m_boardArr[temp.x][temp.y] == opponentTeam)
						toggle = true;
					else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
						can = true;
					else
						break;
				}
				//x-
				temp = now;
				toggle = false;
				while (temp.x != 0 && !can)
				{
					temp = temp - dx;
					if (m_boardArr[temp.x][temp.y] == opponentTeam)
						toggle = true;
					else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
						can = true;
					else
						break;
				}
				//y+
				temp = now;
				toggle = false;
				while (temp.y != 7 && !can)
				{
					temp = temp + dy;
					if (m_boardArr[temp.x][temp.y] == opponentTeam)
						toggle = true;
					else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
						can = true;
					else
						break;
				}
				//y-
				temp = now;
				toggle = false;
				while (temp.y != 0 && !can)
				{
					temp = temp - dy;
					if (m_boardArr[temp.x][temp.y] == opponentTeam)
						toggle = true;
					else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
						can = true;
					else
						break;
				}
				//x+y+
				temp = now;
				toggle = false;
				while (temp.x != 7 && temp.y != 7 && !can)
				{
					temp = temp + dx + dy;
					if (m_boardArr[temp.x][temp.y] == opponentTeam)
						toggle = true;
					else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
						can = true;
					else
						break;
				}
				//x+y-
				temp = now;
				toggle = false;
				while (temp.x != 7 && temp.y != 0 && !can)
				{
					temp = temp + dx - dy;
					if (m_boardArr[temp.x][temp.y] == opponentTeam)
						toggle = true;
					else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
						can = true;
					else
						break;
				}
				//x-y+
				temp = now;
				toggle = false;
				while (temp.x != 0 && temp.y != 7 && !can)
				{
					temp = temp - dx + dy;
					if (m_boardArr[temp.x][temp.y] == opponentTeam)
						toggle = true;
					else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
						can = true;
					else
						break;
				}
				//x-y-
				temp = now;
				toggle = false;
				while (temp.x != 0 && temp.y != 0 && !can)
				{
					temp = temp - dx - dy;
					if (m_boardArr[temp.x][temp.y] == opponentTeam)
						toggle = true;
					else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
						can = true;
					else
						break;
				}

				if (can)
				{
					canPlace.push_back(now);
					continue;
				}
			}
		}
	}
	return canPlace;
}

std::vector<Coord> Board::placeChess(Team currentTeam, Coord coord)
{
	m_boardArr[coord.x][coord.y] = currentTeam;

	//*flip
	std::vector<Coord> flips;
	Team opponentTeam = (currentTeam == Team::White) ? Team::Black : Team::White;
	Coord dx(1, 0), dy(0, 1), now(coord.x, coord.y), temp;
	bool can = false, toggle = false;

	//x+
	can = false;
	temp = now;
	toggle = false;
	while (temp.x != 7 && !can)
	{
		temp = temp + dx;
		if (m_boardArr[temp.x][temp.y] == opponentTeam)
			toggle = true;
		else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
			can = true;
		else
			break;
	}
	if (can)
	{
		temp = now;
		while (temp.x != 7)
		{
			temp = temp + dx;
			if (m_boardArr[temp.x][temp.y] == opponentTeam)
			{
				m_boardArr[temp.x][temp.y] = currentTeam;
				flips.push_back(Coord(temp.x, temp.y));
			}
			else
				break;
		}
	}
	//x-
	can = false;
	temp = now;
	toggle = false;
	while (temp.x != 0 && !can)
	{
		temp = temp - dx;
		if (m_boardArr[temp.x][temp.y] == opponentTeam)
			toggle = true;
		else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
			can = true;
		else
			break;
	}
	if (can)
	{
		temp = now;
		while (temp.x != 0)
		{
			temp = temp - dx;
			if (m_boardArr[temp.x][temp.y] == opponentTeam)
			{
				m_boardArr[temp.x][temp.y] = currentTeam;
				flips.push_back(Coord(temp.x, temp.y));
			}
			else
				break;
		}
	}
	//y+
	can = false;
	temp = now;
	toggle = false;
	while (temp.y != 7 && !can)
	{
		temp = temp + dy;
		if (m_boardArr[temp.x][temp.y] == opponentTeam)
			toggle = true;
		else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
			can = true;
		else
			break;
	}
	if (can)
	{
		temp = now;
		while (temp.y != 7)
		{
			temp = temp + dy;
			if (m_boardArr[temp.x][temp.y] == opponentTeam)
			{
				m_boardArr[temp.x][temp.y] = currentTeam;
				flips.push_back(Coord(temp.x, temp.y));
			}
			else
				break;
		}
	}
	//y-
	can = false;
	temp = now;
	toggle = false;
	while (temp.y != 0 && !can)
	{
		temp = temp - dy;
		if (m_boardArr[temp.x][temp.y] == opponentTeam)
			toggle = true;
		else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
			can = true;
		else
			break;
	}
	if (can)
	{
		temp = now;
		while (temp.y != 0)
		{
			temp = temp - dy;
			if (m_boardArr[temp.x][temp.y] == opponentTeam)
			{
				m_boardArr[temp.x][temp.y] = currentTeam;
				flips.push_back(Coord(temp.x, temp.y));
			}
			else
				break;
		}
	}
	//x+y+
	can = false;
	temp = now;
	toggle = false;
	while (temp.x != 7 && temp.y != 7 && !can)
	{
		temp = temp + dx + dy;
		if (m_boardArr[temp.x][temp.y] == opponentTeam)
			toggle = true;
		else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
			can = true;
		else
			break;
	}
	if (can)
	{
		temp = now;
		while (temp.x != 7 && temp.y != 7)
		{
			temp = temp + dx + dy;
			if (m_boardArr[temp.x][temp.y] == opponentTeam)
			{
				m_boardArr[temp.x][temp.y] = currentTeam;
				flips.push_back(Coord(temp.x, temp.y));
			}
			else
				break;
		}
	}
	//x+y-
	can = false;
	temp = now;
	toggle = false;
	while (temp.x != 7 && temp.y != 0 && !can)
	{
		temp = temp + dx - dy;
		if (m_boardArr[temp.x][temp.y] == opponentTeam)
			toggle = true;
		else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
			can = true;
		else
			break;
	}
	if (can)
	{
		temp = now;
		while (temp.x != 7 && temp.y != 0)
		{
			temp = temp + dx - dy;
			if (m_boardArr[temp.x][temp.y] == opponentTeam)
			{
				m_boardArr[temp.x][temp.y] = currentTeam;
				flips.push_back(Coord(temp.x, temp.y));
			}
			else
				break;
		}
	}
	//x-y+
	can = false;
	temp = now;
	toggle = false;
	while (temp.x != 0 && temp.y != 7 && !can)
	{
		temp = temp - dx + dy;
		if (m_boardArr[temp.x][temp.y] == opponentTeam)
			toggle = true;
		else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
			can = true;
		else
			break;
	}
	if (can)
	{
		temp = now;
		while (temp.x != 0 && temp.y != 7)
		{
			temp = temp - dx + dy;
			if (m_boardArr[temp.x][temp.y] == opponentTeam)
			{
				m_boardArr[temp.x][temp.y] = currentTeam;
				flips.push_back(Coord(temp.x, temp.y));
			}
			else
				break;
		}
	}
	//x-y-
	can = false;
	temp = now;
	toggle = false;
	while (temp.x != 0 && temp.y != 0 && !can)
	{
		temp = temp - dx - dy;
		if (m_boardArr[temp.x][temp.y] == opponentTeam)
			toggle = true;
		else if (m_boardArr[temp.x][temp.y] == currentTeam && toggle)
			can = true;
		else
			break;
	}
	if (can)
	{
		temp = now;
		while (temp.x != 0 && temp.y != 0)
		{
			temp = temp - dx - dy;
			if (m_boardArr[temp.x][temp.y] == opponentTeam)
			{
				m_boardArr[temp.x][temp.y] = currentTeam;
				flips.push_back(Coord(temp.x, temp.y));
			}
			else
				break;
		}
	}

	//*count
	m_blackNum =  m_whiteNum = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (m_boardArr[i][j] == Team::White)
				m_whiteNum++;
			else if (m_boardArr[i][j] == Team::Black)
				m_blackNum++;
		}
	}

	return flips;

}

bool Board::oneSideIsWin(Team& team)
{
	std::vector<Coord> whiteCanPlace = coordCanPlace(Team::White);
	std::vector<Coord> blackCanPlace = coordCanPlace(Team::Black);

	if (whiteCanPlace.size()|| blackCanPlace.size())
		return false;

	if (m_whiteNum > m_blackNum)
		team = Team::White;
	else if (m_whiteNum < m_blackNum)
		team = Team::Black;
	else
		team = Team::None;

	return true;
}
