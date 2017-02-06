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

#define EXAMPLE_WIFI_SSID "notbeloser"
#define EXAMPLE_WIFI_PASS "gogogogo"
#define true 1
#define false 0
static EventGroupHandle_t wifi_event_group;

const int CONNECTED_BIT = BIT0;
static const char *TAG = "example";
/* Constants that aren't configurable in menuconfig */
void eye(void *pvParameters);
typedef struct eye_data {
		int eye;
		double r;
		double  angle;
		int change_time;
}eye_d ;


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


void uart_test()
{
	char temp,str[20];
	int i=0;
	int servo=0,time=1024;
	while(1)
	{
		scanf("%c",&temp);
		if(temp!=0)
		{
			printf("%c",temp);
			str[i]=temp;
			i++;
			fflush(stdout);
			if(temp=='\n')
			{
				if(sscanf(str,"%d %d\n",&servo,&time) == 2)
				{
					ESP_LOGI(TAG,"Servo%d,time =%d",servo,time);
				}
				memset(str,0,sizeof(str));
				i=0;
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
		temp=0;
	}
	vTaskDelete(NULL);
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

void app_main()
{
	nvs_flash_init();
	//initialise_wifi();
	xTaskCreate(&uart_test, "uart_test", 2048, NULL, 6, NULL);
	ledcSetup(0,200,13);
	ledcSetup(1,200,13);
	ledcAttachPin(4,0);
	ledcAttachPin(16,1);



	while(1)
	{

	}

}

