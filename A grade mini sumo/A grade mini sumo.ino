#include <IRremote.h>

#define AI1 
#define AI2 
#define pwmA 

#define BI1 
#define BI2 
#define pwmB

int speed_Correct = 1.15; // jei prireiks

#define leftLineSensor 
#define rightLineSensor 
int klinija;
int dlinija;

#define kaireStatu
#define kaireKampu
#define vidurys
#define desineKampu
#define desineStatu 

int atstumasKaireStatu;
int atstumasKaireKampu;
int atstumasVidurys;
int atstumasDesineKampu;
int atstumasDesineStatu;
int a, b, c, d, e; // pulsam analogu 

#define StartModule 
constexpr uint8_t dataFromStartModule {};
int code = 0; // taktikos kodas

// konfiguraciniai stuff
int greitis = 60; // defaultinis greitis
int reverseGreitis = -150; // vaziuot atgal greitis
int turnBackTime = 200; // milisekundemis
int posukioGreitis = 100;
int sukimosiLaikas = 100; // milisekundemis

unsigned long prevTime = milis();

void drive(int speedA, int speedB);
void sensors();

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
        analogWrite(pwmA, speedA*speed_Correct); // KAZKURIAM REIK UZDET SPEED CORRECT
        digitalWrite(AI1, HIGH);
        digitalWrite(AI2, LOW);
      }
      else
      { 
        analogWrite(pwmA, -speedA*speed_Correct);
        digitalWrite(AI1, LOW);
        digitalWrite(AI2, HIGH);
      }
      if(speedB>0)
      {
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
  a = pulseIn(kaireStatu, HIGH);
  atstumasKaireStatu = (x-1000)*2;
  if(atstumasKaireStatu<0){ atstumasKaireStatu = 0; }

  b = pulseIn(kaireKampu, HIGH);
  atstumasKaireKampu = (y-1000)*2;
  if(atstumasKaireKampu<0){ atstumasKaireKampu = 0; }

  c = pulseIn(vidurys, HIGH);
  atstumasVidurys = (y-1000)*2;
  if(atstumasVidurys<0){ atstumasVidurys = 0; }

  d = pulseIn(desineKampu, HIGH);
  atstumasDesineKampu = (y-1000)*2;
  if(atstumasDesineKampu<0){ atstumasDesineKampu = 0; }
  
  e = pulseIn(desineStatu, HIGH);
  atstumasDesineStatu = (z-1000)*2;
  if(atstumasDesineStatu<0){ atstumasDesineStatu = 0; }

  klinija = digitalRead(leftLineSensor);
  dlinija = digitalRead(rightLineSensor);
}

void setup() {
  IrReceiver.begin(dataFromStartModule);
  pinMode(leftLineSensor, INPUT);
  pinMode(rightLineSensor, INPUT);
  pinMode(StartModule, INPUT);
  //motorai
  pinMode(pwmA, OUTPUT);
  pinMode(AI1, OUTPUT);
  pinMode(AI2, OUTPUT);

  pinMode(pwmB, OUTPUT);
  pinMode(BI1, OUTPUT);
  pinMode(BI2, OUTPUT);

  while(code==0){
    IrReceiver.resume();
    if(IrReceiver.decode()){
      if(IrReceiver.decodedIRData.protocol == NEC){
        code = (IrReceiver.decodedIRData.command);
        break;
      }
    }
  }
  while(digitalRead(StartModule)==LOW){
    if(digitalRead(StartModule)==HIGH){
      break;
    }
  }
  // padaryk taktikos reikala
  if(code == 67){
    while(atstumasDesineKampu>400 && atstumasVidurys>300 && atstumasKaireKampu>400){
      sensors();
      unsigned long currentTime = milis();
      if(currentTime - prevTime>1000){
        drive(80,80);
        delay(50);
        prevTime = currentTime;
      }
    }
  }
}
int state;
void loop() {
  sensors();
  state=0;
  if(code==67){
    if(klinija==0){
      state=1;
    }
    if(dlinija==0){
      state=2;
    }
    if(klinija==0 && dlinija==0){
      state=3;
    }
    if(atstumasVidurys<150){
      state=4;
    }
    if(atstumasKaireKampu<400){
      state=5;
    }
    if(atstumasDesineKampu<400){
      state=6;
    }
    if(atstumasKaireStatu<400){
      state=7;
    }
    if(atstumasDesineStatu<400){
      state=8;
    }

    switch(state){
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
        while(atstumasKaireStatu>400){
          drive(90,-90);
        }
        drive(90,90);
        delay(200);
        while(atstumasDesineKampu>400){
          drive(-90,90);
        }
        prevTime = milis();
        while(atstumasKaireKampu>400){
          unsigned long currentTime = milis();
          drive(90,110);
          if(currentTime-prevTime>1000){
            break;
          }
        }
        while(atstumasVidurys<100){
          drive(255,255);
        }
        break;
      case 5:
        while(atstumasVidurys>400){
          drive(90,-90);
        }
        break;
      case 6:
        while(atstumasVidurys>400){
          drive(-90,90);
        }
        break;
      case 7:
        while(atstumasDesineKampu>400){
          drive(-90,90);
        }
        break;
      case 8:
        while(atstumasKaireKampu>400){
          drive(90,-90);
        }
        break;
    }
  }
  if(code == 12){
    if(klinija==0){
      state=1;
    }
    if(dlinija==0){
      state=2;
    }
    if(klinija==0 && dlinija==0){
      state=3;
    }
    if(atstumasVidurys<150){
      state=4;
    }
    if(atstumasVidurys<200 && atstumasVidurys>150){
      state=5;
    }
    if(atstumasKaireKampu<400){
      state=6;
    }
    if(atstumasDesineKampu<400){
      state=7;
    }
    if(atstumasKaireStatu<400){
      state=8;
    }
    if(atstumasDesineStatu<400){
      state=9;
    }
    switch(state){
      case 0:
        drive(greitis,greitis);
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
        drive(255,255);
        break;
      case 5:
        drive(150,150);
        break;
      case 6:
        while(atstumasVidurys>400){
          drive(90,-90);
        }
        break;
      case 7:
        while(atstumasVidurys>400){
          drive(-90,90);
        }
        break;
      case 8:
        while(atstumasDesineKampu>400){
          drive(-90,90);
        }
        break;
      case 9:
        while(atstumasKaireKampu>400){
          drive(90,-90);
        }
        break;

    }
  }

}
