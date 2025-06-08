#pragma once
#include <cstdint>

enum MessageType : uint8_t {
  MSG_SCENE_START,
  MSG_PICK_ITEM,
  MSG_COUNTDOWN,
  MSG_SWITCH_TO_FIGHT,
  MSG_PLAYER_STATE,
  MSG_GAME_RESULT
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
#pragma pack(pop)
