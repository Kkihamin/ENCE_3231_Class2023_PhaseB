# define LED_Pin 13
String gString_Read;
volatile unsigned char gIsrflag = 0;
unsigned int Timer1 = (62500 * 4) / 1000;

// int let1 = 0;
// int let2 = 0;
// int let3 = 0;
// char gIncomingCharacter;
// int let[4];
// int nextInt;

void setup() {
  pinMode(LED_Pin, OUTPUT);
  digitalWrite(LED_Pin, LOW); 
  Serial.begin(9600);
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = Timer1;            // compare match register 16MHz/256
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts(); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){
    gString_Read = Serial.readStringUntil('\n');
    Serial.println(gString_Read);
    if(gString_Read == "$STR"){
        digitalWrite(LED_Pin, HIGH);
    }else if(gString_Read == "$STP"){
        digitalWrite(LED_Pin, LOW);
    }
  }



}
// void parseString(char myString[4] ){
//   if(myString[1] == 'S' && myString[1] == 'T' && myString[1] == 'R')  {
//       digitalWrite(LED_Pin, HIGH);

//   }else if(myString[1] == 'S' && myString[1] == 'T' && myString[1] == 'R'){
//       digitalWrite(LED_Pin, LOW);
//   }

ISR(Timer1_COMPA_VECT){
  if(Serial.available() > 0) {
    gIsrflag = 1;
  }
}

// }
