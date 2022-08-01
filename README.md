# Catch Robot Contest 2022 Team BG Main Board

キャチロボバトルコンテスト2022チームBGのメイン基板ファームウエアです

## 構成
- libフォルダ: ライブラリいろいろ
  - CanManager: CANの送受信をする。受信はhardIdごとにコールバック関数を指定することで、処理を簡単に増やせる
  - Motor, Sensor, Solenoid, Servo: NHKのCANでつなげる各基板用のファームウェア
  - Stepper: メイン基板から直接回すステッピングモーターのファームウェア
