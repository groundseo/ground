#include <Stepper.h>
#include <SPI.h>
#include <MFRC522.h>
////////////stepmoter//////
const int stepsPerRevolution = 400;
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
int mode; //1:적재  2:출하 3:이동
int x = 1; int y = 2;//현재좌표
int i=1; int j=1; //read좌표
int sand; int k;
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
#define SERIAL2  Serial2//디버그블루투스시리얼

#define LOG_DEBUG

#ifdef LOG_DEBUG
#define M_LOG SERIAL.print
#else
#define M_LOG 
#endif

#define MAX_PWM   100
#define MIN_PWM   40
int Motor_PWM = 45;
int Motor_PWM_2 = 60; //ir보정 pwm

//    ↑A-----B↑   
//     |  ↑  |
//     |  |  |
//    ↑C-----D↑
void ADVANCE()
{
  val_R = digitalRead(sensor_R);
  val_L = digitalRead(sensor_L);
  if(val_R == 0 && val_L == 0)
  {
    MOTORA_FORWARD(Motor_PWM);MOTORB_FORWARD(Motor_PWM);    
    MOTORC_FORWARD(Motor_PWM);MOTORD_FORWARD(Motor_PWM);    
  }
  else if(val_R == 0 && val_L == 1)
  {
    MOTORA_FORWARD(Motor_PWM);MOTORB_FORWARD(Motor_PWM_2);    
    MOTORC_FORWARD(Motor_PWM);MOTORD_FORWARD(Motor_PWM_2);    
  }
  else if(val_R == 1 && val_L == 0)
  {
    MOTORA_FORWARD(Motor_PWM_2);MOTORB_FORWARD(Motor_PWM);    
    MOTORC_FORWARD(Motor_PWM_2);MOTORD_FORWARD(Motor_PWM);    
  }   
  else
  {
    MOTORA_FORWARD(Motor_PWM);MOTORB_FORWARD(Motor_PWM);    
    MOTORC_FORWARD(Motor_PWM);MOTORD_FORWARD(Motor_PWM);
  }
}

//    ↓A-----B↓   
//     |  |  |
//     |  ↓  |
//    ↓C-----D↓
void BACK()
{
  val_R = digitalRead(sensor_R);
  val_L = digitalRead(sensor_L);
  if(val_R == 0 && val_L == 0)
  {
    MOTORA_BACKOFF(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
    MOTORC_BACKOFF(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
  }
  else if(val_R == 0 && val_L == 1)
  {
    MOTORA_BACKOFF(Motor_PWM);MOTORB_BACKOFF(Motor_PWM_2);
    MOTORC_BACKOFF(Motor_PWM);MOTORD_BACKOFF(Motor_PWM_2);
  }
  else if(val_R == 1 && val_L == 0)
  {
    MOTORA_BACKOFF(Motor_PWM_2);MOTORB_BACKOFF(Motor_PWM);
    MOTORC_BACKOFF(Motor_PWM_2);MOTORD_BACKOFF(Motor_PWM);
  }
  else
  {
    MOTORA_BACKOFF(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
    MOTORC_BACKOFF(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
  }
}

//    ↓A-----B↑   
//     |  ←  |
//     |  ←  |
//    ↑C-----D↓
void LEFT()
{
  val_F = digitalRead(sensor_F);
  val_B = digitalRead(sensor_B);
  if(val_F == 0 && val_B == 0)
  {
    MOTORA_BACKOFF(Motor_PWM);MOTORB_FORWARD(Motor_PWM);
    MOTORC_FORWARD(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
  }
  else if(val_F == 1 && val_B == 0)
  {
    MOTORA_BACKOFF(Motor_PWM);MOTORB_FORWARD(Motor_PWM_2);
    MOTORC_FORWARD(Motor_PWM_2);MOTORD_BACKOFF(Motor_PWM);
  }
  else if(val_F == 0 && val_B == 1)
  {
    MOTORA_BACKOFF(Motor_PWM_2);MOTORB_FORWARD(Motor_PWM);
    MOTORC_FORWARD(Motor_PWM);MOTORD_BACKOFF(Motor_PWM_2);
  }
  else
  {
    MOTORA_BACKOFF(Motor_PWM);MOTORB_FORWARD(Motor_PWM);
    MOTORC_FORWARD(Motor_PWM);MOTORD_BACKOFF(Motor_PWM);
  }
}


//    ↑A-----B↓   
//     |  →  |
//     |  →  |
//    ↓C-----D↑
void RIGHT()
{
  val_F = digitalRead(sensor_F);
  val_B = digitalRead(sensor_B);
  if(val_F == 0 && val_B == 0)
  {
    MOTORA_FORWARD(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
    MOTORC_BACKOFF(Motor_PWM);MOTORD_FORWARD(Motor_PWM);
  }
  else if(val_F == 1 && val_B == 0)
  {
    MOTORA_FORWARD(Motor_PWM_2);MOTORB_BACKOFF(Motor_PWM);
    MOTORC_BACKOFF(Motor_PWM);MOTORD_FORWARD(Motor_PWM_2);
  }
  else if(val_F == 0 && val_B == 1)
  {
    MOTORA_FORWARD(Motor_PWM);MOTORB_BACKOFF(Motor_PWM_2);
    MOTORC_BACKOFF(Motor_PWM_2);MOTORD_FORWARD(Motor_PWM);
  }
  else
  {
    MOTORA_FORWARD(Motor_PWM);MOTORB_BACKOFF(Motor_PWM);
    MOTORC_BACKOFF(Motor_PWM);MOTORD_FORWARD(Motor_PWM);
  }
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

void CW()
{
  myStepper.step(stepsPerRevolution);
  delay(500);
}
void UCW()
{
  myStepper.step(-stepsPerRevolution);
  delay(500);
}

void RFID_X_move()
{
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial() ){ return; }
  if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || 
      rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] ) 
  {
    x++;
    sand = 10*x+y;
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
    x--;
    sand = 10*x+y;
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
    y++;
    sand = 10*x+y;
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
    y--;
    sand = 10*x+y;
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
    MOTORA_STOP(Motor_PWM);MOTORB_STOP(Motor_PWM);
    MOTORC_STOP(Motor_PWM);MOTORD_STOP(Motor_PWM);
    k++;
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
  myStepper.setSpeed(60);
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
    if(i > out_i){i = out_i;}
    if(j > out_j){j = out_j;}
    k = 0;
  }

  switch(mode)
  {
  case 1://입하
  {
  if( k < 1)
  {
  if(x < 2 || x > 2)
  {
    for(y ; y < in_j ; ){ ADVANCE(); RFID_Y_move();}
    for(y ; y > in_j ; ){ BACK(); RFID_Y_move_ne();}
    STOP(); delay(500);
    for(x ; x < in_i ; ){ RIGHT(); RFID_X_move();}
    for(x ; x > in_i ; ){ LEFT(); RFID_X_move_ne();}
    STOP(); delay(500);
  }
  else
  {
    for(x ; x < in_i ; ){ RIGHT(); RFID_X_move();}
    for(x ; x > in_i ; ){ LEFT(); RFID_X_move_ne();}
    STOP(); delay(500);
    for(y ; y < in_j ; ){ ADVANCE(); RFID_Y_move();}
    for(y ; y > in_j ; ){ BACK(); RFID_Y_move_ne();}
    STOP(); delay(500);
  }
  }
  
  for( k ; k < 1; ){ADVANCE(); FORK_RFID();}
  for( k ; k < 2; k++){ myStepper.step(-stepsPerRevolution); delay(1000);}
  for( k ; k < 3; ){BACK(); FORK_RFID();}
  delay(500);
  for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}
  for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
  STOP(); delay(500);
  for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
  for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
  STOP(); delay(500);

  for( k ; k < 4; ){ADVANCE(); FORK_RFID();}
  for( k ; k < 5; k++){ myStepper.step(stepsPerRevolution); delay(1000);}
  for( k ; k < 6; ){BACK(); FORK_RFID();}

  }break;
  
  case 2://출하
  {
  if( x+i < ((out_i-x)+(out_i-i)) ) //in_i로 먼저 이동
    {
      if(y == j)
      {
        for(x ; x < i ; ){ LEFT(); RFID_X_move_ne();}
        STOP(); delay(250);
      }
      else
      {
        for(x ; x < in_i ; ){ LEFT(); RFID_X_move_ne(); }
        STOP(); delay(250);  
      }

      for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
      STOP(); delay(250);
      for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
      STOP(); delay(250);
    }
    else //out_i로 먼저 이동
    {
      if(y == j)
      {
        for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
        STOP(); delay(250);
      }
      else
      {
        for(x ; x < out_i ; ){ RIGHT(); RFID_X_move();}
        STOP(); delay(250);
      }

      for(x ; x < out_i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > out_i ; ){ LEFT(); RFID_X_move_ne();}
      STOP(); delay(250);
      for(y ; y < out_j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > out_j ; ){ BACK(); RFID_Y_move_ne();}
      STOP(); delay(250);
    }
  
  for( k ; k < 1; ){ADVANCE(); FORK_RFID();}
  myStepper.step(stepsPerRevolution);delay(1000);
  for( k ; k < 2; ){BACK(); FORK_RFID();}

  for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
  for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
  STOP(); delay(500);
  for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}
  for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
  STOP(); delay(500);

  for( k ; k < 3; ){ADVANCE(); FORK_RFID();}
  myStepper.step(-stepsPerRevolution); delay(1000);
  for( k ; k < 4; ){BACK(); FORK_RFID();}
  }break;
  
  case 3://이동
  {
    if( x+i < ((out_i-x+1)+(out_i-i+1)) ) //in_i로 먼저 이동
    {
      if(y == j)
      {
        for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
        for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
        STOP(); delay(250);
      }
      else
      {
        for(x ; x > in_i ; ){ LEFT(); RFID_X_move_ne(); }
        STOP(); delay(250);  
      }

      for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
      STOP(); delay(250);
      for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
      STOP(); delay(250);
    }
    else //out_i로 먼저 이동
    {
      if(y == j)
      {
        for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
        for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
        STOP(); delay(250);
      }
      else
      {
        for(x ; x < out_i ; ){ RIGHT(); RFID_X_move();}
        STOP(); delay(250);
      }

      for(y ; y < j ; ){ ADVANCE(); RFID_Y_move();}
      for(y ; y > j ; ){ BACK(); RFID_Y_move_ne();}
      STOP(); delay(250);
      for(x ; x < i ; ){ RIGHT(); RFID_X_move();}
      for(x ; x > i ; ){ LEFT(); RFID_X_move_ne();}
      STOP(); delay(250);
    }
  }break;
  }
}
