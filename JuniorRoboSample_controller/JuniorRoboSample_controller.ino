
#define   TRANS_BITRATE   115200  // bitlate

#define   STICK_MARGIN 40
#define   OUTVAL_MAX   30

enum STICK_NAME{
  STICK1_X = 0,
  STICK1_Y,
  STICK2_X,
  STICK2_Y,
  MAX_STICK
};

enum STICK_INFO{
  STICK_PIN = 0,  // ピン番号
  STICK_REV,      // 反転フラグ
  STICK_MIN,      // 最小値
  STICK_MID,      // 中央値
  STICK_MAX,      // 最大値
  MAX_STICKINFO   // 情報の数
};

const int stick_info[MAX_STICK][MAX_STICKINFO]
             // PIN, R, MIN, MID, MAX
            = {{ A0, 0,  0, 512, 1023},    // STICK1_X
               { A1, 0,  0, 512, 1023},    // STICK1_Y
               { A3, 1,  0, 512, 1023},    // STICK2_X
               { A4, 1,  0, 512, 1023} };  // STICK2_Y
               

const int SW1 = 6;    //  6番ピンにスイッチ1を接続：コントローラ(0:不許可 1:許可)
const int SW2 = 7;    //  7番ピンにスイッチ2を接続：左旋回
const int SW3 = 8;    //  8番ピンにスイッチ3を接続：右旋回
const int SW4 = 9;    //  9番ピンにスイッチ4を接続：駆動、アーム切り替え
const int SW5 = 11;   // 11番ピンにスイッチ5を接続：未使用
const int SW6 = 12;   // 12番ピンにスイッチ6を接続：ハンド開閉

enum WRITE_DATA{
  DATA_STICK1_X = 0,
  DATA_STICK1_Y,
  DATA_STICK2_X,
  DATA_STICK2_Y,
  DATA_SW1,
  DATA_SW2,
  MAX_WRITEDATA
};

void setup() {
  Serial.begin(TRANS_BITRATE);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  pinMode(SW5, INPUT_PULLUP);
  pinMode(SW6, INPUT_PULLUP);
}

const int T_S = 10000;        // 制御のサンプリング周期 (usec)
long int startTime, endTime;  // 各周期の開始時間と終了時間
int stick_val_sub[MAX_STICK];

void loop() {
  startTime = micros();  // 制御周期開始時間を保存
  int stick_val[MAX_STICK];
  int swt1 = 1;
  int swt2 = 1;
  int swt3 = 1;
  int swt4 = 1;
  int swt5 = 1;
  int swt6 = 1;
  
  for (int i=0; i<MAX_STICK; i++){  // STICKの本数分、繰り返す
    // 値取得
    stick_val_sub[i] = stick_val[i] = analogRead( stick_info[i][STICK_PIN]);

    // 値を0～100の範囲に換算する
    if( stick_info[i][STICK_MAX] < stick_val[i] ){    // 最大値以上なら100
      stick_val[i] = OUTVAL_MAX;
    }else
    if(stick_val[i] < stick_info[i][STICK_MIN] ){     // 最小値以下なら0
      stick_val[i] = 0;
    }else
    if( stick_info[i][STICK_MID] - STICK_MARGIN <= stick_val[i]
         &&  stick_val[i] <= stick_info[i][STICK_MID] + STICK_MARGIN ){ // 中央付近だったら50
          stick_val[i] = OUTVAL_MAX/2;
    }
    else{
      stick_val[i]
        = (int)((float)OUTVAL_MAX * ( stick_val[i] - stick_info[i][STICK_MIN])
              / (stick_info[i][STICK_MAX] - stick_info[i][STICK_MIN]) );
    }
    
    if(stick_info[i][STICK_REV]){
      stick_val[i] = OUTVAL_MAX - stick_val[i];
    }
  } // forここまで
  
  if(digitalRead(SW1)){
    swt1 = 0;
  }
  if(digitalRead(SW2)){
    swt2 = 0;
  }
  if(digitalRead(SW3)){
    swt3 = 0;
  }
  if(digitalRead(SW4)){
    swt4 = 0;
  }
  if(digitalRead(SW5)){
    swt5 = 0;
  }
  if(digitalRead(SW6)){
    swt6 = 0;
  }
  
  uint8_t data[MAX_WRITEDATA] = {0,1,2,3,4,5};

  data[DATA_STICK1_X] |= (stick_val[STICK1_X] & 0x1f) << 3;
  data[DATA_STICK1_Y] |= (stick_val[STICK1_Y] & 0x1f) << 3;
  data[DATA_STICK2_X] |= (stick_val[STICK2_X] & 0x1f) << 3;
  data[DATA_STICK2_Y] |= (stick_val[STICK2_Y] & 0x1f) << 3;
  data[DATA_SW1] |= (swt1 & 0x1) << 3;
  data[DATA_SW1] |= (swt2 & 0x1) << 4;
  data[DATA_SW1] |= (swt3 & 0x1) << 5;
  data[DATA_SW1] |= (swt4 & 0x1) << 6;
  data[DATA_SW1] |= (swt5 & 0x1) << 7;
  data[DATA_SW2] |= (swt6 & 0x1) << 3;
  
  for(int i=0;i<MAX_WRITEDATA;i++){
    Serial.write(data[i]);
  }
  
//  Serial.print("x1:");
//  Serial.print(stick_val_sub[STICK1_X]);
//  Serial.print(" y1:");
//  Serial.print(stick_val_sub[STICK1_Y]);
//  Serial.print(" x2:");
//  Serial.print(stick_val[STICK2_X]);
//  Serial.print(" y2:");
//  Serial.print(stick_val[STICK2_Y]);
//  Serial.print(" SW1:");
//  Serial.print(swt1);
//  Serial.print(" SW2:");
//  Serial.println(swt2);

  endTime = micros();  // 制御周期終了時間を保存
  delayMicroseconds(max(0, startTime + T_S - endTime));  // 制御周期開始からT_S経つまで待つ
  
}
