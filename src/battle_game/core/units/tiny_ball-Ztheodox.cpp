#include "tiny_ball-Ztheodox.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t ball_body_model_index = 0xffffffffu;
uint32_t ball_turret_model_index = 0xffffffffu;
}  // namespace

Ball::Ball(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (!~ball_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* 
      Ball Body
      半径为1的60边形
      */
      std::vector<ObjectVertex> body_vertices;
      std::vector<uint32_t> body_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f; // \theta=\frac{2\pi}{\precision}(i+1/2)
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        body_vertices.push_back({{sin_theta * 1.0f, cos_theta * 1.0f}, // x=\sin\theta,y=\cos\theta
                                   {0.0f, 0.0f},
                                   {1.0f, 1.0f, 1.0f, 1.0f}});
        body_indices.push_back(i);
        body_indices.push_back((i + 1) % precision);
        body_indices.push_back(precision); // 填充第i个点、其后的点、中心所构成的三角形
      }
      body_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}}); // \precision边形的中心
      ball_body_model_index =
          mgr->RegisterModel(body_vertices, body_indices);
    }

    {
      /*
      Ball Turret
      8根中心对称的，长度1.2，宽度0.2的矩形。其中第1根为主炮塔，颜色为红色，其余颜色为灰色。
      */
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      const int n = 8;
      const float theta_n = glm::pi<float>() / float(n);
      const float r = 0.1f / std::sin(theta_n); // 中间的8边形边长a=0.2，半径r=a/2\sin(\pi/n)
      for (int i = 0; i < n; i++) { // 绘制中间的8边形
        auto theta = (2 * float(i) + 1.0f) * theta_n; // \theta=\frac{2\pi}{n}(i+1/2)
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.push_back({{sin_theta * r, cos_theta * r}, // x=r\sin\theta,y=r\cos\theta
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % n);
        turret_indices.push_back(n);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}}); // 8边形的中心
      for (int i = 1; i < n; i++) { // 绘制7根灰色的矩形
        auto theta = 2 * float(i) * theta_n; // \theta=\frac{2\pi}{n}i
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.push_back({{-0.1f * cos_theta + 1.2f * sin_theta, 1.2f * cos_theta + 0.1f * sin_theta}, 
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}}); // 把x=-0.1,y=1.2顺时针旋转theta角度
        turret_vertices.push_back({{0.1f * cos_theta + 1.2f * sin_theta, 1.2f * cos_theta - 0.1f * sin_theta}, 
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}}); // 把x=0.1,y=1.2顺时针旋转theta角度
        turret_indices.push_back(i - 1);
        turret_indices.push_back(i);
        turret_indices.push_back(n + 2 * i - 1);
        turret_indices.push_back(i);
        turret_indices.push_back(n + 2 * i - 1);
        turret_indices.push_back(n + 2 * i); // 填充矩形
      }
      // 绘制主炮塔，1根红色的矩形
      const float y = r * std::cos(theta_n);
      turret_vertices.push_back({{0.1f, y},
                                 {0.0f, 0.0f},
                                 {1.0f, 0.0f, 0.0f, 1.0f}});
      turret_vertices.push_back({{-0.1f, y},
                                 {0.0f, 0.0f},
                                 {1.0f, 0.0f, 0.0f, 1.0f}});
      turret_vertices.push_back({{0.1f, 1.2f},
                                 {0.0f, 0.0f},
                                 {1.0f, 0.0f, 0.0f, 1.0f}});
      turret_vertices.push_back({{-0.1f, 1.2f},
                                 {0.0f, 0.0f},
                                 {1.0f, 0.0f, 0.0f, 1.0f}});
      turret_indices.push_back(3 * n - 1);
      turret_indices.push_back(3 * n);
      turret_indices.push_back(3 * n + 1);
      turret_indices.push_back(3 * n);
      turret_indices.push_back(3 * n + 1);
      turret_indices.push_back(3 * n + 2);
      ball_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
}

void Ball::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(ball_body_model_index);
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(ball_turret_model_index);
}

void Ball::Update() {
  BallMove();
  TurretRotate();
  Fire();
}

void Ball::BallMove() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player && !ability_use_count_down_) { // 使用技能时不能移动
    auto &input_data = player->GetInputData();
    // 按WASD改变重力方向
    if (input_data.key_down[GLFW_KEY_W]) {
      gravity_ = {0.0f, 2.0f};
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      gravity_ = {0.0f, -2.0f};
    }
    if (input_data.key_down[GLFW_KEY_A]) {
      gravity_ = {-2.0f, 0.0f};
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      gravity_ = {2.0f, 0.0f};
    }
    auto acceleration = gravity_ - 0.5f * velocity_; // 球的加速度等于重力加速度与阻力加速度的和，后者与速度成正比
    velocity_ += kSecondPerTick * acceleration; // 根据加速度更新速度
    auto delta_position = kSecondPerTick * GetSpeedScale() * velocity_; // 根据速度计算位移
    // 如果在某一方向被阻挡，则将该方向的位移置为0，速度以0.2倍反弹
    if (game_core_->IsBlockedByObstacles(position_ + glm::vec2{delta_position.x, 0.0f})) {
      velocity_.x *= -0.2f;
      delta_position.x = 0.0f;
    }
    if (game_core_->IsBlockedByObstacles(position_ + glm::vec2{0.0f, delta_position.y})) {
      velocity_.y *= -0.2f;
      delta_position.y = 0.0f;
    }
    // 更新位置（球不使用旋转位置参量）
    game_core_->PushEventMoveUnit(id_, position_ + delta_position);
  }
}

void Ball::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player && !ability_use_count_down_) { // 使用技能时不能旋转炮塔
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

void Ball::FireInDirection(float direction, float damage) {
  auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, direction);
  GenerateBullet<bullet::CannonBall>(
      position_ + Rotate({0.0f, 1.2f}, direction),
      direction, damage * GetDamageScale(), velocity);
}

void Ball::Fire() {
  if (fire_count_down_ == 0 && !ability_use_count_down_) { // 使用技能时不能开火
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        FireInDirection(turret_rotation_, 1.0f);
        fire_count_down_ = kTickPerSecond;  // Fire间隔1秒
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
  if (ability_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_Q]) {
        ability_use_count_down_ = 129; // 技能持续129帧（约2.13秒）
        ability_count_down_ = 10 * kTickPerSecond; // 技能冷却时间为10秒
      }
    }
  }
  if (ability_count_down_) {
    ability_count_down_--;
  }
  if (ability_use_count_down_) {
    uint32_t n = --ability_use_count_down_;
    if (!(n % 4)) { // 使用技能时，每隔4帧开一次火，伤害0.5。具体效果在游玩时可以展现
      if (!(n % 16)) {
        FireInDirection(turret_rotation_, 0.5f);
        FireInDirection(turret_rotation_ + glm::pi<float>(), 0.5f);
      } else {
        float direction = turret_rotation_ + float((n / 4) % 8) * glm::pi<float>() / 4.0f;
        FireInDirection(direction, 0.5f);
      }
    }
  }
}

bool Ball::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return std::pow(position.x, 2) + std::pow(position.y, 2) < 1.0f; // 在半径为1的圆内即被击中
}

const char *Ball::UnitName() const {
  return "Tiny Ball";
}

const char *Ball::Author() const {
  return "Ztheodox";
}
}  // namespace battle_game::unit
