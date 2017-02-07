#include "Arduino.h"
#include "esp32-hal.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/xtensa_api.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"

typedef struct eye_data{
		int channel_y;//1
		int channel_y_gpio;//2
		int channel_x;//0
		int channel_x_gpio;//15
		double r;//0
		double  angle;//0
		int change_time_ms;//100
		int rev;
}eye_d;

typedef struct bow_data{
	int channel_y;
	int channel_y_gpio;
	int channel_angle;
	int channel_angle_gpio;
	double  angle,y;
	int change_time_ms;
	int rev;
}bow_d;


typedef struct ear_data{
	int channel;
	int channel_gpio;
	double angle;
	int change_time_ms;
	int rev;
}ear_d;

typedef struct  mouth_data{
	int channel;
	int channel_gpio;
	double angle;
	int change_time_ms;
}mouth_d;

typedef struct doll_data{
	eye_d l_eye;
	eye_d r_eye;
	bow_d r_bow;
	bow_d l_bow;
	ear_d l_ear;
	ear_d r_ear;
	mouth_d mouth;
}doll;

doll doll_default_setting();
void doll_init(doll);
void eye_init(eye_d);
void mouth_init(mouth_d);
void ear_init(ear_d);
void bow_init(bow_d);

void eye_set(eye_d);
void mouth_set(mouth_d);
void ear_set(ear_d);
void bow_set(bow_d);
