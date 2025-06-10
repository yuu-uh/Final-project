#ifndef ScoreBoardScene_HPP
#define ScoreBoardScene_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include <vector>

#include "Engine/IScene.hpp"

typedef struct record{
        std::string state;
        int lives;
        std::string datetime;

        bool operator<(const record& other) const{
            if(datetime != other.datetime) return datetime > other.datetime;
            return state == "Win";
        }
}Record;

class ScoreBoardScene final: public Engine::IScene{
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::vector<Record> playerList;
    int cur_page;
    const int page_size = 5; 
    Group* RecordGroup;
    ALLEGRO_SAMPLE_ID bgmId;

    void loadRecord();
    void saveRecord();
    void displayRecord();

public:
    explicit ScoreBoardScene() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick();
    void PrevOnClick();
    void NextOnClick();
};


#endif