#include <EEPROM.h>

#define DEBUG 1

#define BTN_1 4
#define BTN_2 5
#define m_one_ct 6
#define m_two_ct 7

#define currentsense A2
#define steppin 3

#define top_steps 2250

float zeroReference = 0;
float curCurrent = 0;

int topPreset = -1;
int botPreset = -1;

bool movementDirection = 0;
bool isCalibrated = false;
long steps = 2250;

void setup() {
  if(DEBUG) Serial.begin(9600);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);

  pinMode(m_one_ct, OUTPUT);
  pinMode(m_two_ct, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(steppin), step_counter, RISING);

  zeroReference = analogRead(currentsense);
  curCurrent = analogRead(currentsense);
}

void step_counter(){
  if(movementDirection) steps +=1;
  else steps-=1;
}

#define HOLD_THRESHOLD 100
#define IGNORE_THRESHOLD 400
#define HOLD_TO_DOUBLE_THRESHOLD 150
void loop() {
  
  //Wait for a button press
  while(!btnOnePressed() && !btnTwoPressed());//Serial.println("Waiting for first press");
  unsigned long startTime = millis();
  bool bOneInitial = btnOnePressed();
  bool bTwoInitial = btnTwoPressed();
  
  delay(HOLD_THRESHOLD);

  while((!btnOnePressed() && !btnTwoPressed()) && (millis()-startTime < IGNORE_THRESHOLD + 100));//Serial.println("Waiting for second press");
  unsigned long endTime = millis();
  bool bOneFinal = btnOnePressed();
  bool bTwoFinal = btnTwoPressed();
  
  unsigned long elapsedTime = endTime - startTime;
  if(elapsedTime > IGNORE_THRESHOLD){
    Serial.println("Too long between presses... ignoring");
  }
  else{

    if(bTwoFinal == 1 && bOneFinal == 1){
      int elapsedTime = 0;
      while(btnOnePressed() && btnTwoPressed() && elapsedTime <= 200){
        elapsedTime++;
        delay(1);
      }
      if(elapsedTime > 200){
        Serial.println("Bottom set");
        steps = 0;
        delay(500);
        motorForward();
        delay(200);
        motorStop();
      }
      else{
        if(steps > topPreset){ //If steps > top preset -> this pos is top prest
          botPreset = topPreset;
          topPreset = steps;
        }
        else{
          botPreset = steps;
        }
        if(botPreset < 15){
          botPreset = 15;
        }
        if(topPreset < 15){
          topPreset = 15;
        }
        if(botPreset > 2250){
          botPreset = 2250;
        }
        if(topPreset > 2250){
          topPreset = 2250;
        }
        Serial.println("Custom set");
      }
    }
    else if(elapsedTime > HOLD_TO_DOUBLE_THRESHOLD){
      Serial.print("Double Press: ");
      if(bTwoFinal == 1 && bOneFinal == 0){
        Serial.println("Left Button");
        Serial.print("Going to bottom preset ");
        Serial.println(botPreset);
        goTo(botPreset);
      }
      else if(bTwoFinal == 0 && bOneFinal == 1){
        Serial.println("Right button");
        Serial.print("Going to top preset ");
        Serial.println(topPreset);
        goTo(topPreset);
      }
    }
    else{
      Serial.print("Single/Hold Press ");
      if(bTwoFinal == 1 && bOneFinal == 0){
        Serial.println("Left Button - move downwards");
        while(btnTwoPressed()!=0 && steps >= 15)
          motorBackward();
        motorStop();
        if(!(steps >= 10)) Serial.println("At bottom limit");
      }
      else if(bTwoFinal == 0 && bOneFinal == 1){
        Serial.println("Right button - move forwards");
        while(btnOnePressed()!=0 && steps < top_steps)
          motorForward();
        motorStop();
        if(!(steps < top_steps)) Serial.println("At top limit");
      }
    }
  }
  Serial.println("-------------");
  while(btnOnePressed()!=0 || btnTwoPressed()!=0);
  Serial.println("Done");
  motorStop();
}

int difference(int place){
  //Returns a negative number if current place is less than steps - WE ARE BELOW REQUESTED
  //Returns a positive number if current place is greater than steps - WE ARE ABOVE REQUESTED
  return (place - steps);
}

void goTo(int place){
  if(steps < place){ //current pos is below - need to go up
    Serial.print("Driving up to ");
    Serial.println(place);
    while(steps < place){
      motorForward();
    }
    motorStop();
  }
  else{
    Serial.print("Driving down to ");
    Serial.println(place);
    while(steps > place){
      motorBackward();
    }
    motorStop();
  }
  
}

bool btnOnePressed(){
  return (digitalRead(BTN_1) == LOW);
}
bool btnTwoPressed(){
  return (digitalRead(BTN_2) == LOW);
}

void driveUpward(){
  if(steps <= top_steps){
    motorForward();
  }
  else{
    Serial.println("At limit!");
  }
}
void driveDownward(){
  Serial.println("Running");
  if(steps >= 20){
    motorBackward();
  }
  else{
    Serial.println("At limit!");
  }
}
void motorForward(){
      movementDirection = 1;
      digitalWrite(m_two_ct, HIGH);
      digitalWrite(m_one_ct, LOW);
}
void motorBackward(){
    movementDirection = 0;
    digitalWrite(m_two_ct, LOW);
    digitalWrite(m_one_ct, HIGH);
}
void motorStop(){
  digitalWrite(m_two_ct, LOW);
  digitalWrite(m_one_ct, LOW);
  delay(100);
  zeroReference = analogRead(currentsense);
}
