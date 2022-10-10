//
// JuniorRoboSample
// ジュニアロボットチーム用サンプルロボット　メインプログラム
//
// 初版作成：2022.9.6
//

#include <Servo.h>

// プロトタイプ宣言
void pinInit_drive(void);                 // 駆動系 ピン設定
void pinInit_arm(void);                   // アーム系 ピン設定
void drive(int vx, int vy, int emg);      // 駆動(メカナム)動作
void dataProcess(uint8_t data[]);         // 受信データ解析
void arm_updown(int vy, int emg);         // アーム上下 動作
void arm_frontback(int vy, int emg);      // アーム前後 動作

//**********************
// 各種設定
//**********************
#define   TRANS_BITRATE   115200          // 通信速度

#define   OUTVAL_MAX      30				      // コントローラから送信されるアナログスティック最大値
#define   OUTVAL_HALF     (OUTVAL_MAX/2)	// 停止時のアナログスティック値
#define   PWM_MAX         255				      // 最大出力

#define   TRANSDATANUM    6               // コントローラから1度に届くデータ個数

//**********************
// ピン定義
//**********************

// 駆動
const int FL_DIR = 4;     // 4番ピンに左前モータのDIRを接続
const int FL_PWM = 3;     // 3番ピンに左前モータのPWMを接続

const int FR_DIR = 11;    // 11番ピンに右前モータのDIRを接続
const int FR_PWM = 12;    // 12番ピンに右前モータのPWMを接続

const int RL_DIR = 1;     // 1番ピンに左後モータのDIRを接続
const int RL_PWM = 2;     // 2番ピンに左後モータのPWMを接続

const int RR_DIR = 8;     // 8番ピンに右後モータのDIRを接続
const int RR_PWM = 7;     // 7番ピンに右後モータのPWMを接続

// アーム
const int UPDN_IN1 = 5;     //  5番ピンに上下モータのDIRを接続
const int UPDN_EN  = 6;     //  6番ピンに上下モータのPWMを接続

const int FRBK_IN1 = 9;     //  9番ピンに前後モータのDIRを接続
const int FRBK_EN  = 10;    // 10番ピンに前後モータのPWMを接続

// アームリミット
const int LM_UP = 48;       // 48番ピンにリミットスイッチ上を接続
const int LM_DN = 42;       // 42番ピンにリミットスイッチ下を接続

const int LM_FR = 36;       // 36番ピンにリミットスイッチ前を接続
const int LM_BK = 30;       // 30番ピンにリミットスイッチ後を接続

// ハンドサーボ
Servo servo_hand;
const int SRV_HAND = 13;    // 13番ピンにハンド用サーボモータのPWM入力を接続

// その他
const int TRANS_LED = A8;   // アナログ8番ピンに通信成功LEDを接続


//**********************
// 初期設定関数
//**********************
void setup()
{
  Serial.begin(TRANS_BITRATE);
	Serial2.begin(TRANS_BITRATE);

	pinInit_drive();							// 駆動系(メカナム)ピン初期化
  pinInit_arm();                // アーム系ピン初期化
  servo_hand.attach(SRV_HAND);  // ハンド用サーボ ピン設定

}

/////////////////////
// 駆動系 ピン設定
/////////////////////
void pinInit_drive(void)
{
  pinMode(FL_DIR, OUTPUT);    // FL_DIRを出力モードで使用
  analogWrite(FL_PWM, 0);     // モータ出力初期化

  pinMode(FR_DIR, OUTPUT);    // FR_DIRを出力モードで使用
  analogWrite(FR_PWM, 0);     // モータ出力初期化

  pinMode(RL_DIR, OUTPUT);    // RL_DIRを出力モードで使用
  analogWrite(RL_PWM, 0);     // モータ出力初期化

  pinMode(RR_DIR, OUTPUT);    // RR_DIRを出力モードで使用
  analogWrite(RR_PWM, 0);     // モータ出力初期化
}

/////////////////////
// アーム系 ピン設定
/////////////////////
void pinInit_arm(void)
{
  pinMode(UPDN_IN1, OUTPUT);      // UPDN_IN1を出力モードで使用
  analogWrite(UPDN_EN, 0);        // モータ出力は0で初期化

  pinMode(FRBK_IN1, OUTPUT);	  // FRBK_IN1を出力モードで使用
  analogWrite(FRBK_EN, 0);        // モータ出力は0で初期化

  pinMode(LM_UP, INPUT_PULLUP);   // リミットスイッチ上を入力モード（プルアップ）で使用
  pinMode(LM_DN, INPUT_PULLUP);   // リミットスイッチ下を入力モード（プルアップ）で使用
  pinMode(LM_FR, INPUT_PULLUP);   // リミットスイッチ前を入力モード（プルアップ）で使用
  pinMode(LM_BK, INPUT_PULLUP);   // リミットスイッチ後を入力モード（プルアップ）で使用
}

//**********************
// ループ関数
//**********************
void loop(){
  // コントローラからデータを受信
  int serialCount = Serial2.available();
  for(int i=0; i<serialCount; i++){
    static int count = 0;
    static uint8_t rxData[TRANSDATANUM];
    int data = Serial2.read();
    if((data&0x07)!=count){
      count = 0;
      continue;
    }
    else{
      rxData[count] = data;
      count++;
      if(count == TRANSDATANUM){
        count = 0;
        // 受信データ解析
        dataProcess(rxData);
      }
    }
  }
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
  
  // アナログスティック値の取り出し
  stick_val[0] = ( data[0]>>3 ) & 0x1f;  // 左 X(よこ) [0～30]
  stick_val[1] = ( data[1]>>3 ) & 0x1f;  // 左 Y(たて) [0～30]
  stick_val[2] = ( data[2]>>3 ) & 0x1f;  // 右 X(よこ) [0～30]
  stick_val[3] = ( data[3]>>3 ) & 0x1f;  // 右 Y(たて) [0～30]
  
  stick_val[0] = stick_val[0] / 2;  // 左 X(よこ) [0～15]
  stick_val[1] = stick_val[1] / 2;  // 左 Y(たて) [0～15]
  
  // スイッチ情報
  sw1 = (data[4] >> 3) & 0x1; // コントローラ SW1 [0:OFF 1:ON]：動作許可
  sw2 = (data[4] >> 4) & 0x1; // コントローラ SW2 [0:OFF 1:ON]
  sw3 = (data[4] >> 5) & 0x1; // コントローラ SW3 [0:OFF 1:ON]
  sw4 = (data[4] >> 6) & 0x1; // コントローラ SW4 [0:OFF 1:ON]：駆動、アーム前後切替
  
  if(sw4 == 0){
    // 駆動 動作
    drive(stick_val[0]-7, stick_val[1]-7, sw1); // 1_X, 1_Y, sw1
    // アーム前後 停止
    arm_frontback(0, 0);
  }
  else{
    // アーム前後 動作
    arm_frontback(stick_val[1], sw1); // 1_Y, sw1
    // 駆動 停止
    drive(0, 0, 0);
  }

  // アーム上下 動作
  arm_updown(stick_val[3], sw1); // 2_Y, sw1
}

///////////////////////////////////////////////////
// 駆動 動作
// in    vx:  スティックX方向(よこ)の傾き
//              0(左に倒した状態)～15(触れてない)～30(右に倒した状態)
//       vy:  スティックY方向(たて)の傾き
//              0(下に倒した状態)～15(触れてない)～30(上に倒した状態)
//       ena: 動作許可(0:NG,1:OK) 
///////////////////////////////////////////////////

// 右前モータの速度算出
void FL_motor(int stopFlag, int inverse, int power) {
  if (!stopFlag) {
    if (inverse) {
      digitalWrite(FL_DIR, HIGH);
    }
    else {
      digitalWrite(FL_DIR, LOW);
    }
    analogWrite(FL_PWM, power);
  }
  else {
    digitalWrite(FL_DIR, LOW);
    analogWrite(FL_PWM, 0);
  }
}

// 左前モータの速度算出
void FR_motor(int stopFlag, int inverse, int power) {
  if (!stopFlag) {
    if (inverse) {
      digitalWrite(FR_DIR, LOW);
    }
    else {
      digitalWrite(FR_DIR, HIGH);
    }
    analogWrite(FR_PWM, power);
  }
  else {
    digitalWrite(FR_DIR, LOW);
    analogWrite(FR_PWM, 0);
  }
}

// 右後モータの速度算出
void RL_motor(int stopFlag, int inverse, int power) {
  if (!stopFlag) {
    if (inverse) {
      digitalWrite(RL_DIR, HIGH);
    }
    else {
      digitalWrite(RL_DIR, LOW);
    }
    analogWrite(RL_PWM, power);
  }
  else {
    digitalWrite(RL_DIR, LOW);
    analogWrite(RL_PWM, 0);
  }
}

// 左後モータの速度算出
void RR_motor(int stopFlag, int inverse, int power) {
  if (!stopFlag) {
    if (inverse) {
      digitalWrite(RR_DIR, LOW);
    }
    else {
      digitalWrite(RR_DIR, HIGH);
      Serial.println("LOW");
    }
    analogWrite(RR_PWM, power);
  }
  else {
    digitalWrite(RR_DIR, LOW);
    analogWrite(RR_PWM, 0);
  }
}

void drive(int vx, int vy, int emg)
{
  vy = vy * -1;
  int dis = sqrt(vx * vx + vy * vy) ;
  Serial.println(dis);
  int deg = degrees(atan2(vy, vx));
  double p = (double)dis / 8;
  Serial.println(p);
  int power = (int)(255 * p);
  power = min(power, 255);
  Serial.println(power);

//  if (sw1Flag != 0 && emg != 0) {
//    FL_motor(0, 1, power);
//    FR_motor(0, 0, power);
//    RL_motor(0, 1, power);
//    RR_motor(0, 0, power);
//    Serial.println("turnLeft");
//  }
//  else if (sw2Flag != 0 && emg != 0) {
//    FL_motor(0, 0, power);
//    FR_motor(0, 1, power);
//    RL_motor(0, 0, power);
//    RR_motor(0, 1, power);
//    Serial.println("turnRight");
//  }

  if (vx == 0 && vy == 0 || emg == 0) {
    FL_motor(1, 0, 0);
    FR_motor(1, 0, 0);
    RL_motor(1, 0, 0);
    RR_motor(1, 0, 0);
    Serial.println("stop");
  }

  else if (-22 <= deg && deg < 23) {
    FL_motor(0, 0, power);
    FR_motor(0, 1, power);
    RL_motor(0, 1, power);
    RR_motor(0, 0, power);
    Serial.println("right");
  }

  else if (-23 <= deg && deg < 68) {
    FL_motor(0, 0, power);
    FR_motor(1, 0, power);
    RL_motor(1, 0, power);
    RR_motor(0, 0, power);
    Serial.println("rightForward");
  }
  else if (68 <= deg && deg < 113) {
    FL_motor(0, 0, power);
    FR_motor(0, 0, power);
    RL_motor(0, 0, power);
    RR_motor(0, 0, power);
    Serial.println("forward");
  }
  else if (113 <= deg && deg < 158) {
    FL_motor(1, 0, power);
    FR_motor(0, 0, power);
    RL_motor(0, 0, power);
    RR_motor(1, 0, power);
    Serial.println("leftForward");
  }

  else if (158 <= deg && deg <= 180 || -180 <= deg && deg < -157) {
    FL_motor(0, 1, power);
    FR_motor(0, 0, power);
    RL_motor(0, 0, power);
    RR_motor(0, 1, power);
    Serial.println("left");
  }

  else if (-157 <= deg && deg < -112) {
    FL_motor(0, 1, power);
    FR_motor(1, 0, power);
    RL_motor(1, 0, power);
    RR_motor(0, 1, power);
    Serial.println("leftBack");
  }

  else if (-112 <= deg && deg < -67) {
    FL_motor(0, 1, power);
    FR_motor(0, 1, power);
    RL_motor(0, 1, power);
    RR_motor(0, 1, power);
    Serial.println("back");
  }

  else if (-67 <= deg && deg < -22) {
    FL_motor(1, 0, power);
    FR_motor(0, 1, power);
    RL_motor(0, 1, power);
    RR_motor(1, 0, power);
    Serial.println("rightBack");
	}
}

///////////////////////////////////////////////////
// アーム上下 動作
// in    vy:  スティックY方向(たて)の傾き
//              0(下に倒した状態)～15(触れてない)～30(上に倒した状態)
//       ena: 動作許可(0:NG,1:OK) 
///////////////////////////////////////////////////
void arm_updown(int vy, int emg){
  if(emg==0){
    // 停止
    digitalWrite(UPDN_IN1, LOW);
    analogWrite(UPDN_EN, 0);		// モータ出力0％
  }
  else{
    if(vy > OUTVAL_HALF && digitalRead(LM_UP) == 1){
      // 正回転(上方向)
      digitalWrite(UPDN_IN1, LOW);
      vy = ((vy-OUTVAL_HALF) * PWM_MAX) / OUTVAL_HALF;
    }
    else
    if(vy < OUTVAL_HALF && digitalRead(LM_DN) == 1){
      // 逆回転(下方向)
      digitalWrite(UPDN_IN1, HIGH);
      vy = ((OUTVAL_HALF-vy) * PWM_MAX) / OUTVAL_HALF;
    }
    else{
      vy = 0;
    }
    analogWrite(UPDN_EN, vy);
  }
}

///////////////////////////////////////////////////
// アーム前後 動作
// in    vy:  スティックY方向(たて)の傾き
//              0(下に倒した状態)～15(触れてない)～30(上に倒した状態)
//       ena: 動作許可(0:NG,1:OK) 
///////////////////////////////////////////////////
void arm_frontback(int vy, int emg){
  if(emg==0){
    // 停止
    digitalWrite(FRBK_IN1, LOW);
    analogWrite(FRBK_EN, 0);		// モータ出力0％
  }
  else{
    if(vy > OUTVAL_HALF && digitalRead(LM_FR) == 1){
      // 正回転(前方向)
      digitalWrite(FRBK_IN1, LOW);
      vy = ((vy-OUTVAL_HALF) * PWM_MAX) / OUTVAL_HALF;
    }
    else
    if(vy < OUTVAL_HALF && digitalRead(LM_BK) == 1){
      // 逆回転(後方向)
      digitalWrite(FRBK_IN1, HIGH);
      vy = ((OUTVAL_HALF-vy) * PWM_MAX) / OUTVAL_HALF;
    }
    else{
      vy = 0;
    }
    analogWrite(FRBK_EN, vy);
  }
}
