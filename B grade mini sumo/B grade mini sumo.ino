#include <IRremote.h>

#define AI1 7
#define AI2 8
#define pwmA 9

#define BI1 11
#define BI2 12
#define pwmB 10

#define leftLineSensor 26
#define rightLineSensor 27
#define klinija analogRead(leftLineSensor);
#define dlinija analogRead(rightLineSensor);

//sensoriai
#define kairyskampuX 18
#define vidurys 21
int x; // pulsui apskaiciuot
#define desinyskampuX 16
//enable pinai
#define kairyskampuEn 15
#define desinyskampuEn 13
int kairyskampu; // sensoriu vertes
int desinyskampu;

#define StartModule 2
constexpr uint8_t dataFromStartModule {3};

// konfiguraciniai stuff
int greitis = 50; // defaultinis greitis
int reverseGreitis = -150; // vaziuot atgal greitis
int turnBackTime = 200; // milisekundemis
int posukioGreitis = 90;
int sukimosiLaikas = 300; // milisekundemis




void drive(int speedA, int speedB);

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
        analogWrite(pwmA, speedA);
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
    // sensoriai
  pinMode(kairyskampuX, INPUT);
  pinMode(desinyskampuX, INPUT);
  //sensoriuku digital veikimo shit jei veikia nelieti
  pinMode(kairyskampuEn, OUTPUT);
  pinMode(desinyskampuEn, OUTPUT);
  digitalWrite(kairyskampuEn, LOW);
  digitalWrite(desinyskampuEn, LOW);
  delay(100);
  digitalWrite(kairyskampuEn, HIGH);
  digitalWrite(desinyskampuEn, HIGH);

  while(digitalRead(StartModule)==LOW){
    while(code==0){
      if(IrReceiver.decode()){
        code = (IrReceiver.decodedIRData.command);
        break;
      }
    }
    if(digitalRead(StartModule)==HIGH) {
      break;
    }
  }
}
int state;
void loop() {
  //vidurinis analogas
  x = pulseIn(vidurys, HIGH);
  atstumas = (x-1000)*2;
  if(atstumas<0){ atstumas = 0; }

  kairyskampu = digitalRead(kairyskampuX);
  desinyskampu = digitalRead(desinyskampuX);

  state = 0;
  if(klinija<900){
    state = 1;
  }
  if(dlinija<900){
    state = 2;
  }
  if(atstumas<100){
    state = 3;
  }
  if(kairyskampu==0){
    state = 4;
  }
  if(desinyskampu==0){
    state = 5;
  }

  switch (state) {
    case 0:
      drive(greitis,greitis);
      break;
    case 1:
      drive(reverseGreitis,reverseGreitis);
      delay(turnBackTime);
      drive(posukioGreitis,-posukioGreitis);
      delay(sukimosiLaikas);
      break;
    case 2:
      drive(reverseGreitis,reverseGreitis)
      delay(turnBackTime);
      drive(-posukioGreitis,posukioGreitis);
      delay(sukimosiLaikas);
      break;
    case 3:
      drive(200,200);
      break;
    case 4:
      while(atstumas>800){
        x = pulseIn(vidurys, HIGH);
        atstumas = (x-1000)*2;
        drive(-90,90);
      }
      break;
    case 5:
      while(atstumas>800){
        x = pulseIn(vidurys, HIGH);
        atstumas = (x-1000)*2;
        drive(90,-90);
      }
      break;
  }

}


