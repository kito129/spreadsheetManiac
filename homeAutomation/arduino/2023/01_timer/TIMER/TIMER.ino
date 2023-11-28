#define LED_1_PIN 5
#define LED_2_PIN 4
#define LED_3_PIN 3
#define LED_4_PIN 2

#define LED_TIME_PIN 13

#define BUTTON1_PIN 6
#define BUTTON2_PIN 7
#define BUTTON3_PIN 8
#define BUTTON4_PIN 9

#define LIGHT_PIN A5

// DFINE STATE
class State {
  public:
    State(unsigned long start, unsigned long end);
    unsigned long _start;
    unsigned long _end;
};

State::State(unsigned long start, unsigned long end) {

  _start = start;
  _end = end;
};



const unsigned long hour = 60 * 1 * 1000L;

// for minutage
//day
const unsigned long ventola = 7 * 1 * 1000L;
const unsigned long aspiratore = 7 * 1 * 1000L;
const unsigned long interval = 0.5 * 1 * 1000L;

// NIGHT
const unsigned long notte = 20 * 1 * 1000L;
const unsigned long intervalNight = 5 * 1 * 1000L;

//out1 CALDO
unsigned long VAR1 = 0 * 1 * 1000L;
unsigned long VAR2 = 7 * 1 * 1000L;
unsigned long VAR3 = 20 * 1 * 1000L;
unsigned long VAR4 = 27 * 1 * 1000L;
unsigned long VAR5 = 40 * 1 * 1000L;
unsigned long VAR6 = 47 * 1 * 1000L;

State out1 = State (VAR1, VAR2);
State out2 = State (VAR3, VAR4);
State out3 = State (VAR5, VAR6);

State o1[3] = {out1, out2, out3};
int check1[3] = {LOW, LOW, LOW};


//out1 UMIDITà
unsigned long VAR7 = 10 * 1 * 1000L;
unsigned long VAR8 = 25 * 1 * 1000L;
unsigned long VAR9 = 40 * 1 * 1000L;
unsigned long VAR10 = 55 * 1 * 1000L;

State out4 = State (VAR7, VAR8);
State out5 = State (VAR9, VAR10);

State o2[2] = {out4, out5};
int check2[2] = {LOW, LOW};


int state1 = LOW; // hot
int state2 = LOW; // humi
int state3 = LOW; // vetola
int state4 = LOW; // aspiratore

void setup() {

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_4_PIN, OUTPUT);

}

void loop() {

  unsigned long currentMillis = millis() % hour;
    
  //for HOT
  for (int i = 0; i < 3; i++) {
    if ((o1[i]._start <= currentMillis) && (currentMillis <= o1[i]._end)) {

      check1[i] = LOW;
    } else {
      check1[i] = HIGH;
    }
  }  
  if(check1[0] || check1 [1] || check1[2]){
    state1 =LOW;
  } else {
    state1 =HIGH;
  }
  digitalWrite(LED_1_PIN, state1);



  //for HUMIDITY
  for (int j = 0; j < 2; j++) {
    if ((o2[j]._start <= currentMillis) && (currentMillis <= o2[j]._end)) {
      check2[j] = LOW;
    } else {
      check2[j] = HIGH;
    }
  }
  if(check2[0] || check2 [1]){
    state2 =LOW;
  } else {
    state2 =HIGH;
  }
  digitalWrite(LED_2_PIN, state2);
  
  
  
  
  
  

  // VENTOLA
  if (currentMillis % ventola <= interval) {
    if (state3 == HIGH) {
      state3 = LOW;
    } else {
      state3 = HIGH;
    }
  }
  digitalWrite(LED_3_PIN, state3);



  // ASPIRATORE
  if (currentMillis % aspiratore <= interval) {
    if (state4 == HIGH) {
      state4 = LOW;
    } else {
      state4 = HIGH;
    }
  }
  digitalWrite(LED_4_PIN, state4);

  delay(500);
  


}
