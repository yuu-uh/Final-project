#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <tuple>
#include <sstream>
#include <fstream>

#include "Engine/NetWork.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PersonalScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "UI/Component/TextBox.hpp"

void PersonalScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    AddNewObject(new Engine::Image("background/test.png" ,0, 0, w, h));
    AddNewObject(new Engine::Label("Hello", "pirulen.ttf", 100, halfW, halfH/3+50, 255, 255, 255, 255, 0.5, 0.5));
    
    btn = new Engine::ImageButton("stage-select/floor.png", "stage-select/floor.png", halfW-200, halfH/2+100, 400, 100);
    AddNewControlObject(btn);
    btn->SetOnClickCallback(std::bind(&PersonalScene::HostGame, this));
    AddNewObject(new Engine::Label("Host Game", "pirulen.ttf", 48, halfW, halfH/2+150, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/floor.png", "stage-select/floor.png", halfW-200, halfH/2+250, 400, 100);
    AddNewControlObject(btn);
    btn->SetOnClickCallback(std::bind(&PersonalScene::JoinGame, this));
    AddNewObject(new Engine::Label("Join Game", "pirulen.ttf", 48, halfW, halfH/2+300, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/floor.png", "stage-select/floor.png", halfW-200, halfH/2+400, 400, 100);
    AddNewControlObject(btn);
    btn->SetOnClickCallback(std::bind(&PersonalScene::ScoreOnClick, this));
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", 48, halfW, halfH/2+450, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/floor.png", "stage-select/floor.png", halfW-200, halfH+450, 400, 100);
    AddNewControlObject(btn);
    btn->SetOnClickCallback(std::bind(&PersonalScene::ConfirmJoin, this));
    AddNewObject(new Engine::Label("Connect", "pirulen.ttf", 48, halfW, halfH+500, 255, 255, 255, 255, 0.5, 0.5));


    IPEnter = new Engine::TextBox(halfW-350, halfH-100, 700, 100, 0.5, 0.5); 
    portEenter = new Engine::TextBox(halfW-350, halfH+200, 700, 100, 0.5, 0.5); 
    font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 48);

}
void PersonalScene::Draw() const{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    Group::Draw();
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    if(join_mode){
        al_draw_filled_rectangle(0, 0, w, h, al_map_rgba(0, 0, 0, 200));
        al_draw_text(font.get(), al_map_rgb(255, 255, 255), halfW, halfH-200, ALLEGRO_ALIGN_CENTER, "IP");
        al_draw_text(font.get(), al_map_rgb(255, 255, 255), halfW, halfH+100, ALLEGRO_ALIGN_CENTER, "Port");
        IPEnter->Draw();
        portEenter->Draw();
    }
}

void PersonalScene::OnKeyDown(int keycode){
    if (!join_mode) return;

    if (keycode == ALLEGRO_KEY_ENTER) {
        if (focusedBox == IPEnter) {
            focusedBox = portEenter;  // Move focus to port
        } else if (focusedBox == portEenter) {
            ConfirmJoin(); // Automatically try to join
        }
    } else {
        if (focusedBox)
            focusedBox->OnKeyDown(keycode);
    }
}

void PersonalScene::Terminate() {
    IScene::Terminate();
}
void PersonalScene::HostGame() {
    auto& net = NetWork::Instance();
    if (!net.Init()) return;
    if (host) {
        if (!net.StartHost(1234, 1)) {   // port maxClient 
            Engine::LOG(Engine::INFO) << "啟動 Host 失敗";
            return;
        }
    } 
    else {
        if (!net.ConnectToHost(_peerIp, 1234)) {
            Engine::LOG(Engine::INFO) << "無法連線";
            return;
        }
    }
    ENetHost* server = net.GetHost(); 
    ENetAddress addr;
    enet_socket_get_address(server->socket, &addr);
    char ipstr[25];
    enet_address_get_host_ip(&addr, ipstr, sizeof(ipstr));
    Engine::LOG(Engine::INFO) << "Host IP: "  << std::string(ipstr) << " Port: 1234";

    net.SetReceiveCallback([this](const ENetEvent& ev) {
        // 收到對方資料時，塞到 PlayScene 或某個全域玩家狀態裡
        // e.g. PlayScene::Instance().OnRemoteState(ev.packet->data, ev.packet->dataLength);
    });
    Engine::GameEngine::GetInstance().ChangeScene("map");
}
// void PersonalScene::JoinGame(){
//     auto& net = NetWork::Instance();
//     if (!net.Init()) return;
//     join_mode = true;
//     std::string ip;
//     std::cout << "Enter Host IP: ";
//     std::cin  >> ip;
//     int port;
//     std::cout << "Enter Host Port: ";
//     std::cin  >> port;

//     if (!net.ConnectToHost(ip, static_cast<enet_uint16>(port))) {
//         Engine::LOG(Engine::INFO) << "無法連線到 " << ip << ":" << port;
//         return;
//     }
//     net.SetReceiveCallback([this](const ENetEvent& ev) {
       
//     });
//     Engine::GameEngine::GetInstance().ChangeScene("map");
// }

void PersonalScene::JoinGame() {
    join_mode = true;
    focusedBox = IPEnter;
}

void PersonalScene::ConfirmJoin() {
    auto& net = NetWork::Instance();
    if (!net.Init()) return;

    std::string ip = IPEnter->name;
    int port = std::stoi(portEenter->name);

    if (!net.ConnectToHost(ip, static_cast<enet_uint16>(port))) {
        Engine::LOG(Engine::INFO) << "無法連線到 " << ip << ":" << port;
        return;
    }

    net.SetReceiveCallback([this](const ENetEvent& ev) {
        // Handle network event.
    });

    Engine::GameEngine::GetInstance().ChangeScene("map");
}



void PersonalScene::ScoreOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("score");
}