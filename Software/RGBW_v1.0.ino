/*
RGBW LED Controller v 1.9
Jeremy Saglimbeni - 2011 - 
For more information go to 
*/
#include <EEPROM.h>
//--
#include <IRremote.h>     //#include "Adafruit_NECremote.h"
int RECV_PIN = 2;         //#define IRpin         2
int remval = 0;             //Adafruit_NECremote remote(IRpin);
int oldremval;

//--
char datain[9];
int i;
int stay; // delay in seconds to stay on each color in cycle mode
int rate; // rate of color cycle
int cyclego = 0;
int ramp;
int btn = 8; // button 1 input
int btn2 = 4; // button 2 input
int btnv;
int btnv2;
int mode = 0; // manual mode
int w = 3; // pin 3 PWM output
int r = 9; // pin 9 PWM output
int g = 10; // pin 10 PWM output
int b = 11; // pin 11 PWM output
int whtlev; // current white level
int redlev; // current red level
int grnlev; // current green level
int blulev; // current blue level
int index;
int targetw; // desired white level
int targetr; // desired red level
int targetg; // desired green level
int targetb; // desired blue level
char digit1[1];
char digit2[1];
char digit3[1];
String mystring;
IRrecv irrecv(RECV_PIN);  //
decode_results results;   //

void setup() {
  ramp = EEPROM.read(1);
  rate = EEPROM.read(2);
  pinMode(w, OUTPUT); // white output
  pinMode(r, OUTPUT); // red output
  pinMode(g, OUTPUT); // green output
  pinMode(b, OUTPUT); // blue output
  pinMode(btn, INPUT); // button 1
  pinMode(btn2, INPUT); // button 2
  digitalWrite(btn, HIGH); // turn on internal pull up resistor
  digitalWrite(btn2, HIGH); // turn on internal pull up resistor
  Serial.begin(9600); // enable serial communication at 9600 baud
  Serial.println(F("RGBW v3.1"));
  
  for(i = 0 ; i <= 255; i+=1) { 
    analogWrite(b, i);
    delay(3);
  }
  for(i = 0 ; i <= 255; i+=1) { 
    analogWrite(r, i);
    delay(2);
  }
  for(i = 0 ; i <= 255; i+=1) { 
    analogWrite(g, i);
    delay(2);
  }
  for(i = 0 ; i <= 255; i+=1) { 
    analogWrite(w, i);
    delay(2);
  }
  for(i = 255 ; i >= 0; i-=1) { 
    analogWrite(r, i);
    delay(2);
  }
  for(i = 255 ; i >= 0; i-=1) { 
    analogWrite(g, i);
    delay(2);
  }
  for(i = 255 ; i >= 0; i-=1) { 
    analogWrite(b, i);
    delay(5);
  }
  for(i = 255 ; i >= 0; i-=1) { 
    analogWrite(w, i);
    delay(5);
  }
  irrecv.enableIRIn(); // Start the receiver
}
void loop() {

   if (irrecv.decode(&results)) {             //    if (irrecv.decode(&results)) {    
      Serial.println(results.value, HEX);   //    irrecv.resume(); // Receive the next value
      remval = (results.value);               //  }
      Serial.println(remval);
      irrecv.resume(); // Receive the next value
    }   

hearir();
  
index = 0;
while(Serial.available() > 0 && index < 9)
{
   char aChar = Serial.read();
   datain[index] = aChar;
   index++;
   datain[index] = '\0'; // Keep the string NULL terminated   
}
String mystring = datain;
if (mystring.startsWith("white")) {
digit1[1] = datain[6];
digit2[1] = datain[7];
digit3[1] = datain[8];
targetw = atoi(digit2);
targetw = targetw * 2.5;
white(targetw);
}
if (mystring.startsWith("red")) {
digit1[1] = datain[4];
digit2[1] = datain[5];
digit3[1] = datain[6];
targetr = atoi(digit2) * 2.5;
red(targetr);
}
if (mystring.startsWith("green")) {
digit1[1] = datain[6];
digit2[1] = datain[7];
digit3[1] = datain[8];
targetg = atoi(digit2) * 2.5;
green(targetg);
}
if (mystring.startsWith("blue")) {
digit1[1] = datain[5];
digit2[1] = datain[6];
digit3[1] = datain[7];
targetb = atoi(digit2) * 2.5;
blue(targetb);
}
if (mystring.startsWith("cycle")) {
green(0);
blue(0);
red(255);
cyclego = 1;
cycle(rate);
}
if (mystring.startsWith("rate")) {
digit1[1] = datain[5];
digit2[1] = datain[6];
digit3[1] = datain[7];
rate = atoi(digit2);
EEPROM.write(2, rate);
}
if (mystring.startsWith("stop")) {
cyclego = 0;
red(0);
}
if (mystring.startsWith("stay")) {
digit1[1] = datain[5];
digit2[1] = datain[6];
digit3[1] = datain[7];
stay = atoi(digit2) * 1000;
}
if (mystring.startsWith("ramp")) {
digit1[1] = datain[5];
digit2[1] = datain[6];
digit3[1] = datain[7];
ramp = atoi(digit2);
EEPROM.write(1, ramp);
}
if (mystring.startsWith("alloff")) {
digit1[1] = datain[5];
digit2[1] = datain[6];
digit3[1] = datain[7];
red(0);
green(0);
blue(0);
white(0);
}
buttons();
if (cyclego == 1) {
  cycle(rate);
}
delay(75);
datain[1] = 0;

// enable/disable this section to turn on/off serial feedback/debugging
/*
Serial.print("mystring:");
Serial.print(mystring);
Serial.print("   Value:");
Serial.print(datain[6]);
Serial.print(datain[7]);
Serial.print(datain[8]);
Serial.print("   ramp:");
Serial.print(ramp);
Serial.print("   rate:");
Serial.print(rate);
Serial.print("   stay:");
Serial.print(stay);
Serial.print("   cyclego:");
Serial.print(cyclego);
Serial.print("   mode:");
Serial.print(mode);
Serial.print("   targetw:");
Serial.print(targetw);
Serial.print("   targetr:");
Serial.print(targetr);
Serial.print("   targetg:");
Serial.print(targetg);
Serial.print("   targetb:");
Serial.print(targetb);
Serial.print("   whtlev:");
Serial.print(whtlev);
Serial.print("   redlev:");
Serial.print(redlev);
Serial.print("   grnlev:");
Serial.print(grnlev);
Serial.print("   blulev:");
Serial.println(blulev);
*/
}
void buttons() { // check for button presses
  btnv = digitalRead(btn);
btnv2 = digitalRead(btn2);
if (btnv == LOW) { // if button 1 is pressed
if (cyclego == 1) {
    mode = 11;
    manual();
    return;
}
  manual();
  delay(50);
  mode++;
  if (mode == 9) {
    mode = 0;
  }
}
if (btnv2 == LOW) { // if button 2 is pressed
  if (whtlev == 0) {
    white(255);
    return;
  }
  if (whtlev > 0) {
    white(0);
  }
}
}
void white(int targetw)  { // white control
  if (whtlev < targetw) { // if the desired level is higher than the current level
 for (int x = whtlev; x <= targetw; x++) { // start ramping at the current level and brighten to the desired level
        analogWrite(w, x); // write the value to the pin
        delay(ramp); // delay for 8ms
 }
  }
  if (whtlev > targetw) { // if the desired level is lower than the current level
 for (int x = whtlev; x >= targetw; x--) { // start ramping at the current level and darken or dim to the desired level
        analogWrite(w, x); // write the value to the pin
        delay(ramp); // delay 8ms
 }
  }
 whtlev = targetw; // the desired level is now the current level
}

void red(int targetr)  { // red control
  if (redlev < targetr) {
 for (int x = redlev; x <= targetr; x++) {
        analogWrite(r, x);
        delay(ramp);
 }
  }
  if (redlev > targetr) {
 for (int x = redlev; x >= targetr; x--) {
        analogWrite(r, x);
        delay(ramp);
 }
  }
 redlev = targetr;
}

void green(int targetg)  { // green control
  if (grnlev < targetg) {
 for (int x = grnlev; x <= targetg; x++) {
        analogWrite(g, x);
        delay(ramp);
 }
  }
  if (grnlev > targetg) {
 for (int x = grnlev; x >= targetg; x--) {
        analogWrite(g, x);
        delay(ramp);
 }
  }
 grnlev = targetg;
}

void blue(int targetb)  { // blue control
  if (blulev < targetb) {
 for (int x = blulev; x <= targetb; x++) {
        analogWrite(b, x);
        delay(ramp);
 }
  }
  if (blulev > targetb) {
 for (int x = blulev; x >= targetb; x--) {
        analogWrite(b, x);
        delay(ramp);
 }
  }
 blulev = targetb;
}

void cycle(int rate) { // cycle routine
  for(i = 0 ; i <= 255; i+=1) {  // blue on
  if (cyclego == 1) {
    analogWrite(b, i);
    buttons();
    delay(rate);
    blulev = i;
  }
  }
  delay(stay);
  for(i = 255 ; i >= 0; i-=1) { // red off
  if (cyclego == 1) {
    analogWrite(r, i);
    buttons();
    delay(rate);
    redlev = i;
  }
  }
  delay(stay);
  for(i = 0 ; i <= 255; i+=1) { // green off
  if (cyclego == 1) {
    analogWrite(g, i);
    buttons();
    delay(rate);
    grnlev = i;
  }
  }
  delay(stay);
  for(i = 255 ; i >= 0; i-=1) { // blue off
  if (cyclego == 1) {
    analogWrite(b, i);
    buttons();
    delay(rate);
    blulev = i;
  }
  }
  delay(stay);
  for(i = 0 ; i <= 255; i+=1) { // red on
  if (cyclego == 1) {
    analogWrite(r, i);
    buttons();
    delay(rate);
    redlev = i;
  }
  }
  delay(stay);
  for(i = 255 ; i >= 0; i-=1) { // green off
  if (cyclego == 1) {
    analogWrite(g, i);
    buttons();
    delay(rate);
    grnlev = i;
  }
  }
  delay(stay);
}

void manual() { // manual modes
  if (mode == 0) { // red
    red(255);
    green(0);
    blue(0);
  }
  if (mode == 1) { // green
    green(255);
    red(0);
    blue(0);
  }
  if (mode == 2) { // blue
    blue(255);
    red(0);
    green(0);
  }
  if (mode == 3) { // magenta
    red(255);
    blue(255);
    green(0);
  }
  if (mode == 4) { // teal
    green(255);
    blue(255);
    red(0);
  }
  if (mode == 5) { // yellow
    red(255);
    green(255);
    blue(0);
  }
  if (mode == 6) { // RGB white
    red(255);
    green(255);
    blue(255);
  }
  if (mode == 7) { // color cycle
    green(0);
    blue(0);
    cyclego = 1;
    red(255);
  }
  if (mode == 8) { // off
    green(0);
    blue(0);
    red(0);
    cyclego = 0;
  }
  if (mode == 11) {
    targetr = 0;
    targetg = 0;
    targetb = 0;
    if (redlev != 0) {
    red(0);
    }
    if (grnlev != 0) {
    blue(0);
    }
    if (grnlev != 0) {
    green(0);
    }
    cyclego = 0;
    mode = 0;
    }
}

void hearir() { // listen to IR sensor
  //remval=(test);
  
  if (oldremval != remval) {
    if (remval == 6375) { // key 4 - red toggle
    if (redlev == 0) {
      red(255);
      Serial.print("yhea");
    }
    else {
      red(0);
    }
  }
  if (remval == 14535) { // key 5 - green toggle
    if (grnlev == 0) {
      green(255);
    }
    else {
      green(0);
    }
  }
  if (remval == 19125) { // key 6 - blue toggle
    if (blulev == 0) {
      blue(255);
    }
    else {
      blue(0);
    }
  }
  if (remval == -26521) { // key setup - white toggle
    if (whtlev == 0) {
      white(255);
    }
    else {
      white(0);
    }
  }
  if (remval == -23971) { // stop - all off
    green(0);
    blue(0);
    red(0);
    cyclego = 0;
  }
  if (remval == 31365) { // key 1 - red up
    if (redlev <= 245) {
      red(redlev + 10);
    }
  }
  if (remval == 12495) { // key 7 - red down
    if (redlev >= 10) {
      red(redlev - 10);
    }
  }
  if (remval == 23205) { // key 2 - green up
    if (grnlev <= 245) {
      green(grnlev + 10);
    }
  }
  if (remval == 4335) { // key 8 - green down
    if (grnlev >= 10) {
      green(grnlev - 10);
    }
  }
  if (remval == 21165) { // key 3 - blue up
    if (blulev <= 245) {
      blue(blulev + 10);
    }
  }
  if (remval == 17085) { // key 9 - blue down
    if (blulev >= 10) {
      blue(blulev - 10);
    }
  }
  if (remval == -20401) { // key right - white up
    if (whtlev <= 245) {
      white(whtlev + 10);
    }
  }
  if (remval == 26775) { // key left - white down
    if (whtlev >= 10) {
      white(whtlev - 10);
    }
  }
/*  if (remval == 25245) { // key enter/save - save the current color
    EEPROM.write(10, redlev);
    EEPROM.write(11, grnlev);
    EEPROM.write(12, blulev);
    EEPROM.write(13, whtlev);
    red(0);
    green(0);
    blue(0);
    white(0);
    red(EEPROM.read(10));
    green(EEPROM.read(11));
    blue(EEPROM.read(12));
    white(EEPROM.read(13));
    delay(100);
  }
  if (remval == -8161) { // key repeat - recall the saved color
    red(EEPROM.read(10));
    green(EEPROM.read(11));
    blue(EEPROM.read(12));
    white(EEPROM.read(13));
  }
  if (remval == 8925) { // key vol- - rate down
    rate--;
    if (rate <= 0) {
      rate = 0;
    }
    EEPROM.write(2, rate);
    Serial.print("Rate: ");
    Serial.println(rate);
  }
  if (remval == -15811) { // key vol+ - rate up
    rate++;
    if (rate >= 254) {
      rate = 254;
    }
    EEPROM.write(2, rate);
    Serial.print("Rate: ");
    Serial.println(rate);
  }
/*  if (remval == 765) { // key play/pause - pause/resume cycle
    if (cyclepause == 0) {
      cyclego = 0;
      EEPROM.write(8, cyclego);
      cyclepause = 1;
      EEPROM.write(9, cyclepause);
      mode = 8;
      Serial.println(F("Cycle Paused."));
    }
    else {
      cyclego = 1;
      EEPROM.write(8, cyclego);
      cyclepause = 0;
      EEPROM.write(9, cyclepause);
      mode = 7;
      Serial.print(F("Cycle Resumed."));
      red(255);
      green(0);
      blue(0);
    }
  }
  if (remval == -22441) { // key down - RGB dim
    targetr = (redlev - 10);
    targetg = (grnlev - 10);
    targetb = (blulev - 10);
    if (targetr <= 0) {
      targetr = 0;
    }
    if (targetg <= 0) {
      targetg = 0;
    }
    if (targetb <= 0) {
      targetb = 0;
    }
    Serial.print(F("Dim: "));
    Serial.print(F("Red:")); 
    Serial.print(targetr);
    Serial.print(F(" Green:")); 
    Serial.print(targetg);
    Serial.print(F(" Blue:")); 
    Serial.println(targetb);
    red(targetr);
    green(targetg);
    blue(targetb);
  }
  if (remval == -28561) { // key up - RGB bright
    targetr = (redlev + 10);
    targetg = (grnlev + 10);
    targetb = (blulev + 10);
    if (targetr >= 250) {
      targetr = 255;
    }
    if (targetg >= 250) {
      targetg = 255;
    }
    if (targetb >= 250) {
      targetb = 255;
    }
    Serial.print(F("Brighten: "));
    Serial.print(F("Red:")); 
    Serial.print(targetr);
    Serial.print(F(" Green:")); 
    Serial.print(targetg);
    Serial.print(F(" Blue:")); 
    Serial.println(targetb);
    red(targetr);
    green(targetg);
    blue(targetb);
  }*/
  if (remval == -7651) { // key 0/+10 - all on
    red(255);
    green(255);
    blue(255);
    white(255);
  }
  }
  remval = oldremval;
  
}
