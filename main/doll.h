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

typedef struct doll_data{

	typedef struct left_eye_data {
			int y_channel=1;
			int x_channel=0;
			double r=0;
			double  angle=0;
			int change_time_ms=100;
	}left_eye;

	typedef struct right_eye_data {
			int y_channel=3;
			int x_channel=2;
			double r=0;
			double  angle=0;
			int change_time_ms=100;
		}right_eye;

	typedef struct left_bow_data{
		int y_channel=4;
		int x_channel=5;
		double  angle=0,y=0;
		int change_time_ms=100;
	}left_bow;

	typedef struct right_bow_data{
		int y_channel=6;
		int x_channel=7;
		double  angle=0,y=0;
		int change_time_ms=100;
	}left_bow;



}doll;

void doll_init();

void eye_init();
void mouth_init();
void ear_init();
void bow_init();
