#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "events.h"
#include "pwm.h"

#include "bluetooth.h"


uint8_t state = 0;
uint8_t frame[4];
uint8_t pheriphal_id = 4;
bool flagrw = false;
char caracter = 'q';
bool emptyBuffer = true;
uint8_t buffer[2]; 
uint8_t pwmDutty = 50;
uint8_t Getptr = 0;
uint8_t Putptr = 0;
uint8_t fifo[TamFIFO];
uint8_t mode = 0;

//Inicialización UART0 (Buadios(bits/seg), Puertos GPIO como TX/RX, FIFO desactivada, Interrupción RX unicamente)
void uartInit(){
    uart_init(UART_ID, BOUD_RATE);
    gpio_set_function(PICO_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(PICO_UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_fifo_enabled(UART_ID, false);

    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);
}

void on_uart_tx_String(char *msg){
    if(uart_is_writable(UART_ID)){
        uart_puts(UART_ID, msg);
    }

}

/* Método usado para transmitir un caracter usando la interfaz UART */
void on_uart_tx_Char(char msg){
    uart_putc(UART_ID, msg);
}

/* Máquina de estados empleada para la recepción de datos enviados por bluetooth desde el aplicativo Móvil */
void getData(){
    switch (state){
    /* Estado 0: verificación del caracter inicial $ de la trama*/
    case 0:
        if(caracter == '0'){
            state = 1;
            mode = 0;
        }else if(caracter == '1'){
            printf("hol");
            state = 2;
            mode = 1;
        } else {
            state = 0;
        }
        break;
    /* Estado 1: verificación del tipo de solicitud (Monitoreo de perifericos, control de perifericos) */
    case 1:
        pwmDutty = caracter;
        state = 0;
        break;
    /* Estado 2: captura de la orden en caso de que la solicitud sea de control */
    case 2:
            printf("hol1");
        pwmDutty = caracter;
        state = 0;
        break;
    default:
        break;
    }
}

/* Método usado para codificar y transmitir la información usando la interfaz UART del MCU a la cual está conectado
   el módulo bluetooth. ($,periférico,información sensada,caracter de verificación (($+canal+data)%128)) */
void setData(uint8_t ch, uint16_t data){
    char Send[4];
    Send[0] = '$';
    Send[1] = 0x00 | ch;
    Send[2] = (uint8_t)(data * 100 / 4095);
    Send[3] = (Send[0]+Send[1]+Send[2]) % 128;
    for (int i = 0; i < 4; i++){
        on_uart_tx_Char(Send[i]);
    }
}

