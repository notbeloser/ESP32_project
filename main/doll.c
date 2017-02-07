#include "Arduino.h"
#include "stdio.h"
#include "esp32-hal.h"
#include "doll.h"

doll doll_default_setting(){
	doll d;
	d.l_eye.channel_x=0;
	d.l_eye.channel_x_gpio=15;
	d.l_eye.channel_y=1;
	d.l_eye.channel_y_gpio=2;
	d.l_eye.angle=0;
	d.l_eye.r=0;
	d.l_eye.change_time_ms=100;

	d.r_eye.channel_x=2;
	d.r_eye.channel_x_gpio=0;
	d.r_eye.channel_y=3;
	d.r_eye.channel_y_gpio=4;
	d.r_eye.angle=0;
	d.r_eye.r=0;
	d.r_eye.change_time_ms=100;

	d.l_ear.channel=4;
	d.l_ear.channel_gpio=16;
	d.l_ear.angle=0;
	d.l_ear.change_time_ms=100;

	d.r_ear.channel=5;
	d.r_ear.channel_gpio=17;
	d.r_ear.angle=0;
	d.r_ear.change_time_ms=100;

	d.l_bow.channel_angle=6;
	d.l_bow.channel_angle_gpio=5;
	d.l_bow.channel_y=7;
	d.l_bow.channel_y_gpio=18;
	d.l_bow.angle=0;
	d.l_bow.change_time_ms=100;

	d.r_bow.channel_angle=8;
	d.r_bow.channel_angle_gpio=19;
	d.r_bow.channel_y=9;
	d.r_bow.channel_y_gpio=21;
	d.r_bow.angle=0;
	d.r_bow.change_time_ms=100;

	d.mouth.angle=0;
	d.mouth.change_time_ms=100;
	d.mouth.channel=10;
	d.mouth.channel_gpio=22;

	return d;
}
void doll_init(doll d)
{
	eye_init(d.l_eye);
	eye_init(d.r_eye);
	mouth_init(d.mouth);
	ear_init(d.l_ear);
	ear_init(d.r_ear);
	bow_init(d.r_bow);
	bow_init(d.l_bow);
}
void eye_init(eye_d eye){
	ledcSetup(eye.channel_x,200,13);
	ledcSetup(eye.channel_y,200,13);
	ledcAttachPin(eye.channel_x_gpio,eye.channel_x);
	ledcAttachPin(eye.channel_y_gpio,eye.channel_y);
}
void mouth_init(mouth_d m)
{
	ledcSetup(m.channel,200,13);
	ledcAttachPin(m.channel_gpio,m.channel);
}
void ear_init(ear_d ear){
	ledcSetup(ear.channel,200,13);
	ledcAttachPin(ear.channel_gpio,ear.channel);
}
void bow_init(bow_d bow)
{
	ledcSetup(bow.channel_angle,200,13);
	ledcSetup(bow.channel_y,200,13);
	ledcAttachPin(bow.channel_angle_gpio,bow.channel_angle);
	ledcAttachPin(bow.channel_y_gpio,bow.channel_y);
}
void eye_set(eye_d eye){
	double x,y;
	x=eye.r*cos(radians(eye.angle))+1250;
	y=eye.r*sin(radians(eye.angle))+1250;
	ledcWrite(eye.channel_x,(int)(x*0.8911) );
	ledcWrite(eye.channel_y,(int)(y*0.8911));
}
void mouth_set(mouth_d m)
{

}


