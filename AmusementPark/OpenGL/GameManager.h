#pragma once
#include "GameHeader.h"
#include "Viewer.h"

class GameManager
{
public:
	GameManager();

	void Run();

public:
	Viewer* viewer;
};

