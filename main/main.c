/*
 * Punto de acceso (Acess point o AP) 
 * En este ejemplo haremos uso de cada una de las funciones provista por el SDK
 * la cual esconde necesita de registrar un manejador de eventos para wifi e IP
 * en el caso de que nuestra tarjeta se desconecte esta se volvera a conectar de nuevo 
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"

#define SSID                    ("INFINITUMMO01_2.4")
#define PWD                     ("XXXXXXXXXXXXX") 
#define DEFAULT_SCAN_METHOD     (WIFI_FAST_SCAN)                //Realiza un escaneo rapido de redes wifi
#define DEFAULT_SORT_METHOD     (WIFI_CONNECT_AP_BY_SIGNAL)         //Ordena las redes por fuerza de la señal
#define DEFAULT_RSSI            (-127)                          //señal en decibeles
#define DEFAULT_AUTHMODE        (WIFI_AUTH_OPEN)                        

static const char *TAG = "scan";

//Manejador de eventos en caso de conexion, desconexion y adquisicon de direccion IP
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());      //Activa la seccion de memoria flash para guardar algunas configs para conectarse a tu modem
    ESP_ERROR_CHECK(esp_netif_init());       //Inicializa el stack TCP/IP light 

    //Inicializa y carga la configuracion por default para el WIfi    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    //Crea un cadena de eventos para registarlos y atenderlos
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //Registrar eventos de nuestro interes para manejar la conexion a internet
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    // Inicializa nuestra interfaz de red para funcionar como una estacion (esp-netif)
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // Inicializa y arranca el WiFi
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = SSID,
            .password = PWD,
            .scan_method = DEFAULT_SCAN_METHOD,
            .sort_method = DEFAULT_SORT_METHOD,
            .threshold.rssi = DEFAULT_RSSI,
            .threshold.authmode = DEFAULT_AUTHMODE,
        },
    };
    
    //Configura el modo de wifi como estacion usando la configuracion de wifi en wifi_config
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    //Inicializa y desata todos los procesos para conectarse a la red a traves de wifi
    ESP_ERROR_CHECK(esp_wifi_start());
}