#include <IRremote.h>

#define AI1 7
#define AI2 8
#define pwmA 9

#define BI1 11
#define BI2 10
#define pwmB 6

int speed_Correct = 1.15;

#define leftLineSensor 26
#define rightLineSensor 28
int klinija;
int dlinija;

//sensoriai

#define vidurys 21
#define kaireStatu 14 // definink pinus
#define desineStatu 12
int atstumasKaire;
int atstumasVidurys;
int atstumasDesine;
int x, y, z; // pulsam analogu 





#define StartModule 2
constexpr uint8_t dataFromStartModule {3};
int code = 0; // taktikos kodas

// konfiguraciniai stuff
int greitis = 60; // defaultinis greitis
int reverseGreitis = -150; // vaziuot atgal greitis
int turnBackTime = 200; // milisekundemis
int posukioGreitis = 100;
int sukimosiLaikas = 100; // milisekundemis




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
      if(IrReceiver.decode()){
        code = (IrReceiver.decodedIRData.command);
        break;
      }
    }
  code = 0;
  IrReceiver.end();
  IrReceiver.begin(dataFromStartModule);
  while(code==0){
      if(IrReceiver.decode()){
        code = (IrReceiver.decodedIRData.command); // butu faina idet leda, kad zinot, kad tikrai pasirinko kokia taktika
        break;
      }
    }
  while(digitalRead(StartModule)==LOW){
    if(digitalRead(StartModule)==HIGH){
      break;
    }
  }
  if(digitalRead(StartModule)==HIGH){
    if(code == 74){
      //pasisuk i kazkuria puse
    }    
  }
}
int state;
void loop() {
  sensors();
  
  
  Serial.print(atstumasKaire);
  Serial.print(" ");
  Serial.print(atstumasVidurys);
  Serial.print(" ");
  Serial.println(atstumasDesine);
  

  state = 0;
  if(klinija<900){
    state = 1;
  }
  if(dlinija<900){
    state = 2;
  }
  if(klinija<800 && dlinija<800){
    state = 7;
  }
  if(atstumasVidurys<100){
    state = 3;
  }
  if(atstumasKaire<200){
    state = 4;
  }
  if(atstumasDesine<200){
    state = 5;
  }
  if(atstumasVidurys<200 && atstumasVidurys>100){
    state = 6;
  }
  

  switch (state) {
    case 0:
      drive(greitis,greitis);
      break;
    case 1:
      drive(reverseGreitis,reverseGreitis);
      delay(turnBackTime);
      drive(200,-200);
      delay(sukimosiLaikas);
      break;
    case 2:
      drive(reverseGreitis,reverseGreitis);
      delay(turnBackTime);
      drive(-200,200);
      delay(sukimosiLaikas);
      break;
    case 3:
      while(atstumasVidurys<100){
      drive(255,255);
      }
      if(atstumasVidurys>100){
        drive(0,0);
        delay(50);
      }      
      break;
    case 4:
      while(atstumasVidurys>800){
        x = pulseIn(vidurys, HIGH);
        atstumasVidurys = (x-1000)*2;
        drive(-posukioGreitis,posukioGreitis);
      }
      if(atstumasVidurys<400){ // pabandyt reiktu gal veikia
        drive(0,0);
        delay(50);
      }
      break;
    case 5:
      while(atstumasVidurys>800){
        x = pulseIn(vidurys, HIGH);
        atstumasVidurys = (x-1000)*2;
        drive(posukioGreitis,-posukioGreitis);
      }
      if(atstumasVidurys<400){ // pabandyt reiktu gal veikia
        drive(0,0);
        delay(50);
      }
      break;
    case 6:
      drive(180,180);
      break;
    case 7:
      drive(reverseGreitis, reverseGreitis);
      delay(turnBackTime);
      drive(200,-200);
      delay(sukimosiLaikas);
    }

}

void sensors(){
  
  x = pulseIn(kaireStatu, HIGH);
  atstumasKaire = (x-1000)*2;
  if(atstumasKaire<0){ atstumasKaire = 0; }

  y = pulseIn(vidurys, HIGH);
  atstumasVidurys = (y-1000)*2;
  if(atstumasVidurys<0){ atstumasVidurys = 0; }
  
  z = pulseIn(desineStatu, HIGH);
  atstumasDesine = (z-1000)*2;
  if(atstumasDesine<0){ atstumasDesine = 0; }

  klinija = analogRead(leftLineSensor);
  dlinija = analogRead(rightLineSensor);
}


