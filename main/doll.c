#include "doll.h"

void doll_init(doll d)
{

}
void eye(void *s)
{
	int i;
	int eye= ( (eye_d*)s)->eye;
	double r=  ( (eye_d*)s)->r;
	double  angle= ( (eye_d*)s)->angle;
	int change_time = ((eye_d*)s)->change_time;
	double x ,y;
	x= r*cos(radians(angle)) +1100/0.8192;
	y= r*sin(radians(angle)) +1100/0.8192;
	ledcWrite(0,(int)(x*0.8192));
	ledcWrite(1,(int)(y*0.8192));
	vTaskDelete(NULL);
}
