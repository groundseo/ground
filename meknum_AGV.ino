#include <Stepper.h>
#include <SPI.h>
#include <MFRC522.h>
////////////stepmoter//////
const int stepsPerRevolution = 1500;
Stepper myStepper(stepsPerRevolution, 40, 41, 47, 48);
////////////ir//////
#define sensor_R 29 
#define sensor_L 30
#define sensor_F 25 
#define sensor_B 38
int val_R; int val_L; int val_F; int val_B;
/////////////RFid///////
#define RST_PIN 44
#define SS_PIN 53
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key; 
byte nuidPICC[4];
//////////////////
int mode; //1:적재 2:출하 3:이동
int x = 1; int y = 2;//현재좌표
int i=1; int j=1; //read좌표
int sand; int k; int n;
int pre_x; int pre_y;//직전좌표 보정할때 사용
#define in_i 1 //입구 좌표
#define in_j 2
#define out_i 3 //출구 좌표
#define out_j 2
////////////////////////
#define PWMA 12    //A모터방향
#define DIRA1 34 
#define DIRA2 35  //A모터 회전수

#define PWMB 8    //B모터 방향
#define DIRB1 37 
#define DIRB2 36  //B모터 회전수

#define PWMC 9   //C모터 방향
#define DIRC1 43 
#define DIRC2 42  //C모터 회전수

#define PWMD 5    //D모터 방향
#define DIRD1 A4  //26  
#define DIRD2 A5  //27  //D모터 회전수

#define MOTORA_FORWARD(pwm)    do{digitalWrite(DIRA1,LOW); digitalWrite(DIRA2,HIGH);analogWrite(PWMA,pwm);}while(0)
#define MOTORA_STOP(x)         do{digitalWrite(DIRA1,LOW); digitalWrite(DIRA2,HIGH); analogWrite(PWMA,0);}while(0)
#define MOTORA_BACKOFF(pwm)    do{digitalWrite(DIRA1,HIGH);digitalWrite(DIRA2,LOW); analogWrite(PWMA,pwm);}while(0)

#define MOTORB_FORWARD(pwm)    do{digitalWrite(DIRB1,HIGH); digitalWrite(DIRB2,LOW);analogWrite(PWMB,pwm);}while(0)
#define MOTORB_STOP(x)         do{digitalWrite(DIRB1,HIGH); digitalWrite(DIRB2,LOW); analogWrite(PWMB,0);}while(0)
#define MOTORB_BACKOFF(pwm)    do{digitalWrite(DIRB1,LOW);digitalWrite(DIRB2,HIGH); analogWrite(PWMB,pwm);}while(0)

#define MOTORC_FORWARD(pwm)    do{digitalWrite(DIRC1,LOW); digitalWrite(DIRC2,HIGH);analogWrite(PWMC,pwm+5);}while(0)
#define MOTORC_STOP(x)         do{digitalWrite(DIRC1,LOW); digitalWrite(DIRC2,HIGH); analogWrite(PWMC,0);}while(0)
#define MOTORC_BACKOFF(pwm)    do{digitalWrite(DIRC1,HIGH);digitalWrite(DIRC2,LOW); analogWrite(PWMC,pwm+5);}while(0)

#define MOTORD_FORWARD(pwm)    do{digitalWrite(DIRD1,HIGH); digitalWrite(DIRD2,LOW);analogWrite(PWMD,pwm+5);}while(0)
#define MOTORD_STOP(x)         do{digitalWrite(DIRD1,HIGH); digitalWrite(DIRD2,LOW); analogWrite(PWMD,0);}while(0)
#define MOTORD_BACKOFF(pwm)    do{digitalWrite(DIRD1,LOW);digitalWrite(DIRD2,HIGH); analogWrite(PWMD,pwm+5);}while(0)
 
#define SERIAL  Serial//디버그시리얼

#define MAX_PWM   100
#define MIN_PWM   40
int Motor_PWM = 40;
int Motor_PWM_2 = 75; //ir보정 pwm

void IR_READ()
{
  val_R = digitalRead(sensor_R); val_L = digitalRead(sensor_L);
  val_F = digitalRead(sensor_F); val_B = digitalRead(sensor_B);
}
//    ↑A-----B↑   
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑
void WAY_11h()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_FORWARD(Motor_PWM_2);    
  MOTORC_FORWARD(Motor_PWM_2);MOTORD_FORWARD(Motor_PWM);    
}
void WAY_12h()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_FORWARD(Motor_PWM);    
  MOTORC_FORWARD(Motor_PWM);MOTORD_FORWARD(Motor_PWM);    
}
void WAY_1h()
{
  MOTORA_FORWARD(Motor_PWM_2);MOTORB_FORWARD(Motor_PWM);    
  MOTORC_FORWARD(Motor_PWM);MOTORD_FORWARD(Motor_PWM_2);    
}
//    ↑A-----B↓   
//     |  →  |
//     |  →  |
//    ↓C-----D↑
void WAY_2h()
{
  MOTORA_FORWARD(Motor_PWM_2);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_FORWARD(Motor_PWM_2);
}
void WAY_3h()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_FORWARD(Motor_PWM);
}
void WAY_4h()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_BACKOFF(Motor_PWM_2);
  MOTORC_BACKOFF(Motor_PWM_2);MOTORD_FORWARD(Motor_PWM);
}
//    ↓A-----B↓   
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓
void WAY_5h()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_BACKOFF(Motor_PWM_2);
  MOTORC_BACKOFF(Motor_PWM_2);MOTORD_BACKOFF(Motor_PWM);
}
void WAY_6h()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
}
void WAY_7h()
{
  MOTORA_BACKOFF(Motor_PWM_2);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_BACKOFF(Motor_PWM_2);
}
//    ↓A-----B↑   
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓
void WAY_8h()
{
  MOTORA_BACKOFF(Motor_PWM_2);MOTORB_FORWARD(Motor_PWM);
  MOTORC_FORWARD(Motor_PWM);MOTORD_BACKOFF(Motor_PWM_2);
}
void WAY_9h()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_FORWARD(Motor_PWM);
  MOTORC_FORWARD(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
}
void WAY_10h()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_FORWARD(Motor_PWM_2);
  MOTORC_FORWARD(Motor_PWM_2);MOTORD_BACKOFF(Motor_PWM);
}
//    ↑A-----B↓   
//     |  ⟳  |
//     |     |
//    ↑C-----D↓
void CW()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_FORWARD(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
}
void CW_12()
{
  MOTORA_FORWARD(Motor_PWM_2);MOTORB_FORWARD(Motor_PWM);    
  MOTORC_FORWARD(Motor_PWM_2);MOTORD_FORWARD(Motor_PWM);
}
void CW_3h()
{
  MOTORA_FORWARD(Motor_PWM_2);MOTORB_BACKOFF(Motor_PWM_2);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_FORWARD(Motor_PWM); 
}
void CW_6h()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_BACKOFF(Motor_PWM_2);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_BACKOFF(Motor_PWM_2);
}
void CW_9h()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_FORWARD(Motor_PWM);
  MOTORC_FORWARD(Motor_PWM_2);MOTORD_BACKOFF(Motor_PWM_2);
}
//    ↓A-----B↑   
//     |  ⤺  |
//     |  ⤻  |
//    ↓C-----D↑
void UCW()
{
  MOTORA_BACKOFF(Motor_PWM);MOTORB_FORWARD(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM);MOTORD_FORWARD(Motor_PWM);
}
void UCW_12h()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_FORWARD(Motor_PWM_2);    
  MOTORC_FORWARD(Motor_PWM);MOTORD_FORWARD(Motor_PWM_2);
}
void UCW_3h()
{
  MOTORA_FORWARD(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM_2);MOTORD_FORWARD(Motor_PWM_2); 
}
void UCW_6h()
{
  MOTORA_BACKOFF(Motor_PWM_2);MOTORB_BACKOFF(Motor_PWM);
  MOTORC_BACKOFF(Motor_PWM_2);MOTORD_BACKOFF(Motor_PWM);
}
void UCW_9h()
{
  MOTORA_BACKOFF(Motor_PWM_2);MOTORB_FORWARD(Motor_PWM_2);
  MOTORC_FORWARD(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
}
//    =A-----B=  
//     |  =  |
//     |  =  |
//    =C-----D=
void STOP()
{
  MOTORA_STOP(Motor_PWM);MOTORB_STOP(Motor_PWM);
  MOTORC_STOP(Motor_PWM);MOTORD_STOP(Motor_PWM);
}

void ADVANCE()
{
  IR_READ();
  if((val_F == 1 && val_B == 1 && val_L == 0 && val_R == 0) ||
     (val_F == 1 && val_B == 1 && val_L == 1 && val_R == 1)){ WAY_12h(); }
  else if((val_F == 1 && val_B == 0 && val_L == 0 && val_R == 1) || 
          (val_F == 0 && val_B == 1 && val_L == 1 && val_R == 0)){ UCW_12h(); }//회전하며 전진
  else if((val_F == 1 && val_B == 0 && val_L == 1 && val_R == 0) ||
          (val_F == 0 && val_B == 1 && val_L == 0 && val_R == 1)){ CW_12(); }
  else if((val_F == 0 && val_B == 0 && val_L == 0 && val_R == 1) || 
          (val_F == 1 && val_B == 1 && val_L == 0 && val_R == 1)){ WAY_1h(); } //평행
  else if((val_F == 0 && val_B == 0 && val_L == 1 && val_R == 0) ||
          (val_F == 1 && val_B == 1 && val_L == 1 && val_R == 0)){ WAY_11h(); }
  else{ WAY_12h(); }
}

void BACK()
{
  IR_READ();
  if((val_F == 1 && val_B == 1 && val_L == 0 && val_R == 0) ||
     (val_F == 1 && val_B == 1 && val_L == 1 && val_R == 1)){ WAY_6h(); }
  else if((val_F == 1 && val_B == 0 && val_L == 0 && val_R == 1) || 
          (val_F == 0 && val_B == 1 && val_L == 1 && val_R == 0)){ UCW_6h(); }//회전하며 후진
  else if((val_F == 1 && val_B == 0 && val_L == 1 && val_R == 0) ||
          (val_F == 0 && val_B == 1 && val_L == 0 && val_R == 1)){ CW_6h(); }
  else if((val_F == 0 && val_B == 0 && val_L == 0 && val_R == 1) ||
          (val_F == 1 && val_B == 1 && val_L == 0 && val_R == 1)){ WAY_5h(); }//평행
  else if((val_F == 0 && val_B == 0 && val_L == 1 && val_R == 0) ||
          (val_F == 1 && val_B == 1 && val_L == 1 && val_R == 0)){ WAY_7h(); }
  else{ WAY_6h(); }
}

void LEFT()
{
  IR_READ();
  if((val_F == 0 && val_B == 0 && val_L == 1 && val_R == 1) ||
     (val_F == 1 && val_B == 1 && val_L == 1 && val_R == 1)){ WAY_9h(); }
  else if((val_F == 1 && val_B == 0 && val_L == 0 && val_R == 1) || 
          (val_F == 0 && val_B == 1 && val_L == 1 && val_R == 0)){ CW_9h(); }//회전하며 9시방향
  else if((val_F == 1 && val_B == 0 && val_L == 1 && val_R == 0) ||
          (val_F == 0 && val_B == 1 && val_L == 0 && val_R == 1)){ UCW_9h(); }
  else if((val_F == 1 && val_B == 0 && val_L == 0 && val_R == 0) || 
          (val_F == 1 && val_B == 0 && val_L == 1 && val_R == 1)){ WAY_10h(); }//평행
  else if((val_F == 0 && val_B == 1 && val_L == 0 && val_R == 0) ||
          (val_F == 0 && val_B == 1 && val_L == 1 && val_R == 1)){ WAY_8h(); }
  else{ WAY_9h(); }
}

void RIGHT()
{
  IR_READ();
  if((val_F == 0 && val_B == 0 && val_L == 1 && val_R == 1) ||
     (val_F == 1 && val_B == 1 && val_L == 1 && val_R == 1)){ WAY_3h(); }
  else if((val_F == 1 && val_B == 0 && val_L == 0 && val_R == 1) || 
          (val_F == 0 && val_B == 1 && val_L == 1 && val_R == 0)){ CW_3h(); }//회전하며 3시방향
  else if((val_F == 1 && val_B == 0 && val_L == 1 && val_R == 0) ||
          (val_F == 0 && val_B == 1 && val_L == 0 && val_R == 1)){ UCW_3h(); }
  else if((val_F == 1 && val_B == 0 && val_L == 0 && val_R == 0) ||
          (val_F == 1 && val_B == 0 && val_L == 1 && val_R == 1)){ WAY_2h(); }//평행
  else if((val_F == 0 && val_B == 1 && val_L == 0 && val_R == 0) ||
          (val_F == 0 && val_B == 1 && val_L == 1 && val_R == 1)){ WAY_4h(); }
  else{ WAY_3h(); }
}

void CROSS_YSET()//ir을 y축에 정렬
{
  IR_READ();
  if((val_F == 1 && val_B == 0 && val_L == 0 && val_R == 1) ||
     (val_F == 0 && val_B == 1 && val_L == 1 && val_R == 0)){ CW(); }
  else if((val_F == 1 && val_B == 0 && val_L == 1 && val_R == 0) ||
          (val_F == 0 && val_B == 1 && val_L == 0 && val_R == 1)){ UCW(); }
  else if((val_F == 0 && val_B == 0 && val_L == 0 && val_R == 1) ||
          (val_F == 1 && val_B == 1 && val_L == 0 && val_R == 1)){ WAY_3h(); }
  else if((val_F == 0 && val_B == 0 && val_L == 1 && val_R == 0) ||
          (val_F == 1 && val_B == 1 && val_L == 1 && val_R == 0)){ WAY_9h(); }
  else{ n++; }
}
void CROSS_XSET()//ir을 x축에 정렬
{
  IR_READ();
  if((val_F == 1 && val_B == 0 && val_L == 0 && val_R == 1) ||
     (val_F == 0 && val_B == 1 && val_L == 1 && val_R == 0)){ CW(); }
  else if((val_F == 1 && val_B == 0 && val_L == 1 && val_R == 0) ||
          (val_F == 0 && val_B == 1 && val_L == 0 && val_R == 1)){ UCW(); }
  else if((val_F == 1 && val_B == 0 && val_L == 0 && val_R == 1) ||
          (val_F == 1 && val_B == 0 && val_L == 1 && val_R == 1)){ WAY_12h(); }
  else if((val_F == 0 && val_B == 1 && val_L == 0 && val_R == 0) ||
          (val_F == 0 && val_B == 1 && val_L == 1 && val_R == 1)){ WAY_6h(); }  
  else{ n++; }
}
void CROSS_SET()
{
  IR_READ();
  if(val_F == 1 && val_B == 1 && val_L == 1 && val_R == 1){ STOP(); }
  else if((x - pre_x) == 0)
  {
    for(n = 0; n < 1;){ CROSS_XSET(); if(val_F == 1 && val_B == 1){STOP(); n++;}}
    if((y - pre_y) == 1) //(0,1)방향 보정
    { for(n = 0; n < 1;){ ADVANCE(); if(val_L == 1 && val_R == 1){STOP(); n++;}} }
    else//(0,-1)방향 보정
    { for(n = 0; n < 1;){ BACK(); if(val_L == 1 && val_R == 1){STOP(); n++;}} }
  }
  else
  {
    for(n = 0; n < 1;){ CROSS_YSET(); if(val_L == 1 && val_R == 1){STOP(); n++;}}
    if(x - pre_x == 1) //(1,0)방향 보정
    { for(n = 0; n < 1;){ RIGHT(); if(val_F == 1 && val_B == 1){STOP(); n++;}} }
    else //(-1,0)방향 보정
    { for(n = 0; n < 1;){ LEFT(); if(val_F == 1 && val_B == 1){STOP(); n++;}} }
  }
}
void FORK_CROSS_SET()//포크구동후 크로스셋팅
{
  IR_READ();
  if(val_F == 1 && val_B == 1 && val_L == 1 && val_R == 1){ STOP(); }
  else
  {
    for(n = 0; n < 1;){ CROSS_XSET(); if(val_F == 1 && val_B == 1){STOP(); n++;}}
    for(n = 0; n < 1;){ BACK(); if(val_L == 1 && val_R == 1){STOP(); n++;}}
  }
}

void FORK_UP()
{
  myStepper.step(-stepsPerRevolution); delay(2500);
}

void FORK_DOWN()
{
  myStepper.step(stepsPerRevolution); delay(2500);
}

void RFID_X_move()
{
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ){ return; }
  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || 
      rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] ) 
  {
    pre_x = x; pre_y = y;
    x++; sand = 10*x+y;
    Serial2.write(sand);
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) { nuidPICC[i] = rfid.uid.uidByte[i]; }
   }
}

void RFID_X_move_ne()
{
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ){ return; }
  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || 
      rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] ) 
  {
    pre_x = x; pre_y = y;
    x--; sand = 10*x+y;
    Serial2.write(sand);
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) { nuidPICC[i] = rfid.uid.uidByte[i]; }
  }
}

void RFID_Y_move()
{
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ){ return; }
  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || 
      rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] ) 
  {
    pre_x = x; pre_y = y;
    y++; sand = 10*x+y;
    Serial2.write(sand);
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) { nuidPICC[i] = rfid.uid.uidByte[i]; }
  }
}

void RFID_Y_move_ne()
{
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ){ return; }
  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || 
      rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] ) 
  {
    pre_x = x; pre_y = y;
    y--; sand = 10*x+y;
    Serial2.write(sand);
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) { nuidPICC[i] = rfid.uid.uidByte[i]; }
  }
}

void FORK_RFID()
{
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ){ return; }
  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || 
      rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] ) 
  {
    STOP(); k++;
    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) { nuidPICC[i] = rfid.uid.uidByte[i]; }
  }
}

void IO_init()
{
   pinMode(PWMA, OUTPUT); pinMode(DIRA1, OUTPUT);pinMode(DIRA2, OUTPUT);
   pinMode(PWMB, OUTPUT); pinMode(DIRB1, OUTPUT);pinMode(DIRB2, OUTPUT);
   pinMode(PWMC, OUTPUT); pinMode(DIRC1, OUTPUT);pinMode(DIRC2, OUTPUT);
   pinMode(PWMD, OUTPUT); pinMode(DIRD1, OUTPUT);pinMode(DIRD2, OUTPUT);
   STOP();
}

void setup()
{
  SERIAL.begin(9600);
  Serial2.begin(9600);
  SPI.begin(); // Init SPI bus
  myStepper.setSpeed(30);
  IO_init();
  rfid.PCD_Init(); // Init MFRC522 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  pinMode(sensor_R, INPUT); pinMode(sensor_L, INPUT);
  pinMode(sensor_F, INPUT); pinMode(sensor_B, INPUT);
}

void loop()
{
  if (Serial2.available())
  {
    mode= Serial2.read()-48;
    delay(100);
    i= Serial2.read()-48;
    delay(100);
    j= Serial2.read()-48;
    if(i > out_i){i = out_i;} if(i < 1){i = 1;}
    if(j > out_j){j = out_j;} if(j < 1){j = 1;}
    k = 0; n = 0;
  
  switch(mode)
  {
  case 1://입하
  {
    if(x < 2 || x > 2)
    {
      for(y ; y < in_j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > in_j ; ){ BACK(); RFID_Y_move_ne();}
      CROSS_SET(); delay(250);
      for(x ; x < in_i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > in_i ; ){ LEFT(); RFID_X_move_ne();}
      CROSS_SET(); delay(250);
    }
    else
    {
      for(x ; x < in_i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > in_i ; ){ LEFT(); RFID_X_move_ne();}
      CROSS_SET(); delay(250);
      for(y ; y < in_j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > in_j ; ){ BACK(); RFID_Y_move_ne();}
      CROSS_SET(); delay(250);
    }
    for( k ; k < 1; ){ADVANCE(); FORK_RFID();}//포크 구동
    for( k ; k < 2; k++){ FORK_UP();}
    for( k ; k < 3; ){BACK(); FORK_RFID();}
    FORK_CROSS_SET(); delay(250);
    for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}//받은 좌표로 이동
    for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
    CROSS_SET(); delay(250);
    for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
    for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
    CROSS_SET(); delay(250); 
    for( k ; k < 4; ){ADVANCE(); FORK_RFID();}//포크 구동후 스톱
    for( k ; k < 5; k++){ FORK_DOWN();}
    for( k ; k < 6; ){BACK(); FORK_RFID();}
    FORK_CROSS_SET();
  }break;
  
  case 2://출하
  {
    if( x+i < ((out_i-x+1)+(out_i-i+1)) ) //in_i로 먼저 이동
    {
      if(y == j)
      {
        for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
        for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
        CROSS_SET(); delay(250);
      }
      else
      {
        for(x ; x > in_i ; ){ LEFT(); RFID_X_move_ne(); }
        CROSS_SET(); delay(250);
      }
      for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
      CROSS_SET(); delay(250);
      for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
      CROSS_SET(); delay(250);
    }
    else //out_i로 먼저 이동
    {
      if(y == j)
      {
        for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
        for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
        CROSS_SET(); delay(250);
      }
      else
      {
        for(x ; x < out_i ; ){ RIGHT(); RFID_X_move();}
        CROSS_SET(); delay(250);
      }
      for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
      CROSS_SET(); delay(250);
      for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
      CROSS_SET(); delay(250);
    }
    for( k ; k < 1; ){ADVANCE(); FORK_RFID();}
    for( k ; k < 2; k++){ FORK_UP();}
    for( k ; k < 3; ){BACK(); FORK_RFID();}
    FORK_CROSS_SET(); delay(250);
    for(x ; x < out_i ; ){ RIGHT(); RFID_X_move();}
    for(x ; x > out_i ; ){ LEFT(); RFID_X_move_ne();}
    CROSS_SET(); delay(250);
    for(y ; y < out_j ; ){ ADVANCE(); RFID_Y_move();}
    for(y ; y > out_j ; ){ BACK(); RFID_Y_move_ne();}
    CROSS_SET(); delay(250); 
    for( k ; k < 4; ){ADVANCE(); FORK_RFID();}
    for( k ; k < 5; k++){ FORK_DOWN();}
    for( k ; k < 6; ){BACK(); FORK_RFID();}
    FORK_CROSS_SET();
  }break;

  case 3://이동
  {
    if( x+i < ((out_i-x+1)+(out_i-i+1)) ) //in_i로 먼저 이동
    {
      if(y == j)
      {
        for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
        for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
        CROSS_SET(); delay(250);
      }
      else
      {
        for(x ; x > in_i ; ){ LEFT(); RFID_X_move_ne(); }
        CROSS_SET(); delay(250);
      }
      for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
      CROSS_SET(); delay(250);
      for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
      CROSS_SET(); delay(250);
    }
    else //out_i로 먼저 이동
    {
      if(y == j)
      {
        for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
        for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
        CROSS_SET(); delay(250);
      }
      else
      {
        for(x ; x < out_i ; ){ RIGHT(); RFID_X_move();}
        CROSS_SET(); delay(250);
      }
      for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
      CROSS_SET(); delay(250);
      for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
      CROSS_SET(); delay(250);
    }
  }break;
  }
  }
}
