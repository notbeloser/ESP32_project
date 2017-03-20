

#include "string.h"
#include "SoftwareSerial.h"
#define l_x 14
#define l_y 15
#define r_x 16
#define r_y 17
#define l_z 18
#define r_z 19
#define sw_mouth 4
int a,b,c,d,e,f,g;
char c1[200];
//SoftwareSerial S(5,6);

void setup() {
Serial.begin(115200);
//S.begin(115200);
pinMode(sw_mouth,INPUT_PULLUP);

//pinMode(9,OUTPUT);
}

void loop() {

  sprintf(c1,"%d,%d,%d,%d,%d,%d,%d\n",analogRead(l_x),analogRead(l_y),analogRead(r_x),analogRead(r_y),analogRead(l_z),analogRead(r_z),!digitalRead(sw_mouth));
  Serial.print(c1);
  memset(c1,0,sizeof(c1));
  delay(50);
  
//  a=analogRead(l_x)>>3;
//  b=analogRead(l_y)>>3;
//  c=analogRead(r_x)>>3;
//  d=analogRead(r_y)>>3;
//  e=analogRead(l_z)>>3;
//  f=analogRead(r_z)>>3;
//  g=!digitalRead(sw_mouth);
//
//  noteOn(0x90,0x2a,0x7f);
//  delay(50);
//  noteOn(0x80,0x2a,0);
//  delay(50);



}
void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
