#include "string.h"

#define l_x 14
#define l_y 15
#define r_x 16
#define r_y 17
#define l_z 18
#define r_z 19
#define sw_mouth 4
char c[200];
void setup() {
Serial.begin(9600);
pinMode(sw_mouth,INPUT_PULLUP);
}

void loop() {

  sprintf(c,"%d,%d,%d,%d,%d,%d,%d\n",analogRead(l_x),analogRead(l_y),analogRead(r_x),analogRead(r_y),analogRead(l_z),analogRead(r_z),!digitalRead(sw_mouth));
  Serial.print(c);
  memset(c,0,sizeof(c));
  delay(20);
}
