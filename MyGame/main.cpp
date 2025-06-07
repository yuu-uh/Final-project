// [main.cpp]
// This is the entry point of your game.
// You can register your scenes here, and start the game.
#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LogScene.hpp"
#include "Scene/PersonalScene.hpp"
#include "Scene/MapScene.hpp"

int main(int argc, char **argv) {
	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

    game.AddNewScene("log", new LogScene());
	game.AddNewScene("map", new MapScene());
	game.AddNewScene("personal", new PersonalScene());
	// game.AddNewScene("play", new PlayScene());
	// game.AddNewScene("result", new ResultScene());
	// game.AddNewScene("scoreboard", new ScoreBoardScene());
	// game.AddNewScene("settings", new SettingsScene());
	// game.AddNewScene("story", new StoryScene());

    // TODO HACKATHON-1 (1/1): Change the start scene 
	game.Start("personal", 60, 1600, 832);
	return 0;
}
