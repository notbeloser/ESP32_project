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
static void eye_set_loop(eye_d* f){
	double x,y,r,angle;
	x=ledcRead(f->channel_x)/0.8192;
	y=ledcRead(f->channel_y)/0.8192;
	if(f->rev){
		r=sqrt(  pow(eye_center-x,2) + pow(eye_center-y,2 ) );
		angle =  degrees(acos((eye_center-x)/r));
	}
	else{
		r=sqrt(  pow(x-eye_center,2) + pow(eye_center-y,2 ) );
		angle =  degrees(acos((x-eye_center)/r));
	}

	double r_final,angle_final;
	int loop_time;
	loop_time =f->change_time_ms / delay_time;
	r_final = f->r;
	angle_final = f->angle;
	double r_add,angle_add;
	r_add=(r_final-r)/loop_time;
	angle_add= (angle_final-angle)/loop_time;

	for(int i=0;i<loop_time;i++){
		f->r=r=r+r_add;
		f->angle=angle = angle+angle_add;
		eye_set(*f);
		delay(delay_time);
	}
	f->r = r_final;
	f->angle = angle_final;
	eye_set(*f);
	vTaskDelete(NULL);
}
void eye_set_with_time(eye_d eye){
	xTaskCreate(&eye_set_loop, "eye_set_loop", 2048,(void *)(&eye), 6, NULL);
}

void mouth_set(mouth_d m){
	int duty = (servo_max-m.angle * 92 /9)* 0.8192;
	ledcWrite(m.channel,duty);
}
static void mouth_set_loop(mouth_d *m){
	double angle= (servo_max - ledcRead(m->channel )/ 0.8192)*9/92;
	double angle_final = angle;
	int loop_time = m->change_time_ms /delay_time;
	double angle_add = ( m -> angle -angle)/loop_time;

	for(int i=0;i<loop_time;i++){
		angle=angle+angle_add;
		m->angle = angle;
		mouth_set( *m );
		delay(delay_time);
	}
	m->angle = angle_final;
	mouth_set(*m );
	vTaskDelete(NULL);
}
void mouth_set_with_time(mouth_d m){
	xTaskCreate(&mouth_set_loop, "mouth_set_loop", 2048, (void *)(&m), 6, NULL);
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
static void ear_set_loop(ear_d* e){
	double angle;
	if(e->rev){
		angle = (servo_max-ledcRead(e->channel)/0.8192-750)*9/92;
	}
	else{
		angle = (ledcRead(e->channel)/0.8192 -750 )*9/92;
	}
	double angle_final = angle;
	int loop_time = e->change_time_ms /delay_time;
	double angle_add = (e->angle - angle)/loop_time;
	for(int i=0;i<loop_time;i++){
		e->angle=angle=angle+angle_add;
		ear_set(*e);
		delay(delay_time);
	}
	e->angle=angle_final;
	ear_set(*e);
	vTaskDelete(NULL);
}
void ear_set_with_time(ear_d e){
	xTaskCreate(&ear_set_loop,"ear_set_loop",2048,(void*)(&e),6,NULL);
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
static void bow_set_loop(bow_d* b){
	const double l = 12.59;//length of servo stick
	const double lowest_deg = 39;
	const double low_height_lowest = l*tan(radians(lowest_deg));

	//height
	double height_duty = ledcRead(b->channel_y);
	double angle_move_old;
	if(b->rev){
		angle_move_old =(3070- height_duty/0.8192)*9/92;
	}
	else{
		angle_move_old = (height_duty/0.8192-750)*9/92;
	}
	double height = l*tan(radians(angle_move_old + lowest_deg)) - low_height_lowest;
	//--------angle-----------
	double angle= (ledcRead(b->channel_angle)/0.8192 -1430)*9/92;
	//---------------------------
	double height_final,angle_final;
	height_final=b->y;
	angle_final = b->angle;
	double height_add,angle_add;
	int loop_time = b->change_time_ms /delay_time;
	height_add=(height_final - height) /loop_time;
	angle_add = (angle_final - angle)/loop_time;
	for(int i =0 ;i<loop_time;i++){
		height=height+height_add;
		angle = angle+angle_add;
		b->y = height;
		b->angle = angle;
		bow_set(*b);
		delay(delay_time);
	}
	b->y=height_final;
	b->angle = angle_final;
	bow_set(*b);
	vTaskDelete(NULL);
}
void bow_set_with_time(bow_d b){
	xTaskCreate(&bow_set_loop,"bow_set_loop",2048,(void*)(&b),6,NULL);
}




