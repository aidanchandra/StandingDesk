#define sleep_in 8
#define en_in1 6
#define ph_in2 7
#define isense A0
#define GPIO1 A1
#define GPIO2 A2
#define btn1 4
#define btn2 5
#define fault 12

void setup() {
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(en_in1, OUTPUT);
  pinMode(ph_in2, OUTPUT);
  pinMode(sleep_in, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(fault), reportFault, CHANGE);
  resetMotor();
  Serial.begin(9600);
}

void loop() {
//    Serial.println(analogRead(GPIO2));
  while(buttonOne()) driveMotor(0);
  while(buttonTwo()) driveMotor(1);
}

void enableMotor(){
  digitalWrite(sleep_in, HIGH);
}
void disableMotor(){
  digitalWrite(sleep_in, LOW);
}
bool buttonOne(){
  return (digitalRead(btn1) == LOW);
}
bool buttonTwo(){
  return (digitalRead(btn2) == LOW);
}

void enableBrake(){
  digitalWrite(en_in1, LOW);
}

void driveMotor(int dir){
  if(dir == 0){
    digitalWrite(en_in1, HIGH);
    digitalWrite(ph_in2, HIGH);
  }
  else{
    digitalWrite(en_in1, HIGH);
    digitalWrite(ph_in2, LOW);
  }
}

void resetMotor(){
  disableMotor();
  delay(200);
  enableMotor();
}
void reportFault(){
  Serial.print("Fault status: ");
  Serial.println(digitalRead(fault));
}
