#include <string.h>
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/xtensa_api.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "driver/ledc.h"
#include "driver/pcnt.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "rom/uart.h"
#include "driver/i2c.h"
#include "soc/soc.h"
#include "soc/ledc_reg.h"
#include "soc/ledc_struct.h"
#include "soc/gpio_sig_map.h"

#include "Arduino.h"
#include "esp32-hal.h"
#include "doll.h"
#define EXAMPLE_WIFI_SSID "notbeloser"
#define EXAMPLE_WIFI_PASS "gogogogo"
#define true 1
#define false 0
static EventGroupHandle_t wifi_event_group;
doll d;
const int CONNECTED_BIT = BIT0;
static const char *TAG = "example";
/* Constants that aren't configurable in menuconfig */
hw_timer_t* timer;
uart_t* uart;
char c[200];
int flag=0;
int adc[7];
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();

        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);

        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

//
//void uart_test(){
//	char temp,str[20];
//	int i=0;
//	double time;
//	int servo=0;
//	while(1){
//		scanf("%c",&temp);
//		if(temp!=0){
//			printf("%c",temp);
//			str[i]=temp;
//			i++;
//			fflush(stdout);
//			if(temp=='\n'){
//				if(sscanf(str,"%d %lf\n",&servo,&time) == 2){
//					ESP_LOGI(TAG,"Servo%d,time =%lf",servo,time);
////					ledcWrite(servo,(int)(time*0.8192));
//					switch(servo){
//					case 0:
//						d.l_eye.r=time;
//						break;
//					case 1:
//						d.l_eye.angle=time;
//						break;
//					case 2:
//						d.r_eye.r=time;
//						break;
//					case 3:
//						d.r_eye.angle=time;
//						break;
//					case 4:
//						d.l_ear.angle=time;
//						break;
//					case 5:
//						d.r_ear.angle=time;
//						break;
//					case 6:
//						d.l_bow.angle=time;
//						break;
//					case 7:
//						d.l_bow.y=time;
//						break;
//					case 8:
//						d.r_bow.angle=time;
//						break;
//					case 9:
//						d.r_bow.y=time;
//						break;
//					case 10:
//						d.mouth.angle=time;
//						break;
//					}
//					doll_set(d);
//				}
//				memset(str,0,sizeof(str));
//				i=0;
//			}
//		}
//		vTaskDelay(10 / portTICK_PERIOD_MS);
//		temp=0;
//	}
//	vTaskDelete(NULL);
//}
#define adc_eye_x 35
#define adc_eye_y 32
#define adc_bow_angle 33
#define adc_bow_y 25
#define adc_left_ear 26
#define adc_right_ear 27
#define sw_mouth 14
volatile extern void timer_ISR(){
//	if(d.l_bow.loop_time>0){
//		ledcWrite(d.l_bow.channel_angle,ledcRead(d.l_bow.channel_angle) + d.l_bow.angle_add);
//		ledcWrite(d.l_bow.channel_y,ledcRead(d.l_bow.channel_y) + d.l_bow.y_add);
//		d.l_bow.loop_time --;
//	}
//	if(d.r_bow.loop_time>0){
//		ledcWrite(d.r_bow.channel_angle,ledcRead(d.r_bow.channel_angle) + d.r_bow.angle_add);
//		ledcWrite(d.r_bow.channel_y,ledcRead(d.r_bow.channel_y) + d.r_bow.y_add);
//		d.r_bow.loop_time --;
//	}
//	if(d.l_eye.loop_time>0){
//		ledcWrite(d.l_eye.channel_x,ledcRead(d.l_eye.channel_x)+d.l_eye.x_add);
//		ledcWrite(d.l_eye.channel_y,ledcRead(d.l_eye.channel_y)+d.l_eye.y_add);
//		d.l_eye.loop_time--;
//	}
//	if(d.r_eye.loop_time>0){
//		ledcWrite(d.r_eye.channel_x,ledcRead(d.r_eye.channel_x)+d.r_eye.x_add);
//		ledcWrite(d.r_eye.channel_y,ledcRead(d.r_eye.channel_y)+d.r_eye.y_add);
//		d.r_eye.loop_time--;
//	}
//	if(d.l_ear.loop_time>0){
//		ledcWrite(d.l_ear.channel,ledcRead(d.l_ear.channel)+d.l_ear.duty_add);
//		d.l_ear.loop_time--;
//	}
//	if(d.r_ear.loop_time>0){
//		ledcWrite(d.r_ear.channel,ledcRead(d.r_ear.channel)+d.r_ear.duty_add);
//		d.r_ear.loop_time--;
//	}
//	if(d.mouth.loop_time>0){
//		ledcWrite(d.mouth.channel,ledcRead(d.mouth.channel) + d.mouth.duty_add);
//		d.mouth.loop_time--;
//	}
	//	if(d.l_bow.loop_time>0){
	//		ledcWrite(d.l_bow.channel_angle,ledcRead(d.l_bow.channel_angle) + d.l_bow.angle_add);
	//		ledcWrite(d.l_bow.channel_y,ledcRead(d.l_bow.channel_y) + d.l_bow.y_add);
	//		d.l_bow.loop_time --;
	//	}
	//	if(d.r_bow.loop_time>0){
	//		ledcWrite(d.r_bow.channel_angle,ledcRead(d.r_bow.channel_angle) + d.r_bow.angle_add);
	//		ledcWrite(d.r_bow.channel_y,ledcRead(d.r_bow.channel_y) + d.r_bow.y_add);
	//		d.r_bow.loop_time --;
	//	}
	//	if(d.l_eye.loop_time>0){
	//		ledcWrite(d.l_eye.channel_x,ledcRead(d.l_eye.channel_x)+d.l_eye.x_add);
	//		ledcWrite(d.l_eye.channel_y,ledcRead(d.l_eye.channel_y)+d.l_eye.y_add);
	//		d.l_eye.loop_time--;
	//	}
	//	if(d.r_eye.loop_time>0){
	//		ledcWrite(d.r_eye.channel_x,ledcRead(d.r_eye.channel_x)+d.r_eye.x_add);
	//		ledcWrite(d.r_eye.channel_y,ledcRead(d.r_eye.channel_y)+d.r_eye.y_add);
	//		d.r_eye.loop_time--;
	//	}
	//	if(d.l_ear.loop_time>0){
	//		ledcWrite(d.l_ear.channel,ledcRead(d.l_ear.channel)+d.l_ear.duty_add);
	//		d.l_ear.loop_time--;
	//	}
	//	if(d.r_ear.loop_time>0){
	//		ledcWrite(d.r_ear.channel,ledcRead(d.r_ear.channel)+d.r_ear.duty_add);
	//		d.r_ear.loop_time--;
	//	}
	//	if(d.mouth.loop_time>0){
	//		ledcWrite(d.mouth.channel,ledcRead(d.mouth.channel) + d.mouth.duty_add);
	//		d.mouth.loop_time--;
	//	}

}
void timer_setting(){
	timer = timerBegin(2 ,timer_divider, 1);
	printf("timer count %d\n",timer_count);
	timerAlarmWrite(timer,timer_count, true);
	timerSetAutoReload(timer, true);
	timerAlarmEnable(timer);
	timerAttachInterrupt(timer, timer_ISR, true);
	timerStop(timer);
	timerWrite(timer,0);
	timerStart(timer);
}

void app_main()
{
//	pinMode(sw_mouth,INPUT_PULLUP);
//	analogSetCycles(255);
//	analogSetClockDiv(255);
	nvs_flash_init();
	//initialise_wifi();
	//xTaskCreate(&uart_test, "uart_test", 2048, NULL, 6, NULL);
	d=doll_default_setting();
	doll_init(d);
	//timer_setting();
	uart=uartBegin(1, 115200, SERIAL_8N1, 23,34, 2048, 0);
	while(1){
		while(uartAvailable(uart)>0)
		{
			c[flag]=uartRead(uart);
			flag++;
		}
		if(sscanf(c,"%d,%d,%d,%d,%d,%d,%d\n",&adc[0],&adc[1],&adc[2],&adc[3],&adc[4],&adc[5],&adc[6] ) == 7){
			for(int i=0;i<7;i++)
				printf("%d,",adc[i]);
			printf("\n");
			double bow_angle,bow_y,eye_x,eye_y,ear_l,ear_r,mouth;
			bow_angle=((double)adc[0]-512)/512*45;
			bow_y=((double)adc[1]/1024)*30;
			eye_x=((double)adc[2]-511)/1.4;
			eye_y=((double)adc[3]-511)/1.4;
			ear_r=(1023-(double)adc[4])/1024*60;
			ear_l=(1023-(double)adc[5])/1024*60;
			mouth=adc[6]*30-2;

			double eye_r = sqrt(pow(eye_x,2) + pow(eye_y,2) );
			double eye_angle=degrees(atan2(eye_y+0.001,eye_x+0.001));
			d.l_eye.r = d.r_eye.r = -eye_r;
			d.l_eye.angle  = -eye_angle;
			d.r_eye.angle = -eye_angle;
			d.l_ear.angle = ear_l;
			d.r_ear.angle = ear_r;
			d.l_bow.angle =-bow_angle;
			d.r_bow.angle=bow_angle;
			d.l_bow.y=d.r_bow.y=bow_y;
			d.mouth.angle=mouth;
			doll_set(d);
		}
		flag=0;
	}

}

