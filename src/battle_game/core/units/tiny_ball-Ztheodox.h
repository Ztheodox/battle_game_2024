#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Ball : public Unit {
 public:
  Ball(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
  void BallMove();
  void TurretRotate();
  void FireInDirection(float direction, float damage); // 以……方向开火
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  glm::vec2 velocity_{0.0f}; // 球的运动速度，二维向量，初始为0
  glm::vec2 gravity_{0.0f, -2.0f}; // 球的重力加速度，二维向量，初始向下
  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t ability_count_down_{0}; // 技能的冷却时间
  uint32_t ability_use_count_down_{0}; // 技能的持续时间
};
}  // namespace battle_game::unit
