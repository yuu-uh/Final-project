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
#include <allegro5/allegro_primitives.h>
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
#include "UI/Component/TextBox.hpp"

const std::vector<std::string> PersonalScene::JobNames =  { "Engineer", "Mage", "Paladin", "Rogue" };
void PersonalScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton* btn;

    AddNewObject(new Engine::Image("background/test.png", 0, 0, w, h));
    AddNewObject(new Engine::Label("Mirror Duel", "pirulen.ttf", 100, halfW - 50, 120, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("GUI/button3.png", "GUI/button3.png", halfW - 630, halfH / 2+10, 500, 120);
    btn->SetOnClickCallback(std::bind(&PersonalScene::HostGame, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Host Game", "pirulen.ttf", 36, halfW -320, halfH / 2 +70, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("GUI/button1.png", "GUI/button1.png", halfW - 630, halfH / 2 + 160, 500, 120);
    btn->SetOnClickCallback(std::bind(&PersonalScene::JoinGame, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Join Game", "pirulen.ttf", 36, halfW -320, halfH / 2+220, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("GUI/button2.png", "GUI/button2.png", halfW - 630, halfH / 2 + 290, 500, 120);
    btn->SetOnClickCallback(std::bind(&PersonalScene::ScoreOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", 36, halfW - 320, halfH / 2 +350, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("GUI/button2.png", "GUI/button2.png", halfW - 630, halfH / 2 +430, 500, 120);
    btn->SetOnClickCallback(std::bind(&PersonalScene::SettingOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Setting", "pirulen.ttf", 36, halfW - 320, halfH / 2 +490, 255, 255, 255, 255, 0.5, 0.5));

    AddNewObject(new Engine::Image("GUI/player.png", 800, 200, 516, 568));
    
    currentJobIndex = 0;
    jobPreview = new Engine::Image("mapScene/"+ JobNames[currentJobIndex] +".png", 870, 370, 300,260);
    AddNewObject(jobPreview);
    jobLabel = new Engine::Label(JobNames[currentJobIndex], "pirulen.ttf", 32, 980, 700,255,255,255,255,0.5,0.5);
    AddNewObject(jobLabel);
    auto skip = new Engine::ImageButton("GUI/Skip.png","GUI/Skip.png",1120, 670, 60,60);
    skip->SetOnClickCallback([this](){
        currentJobIndex = (currentJobIndex + 1) % JobNames.size();
        std::string newPath = "mapScene/" + JobNames[currentJobIndex] + ".png";
        RemoveObject(jobPreview->GetObjectIterator());
        jobPreview = new Engine::Image(newPath, 870, 370, 300,260);
        AddNewObject(jobPreview);
        jobLabel->Text = JobNames[currentJobIndex];
    });
    AddNewControlObject(skip);

    IPEnter = new Engine::TextBox(halfW-350, halfH-150, 700, 100, 0.5, 0.5); 
    portEenter = new Engine::TextBox(halfW-350, halfH+150, 700, 100, 0.5, 0.5); 
    font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 48);
    bgmInstance = AudioHelper::PlaySample("others.ogg", true, AudioHelper::BGMVolume);
}

void PersonalScene::Update(float dt) {
    IScene::Update(dt);
    if (waitConn) {
        auto& net = NetWork::Instance();
        net.Service(0);
        if (net.isConnected()) {
            hostMode = false;
            Engine::LOG(Engine::INFO) << "NetWork::isConnected() == true, go to mapScene";
            Terminate();
        }
    }
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
        al_draw_text(font.get(), al_map_rgb(255, 255, 255), halfW, halfH-250, ALLEGRO_ALIGN_CENTER, "IP");
        al_draw_text(font.get(), al_map_rgb(255, 255, 255), halfW, halfH+50, ALLEGRO_ALIGN_CENTER, "Port");
        IPEnter->Draw();
        portEenter->Draw();
    }
    if (hostMode) {
        al_draw_filled_rectangle(0,0,w,h, al_map_rgba(0,0,0,200));
        ALLEGRO_FONT* f = font.get();
        al_draw_text(f, al_map_rgb(255,255,255), halfW, halfH-70, ALLEGRO_ALIGN_CENTER, hostIdInfo.c_str());
        al_draw_text(f, al_map_rgb(255,255,255), halfW, halfH, ALLEGRO_ALIGN_CENTER, hostPortInfo.c_str());
        al_draw_text(f, al_map_rgb(255,255,255), halfW, halfH+70, ALLEGRO_ALIGN_CENTER, "Waiting for client…");
        return; 
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

void PersonalScene::HostGame() {
    Engine::GameEngine::job = JobNames[currentJobIndex];
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
        if(e.type==ENET_EVENT_TYPE_CONNECT){
            join_mode = false;
            PacketHeader hdr{ MSG_JOB_SELECT, sizeof(uint8_t) };
            uint8_t buf[sizeof(hdr)+1];
            std::memcpy(buf, &hdr, sizeof(hdr));
            uint8_t jobId = static_cast<uint8_t>(currentJobIndex);
            std::memcpy(buf + sizeof(hdr), &jobId, 1);
            NetWork::Instance().Send(buf, sizeof(buf), 0);
        }
        else if (e.type == ENET_EVENT_TYPE_RECEIVE) {
            PacketHeader hdr;
            std::memcpy(&hdr, e.packet->data, sizeof(hdr));
            if (hdr.type == MSG_JOB_SELECT) {
                uint8_t clientJob = *(e.packet->data + sizeof(hdr));
                Engine::GameEngine::conjob = PersonalScene::JobNames[clientJob];
                Terminate();
                Engine::GameEngine::GetInstance().ChangeScene("map");
            }
            enet_packet_destroy(e.packet);      
        }
    });
    waitConn = true;
    hostMode = true;
    hostIdInfo = "Your ID: " + ip;
    hostPortInfo = "Port: " + std::to_string(1234);
}


void PersonalScene::JoinGame() {
    Engine::GameEngine::job = JobNames[currentJobIndex];
    join_mode = true;
    focusedBox = IPEnter;
}

void PersonalScene::ConfirmJoin() {
    auto& net = NetWork::Instance();
    net.myId = 1;
    if (!net.Init()) return;

    std::string ip = IPEnter->name;
    int port = std::stoi(portEenter->name);

    std::cout << ip << " " << port << '\n';

    if (!net.ConnectToHost(ip, static_cast<enet_uint16>(port))) {
        Engine::LOG(Engine::INFO) << "cannot connect " << ip << ":" << port;
        return;
    }
    net.SetReceiveCallback([this](const ENetEvent& e){
        if(e.type==ENET_EVENT_TYPE_CONNECT){
            join_mode = false;
            PacketHeader hdr{ MSG_JOB_SELECT, sizeof(uint8_t) };
            uint8_t buf[sizeof(hdr)+1];
            std::memcpy(buf, &hdr, sizeof(hdr));
            uint8_t jobId = static_cast<uint8_t>(currentJobIndex);
            std::memcpy(buf + sizeof(hdr), &jobId, 1);
            NetWork::Instance().Send(buf, sizeof(buf), 0);
        }
        else if (e.type == ENET_EVENT_TYPE_RECEIVE) {
            PacketHeader hdr;
            std::memcpy(&hdr, e.packet->data, sizeof(hdr));
            if (hdr.type == MSG_JOB_SELECT) {
                uint8_t clientJob = *(e.packet->data + sizeof(hdr));
                Engine::GameEngine::conjob = PersonalScene::JobNames[clientJob];
                Terminate();
                Engine::GameEngine::GetInstance().ChangeScene("map");
            }
            enet_packet_destroy(e.packet);
        }          
    });
    waitConn = true;
}
void PersonalScene::ScoreOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}
void PersonalScene::SettingOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("setting");
}
void PersonalScene::Terminate() {
    IScene::Terminate();
    waitConn = false;
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
}
