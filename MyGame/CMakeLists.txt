cmake_minimum_required(VERSION 3.27)
project(FinalProject)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/enet-master/include)
add_subdirectory(enet-master)

include_directories(.)
include_directories(Resource)

file(COPY Resource/audios DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/fonts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY Resource DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY allegro_monolith-5.2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(FinalProject
    # Bullet/Bullet.cpp
    # Bullet/Bullet.hpp

    Engine/Allegro5Exception.cpp
    Engine/Allegro5Exception.hpp
    Engine/AudioHelper.cpp
    Engine/AudioHelper.hpp
    Engine/Collider.cpp
    Engine/Collider.hpp
    Engine/GameEngine.cpp
    Engine/GameEngine.hpp
    Engine/Group.cpp
    Engine/Group.hpp
    Engine/IControl.cpp
    Engine/IControl.hpp
    Engine/IObject.cpp
    Engine/IObject.hpp
    Engine/IScene.cpp
    Engine/IScene.hpp
    Engine/LOG.cpp
    Engine/LOG.hpp
    Engine/Point.cpp
    Engine/Point.hpp
    Engine/Resources.cpp
    Engine/Resources.hpp
    Engine/Sprite.cpp
    Engine/Sprite.hpp
    Engine/NetWork.cpp
    Engine/NetWork.hpp

    Player/Player.cpp
    Player/Player.hpp

    Scene/LogScene.cpp
    Scene/LogScene.hpp
    Scene/MapScene.cpp
    Scene/MapScene.hpp
    Scene/PersonalScene.cpp
    Scene/PersonalScene.hpp
    Scene/PlayScene.cpp
    Scene/PlayScene.hpp
    # Scene/ResultScene.cpp
    # Scene/ResultScene.hpp
    # Scene/ScoreBoardScene.cpp
    # Scene/ScoreBoardScene.hpp
    # Scene/SettingsScene.cpp
    # Scene/SettingsScene.hpp
    # Scene/StoryScene.cpp
    # Scene/StoryScene.hpp

    # UI/Animation/DirtyEffect.cpp
    # UI/Animation/DirtyEffect.hpp
    # UI/Animation/ExplosionEffect.cpp
    # UI/Animation/ExplosionEffect.hpp

    UI/Component/Image.cpp
    UI/Component/Image.hpp
    UI/Component/ImageButton.cpp
    UI/Component/ImageButton.hpp
    UI/Component/Label.cpp
    UI/Component/Label.hpp
    UI/Component/Slider.cpp
    UI/Component/Slider.hpp

    Items/Item.hpp
    Items/Item.cpp
    
    Soldier/Soldier.hpp
    Soldier/Soldier.cpp
    Soldier/vikin.hpp
    Soldier/vikin.cpp
    Soldier/master.hpp
    Soldier/master.cpp
    Soldier/ninja.hpp
    Soldier/ninja.cpp
    Soldier/shooter.hpp
    Soldier/shooter.cpp
    Soldier/slime.hpp
    Soldier/slime.cpp
    Soldier/dragen.hpp
    Soldier/dragen.cpp

    main.cpp
)

IF (WIN32)
    target_include_directories(FinalProject PUBLIC C:/allegro/include)
    target_link_directories(FinalProject PUBLIC C:/allegro/lib)
    target_compile_options(FinalProject PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(FinalProject
    PRIVATE
      liballegro_monolith
      enet
      ws2_32
      winmm
  )
ELSE()
    find_package(PkgConfig)
    pkg_check_modules(ALLEGRO5 REQUIRED allegro-5 allegro_main-5 allegro_font-5 allegro_dialog-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)
    target_include_directories(FinalProject PUBLIC lib)
    target_include_directories(FinalProject PUBLIC ${ALLEGRO5_INCLUDE_DIRS})
    target_compile_options(FinalProject PUBLIC ${ALLEGRO5_CFLAGS_OTHER})
    target_link_libraries(FinalProject PRIVATE ${ALLEGRO5_LIBRARIES}  )
ENDIF()

target_include_directories(FinalProject
  PUBLIC
    ${CMAKE_SOURCE_DIR}/enet-master/include
)
target_link_libraries(FinalProject
  PRIVATE
    enet
)