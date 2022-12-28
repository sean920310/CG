#include "GameManager.h"

GameManager::GameManager()
{
	viewer = new Viewer(INIT_SCREEN_WIDTH, INIT_SCREEN_HEIGHT, "OpenGL");
}

void GameManager::Run()
{
	viewer->Init();
	viewer->Update();
}
