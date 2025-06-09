#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "ScoreBoardScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "Engine/LOG.hpp"

void ScoreBoardScene::loadRecord(){
    playerList.clear();
    std::ifstream fin("Resource/ScoreBoard.txt");
    Engine::LOG(Engine::INFO)<<"file loaded";

    std::string line;
    while (std::getline(fin, line)){
        Record newRec;
        std::stringstream ss(line);
        ss>>newRec.state>>newRec.lives>>newRec.datetime;
        playerList.push_back(newRec);
        //Engine::LOG(Engine::INFO)<<newRec.state<<"loaded";
    }
    std::sort(playerList.begin(), playerList.end());
}

void ScoreBoardScene::saveRecord(){
    std::ofstream fout("Resource/ScoreBoard.txt");
    for(auto& p: playerList){
        fout<<p.state<<" "<<p.lives<<" "<<p.datetime<<std::endl;
    }
    fout.flush();
    fout.close();
}

void ScoreBoardScene::displayRecord(){
    RecordGroup->Clear();
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    int start = cur_page * page_size;
    int end = std::min(cur_page * page_size + page_size, (int)playerList.size());
    for(int i=start; i<end; i++){
        Record& cur = playerList[i];
        std::string info = cur.state+" "+std::to_string(cur.lives);
        RecordGroup->AddNewObject(new Engine::Label(info, "pirulen.ttf", 50, halfW-150, 200+(i-start)*70, 255, 255, 255, 255, 0.5, 0.5));
        RecordGroup->AddNewObject(new Engine::Label(cur.datetime, "pirulen.ttf", 30, halfW+400, 200+(i-start)*70, 255, 255, 255, 255, 0.5, 0.5));
    }
}

void ScoreBoardScene::Initialize(){
    loadRecord();
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    cur_page = 0;
    AddNewObject(RecordGroup = new Group());
    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 80, halfW, 100, 255, 255, 255, 255, 0.5, 0.5));
    displayRecord();

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("GUI/button1.png", "GUI/button1.png", halfW - 250, halfH / 2 + 400, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH / 2 + 450, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("GUI/skip1.png", "GUI/skip1.png", halfW - 400, halfH / 2 + 400, 100, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::PrevOnClick, this));
    AddNewControlObject(btn);
    //AddNewObject(new Engine::Label("Prev", "pirulen.ttf", 48, halfW-500, halfH / 2 + 450, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("GUI/Skip.png", "GUI/Skip.png", halfW + 200, halfH / 2 + 400, 100, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene::NextOnClick, this));
    AddNewControlObject(btn);
    //AddNewObject(new Engine::Label("Next", "pirulen.ttf", 48, halfW+500, halfH / 2 + 450, 0, 0, 0, 255, 0.5, 0.5));

    bgmInstance = AudioHelper::PlaySample("others.ogg", true, AudioHelper::BGMVolume);
}

void ScoreBoardScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void ScoreBoardScene::BackOnClick(){
    Engine::GameEngine::GetInstance().ChangeScene("personal");
}

void ScoreBoardScene::PrevOnClick(){
    if(cur_page>0) {
        cur_page--;
        displayRecord();
    }
}

void ScoreBoardScene::NextOnClick(){
    if(cur_page < (playerList.size() / page_size)){
        cur_page++;
        displayRecord();
    }
}

