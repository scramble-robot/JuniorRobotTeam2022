//
// JuniorRoboSample
//
// 初版作成：2022.9.6
//

//**********************
// ピン定義
//**********************
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

const int stick_x = A0;		// Analog 0番ピンにアナログスティックを接続
const int stick_y = A1;		// Analog 0番ピンにアナログスティックを接続

const int SW1 = A3;				// Analog 0番ピンにアナログスティックを接続
const int SW2 = A4;				// Analog 0番ピンにアナログスティックを接続

int vx = 0;		// アナログスティックX軸の傾きを入れる変数
int vy = 0;		// アナログスティックY軸の傾きを入れる変数


//**********************
// 初期設定関数
//**********************

void setup()
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

	pinMode(SW1, INPUT_PULLUP);		// SW1を入力モード（プルアップあり）で使用
	pinMode(SW2, INPUT_PULLUP);		// SW2を入力モード（プルアップあり）で使用
}


//**********************
// ループ関数
//**********************
void loop()
{
	vx = analogRead( stick_x );   // アナログスティックX軸の電圧を測定
	vy = analogRead( stick_y );   // アナログスティックY軸の電圧を測定

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
	}
	else if(digitalRead(SW1)==0){	// SW1が押されたら
		digitalWrite(FL_IN1, LOW);	// 左前モータを逆回転
		digitalWrite(FL_IN2, HIGH);
		digitalWrite(FR_IN1, LOW);	// 右前モータを逆回転
		digitalWrite(FR_IN2, HIGH);
		digitalWrite(RL_IN1, LOW);	// 左後モータを逆回転
		digitalWrite(RL_IN2, HIGH);
		digitalWrite(RR_IN1, LOW);	// 右後モータを逆回転
		digitalWrite(RR_IN2, HIGH);
	}
	else
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
