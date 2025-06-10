#include "PathHelper.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_path.h>

namespace Engine {
    std::string GetScoreboardPathHelper() {
        ALLEGRO_PathHelper* PathHelper = al_get_standard_PathHelper(ALLEGRO_USER_DATA_PathHelper);
        al_append_PathHelper_component(PathHelper, "TowerDefenseGame");
        al_make_directory(al_PathHelper_cstr(PathHelper, '/'));
        al_append_PathHelper_component(PathHelper, "ScoreBoard.txt");

        std::string fullPathHelper = al_PathHelper_cstr(PathHelper, '/');
        al_destroy_PathHelper(PathHelper);
        return fullPathHelper;
    }
}
