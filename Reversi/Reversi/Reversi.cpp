#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "GameManager.h"

int main()
{
    GameManager game;
    //while (1)
    //{
    //    switch (game.menu())
    //    {
    //    case 0:
    //        return 0;
    //        break;
    //    case 1:
    //        game.inGame(InGameState::start);
    //        break;
    //    case 2:
    //        try
    //        {
    //            game.readFile();
    //        }
    //        catch (const char* e)
    //        {
    //            std::cout << e << std::endl;
    //            break;
    //        }
    //        game.inGame(InGameState::inputFile);
    //        break;
    //    default:
    //        break;
    //    }
    //}
    while (true)
    {
        switch (game.menu())
        {
        case 0:
            return 0;
        default:
            break;
        }
    }
    //end of application
    return 0;
}