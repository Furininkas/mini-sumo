#include <IRremote.h>

#define AI1 23 
#define AI2 22 
#define pwmA 17 

#define BI1 19 
#define BI2 21 
#define pwmB 16 
#define lempute 25

int speed_Correct = 1.15; 

#define leftLineSensor 18
#define rightLineSensor 5
int klinija;
int dlinija;

#define kaireStatu 32
#define kaireKampu 35
#define vidurys 34
#define desineKampu 39 
#define desineStatu 36

int atstumasKaireStatu;
int atstumasKaireKampu;
int atstumasVidurys;
int atstumasDesineKampu;
int atstumasDesineStatu;
int a, b, c, d, e; // pulsam analogu 

#define StartModule 27
constexpr uint8_t dataFromStartModule {26};
int code = 0; // taktikos kodas

// konfiguraciniai stuff
int greitis = 60; // defaultinis greitis
int reverseGreitis = -120; // vaziuot atgal greitis
int turnBackTime = 100; // milisekundemis
int posukioGreitis = 100;
int sukimosiLaikas = 60; // milisekundemis

double konstanta = 0.4;
double speed;
unsigned long milk;

unsigned long prevTime = millis();
unsigned long currentTime;

void drive(int speedA, int speedB);
void sensors();
void vaziuojam(int state);

void drive(int speedA, int speedB)//speed from -255 to 255; if both speeds = to 0 - stall
{
  if(digitalRead(StartModule))
  {
    if(speedA == 0 && speedB == 0)
    {
      analogWrite(pwmA, 255);//motor A speed
      analogWrite(pwmB, 255);//motor B speed
      digitalWrite(AI1, HIGH);//front pin
      digitalWrite(AI2, HIGH);//back pin
      digitalWrite(BI1, HIGH);//front pin
      digitalWrite(BI2, HIGH);//back pin
    }
    else
    {
      if(speedA>0)
      {
        if(speedA>255) speedA=255;
        analogWrite(pwmA, speedA); // KAZKURIAM REIK UZDET SPEED CORRECT
        digitalWrite(AI1, HIGH);
        digitalWrite(AI2, LOW);
      }
      else
      { 
        analogWrite(pwmA, -speedA);
        digitalWrite(AI1, LOW);
        digitalWrite(AI2, HIGH);
      }
      if(speedB>0)
      {
        if(speedB>255) speedB=255;
        analogWrite(pwmB, speedB);
        digitalWrite(BI1, HIGH);
        digitalWrite(BI2, LOW);
      }
      else
      { 
        analogWrite(pwmB, -speedB);
        digitalWrite(BI1, LOW);
        digitalWrite(BI2, HIGH);
      }
    }
  }
  else
  {
    //if star module LOW shoot off motors
    analogWrite(pwmA, 0);
    analogWrite(pwmB, 0);

  }
}
void sensors()
{
  a = pulseIn(kaireStatu, HIGH); // gali reiketi offsettint
  atstumasKaireStatu = (a-1000)/10*20;
  if(atstumasKaireStatu<0){ atstumasKaireStatu = 0; }

  b = pulseIn(kaireKampu, HIGH);
  atstumasKaireKampu = (b-1000)/10*20;
  if(atstumasKaireKampu<0){ atstumasKaireKampu = 0; }

  c = pulseIn(vidurys, HIGH);
  atstumasVidurys = (c-1000)/10*20;
  if(atstumasVidurys<0){ atstumasVidurys = 0; }

  d = pulseIn(desineKampu, HIGH);
  atstumasDesineKampu = (d-1000)/10*20;
  if(atstumasDesineKampu<0){ atstumasDesineKampu = 0; }
  
  e = pulseIn(desineStatu, HIGH);
  atstumasDesineStatu = (e-1000)/10*20;
  if(atstumasDesineStatu<0){ atstumasDesineStatu = 0; }

  klinija = digitalRead(leftLineSensor);
  dlinija = digitalRead(rightLineSensor);
  Serial.print(atstumasKaireStatu);
  Serial.print(" ");
  Serial.print(atstumasKaireKampu);
  Serial.print(" ");
  Serial.print(atstumasVidurys);
  Serial.print(" ");
  Serial.print(atstumasDesineKampu);
  Serial.print(" ");
  Serial.print(atstumasDesineStatu);
  Serial.print(" ");
  Serial.print(klinija);
  Serial.print(" ");
  Serial.println(dlinija);
}

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(dataFromStartModule);
  pinMode(leftLineSensor, INPUT);
  pinMode(rightLineSensor, INPUT);
  pinMode(StartModule, INPUT);

  pinMode(kaireStatu, INPUT);
  pinMode(kaireKampu, INPUT);
  pinMode(vidurys, INPUT);
  pinMode(desineKampu, INPUT);
  pinMode(desineStatu, INPUT);
  
  //motorai
  pinMode(pwmA, OUTPUT);
  pinMode(AI1, OUTPUT);
  pinMode(AI2, OUTPUT);

  pinMode(pwmB, OUTPUT);
  pinMode(BI1, OUTPUT);
  pinMode(BI2, OUTPUT);
  pinMode(lempute, OUTPUT);

  while(code==0){
    IrReceiver.resume();
    if(IrReceiver.decode()){
      if(IrReceiver.decodedIRData.protocol == NEC){
        code = (IrReceiver.decodedIRData.command);
        break;
      }
    }
  }
  digitalWrite(lempute, HIGH);
  Serial.println(code);
  while(digitalRead(StartModule)==LOW){
    if(digitalRead(StartModule)==HIGH){
      break;
    }
  }
 // delay(5000);
  // padaryk taktikos reikala
  if(code == 67){
    sensors();
    drive(60,60);
    delay(100);
    drive(0,0);
    delay(100);
    while(atstumasDesineKampu>340 && atstumasVidurys>340 && atstumasKaireKampu>340){
      sensors();
      currentTime = millis();
      
      drive(0,0);
      if(currentTime - prevTime>1000){
        sensors();
        drive(80,80);
        delay(50);
        prevTime = currentTime;
      }
      if(currentTime - prevTime>5000){
        break;
      }
    }
  }
  if(code == 12){
    drive(60,60);
    delay(400);
  }
  if(code == 24){
    sensors();
    drive(50,50);      
    delay(50);
    drive(-50,50);
    delay(100);
    while(klinija==1){
      sensors();
      drive(70,70);
    }
    drive(0,0);
    delay(50);
    drive(90,-90);
    delay(150);
    drive(70,70);
    delay(200);    
  }
  if(code == 94){
    sensors();
    drive(50,50);      
    delay(50);
    drive(50,-50);
    delay(100);
    while(dlinija==1){
      sensors();
      drive(70,70);
    }
    drive(0,0);
    delay(50);
    drive(-90,90);
    delay(150);
    drive(70,70);
    delay(200);    
  }
}
int state;
int timeA;
int currentTime1;
void vaziuojam(int state){
  timeA = millis();
  switch(state){
      case 0:
        if(code==8){
          drive(50,50);
        } else {
          drive(0,0);
        }
        break;
      case 1:
        drive(reverseGreitis, reverseGreitis);
        delay(turnBackTime);
        drive(150,-150);
        delay(sukimosiLaikas);
        break;
      case 2:
        drive(reverseGreitis, reverseGreitis);
        delay(turnBackTime);
        drive(-150,150);
        delay(sukimosiLaikas);
        break;
      case 3:
        drive(reverseGreitis,reverseGreitis);
        delay(turnBackTime);
        drive(150,-150);
        delay(sukimosiLaikas);
        break;
      case 4:
        milk = millis();
        while(atstumasVidurys<50){
          speed = (millis()-milk)*konstanta;
          sensors();
          drive(speed,speed);
        } 
        milk=0;
        break; 
      case 5:
        while(atstumasVidurys>300){
          currentTime1 = millis();
          if(currentTime1 - timeA>400) break;
          sensors();
          drive(50,-50);
        }
        drive(0,0);
        delay(50);
        currentTime1 = 0;
        break;
      case 6:
        while(atstumasVidurys>300){
          currentTime1 = millis();
          if(currentTime1 - timeA>400) break;
          sensors();
          drive(-50,50);
        }
        drive(0,0);
        delay(50);
        currentTime1 = 0;
        break;
      case 7:
        while(atstumasVidurys>400){
          currentTime1 = millis();
          if(currentTime1 - timeA>600) break;
          sensors();
          drive(-50,50);
        }
        drive(0,0);
        delay(50);
        currentTime1 = 0;
        break;
      case 8:
        while(atstumasVidurys>400){
          currentTime1 = millis();
          if(currentTime1 - timeA>600) break;
          sensors();
          drive(50,-50);
        }
        drive(0,0);
        delay(50);
        currentTime1 = 0;
        break;
      case 9:
        drive(0,0);
        break;
      case 10:
        drive(80,80);
        delay(50);
        drive(0,0);
        delay(50);
        break;
    }
}

int timePriekiui;
int timePaskutinis=0;
void loop() {
  sensors();
  state=0;
  timePriekiui = millis();
  if(timePriekiui-timePaskutinis>1000){
    if(atstumasVidurys>100){
      state=10;
    }
    timePaskutinis=timePriekiui;
  } 
    if(atstumasKaireStatu<250){
      state=7;
    }
    if(atstumasDesineStatu<250){
      state=8;
    }
    if(atstumasKaireKampu<250){
      state=5;
    }
    if(atstumasDesineKampu<250){
      state=6;
    }
   /* if(atstumasKaireKampu<250 && atstumasDesineKampu<250){
      state=9;
    } */
    if(atstumasVidurys<80){
      state=4;
    } 
    if(klinija==0){
      state=1;
    }
    if(dlinija==0){
      state=2;
    } 
    if(klinija==0 && dlinija==0){
      state=3;
    } 
   //Serial.println(state);
  
  if(code==67 || code==12 || code==24 || code==94){  
    vaziuojam(state);
  }
  if(code==8){
    vaziuojam(state);
  }
}
