//
// JuniorRoboSample
// ジュニアロボットチーム用サンプルロボット　メインプログラム
//
// 初版作成：2022.9.6
//

// プロトタイプ宣言
void pinInit_drive(void);           // 駆動系 ピン設定
void drive(int vx, int vy, int emg);         // 駆動(メカナム)動作
void dataProcess(uint8_t data[]);   // 受信データ解析

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


//**********************
// 初期設定関数
//**********************
void setup()
{
	Serial2.begin(TRANS_BITRATE);

	pinInit_drive();							// 駆動系(メカナム)ピン初期化

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
  
  // スイッチ情報
  sw1 = (data[4] >> 3) & 0x1; // コントローラ SW1 [0:OFF 1:ON]
  sw2 = (data[4] >> 4) & 0x1; // コントローラ SW2 [0:OFF 1:ON]
  sw3 = (data[4] >> 5) & 0x1; // コントローラ SW3 [0:OFF 1:ON]
  sw4 = (data[4] >> 6) & 0x1; // コントローラ SW4 [0:OFF 1:ON]
  
  // 駆動 動作
  drive(stick_val[0], stick_val[1], sw1); // 1_X, 1_Y, sw1
}

///////////////////////////////////////////////////
// 駆動 動作
// in    vx:  スティックX方向(よこ)の傾き
//              0(左に倒した状態)～15(触れてない)～30(右に倒した状態)
//       vy:  スティックY方向(たて)の傾き
//              0(下に倒した状態)～15(触れてない)～30(上に倒した状態)
//       ena: 動作許可(0:NG,1:OK) 
///////////////////////////////////////////////////
void drive(int vx, int vy, int emg)
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
