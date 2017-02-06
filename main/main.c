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
/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;
static const char *TAG = "example";
/* Constants that aren't configurable in menuconfig */
void eye(int eye,int r,double  angle);
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
//		.ap ={
//				.ssid = "ESP32",
//				.password = "testtest",
//				.ssid_len = 8,
//				.channel = 6.,            /**< Channel of ESP32 soft-AP */
//				.authmode = WIFI_AUTH_WPA_WPA2_PSK,  /**< Auth mode of ESP32 soft-AP. Do not support AUTH_WEP in soft-AP mode */
//				.ssid_hidden=0,        /**< Broadcast SSID or not, default 0, broadcast the SSID */
//				.max_connection=4,     /**< Max number of stations allowed to connect in, default 4, max 4 */
//		},
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}


/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "192.168.43.56"
static const char  * WEB_PORT 	 ="8000";
#define WEB_URL "192.168.43.56"

//static const char *REQUEST = "GET / HTTP/1.1\n"
//    "Host: "WEB_SERVER"\n"
//    "User-Agent: esp-idf/1.0 esp32\n"
//    "\n";

static const char *REQUEST = "POST /token/ HTTP/1.1\n"
		"Host:192.168.43.56:8000"
		"Content-Type:application/x-www-form-urlencoded\n"
		"uid=123456&account=example@gmail.com&password=0000000\n"
		;
static void http_get_task(void *pvParameters)
{
	const struct addrinfo hints = {
	        .ai_family = AF_INET,
	        .ai_socktype = SOCK_STREAM,
	    };
	    struct addrinfo *res;
	    struct in_addr *addr;
	    int s, r;
	    char recv_buf[64];

	    while(1) {
	        /* Wait for the callback to set the CONNECTED_BIT in the
	           event group.
	        */
	        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
	                            false, true, portMAX_DELAY);
	        ESP_LOGI(TAG, "Connected to AP");

	        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

	        if(err != 0 || res == NULL) {
	            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
	            vTaskDelay(1000 / portTICK_PERIOD_MS);
	            continue;
	        }

	        /* Code to print the resolved IP.

	           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
	        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
	        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

	        s = socket(res->ai_family, res->ai_socktype, 0);
	        if(s < 0) {
	            ESP_LOGE(TAG, "... Failed to allocate socket.");
	            freeaddrinfo(res);
	            vTaskDelay(1000 / portTICK_PERIOD_MS);
	            continue;
	        }
	        ESP_LOGI(TAG, "... allocated socket\r\n");

	        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
	            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
	            close(s);
	            freeaddrinfo(res);
	            vTaskDelay(4000 / portTICK_PERIOD_MS);
	            continue;
	        }

	        ESP_LOGI(TAG, "... connected");
	        freeaddrinfo(res);

	        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
	            ESP_LOGE(TAG, "... socket send failed");
	            close(s);
	            vTaskDelay(4000 / portTICK_PERIOD_MS);
	            continue;
	        }
	        ESP_LOGI(TAG, "... socket send success");

	        /* Read HTTP response */
	        do {
	            bzero(recv_buf, sizeof(recv_buf));
	            r = read(s, recv_buf, sizeof(recv_buf)-1);
	            for(int i = 0; i < r; i++) {
	               putchar(recv_buf[i]);
	            }
	        } while(r > 0);

	        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
	        close(s);
	        for(int countdown = 10; countdown >= 0; countdown--) {
	            ESP_LOGI(TAG, "%d... ", countdown);
	            vTaskDelay(1000 / portTICK_PERIOD_MS);
	        }
	        ESP_LOGI(TAG, "Starting again!");
	    }
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
					ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, servo,time, 50);
					ledc_fade_start(servo, LEDC_FADE_NO_WAIT);
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



void eye(int eye,int r,double  angle)
{
	int x ,y;
	x= (int)(r*cos(angle)) +1100;
	y= (int)(r*sin(angle) )+1100;
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,eye*2,x,100);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE,eye*2+1,y,100);
	ledc_fade_start(eye*2, LEDC_FADE_NO_WAIT);
	ledc_fade_start(eye*2+1, LEDC_FADE_NO_WAIT);
}

#define Servo_0    	4
#define Servo_1		16
#define Servo_2		17
#define Servo_3		5
#define Servo_4		18
void servo_setting(int bit,int hz,int speed_mode,int timer_num)
{
	ledc_timer_config_t ledc_timer = {
		//set timer counter bit number
		.bit_num = bit,
		//set frequency of pwm
		.freq_hz = hz,
		//timer mode,
		.speed_mode = speed_mode,
		//timer index
		.timer_num = timer_num
	};
	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel = {
		//set LEDC channel 0
		.channel = speed_mode,
		//set the duty for initialization.(duty range is 0 ~ ((2**bit_num)-1)
		.duty = 819,
		//GPIO number
		.gpio_num = Servo_0,
		//GPIO INTR TYPE, as an example, we enable fade_end interrupt here.
		.intr_type = LEDC_INTR_FADE_END,
		//set LEDC mode, from ledc_mode_t
		.speed_mode = speed_mode,//low speed mode
		//set LEDC timer source, if different channel use one timer,
		//the frequency and bit_num of these channels should be the same
		.timer_sel = timer_num
	};
	ledc_channel_config(&ledc_channel);

	ledc_channel.channel=LEDC_CHANNEL_1;
	ledc_channel.gpio_num= Servo_1;
	ledc_channel_config(&ledc_channel);

	//ledc_channel.speed_mode=LEDC_HIGH_SPEED_MODE;
	ledc_channel.channel=LEDC_CHANNEL_2;
	ledc_channel.gpio_num= Servo_2;
	ledc_channel_config(&ledc_channel);


	ledc_channel.channel=LEDC_CHANNEL_3;
	ledc_channel.gpio_num= Servo_3;
	ledc_channel_config(&ledc_channel);

	ledc_channel.channel=LEDC_CHANNEL_4;
	ledc_channel.gpio_num= Servo_4;
	ledc_channel_config(&ledc_channel);

	ledc_fade_func_install(0);

}



extern   void app_main()
{
	nvs_flash_init();
	//initialise_wifi();
	//	i2cInit(0, 0x40, 0);
	//xTaskCreate(&http_get_task, "http_get_task", 2048, NULL, 5, NULL);
	//servo_setting(13,100,LEDC_HIGH_SPEED_MODE,LEDC_TIMER_0);
	//xTaskCreate(&uart_test, "uart_test", 2048, NULL, 6, NULL);
	//xTaskCreate(&mouth_test, "mouth_test", 2048, NULL, 5, NULL);
	ledcSetup(0, 200, 13);
	ledcAttachPin(4 ,0);

	while(1)
	{
		ledcWrite(0,819);
		delay(1000);
		ledcWrite(0,819*2);
		delay(1000);
	}

}

