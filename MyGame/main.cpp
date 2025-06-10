#include "Engine/GameEngine.hpp"
#include "Engine/LOG.hpp"
#include "Scene/LogScene.hpp"
#include "Scene/PersonalScene.hpp"
#include "Scene/MapScene.hpp"
#include "Scene/PlayScene.hpp"
#include "Scene/StoryScene.hpp"
#include "Scene/ResultScene.hpp"
#include "Scene/SettingsScene.hpp"
#include "Engine/NetWork.hpp"
#include "Scene/ScoreBoardScene.hpp"

int main(int argc, char **argv) {
	if (!NetWork::Instance().Init()) {
        return -1;
    }
#ifdef _WIN32
	SetProcessDPIAware();	
#endif

	Engine::LOG::SetConfig(true);
	Engine::GameEngine& game = Engine::GameEngine::GetInstance();

    game.AddNewScene("log", new LogScene());
	game.AddNewScene("map", new MapScene());
	game.AddNewScene("personal", new PersonalScene());
	game.AddNewScene("play", new PlayScene());
	game.AddNewScene("result", new ResultScene());
	game.AddNewScene("scoreboard", new ScoreBoardScene());
	game.AddNewScene("setting", new SettingsScene());
	game.AddNewScene("story", new StoryScene());

	game.Start("story", 60, 1600, 832);
	return 0;
}
