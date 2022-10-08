//
// JuniorRoboSample
// ジュニアロボットチーム用サンプルロボット　メインプログラム
//
// 初版作成：2022.9.6
//

#include <Servo.h>

// プロトタイプ宣言
void pinInit_drive(void);			// 駆動系 ピン設定
void pinInit_arm(void);       // アーム系 ピン設定
void drive(int vx, int vy);		// 駆動(メカナム)動作

//**********************
// ピン定義
//**********************

// 駆動
const int FL_IN1 = 1;			// 1番ピンに左前モータのIN1を接続
const int FL_IN2 = 2;			// 2番ピンに左前モータのIN2を接続
const int FL_EN  = 3;			// 3番ピンに左前モータのENAを接続

const int FR_IN1 = 4;			// 4番ピンに右前モータのIN3を接続
const int FR_IN2 = 7;			// 7番ピンに右前モータのIN4を接続
const int FR_EN  = 5;			// 5番ピンに右前モータのENBを接続

const int RL_IN1 = 8;			// 8番ピンに左後モータのIN1を接続
const int RL_IN2 = 9;			// 9番ピンに左後モータのIN2を接続
const int RL_EN  = 10;		// 10番ピンに左後モータのENAを接続

const int RR_IN1 = 12;		// 12番ピンに右後モータのIN3を接続
const int RR_IN2 = 13;		// 13番ピンに右後モータのIN4を接続
const int RR_EN  = 11;		// 11番ピンに右後モータのENBを接続

// アーム
const int UPDN_IN1 = 1;		// 1番ピンに上下モータのIN1を接続
const int UPDN_IN2 = 2;		// 2番ピンに上下モータのIN2を接続
const int UPDN_EN  = 3;		// 3番ピンに上下モータのENAを接続

const int FRBK_IN1 = 4;		// 4番ピンに前後モータのIN3を接続
const int FRBK_IN2 = 7;		// 7番ピンに前後モータのIN4を接続
const int FRBK_EN  = 5;		// 5番ピンに前後モータのENBを接続

// アームリミット
const int LM_UP = 3;			// 3番ピンにリミットスイッチ上を接続
const int LM_DN = 4;			// 4番ピンにリミットスイッチ下を接続

const int LM_FR = 3;			// 3番ピンにリミットスイッチ前を接続
const int LM_BK = 4;			// 4番ピンにリミットスイッチ後を接続

// ハンドサーボ
Servo servo_hand;
const int SRV_HAND = 8;   // 8番ピンにハンド用サーボモータのPWM入力を接続

// その他
const int stick_x = A0;		// Analog 0番ピンにアナログスティックX軸を接続
const int stick_y = A1;		// Analog 1番ピンにアナログスティックY軸を接続

const int SW1 = A3;				// Analog 3番ピンにスイッチ1を接続
const int SW2 = A4;				// Analog 4番ピンにスイッチ2を接続


//**********************
// 初期設定関数
//**********************
void setup()
{
	pinInit_drive();							// 駆動系(メカナム)ピン初期化
  pinInit_arm();                // アーム系ピン初期化
  servo_hand.attach(SRV_HAND);  // ハンド用サーボ ピン設定

	pinMode(SW1, INPUT_PULLUP);		// SW1を入力モード（プルアップあり）で使用
	pinMode(SW2, INPUT_PULLUP);		// SW2を入力モード（プルアップあり）で使用
}

/////////////////////
// 駆動系 ピン設定
/////////////////////
void pinInit_drive(void)
{
	pinMode(FL_IN1, OUTPUT);		// FL_IN1を出力モードで使用
	pinMode(FL_IN2, OUTPUT);		// FL_IN2を出力モードで使用
	pinMode(FL_EN, OUTPUT);			// FL_ENを出力モードで使用
	analogWrite(FL_EN, 255);		// モータ出力100％

	pinMode(FR_IN1, OUTPUT);		// FR_IN1を出力モードで使用
	pinMode(FR_IN2, OUTPUT);		// FR_IN2を出力モードで使用
	pinMode(FR_EN, OUTPUT);			// FR_ENを出力モードで使用
	analogWrite(FR_EN, 255);		// モータ出力100％

	pinMode(RL_IN1, OUTPUT);		// RL_IN1を出力モードで使用
	pinMode(RL_IN2, OUTPUT);		// RL_IN2を出力モードで使用
	pinMode(RL_EN, OUTPUT);			// RL_ENを出力モードで使用
	analogWrite(RL_EN, 255);		// モータ出力100％

	pinMode(RR_IN1, OUTPUT);		// RR_IN1を出力モードで使用
	pinMode(RR_IN2, OUTPUT);		// RR_IN2を出力モードで使用
	pinMode(RR_EN, OUTPUT);			// RR_ENを出力モードで使用
	analogWrite(RR_EN, 255);		// モータ出力100％
}

/////////////////////
// アーム系 ピン設定
/////////////////////
void pinInit_arm(void)
{
	pinMode(UPDN_IN1, OUTPUT);	    // FL_IN1を出力モードで使用
	pinMode(UPDN_IN2, OUTPUT);	    // FL_IN2を出力モードで使用
	pinMode(UPDN_EN, OUTPUT);		    // FL_ENを出力モードで使用
	analogWrite(UPDN_EN, 255);	    // モータ出力100％

	pinMode(FRBK_IN1, OUTPUT);		  // FR_IN1を出力モードで使用
	pinMode(FRBK_IN2, OUTPUT);		  // FR_IN2を出力モードで使用
	pinMode(FRBK_EN, OUTPUT);			  // FR_ENを出力モードで使用
	analogWrite(FRBK_EN, 255);		  // モータ出力100％

  pinMode(LM_UP, INPUT_PULLUP);   // リミットスイッチ上を入力モード（プルアップ）で使用
  pinMode(LM_DN, INPUT_PULLUP);   // リミットスイッチ下を入力モード（プルアップ）で使用
  pinMode(LM_FR, INPUT_PULLUP);   // リミットスイッチ前を入力モード（プルアップ）で使用
  pinMode(LM_BK, INPUT_PULLUP);   // リミットスイッチ後を入力モード（プルアップ）で使用
}

//**********************
// ループ関数
//**********************
void loop()
{
	int vx = 0;		// アナログスティックX軸の傾きを入れる変数
	int vy = 0;		// アナログスティックY軸の傾きを入れる変数

	vx = analogRead( stick_x );   // アナログスティックX軸の電圧を測定
	vy = analogRead( stick_y );   // アナログスティックY軸の電圧を測定

	drive(vx,vy);
}

/////////////////////////////
// 駆動（メカナム）動作
// in		vx: Xスティックの傾き
//			vy: Yスティックの傾き
/////////////////////////////
void drive(int vx, int vy)
{
	if(vx < 411){	// スティックが左に傾いていれば
		digitalWrite(FL_IN1, HIGH);	// 左前モータを正回転
		digitalWrite(FL_IN2, LOW);
		digitalWrite(FR_IN1, HIGH);	// 右前モータを正回転
		digitalWrite(FR_IN2, LOW);
		digitalWrite(RL_IN1, LOW);	// 左後モータを逆回転
		digitalWrite(RL_IN2, HIGH);
		digitalWrite(RR_IN1, LOW);	// 右後モータを逆回転
		digitalWrite(RR_IN2, HIGH);
	}else
	if(vx > 611){	// スティックが右に傾いていれば
		digitalWrite(FL_IN1, LOW);	// 左前モータを逆回転
		digitalWrite(FL_IN2, HIGH);
		digitalWrite(FR_IN1, LOW);	// 右前モータを逆回転
		digitalWrite(FR_IN2, HIGH);
		digitalWrite(RL_IN1, HIGH);	// 左後モータを正回転
		digitalWrite(RL_IN2, LOW);
		digitalWrite(RR_IN1, HIGH);	// 右後モータを正回転
		digitalWrite(RR_IN2, LOW);
	}else
	if(vy < 411){	// スティックが上に傾いていれば
		digitalWrite(FL_IN1, LOW);	// 左前モータを逆回転
		digitalWrite(FL_IN2, HIGH);
		digitalWrite(FR_IN1, HIGH);	// 右前モータを正回転
		digitalWrite(FR_IN2, LOW);
		digitalWrite(RL_IN1, LOW);	// 左後モータを逆回転
		digitalWrite(RL_IN2, HIGH);
		digitalWrite(RR_IN1, HIGH);	// 右後モータを正回転
		digitalWrite(RR_IN2, LOW);
	}else
	if(vy > 611){	// スティックが下に傾いていれば
		digitalWrite(FL_IN1, HIGH);	// 左前モータを正回転
		digitalWrite(FL_IN2, LOW);
		digitalWrite(FR_IN1, LOW);	// 右前モータを逆回転
		digitalWrite(FR_IN2, HIGH);
		digitalWrite(RL_IN1, HIGH);	// 左後モータを正回転
		digitalWrite(RL_IN2, LOW);
		digitalWrite(RR_IN1, LOW);	// 右後モータを逆回転
		digitalWrite(RR_IN2, HIGH);
	}else
	if(digitalRead(SW1)==0){	// SW1が押されたら
		digitalWrite(FL_IN1, LOW);	// 左前モータを逆回転
		digitalWrite(FL_IN2, HIGH);
		digitalWrite(FR_IN1, LOW);	// 右前モータを逆回転
		digitalWrite(FR_IN2, HIGH);
		digitalWrite(RL_IN1, LOW);	// 左後モータを逆回転
		digitalWrite(RL_IN2, HIGH);
		digitalWrite(RR_IN1, LOW);	// 右後モータを逆回転
		digitalWrite(RR_IN2, HIGH);
	}else
	if(digitalRead(SW2)==0){	// SW2が押されたら
		digitalWrite(FL_IN1, HIGH);	// 左前モータを正回転
		digitalWrite(FL_IN2, LOW);
		digitalWrite(FR_IN1, HIGH);	// 右前モータを正回転
		digitalWrite(FR_IN2, LOW);
		digitalWrite(RL_IN1, HIGH);	// 左後モータを正回転
		digitalWrite(RL_IN2, LOW);
		digitalWrite(RR_IN1, HIGH);	// 右後モータを正回転
		digitalWrite(RR_IN2, LOW);    
	}
	else{   // スティックが傾いていない　かつ　ボタンが押されていなければ全てのモータをブレーキ
		digitalWrite(FL_IN1, HIGH);
		digitalWrite(FL_IN2, HIGH);
		digitalWrite(FR_IN1, HIGH);
		digitalWrite(FR_IN2, HIGH);
		digitalWrite(RL_IN1, HIGH);
		digitalWrite(RL_IN2, HIGH);
		digitalWrite(RR_IN1, HIGH);
		digitalWrite(RR_IN2, HIGH);
	}
}
