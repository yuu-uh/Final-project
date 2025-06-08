#include <cstring>
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "Ws2_32.lib")
#else
  #include <sys/types.h>
  #include <ifaddrs.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
#endif
#include <allegro5/allegro_audio.h>
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

static std::string getLocalIP() {
#ifdef _WIN32
    WSADATA wsa{0};
    WSAStartup(MAKEWORD(2,2), &wsa);
    char name[256];
    gethostname(name, sizeof(name));
    addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_INET;
    if (getaddrinfo(name, nullptr, &hints, &res) == 0 && res) {
        sockaddr_in* sock = reinterpret_cast<sockaddr_in*>(res->ai_addr);
        char buf[25];
        inet_ntop(AF_INET, &sock->sin_addr, buf, sizeof(buf));
        freeaddrinfo(res);
        WSACleanup();
        return buf;
    }
    WSACleanup();
    return "127.0.0.1";
#endif
}

void PersonalScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int cx = w / 2;
    int cy = h / 2;
    Engine::ImageButton* btn;

    AddNewObject(new Engine::Image("background/test.png", 0,0,w,h));
    AddNewObject(new Engine::Label("Hello","pirulen.ttf",100,cx,cy/3+50,255,255,255,255,0.5,0.5));

    btn = new Engine::ImageButton("stage-select/floor.png","stage-select/floor.png",cx-200,cy/2+100,400,100);
    btn->SetOnClickCallback(std::bind(&PersonalScene::HostGame, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Host Game","pirulen.ttf",48,cx,cy/2+150,255,255,255,255,0.5,0.5));

    btn = new Engine::ImageButton("stage-select/floor.png","stage-select/floor.png",cx-200,cy/2+250,400,100);
    btn->SetOnClickCallback(std::bind(&PersonalScene::JoinGame, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Join Game","pirulen.ttf",48,cx,cy/2+300,255,255,255,255,0.5,0.5));

    btn = new Engine::ImageButton("stage-select/floor.png","stage-select/floor.png",cx-200,cy/2+400,400,100);
    btn->SetOnClickCallback(std::bind(&PersonalScene::ScoreOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Score","pirulen.ttf",48,cx,cy/2+450,255,255,255,255,0.5,0.5));
}

void PersonalScene::Update(float dt) {
    IScene::Update(dt);
    if (waitConn) {
        NetWork::Instance().Service(0);
    }
}

void PersonalScene::HostGame() {
    auto& net = NetWork::Instance();
    net.myId = 0;
    if (!net.Init()) return;
    host = true;
    if (!net.StartHost(1234,1)) {
        Engine::LOG(Engine::INFO) << "Host fail";
        return;
    }
    std::string ip = getLocalIP();
    Engine::LOG(Engine::INFO) << ip << ": " << 1234;
    net.SetReceiveCallback([this](const ENetEvent& e){
        if(e.type==ENET_EVENT_TYPE_CONNECT)
            Engine::GameEngine::GetInstance().ChangeScene("map");
    });
    waitConn = true;
}

void PersonalScene::JoinGame() {
    auto& net = NetWork::Instance();
    net.myId = 1;
    if (!net.Init()) return;
    std::string ip;
    int p;
    std::cout<<"IP:"; std::cin>>ip;
    std::cout<<"Port:"; std::cin>>p;
    if (!net.ConnectToHost(ip,(enet_uint16)p)) {
        Engine::LOG(Engine::INFO)<<"Connect fail";
        return;
    }
    net.SetReceiveCallback([this](const ENetEvent& e){
        if(e.type==ENET_EVENT_TYPE_CONNECT)
            Engine::GameEngine::GetInstance().ChangeScene("map");
    });
    waitConn = true;
}

void PersonalScene::ScoreOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("score");
}

void PersonalScene::Terminate() {
    IScene::Terminate();
    waitConn = false;
}
