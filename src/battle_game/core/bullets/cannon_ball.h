#pragma once
#include "battle_game/core/bullet.h"

namespace battle_game::bullet {
class CannonBall : public Bullet {
 public:
  CannonBall(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity,
             float angular_velocity,
             uint32_t lifetime);
  ~CannonBall() override;
  void Render() override;
  void Update() override;

 private:
  glm::vec2 velocity_{};
  float angular_velocity_{}; // 旋转的角速度
  uint32_t lifetime_{}; // 最多存在的帧数
  uint32_t time_{0}; // 已存在的帧数
};
}  // namespace battle_game::bullet
