#ifndef FlyEnemy_HPP
#define FlyEnemy_HPP
#include "Enemy.hpp"

class FlyEnemy : public Enemy {
public:
    FlyEnemy(int x, int y);
    void Update(float deltaTime) override;
};
#endif  
