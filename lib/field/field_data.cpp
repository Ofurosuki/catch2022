#include "field_data.h"

const float jaga_3_degree = 60.0;
// 3連のじゃがりこのx軸に対してなす角
//もし、シュートの角度が違えばさらに追加
const float x1_max = 0.0f;
const float r_max = 814.0f;

Z_height z_height;

position here = {0, 0, 0};

position jaga[16] = {
    {-735, 448, 0}, {-635, 548, 0}, {-635, 348, 0}, {-535, 448, 0},
    {-435, 548, 0}, {-435, 348, 0}, {-335, 448, 0}, {-235, 548, 0},
    {-235, 348, 0}, {-135, 448, 0}, {-35, 548, 0},  {-35, 348, 0},
    {65, 448, 0},   {165, 548, 0},  {165, 348, 0},  {265, 448, 0}};

position sharejagaR[11] = {{325.1, 703, 0},  {185.1, 703, 0},  {45.1, 703, 0},
                           {-94.9, 703, 0},  {-234.9, 703, 0}, {-374.9, 703, 0},
                           {-514.9, 703, 0}, {-654.9, 703, 0}, {-794.9, 703, 0},
                           {0, 0, 0},        {0, 0, 0}};

position sharejagaB[11] = {{-794.9, 703, 0}, {-654.9, 703, 0}, {-514.9, 703, 0},
                           {-374.9, 703, 0}, {-234.9, 703, 0}, {-94.9, 703, 0},
                           {45.1, 703, 0},   {185.1, 703, 0},  {325.1, 703, 0},
                           {0, 0, 0},        {0, 0, 0}};

position shootRed[18] = {{-985, -29, 0},  {-885, -29, 0},  {-785, -29, 0},
                         {-985, -129, 0}, {-885, -129, 0}, {-785, -129, 0},
                         {-985, -230, 0}, {-885, -230, 0}, {-785, -230, 0},
                         {-985, -330, 0}, {-885, -330, 0}, {-785, -330, 0},
                         {-985, -431, 0}, {-885, -431, 0}, {-785, -431, 0},
                         {-985, -531, 0}, {-885, -531, 0}, {-785, -531, 0}};

position shootBlue[18] = {{315, -29, 0},  {415, -29, 0},  {515, -29, 0},
                          {315, -129, 0}, {415, -129, 0}, {515, -129, 0},
                          {315, -230, 0}, {415, -230, 0}, {515, -230, 0},
                          {315, -330, 0}, {415, -330, 0}, {515, -330, 0},
                          {315, -431, 0}, {415, -431, 0}, {515, -431, 0},
                          {315, -531, 0}, {415, -531, 0}, {515, -531, 0}};

position shoot[18];  // shoot red or blue が代入される

position shootBwall[18] = {{215, -129, 0}, {315, 71, 0},   {415, 71, 0},
                           {215, -230, 0}, {0, 0, 0},      {615, -230, 0},
                           {215, -330, 0}, {0, 0, 0},      {615, -330, 0},
                           {215, -431, 0}, {0, 0, 0},      {615, -431, 0},
                           {215, -531, 0}, {0, 0, 0},      {615, -531, 0},
                           {215, -631, 0}, {315, -631, 0}, {415, -631, 0}};

position shootRwall[18] = {{-885, 71, 0},    {-785, 71, 0},   {-685, 71, 0},
                           {-1085, -29, 0},  {0, 0, 0},       {-685, -29, 0},
                           {-1085, -129, 0}, {0, 0, 0},       {-685, -129, 0},
                           {-1085, -230, 0}, {0, 0, 0},       {-685, -230, 0},
                           {-1085, -330, 0}, {0, 0, 0},       {-685, -330, 0},
                           {-885, -631, 0},  {-785, -631, 0}, {-685, -431, 0}};
