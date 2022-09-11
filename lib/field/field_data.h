#pragma once
enum Team { Red = 0, Blue };
typedef struct {
  float x = 0;
  float y = 0;
  float x_1 = 0;
} position;

typedef struct {
  float z_up = 150.0;
  float z_down_take = 30.0f;
  float z_down = 2.0f;
  float z_down_common = 40.0f;
  float z_down_common_take = 230.0f;
  float z_down_release = 15.0f;
  float z_down_2nd_release = 115.0f;

} Z_height;
extern position here;
extern const float jaga_3_degree;
extern const float x1_max;
extern Z_height z_height;
extern position jaga[16];
extern position sharejagaR[11];
extern position sharejagaB[11];
extern position shootRed[18];
extern position shootBlue[18];
extern position shoot[18];

// Keita added
extern position shootRwall[18];  // shootingBox Red の外周の座標
extern position shootBwall[18];  // shootingBox Blue の外周の座標