#define BUTTON_1  2
#define BUTTON_2  3
#define GREEN_LED 4
#define RED_LED   5
#define LED_Pin   13
#define BUZZER    6

#define DATA      9
#define LATCH     8
#define CLOCK     7

#define DIGIT_4   10
#define DIGIT_3   11
#define DIGIT_2   12
#define DIGIT_1   13


bool gbegin = false;
bool gbuzzer = false;
 
unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};

volatile unsigned char gcount1 = 0;
volatile unsigned char gcount2 = 3;
volatile unsigned char gcount3 = 0;
volatile unsigned char gcount4 = 0;

String gString_Read;
volatile unsigned char gIsrflag = 0;
unsigned int gTimer1 = (62500 * 4) / 1000;

// int let1 = 0;
// int let2 = 0;
// int let3 = 0;
// char gIncomingCharacter;
// int let[4];
// int nextInt;

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Button Pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), Button_1_ISR, RISING);
  pinMode(BUTTON_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), Button_2_ISR, RISING);

  // Buzer Pins
  pinMode(BUZZER, OUTPUT);

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // Shift Register Pins
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
  //pinMode(LED_Pin, OUTPUT);
  //digitalWrite(LED_Pin, LOW); 
  Serial.begin(9600);
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = gTimer1;            // compare match register 16MHz/256
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts(); 
  display_on(10);
  display_on(11);
  display_on(12);
  display_on(13);
}

void Activate_Buzzer(){
  gbuzzer = true;
  unsigned char i;
  unsigned char sleep_time = 1;
  for( i = 0; i < 100; i++){
    digitalWrite(BUZZER,HIGH);
    delay(sleep_time);
    digitalWrite(BUZZER, LOW);
    delay(sleep_time);
  }
}

void Display(unsigned char num, int digit){
  display_on(digit);
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, table[num]);
  digitalWrite(LATCH, HIGH);

}

void Display_Numbers()
{
  Display(gcount1, 10);
  delay(10);
  Display(gcount2, 11);
  delay(10);
  Display(gcount3, 12);
  delay(10);
  Display(gcount4, 13);
  delay(10); 
}

void display_on(int digit)
{
   display_off();
   digitalWrite(digit, LOW);
}

void display_off(){
  digitalWrite(DIGIT_1, HIGH);
  digitalWrite(DIGIT_2, HIGH);
  digitalWrite(DIGIT_3, HIGH);
  digitalWrite(DIGIT_4, HIGH);
}

void Reset(){
  gcount1 = 0;
  gcount2 = 0;
  gcount3 = 0;
  gcount4 = 0;
}

void Button_1_ISR()
{
  unsigned int buttonState = 0;  // variable for reading the pushbutton status
    
  buttonState = digitalRead(BUTTON_1);
  digitalWrite(RED_LED, buttonState);
   if(gcount1 > 8){
    gcount1 = 0;
    if(gcount2 > 8){
      gcount2 = 0;
      if(gcount3 > 8){
        gcount3 = 0;
        if(gcount4 > 8){
          gcount4 = 0;
        } else{
          gcount4++;
        }
      } else{
        gcount3++;
      }
    }else{
      gcount2++; 
    }
    
  }else{
    gcount1++;
  }
}

void Button_2_ISR()
{ 
  unsigned int buttonState = 0;  // variable for reading the pushbutton status
   
  buttonState = digitalRead(BUTTON_2);
  digitalWrite(GREEN_LED, buttonState);
  if(gbuzzer){
    gbuzzer = false;
    Reset();
  }
  gbegin = !gbegin;
}
ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
  if(gbegin){
    if(gcount2 == 0 && gcount1 > 0){
      gcount1--;    
    }else if(gcount2 > 0 && gcount1 > 0){
      gcount1--;
    }
    if(gcount1 == 0 && gcount2 > 0){
      gcount2--;
      gcount1 = 9;
    }
    if(gcount3 > 0 && gcount2 == 0 && gcount1 == 0) {
      gcount3--; 
      gcount2 = 9;
    }
    if(gcount4 > 0 && gcount3 == 0 && gcount2 == 0 && gcount1 == 0){
      gcount4--; 
      gcount3 = 9;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){
    gString_Read = Serial.readStringUntil('\n');
    Serial.println(gString_Read);
    if(gString_Read == "$STR"){
        digitalWrite(LED_Pin, HIGH);
        //digitalWrite(GREEN_LED, HIGH);
    }else if(gString_Read == "$STP"){
        digitalWrite(LED_Pin, LOW);
        //digitalWrite(RED_LED, HIGH);
    }
  }
  if(gcount1 == 0 && gcount2 == 0 && gcount3 == 0 && gcount4 == 0){
    Activate_Buzzer();
  }
  Display_Numbers();



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
