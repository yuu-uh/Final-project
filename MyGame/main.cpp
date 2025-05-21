// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LoseScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StageSelectScene.hpp"
#include "Scene/WinScene.hpp"
#include "Scene/StartScene.h"
#include "Scene/SettingsScene.hpp"
#include "Scene/ScoreBoardScene1.hpp"
#include "Scene/ScoreBoardScene2.hpp"
#include "Scene/LogScene.hpp"

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

    // TODO HACKATHON-2 (2/3): Register Scenes here 
    game.AddNewScene("stage-select", new StageSelectScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("lose", new LoseScene());
	game.AddNewScene("win", new WinScene());
	game.AddNewScene("settings", new SettingsScene());
	game.AddNewScene("start", new StartScene());
	game.AddNewScene("scoreboard1", new ScoreBoardScene1());
	game.AddNewScene("scoreboard2", new ScoreBoardScene2());
	game.AddNewScene("log", new LogScene());

    // TODO HACKATHON-1 (1/1): Change the start scene 
	game.Start("log", 60, 1600, 832);
	return 0;
}
