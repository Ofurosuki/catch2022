# Catch Robot Contest 2022 Team BG Main Board

キャチロボバトルコンテスト2022チームBGのメイン基板ファームウエアです

## 構成
- libフォルダ: ライブラリいろいろ
  - CanManager: CANの送受信をする。受信はhardIdごとにコールバック関数を指定することで、処理を簡単に増やせる
  - Motor, Sensor, Solenoid, Servo: NHKのCANでつなげる各基板用のファームウェア
  - Stepper: メイン基板から直接回すステッピングモーターのファームウェア


## ID表

|hardId|名前|備考|
|--|--|--|
|0x01|DCモーター||
|0x02|サーボ||
|0x03|電磁弁||
|0x04|センサー||

## ピン配置
|ピン番号|名前|備考|
|--|--|--|
|PB12|ステッパ0方向||
|PB13|ステッパ0パルス||
|PB14|ステッパ1方向||
|PB15|ステッパ1パルス||
|PC6|ステッパ2方向||
|PC7|ステッパ2パルス||
|PC8|ステッパ3方向||
|PC9|ステッパ3パルス||
|PB10|ポンプ||
|PA8|7セグCLK||
|PA9|7セグDO||
