//
// JuniorRoboSample
// ジュニアロボットチーム用サンプルロボット　メインプログラム
//
// 初版作成：2022.9.6
//

#include <Servo.h>

// プロトタイプ宣言
void pinInit_drive(void);                             // 駆動系 ピン設定
void pinInit_arm(void);                               // アーム系 ピン設定
void pinInit_hand(void);                              // サーボハンド ピン設定
void dataProcess(uint8_t data[]);                     // 受信データ解析
void FL_motor(int inverse, int power);                // 左前モータの動作指令
void FR_motor(int inverse, int power);                // 右前モータの動作指令
void RL_motor(int inverse, int power);                // 左後モータの動作指令
void RR_motor(int inverse, int power);                // 右後モータの動作指令
void drive(int vx, int vy, int turn_right, int turn_left);      
                                                      // 駆動(メカナム)動作
void arm_updown(int vy);                              // アーム上下 動作
void arm_frontback(int vy);                           // アーム前後 動作
void hand_openclose(int sw);                          // ハンドサーボ開閉 動作

//**********************
// 各種設定
//**********************
#define   TRANS_BITRATE   115200          // 通信速度[bps]

#define   OUTVAL_MAX      30              // コントローラから送信されるアナログスティック最大値
#define   OUTVAL_HALF     (OUTVAL_MAX/2)  // 停止時のアナログスティック値
#define   PWM_TURN        120             // 旋回時のPWM出力値
#define   PWM_MAX         255             // 最大出力

#define   HAND_OPEN       0               // サーボハンドOPEN時の出力値
#define   HAND_CLOSE      90              // サーボハンドCLOSE時の出力値

#define   T_S             10000           // loop関数のサンプリング周期[usec]
#define   TRANSDATANUM    6               // コントローラから1度に届くデータ個数
#define   TRANSERRCNT     10              // 通信失敗でエラーとする回数


//**********************
// ピン定義
//**********************

// 駆動
const int FL_DIR = 4;       // 4番ピンに左前モータのDIRを接続
const int FL_PWM = 3;       // 3番ピンに左前モータのPWMを接続

const int FR_DIR = 11;      // 11番ピンに右前モータのDIRを接続
const int FR_PWM = 12;      // 12番ピンに右前モータのPWMを接続

const int RL_DIR = 14;      // 14番ピンに左後モータのDIRを接続
const int RL_PWM = 2;       // 2番ピンに左後モータのPWMを接続

const int RR_DIR = 8;       // 8番ピンに右後モータのDIRを接続
const int RR_PWM = 7;       // 7番ピンに右後モータのPWMを接続

// アーム
const int UPDN_DIR = 5;     //  5番ピンに上下モータのDIRを接続
const int UPDN_PWM = 6;     //  6番ピンに上下モータのPWMを接続

const int FRBK_DIR = 9;     //  9番ピンに前後モータのDIRを接続
const int FRBK_PWM = 10;    // 10番ピンに前後モータのPWMを接続

// アームリミット
const int LM_UP = 48;       // 48番ピンにリミットスイッチ上を接続
const int LM_DN = 42;       // 42番ピンにリミットスイッチ下を接続

const int LM_FR = 36;       // 36番ピンにリミットスイッチ前を接続
const int LM_BK = 30;       // 30番ピンにリミットスイッチ後を接続

// ハンドサーボ
Servo servo_hand;
const int SRV_HAND = 13;    // 13番ピンにハンド用サーボモータのPWM入力を接続

// その他
const int TRANS_LED = 52;   // 52番ピンに通信成功LEDを接続


//**********************
// 初期設定関数
//**********************
void setup()
{
  Serial.begin(TRANS_BITRATE);    // デバッグ用シリアル通信 初期化
  Serial2.begin(TRANS_BITRATE);   // コントローラとのシリアル通信 初期化

  pinInit_drive();                // 駆動系(メカナム)ピン初期化
  pinInit_arm();                  // アーム系ピン初期化
  pinInit_hand();                 // ハンドサーボ初期化
  
  pinMode(TRANS_LED, OUTPUT);     // 通信成功LED ピン設定
  digitalWrite(TRANS_LED, LOW);   // 通信成功LED 消灯
}

/////////////////////
// 駆動系 ピン設定
/////////////////////
void pinInit_drive(void)
{
  pinMode(FL_DIR, OUTPUT);        // FL_DIRを出力モードで使用
  analogWrite(FL_PWM, 0);         // モータ出力初期化

  pinMode(FR_DIR, OUTPUT);        // FR_DIRを出力モードで使用
  analogWrite(FR_PWM, 0);         // モータ出力初期化

  pinMode(RL_DIR, OUTPUT);        // RL_DIRを出力モードで使用
  analogWrite(RL_PWM, 0);         // モータ出力初期化

  pinMode(RR_DIR, OUTPUT);        // RR_DIRを出力モードで使用
  analogWrite(RR_PWM, 0);         // モータ出力初期化
}

/////////////////////
// アーム系 ピン設定
/////////////////////
void pinInit_arm(void)
{
  pinMode(UPDN_DIR, OUTPUT);      // UPDN_DIRを出力モードで使用
  analogWrite(UPDN_PWM, 0);       // モータ出力初期化

  pinMode(FRBK_DIR, OUTPUT);      // FRBK_DIRを出力モードで使用
  analogWrite(FRBK_PWM, 0);       // モータ出力初期化

  pinMode(LM_UP, INPUT_PULLUP);   // リミットスイッチ上を入力モード（プルアップ）で使用
  pinMode(LM_DN, INPUT_PULLUP);   // リミットスイッチ下を入力モード（プルアップ）で使用
  pinMode(LM_FR, INPUT_PULLUP);   // リミットスイッチ前を入力モード（プルアップ）で使用
  pinMode(LM_BK, INPUT_PULLUP);   // リミットスイッチ後を入力モード（プルアップ）で使用
}

/////////////////////
// ハンドサーボ 初期設定
/////////////////////
void pinInit_hand(void){
  servo_hand.attach(SRV_HAND);    // ハンド用サーボ ピン設定
  servo_hand.write(HAND_OPEN);    // サーボモーターをOPEN位置まで動かす
}

/////////////////////
// モータ全停止
/////////////////////
void mtr_all_stop(void){
  drive(OUTVAL_HALF, OUTVAL_HALF, 0, 0);
  arm_frontback(OUTVAL_HALF);
  arm_updown(OUTVAL_HALF);
}

//**********************
// ループ関数
//**********************
void loop(){
  // 各周期の開始時間と終了時間[usec]
  long int startTime, endTime;
  
  // 制御周期開始時間を保存
  startTime = micros();
  
  // 通信エラー検出回数
  static int errcnt = 0;
  
  // コントローラからデータを受信
  int serialCount = Serial2.available();

  // 受信データなければエラーかも？
  if( serialCount == 0 ){
    errcnt++;
  }
  
  for(int i=0; i<serialCount; i++){
    static int count = 0;
    static uint8_t rxData[TRANSDATANUM];
    int data = Serial2.read();
    if((data&0x07)!=count){
      count = 0;
      errcnt++;
      continue;
    }
    else{
      rxData[count] = data;
      count++;
      if(count == TRANSDATANUM){
        count = 0;
        // 受信データ解析
        dataProcess(rxData);
        errcnt = 0;
        digitalWrite(TRANS_LED, HIGH);  // 通信成功LED 点灯
      }
    }
  }
  
  // エラーが一定回数以上続いたら全部止める
  if( errcnt >= TRANSERRCNT){
    errcnt = TRANSERRCNT;
    digitalWrite(TRANS_LED, LOW);     // 通信成功LED 消灯
    // 全停止指令
    mtr_all_stop();
  }
  
  // 制御周期終了時間を保存
  endTime = micros();
  
  // 制御周期開始からT_S[us]経つまで待つ (既に過ぎていたら待たない)
  delayMicroseconds(max(0, startTime + T_S - endTime));
}

///////////////////////////////////////////////////
// 受信データ解析
// in  data[]: コントローラから受信したデータ
///////////////////////////////////////////////////
void dataProcess(uint8_t data[]){

  int stick_val[4];   // アナログスティック
  int sw1 = 0;        // コントローラ SW1
  int sw2 = 0;        // コントローラ SW2
  int sw3 = 0;        // コントローラ SW3
  int sw4 = 0;        // コントローラ SW4
  int sw5 = 0;        // コントローラ SW5
  int sw6 = 0;        // コントローラ SW6
  
  // アナログスティック値の取り出し
  stick_val[0] = ( data[0]>>3 ) & 0x1f;  // 左 X(よこ) [0～30]
  stick_val[1] = ( data[1]>>3 ) & 0x1f;  // 左 Y(たて) [0～30]
  stick_val[2] = ( data[2]>>3 ) & 0x1f;  // 右 X(よこ) [0～30]
  stick_val[3] = ( data[3]>>3 ) & 0x1f;  // 右 Y(たて) [0～30]
  
  // スイッチ情報
  sw1 = (data[4] >> 3) & 0x1; // コントローラ SW1 [0:OFF 1:ON]: 動作許可
  sw2 = (data[4] >> 4) & 0x1; // コントローラ SW2 [0:OFF 1:ON]: 左旋回
  sw3 = (data[4] >> 5) & 0x1; // コントローラ SW3 [0:OFF 1:ON]: 右旋回
  sw4 = (data[4] >> 6) & 0x1; // コントローラ SW4 [0:OFF 1:ON]: 駆動、アーム前後切替
  sw5 = (data[4] >> 7) & 0x1; // コントローラ SW5 [0:OFF 1:ON]: (未使用)
  sw6 = (data[5] >> 3) & 0x1; // コントローラ SW6 [0:OFF 1:ON]: ハンドOPEN/CLOSE

  if(sw1 == 0){
    // 動作許可SWがOFF → すぐに全停止！
    mtr_all_stop();
  }
  else{
    // 動作許可SWがON  → 動いてよい
    if(sw4 == 0){
      // アーム前後 停止
      arm_frontback(OUTVAL_HALF);
      // 駆動 動作
      drive(stick_val[0], stick_val[1], sw3, sw2); // 左_X, 左_Y, 右旋回ボタン, 左旋回ボタン
    }
    else{
      // 駆動 停止
      drive(OUTVAL_HALF, OUTVAL_HALF, 0, 0);
      // アーム前後 動作
      arm_frontback(stick_val[1]);  // 左_Y
    }

    // アーム上下 動作
    arm_updown(stick_val[3]);       // 右_Y

    // ハンドサーボ開閉 動作
    hand_openclose(sw6);
  }
}

// 左前モータの動作指令
void FL_motor(int inverse, int power) {
  if ( inverse == 0 ) {
    // 前進
    digitalWrite(FL_DIR, HIGH);
  }
  else {
    // 後退
    digitalWrite(FL_DIR, LOW);
  }
  // PWM出力
  analogWrite(FL_PWM, power);
}

// 右前モータの動作指令
void FR_motor(int inverse, int power) {
  if ( inverse == 0 ) {
    // 前進
    digitalWrite(FR_DIR, LOW);
  }
  else {
    // 後退
    digitalWrite(FR_DIR, HIGH);
  }
  // PWM出力
  analogWrite(FR_PWM, power);
}

// 左後モータの動作指令
void RL_motor(int inverse, int power) {
  if ( inverse == 0 ) {
    // 前進
    digitalWrite(RL_DIR, HIGH);
  }
  else {
    // 後退
    digitalWrite(RL_DIR, LOW);
  }
  // PWM出力
  analogWrite(RL_PWM, power);
}

// 右後モータの動作指令
void RR_motor(int inverse, int power) {
  if (inverse == 0) {
    // 前進
    digitalWrite(RR_DIR, LOW);
  }
  else {
    // 後退
    digitalWrite(RR_DIR, HIGH);
  }
  // PWM出力
  analogWrite(RR_PWM, power);
}

///////////////////////////////////////////////////
// 駆動 動作
// in       vx:  スティックX方向(よこ)の傾き
//                0(左に倒した状態)～15(触れてない)～30(右に倒した状態)
//          vy:  スティックY方向(たて)の傾き
//                0(下に倒した状態)～15(触れてない)～30(上に倒した状態)
//  turn_right: 右旋回指令(1:右旋回)
//  turn_left : 左旋回指令(1:左旋回)
///////////////////////////////////////////////////
void drive(int vx, int vy, int turn_right, int turn_left)
{

  if (turn_left != 0) {
    // 左旋回スイッチがON
    FL_motor(1, PWM_TURN);
    FR_motor(0, PWM_TURN);
    RL_motor(1, PWM_TURN);
    RR_motor(0, PWM_TURN);
    Serial.println("turnLeft");
  }
  else if (turn_right != 0) {
    // 右旋回スイッチがON
    FL_motor(0, PWM_TURN);
    FR_motor(1, PWM_TURN);
    RL_motor(0, PWM_TURN);
    RR_motor(1, PWM_TURN);
    Serial.println("turnRight");
  }
  else if(vx == OUTVAL_HALF && vy == OUTVAL_HALF){
    // アナログスティックが倒れていない
    FL_motor(0, 0);
    FR_motor(0, 0);
    RL_motor(0, 0);
    RR_motor(0, 0);
    Serial.println("stop");
  }
  else{
    // 旋回スイッチがどちらもOFF && アナログスティックが倒れている

    // 0～30 を -7～8に換算
    // -7(左に倒した状態)～0(触れてない)～8(右に倒した状態)
    vx = ( vx / 2 ) - 7;
    vy = ( vy / 2 ) - 7;

    // アナログスティックの傾きから移動方向と速度を計算
    int dis = sqrt(vx * vx + vy * vy) ;
    Serial.println(dis);
    int deg = degrees(atan2(vy, vx));
    double p = (double)dis / 7;
    Serial.println(p);
    int power = (int)(PWM_MAX * p);
    power = min(power, PWM_MAX);
    Serial.println(power);
    
    // 角度によってモータの動作方向を切り替える
    if (-22 <= deg && deg < 23) {
      FL_motor(0, power);
      FR_motor(1, power);
      RL_motor(1, power);
      RR_motor(0, power);
      Serial.println("right");
    }
    else if (23 <= deg && deg < 68) {
      FL_motor(0, power);
      FR_motor(0, 0);
      RL_motor(0, 0);
      RR_motor(0, power);
      Serial.println("rightForward");
    }
    else if (68 <= deg && deg < 113) {
      FL_motor(0, power);
      FR_motor(0, power);
      RL_motor(0, power);
      RR_motor(0, power);
      Serial.println("forward");
    }
    else if (113 <= deg && deg < 158) {
      FL_motor(0, 0);
      FR_motor(0, power);
      RL_motor(0, power);
      RR_motor(0, 0);
      Serial.println("leftForward");
    }
    else if (158 <= deg && deg <= 180 || -180 <= deg && deg < -157) {
      FL_motor(1, power);
      FR_motor(0, power);
      RL_motor(0, power);
      RR_motor(1, power);
      Serial.println("left");
    }
    else if (-157 <= deg && deg < -112) {
      FL_motor(1, power);
      FR_motor(0, 0);
      RL_motor(0, 0);
      RR_motor(1, power);
      Serial.println("leftBack");
    }
    else if (-112 <= deg && deg < -67) {
      FL_motor(1, power);
      FR_motor(1, power);
      RL_motor(1, power);
      RR_motor(1, power);
      Serial.println("back");
    }
    else if (-67 <= deg && deg < -22) {
      FL_motor(0, 0);
      FR_motor(1, power);
      RL_motor(1, power);
      RR_motor(0, 0);
      Serial.println("rightBack");
    }
    else{
      FL_motor(0, 0);
      FR_motor(0, 0);
      RL_motor(0, 0);
      RR_motor(0, 0);
      Serial.println("stop");
    }
  }
}

///////////////////////////////////////////////////
// アーム上下 動作
// in      vy: スティックY方向(たて)の傾き
//               0(下に倒した状態)～15(触れてない)～30(上に倒した状態)
///////////////////////////////////////////////////
void arm_updown(int vy)
{
  int dir = LOW;

  if(vy > OUTVAL_HALF && digitalRead(LM_UP) == 1){
    // 正回転(上方向)
    dir = LOW;
    vy = ((vy-OUTVAL_HALF) * PWM_MAX) / OUTVAL_HALF;
  }
  else
  if(vy < OUTVAL_HALF && digitalRead(LM_DN) == 1){
    // 逆回転(下方向)
    dir = HIGH;
    vy = ((OUTVAL_HALF-vy) * PWM_MAX) / OUTVAL_HALF;
  }
  else{ // (vy == OUTVAL_HALF)
    // 停止
    vy = 0;
  }

  // 回転方向
  digitalWrite(UPDN_DIR, dir);

  // PWM出力
  analogWrite(UPDN_PWM, vy);
}

///////////////////////////////////////////////////
// アーム前後 動作
// in      vy: スティックY方向(たて)の傾き
//                0(下に倒した状態)～15(触れてない)～30(上に倒した状態)
///////////////////////////////////////////////////
void arm_frontback(int vy)
{
  int dir = LOW;

  if(vy > OUTVAL_HALF && digitalRead(LM_FR) == 1){
    // 正回転(前方向)
    dir = LOW;
    vy = ((vy-OUTVAL_HALF) * PWM_MAX) / OUTVAL_HALF;
  }
  else
  if(vy < OUTVAL_HALF && digitalRead(LM_BK) == 1){
    // 逆回転(後方向)
    dir = HIGH;
    vy = ((OUTVAL_HALF-vy) * PWM_MAX) / OUTVAL_HALF;
  }
  else{ // (vy == OUTVAL_HALF)
    // 停止
    vy = 0;
  }

  // 回転方向
  digitalWrite(FRBK_DIR, dir);

  // PWM出力
  analogWrite(FRBK_PWM, vy);

}

///////////////////////////////////////////////////
// ハンドサーボ 動作
// in      sw: ハンドサーボ 開閉指令 (0:OPEN 1:CLOSE)
///////////////////////////////////////////////////
void hand_openclose(int sw)
{
  if( sw == 0 ){
    // OPEN
    servo_hand.write(HAND_OPEN);          // サーボモーターをOPEN位置まで動かす
  }
  else{
    // CLOSE
    servo_hand.write(HAND_CLOSE);          // サーボモーターをCLOSE位置まで動かす
  }
}
