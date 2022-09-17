#pragma once
enum Team { Red = 0, Blue };
struct position {
  float x = 0;
  float y = 0;
  float x_1 = 0;
  bool operator==(const position& pos) const {
    return (x == pos.x && y == pos.y && x_1 == pos.x_1);
  }
  bool operator!=(const position& pos) const {
    return (x != pos.x || y != pos.y || x_1 != pos.x_1);
  }
};

typedef struct {
  float z_up = 150.0;
  float z_down_take = -214.5f;
  float z_down = -144.5f;
  float z_down_common = -108.7f;
  float z_down_common_take = -179.0f;
  float z_down_release = -114.5f;
  float z_down_2nd_release = 0;

} Z_height;
extern position here;
extern const float jaga_3_degree;
extern const float x1_min;
extern const float r_max;
extern Z_height z_height;
extern position jaga[16];
extern position jagaB[16];
extern position jagaR[16];
extern position sharejagaR[11];
extern position sharejagaB[11];
extern position shootRed[18];
extern position shootBlue[18];
extern position shoot[18];

// Keita added
extern position shootRwall[18];  // shootingBox Red の外周の座標
extern position shootBwall[18];  // shootingBox Blue の外周の座標