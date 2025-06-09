#pragma once
#include <cstdint>

enum MessageType : uint8_t {
  MSG_SCENE_START,
  MSG_PICK_ITEM,
  MSG_COUNTDOWN,
  MSG_SWITCH_TO_FIGHT,
  MSG_PLAYER_STATE,
  MSG_GAME_RESULT,
  MSG_PLACE_SOLDIER,
  MSG_REMOVE_SOLDIER,
  MSG_CASTLE_DAMAGE,
  MSG_GAME_END
};

#pragma pack(push,1)
struct PacketHeader {
  MessageType type;
  uint16_t    length;
};

struct PickItem {
  uint8_t  playerId;
  uint8_t  itemId;
};

struct Countdown {
  float    remaining;
};

struct PlayerState {
  uint8_t   playerId;
  float     x, y;
  uint8_t   action;
};

struct GameResult {
  uint8_t  winnerId;
  uint16_t score[2];
};

struct PlaceSoldier{
  uint8_t  playerId;      // Which player placed the soldier
  uint8_t  soldierType;   // Type of soldier (0=ninja, 1=vikin, 2=master, etc.)
  uint8_t  x, y;          // Grid position
  uint32_t soldierId; // Unique ID for this soldier instance
};

struct RemoveSoldier {
  uint8_t  playerId;
  uint32_t soldierId;     // ID of soldier to remove
};

struct CastleDamage {
    uint8_t  attackingPlayerId;  // Who is dealing damage
    uint8_t  damage;             // Amount of damage (usually 1)
};

struct GameEnd {
    uint8_t  winnerId;           // 0 = draw, 1 = player1, 2 = player2
    uint8_t  player1Lives;
    uint8_t  player2Lives;
    uint8_t  reason;             // 0 = lives, 1 = timeout
};
#pragma pack(pop)
