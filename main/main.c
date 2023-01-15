/*
 * Punto de acceso (Acess point o AP) 
 * En este ejemplo haremos uso de la funcion provista por el SDK
 * la cual esconde la mayoria de detalles necesarios y conecta nuestra tarjeta a la red 
 * que seleccionamos usando el menuconfig
 */

#include <stdio.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "protocol_examples_common.h"
#include "esp_log.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());      //Activa la seccion de memoria flash para guardar algunas configs para conectarse a tu modem
    ESP_ERROR_CHECK(esp_netif_init());      
    ESP_ERROR_CHECK(esp_event_loop_create_default());   //Crea un manejador de eventos IP por default
                                                        // en cuanto nuestra tarjeta adquiera una direccion IP, esta se mostrara

    ESP_ERROR_CHECK(example_connect());
}