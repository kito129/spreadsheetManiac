

#define LED_1_PIN 2
#define LED_2_PIN 3
#define LED_3_PIN 4
#define LED_4_PIN 5

#define LIGHT_A7_PIN A7


#define BUTTON_PIN 10
#define LED_NIGHT_PIN 13

#define SIZE 60

int nightStatus = 0;


// my state

int hot[SIZE];
int humidity[SIZE];
int vent[SIZE];
int air[SIZE];

int night[SIZE];

int all[SIZE];


void fullWithEmpty(int list[], int size){
  for(int i=0;i<size;i++){
      list[i] = HIGH;
  }
}

void fullWithHigh(int list[], int size,int start, int end){
  for(int i=0;i<size;i++){
    if(start<=i && i<=end){
      list[i] = LOW;
    }
  }
}

void printList(int list[], int size){
  Serial.println("\nList:");
  for(int i=0;i<size;i++){
    Serial.println(list[i]);
  }
}


int getCurrent(int list[], int size,int current){
  return list[current];
}


void printStatus(int hot[], int humidity[],int vent[], int air[],int minute){
  Serial.println("Status:");
  Serial.println(getCurrent(hot,SIZE,minute));
  Serial.println(getCurrent(humidity,SIZE,minute));
  Serial.println(getCurrent(vent,SIZE,minute));
  Serial.println(getCurrent(air,SIZE,minute));
  
}

void setup() {
  Serial.begin(9600);


  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_4_PIN, OUTPUT);

  pinMode(LIGHT_A7_PIN, INPUT);

  pinMode(LED_NIGHT_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);


  fullWithEmpty(all,SIZE);
  fullWithHigh(all,SIZE,0,59);

  fullWithEmpty(hot,SIZE);

  printList(hot,SIZE);

  fullWithEmpty(air,SIZE);
  fullWithHigh(air,SIZE,1,4);
  fullWithHigh(air,SIZE,7,10);
  fullWithHigh(air,SIZE,11,13);
  fullWithHigh(air,SIZE,17,20);
  fullWithHigh(air,SIZE,20,23);
  fullWithHigh(air,SIZE,27,30);
  fullWithHigh(air,SIZE,31,33);
  fullWithHigh(air,SIZE,37,40);
  fullWithHigh(air,SIZE,41,43);
  fullWithHigh(air,SIZE,47,50);
  fullWithHigh(air,SIZE,51,53);
  fullWithHigh(air,SIZE,57,59);

  printList(air,SIZE);

  fullWithEmpty(vent,SIZE);
  fullWithHigh(vent,SIZE,0,5);
  fullWithHigh(vent,SIZE,6,9);
  fullWithHigh(vent,SIZE,10,15);
  fullWithHigh(vent,SIZE,16,19);
  fullWithHigh(vent,SIZE,20,25);
  fullWithHigh(vent,SIZE,26,29);
  fullWithHigh(vent,SIZE,30,35);
  fullWithHigh(vent,SIZE,36,39);
  fullWithHigh(vent,SIZE,40,45);
  fullWithHigh(vent,SIZE,46,49);
  fullWithHigh(vent,SIZE,50,55);
  fullWithHigh(vent,SIZE,56,59);



  printList(vent,SIZE);
      
  fullWithEmpty(humidity,SIZE);

  printList(humidity,SIZE);

  fullWithEmpty(night,SIZE);
  fullWithHigh(night,SIZE,1,3);
  fullWithHigh(night,SIZE,7,9);
  fullWithHigh(night,SIZE,11,13);
  fullWithHigh(night,SIZE,17,19);
  fullWithHigh(night,SIZE,21,23);
  fullWithHigh(night,SIZE,27,29);
  fullWithHigh(night,SIZE,31,33);
  fullWithHigh(night,SIZE,37,39);
  fullWithHigh(night,SIZE,41,43);
  fullWithHigh(night,SIZE,47,49);
  fullWithHigh(night,SIZE,51,53);
  fullWithHigh(night,SIZE,57,59);

}

void loop() {


  int value = analogRead(LIGHT_A7_PIN);
  Serial.println("Analog value : ");
  Serial.println(value);

  // CHECK FOR NIGHT
  //change state
  if(value>200){
    nightStatus = 0;
  } else {
    nightStatus = 1;
    }
  digitalWrite(LED_NIGHT_PIN,nightStatus);

  
  unsigned long runMillis= millis();
  unsigned long allSeconds=millis()/1000;
  
  int runHours= allSeconds/3600;
  int secsRemaining=allSeconds%3600;
  int runMinutes=secsRemaining/60;
  int runSeconds=secsRemaining%60;
  
  char buf[21];
  sprintf(buf,"Runtime%02d:%02d:%02d",runHours,runMinutes,runSeconds);
  Serial.println(buf);

  

  if(nightStatus==0){

     Serial.println("----IS DAYY------");

    // IS DAY
     //check
    digitalWrite(LED_1_PIN,getCurrent(hot,SIZE,runMinutes));
    digitalWrite(LED_2_PIN,HIGH);
    digitalWrite(LED_3_PIN,getCurrent(vent,SIZE,runMinutes));
    digitalWrite(LED_4_PIN,getCurrent(air,SIZE,runMinutes));
  
    printStatus(hot,humidity,vent,vent,runMinutes);
  
  } else if(nightStatus==1){

    // IS NIGHT

    Serial.println("----IS NIGHTTT------");


     //check
    digitalWrite(LED_1_PIN,getCurrent(night,SIZE,runMinutes));
    digitalWrite(LED_2_PIN,HIGH);
    digitalWrite(LED_3_PIN,getCurrent(night,SIZE,runMinutes));
    digitalWrite(LED_4_PIN,getCurrent(night,SIZE,runMinutes));

    Serial.println("Night Status");
    Serial.println(getCurrent(night,SIZE,runMinutes));
    
  }else if(nightStatus==2){

    // ALL ON

    Serial.println("----ALL ON------");


     //check
    digitalWrite(LED_1_PIN,getCurrent(all,SIZE,runMinutes));
    digitalWrite(LED_2_PIN,HIGH);
    digitalWrite(LED_3_PIN,getCurrent(all,SIZE,runMinutes));
    digitalWrite(LED_4_PIN,getCurrent(all,SIZE,runMinutes));

    Serial.println("All On");
    Serial.println(getCurrent(all,SIZE,runMinutes));
    digitalWrite(LED_NIGHT_PIN,LOW);
    
    
  }

  
  delay(1000);
}
