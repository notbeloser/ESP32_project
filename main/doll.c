#include "doll.h"

doll doll_default_setting(){
	doll d;
	d.l_eye.channel_x=0;
	d.l_eye.channel_x_gpio=15;
	d.l_eye.channel_y=1;
	d.l_eye.channel_y_gpio=2;
	d.l_eye.angle=0;
	d.l_eye.r=0;
	d.l_eye.change_time_ms=300;
	d.l_eye.rev=0;

	d.r_eye.channel_x=2;
	d.r_eye.channel_x_gpio=0;
	d.r_eye.channel_y=3;
	d.r_eye.channel_y_gpio=4;
	d.r_eye.angle=0;
	d.r_eye.r=0;
	d.r_eye.change_time_ms=300;
	d.r_eye.rev=1;

	d.l_ear.channel=4;
	d.l_ear.channel_gpio=16;
	d.l_ear.angle=0;
	d.l_ear.change_time_ms=300;
	d.l_ear.rev=1;

	d.r_ear.channel=5;
	d.r_ear.channel_gpio=17;
	d.r_ear.angle=0;
	d.r_ear.change_time_ms=300;
	d.r_ear.rev=0;

	d.l_bow.channel_angle=6;
	d.l_bow.channel_angle_gpio=5;
	d.l_bow.channel_y=7;
	d.l_bow.channel_y_gpio=18;
	d.l_bow.angle=0;
	d.l_bow.y=0;
	d.l_bow.change_time_ms=300;
	d.l_bow.rev=1;

	d.r_bow.channel_angle=8;
	d.r_bow.channel_angle_gpio=19;
	d.r_bow.channel_y=9;
	d.r_bow.channel_y_gpio=21;
	d.r_bow.angle=0;
	d.r_bow.y=0;
	d.r_bow.change_time_ms=300;
	d.r_bow.rev=0;

	d.mouth.angle=0;
	d.mouth.change_time_ms=300;
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

	eye_set(d.l_eye);
	eye_set(d.r_eye);
	mouth_set(d.mouth);
	ear_set(d.l_ear);
	ear_set(d.r_ear);
	bow_set(d.r_bow);
	bow_set(d.l_bow);
}
void doll_set(doll d){
	eye_set(d.l_eye);
	eye_set(d.r_eye);
	mouth_set(d.mouth);
	ear_set(d.l_ear);
	ear_set(d.r_ear);
	bow_set(d.r_bow);
	bow_set(d.l_bow);
}
void eye_init(eye_d eye){
	ledcSetup(eye.channel_x,100,13);
	ledcSetup(eye.channel_y,100,13);
	ledcAttachPin(eye.channel_x_gpio,eye.channel_x);
	ledcAttachPin(eye.channel_y_gpio,eye.channel_y);
}
void mouth_init(mouth_d m)
{
	ledcSetup(m.channel,100,13);
	ledcAttachPin(m.channel_gpio,m.channel);
}
void ear_init(ear_d ear){
	ledcSetup(ear.channel,100,13);
	ledcAttachPin(ear.channel_gpio,ear.channel);
}
void bow_init(bow_d bow)
{
	ledcSetup(bow.channel_angle,100,13);
	ledcSetup(bow.channel_y,100,13);
	ledcAttachPin(bow.channel_angle_gpio,bow.channel_angle);
	ledcAttachPin(bow.channel_y_gpio,bow.channel_y);
}

void eye_set(eye_d eye){
	double x,y;

	y=-eye.r*sin(radians(eye.angle))+eye_center;
	if(eye.rev){
		x=-eye.r*cos(radians(eye.angle))+eye_center;
	}
	else{
		x=eye.r*cos(radians(eye.angle))+eye_center;
	}
	ledcWrite(eye.channel_y,(int)(y*0.8192));
	ledcWrite(eye.channel_x,(int)(x*0.8192));
}

void eye_set_with_time(eye_d* f){
	double x,y;

		y=-f->r*sin(radians(f->angle))+eye_center;
		if(f->rev){
			x=-f->r*cos(radians(f->angle))+eye_center;
		}
		else{
			x=f->r*cos(radians(f->angle))+eye_center;
		}
		f->loop_time = f->change_time_ms/delay_time;
		f->x_add=(x-ledcRead(f->channel_x))/f->loop_time;
		f->y_add=(y-ledcRead(f->channel_y))/f->loop_time;
}

void mouth_set(mouth_d m){
	int duty = (servo_max-(32-m.angle) * 92 /9)* 0.8192;
	ledcWrite(m.channel,duty);
}

void mouth_set_with_time(mouth_d *m){
	m->loop_time = m->change_time_ms/20;
	m->duty_add = ((servo_max-(32-m->angle) * 92 /9)* 0.8192 - ledcRead(m->channel) )/m->loop_time;
}

void ear_set(ear_d e){
	int duty;
	if(e.rev){
		duty =  (servo_max -  (e.angle * 92 /9 +750) )*0.8192;
	}
	else{
		duty =  (e.angle * 92/9 +750)*0.8192;
	}
	ledcWrite(e.channel,duty);
}

void ear_set_with_time(ear_d *e){
	e->loop_time = e->change_time_ms/delay_time;
	if(e->rev){
		e->duty_add =  ((servo_max -  (e->angle * 92 /9 +750) )*0.8192 - ledcRead(e->channel))/e->loop_time;
	}
	else{
		e->duty_add =  ((e->angle * 92/9 +750)*0.8192-ledcRead(e->channel) )/e->loop_time;
	}

}

void bow_set(bow_d b){
	const double l = 12.59;//length of servo stick
	const double lowest_deg = 39;
	const double low_height_lowest = l*tan(radians(lowest_deg));
	double height= b.y +low_height_lowest;
	double rad = atan2(height,l);
	double angle_move = degrees(rad) - lowest_deg;
	int duty;
	if(b.rev){
		duty = (servo_max - 92/9 * angle_move) * 0.8192;
	}
	else{
		duty = (92/9 * angle_move + servo_min) * 0.8192;
	}
	ledcWrite(b.channel_y,duty);
	int angle_duty = (1430 + 92/9 * b.angle)*0.8192;
	ledcWrite(b.channel_angle,angle_duty);
}

void bow_set_with_time(bow_d *b){
	const double l = 12.59;//length of servo stick
	const double lowest_deg = 39;
	const double low_height_lowest = l*tan(radians(lowest_deg));
	double height= b->y +low_height_lowest;
	double rad = atan2(height,l);
	double angle_move = degrees(rad) - lowest_deg;
	int duty;
	if(b->rev){
		duty = (servo_max - 92/9 * angle_move) * 0.8192;
	}
	else{
		duty = (92/9 * angle_move + servo_min) * 0.8192;
	}
	b->loop_time = b->change_time_ms / delay_time;
	int angle_duty = (1430 + 92/9 * b->angle)*0.8192;
	b->y_add = (duty - ledcRead(b->channel_y))/b->loop_time;
	b->angle_add = (angle_duty-ledcRead(b->channel_angle))/b->loop_time;
}




