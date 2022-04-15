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
float acum = 0;
bool flagDecimals = false;
int decimal = 0;
bool flag_decimal = false;
char bufferDecimals[20] = "--------------------";
int counterDecimals = 0;
bool flagSendData = false;
bool constantIdentifier = false;

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
        case 0:
            if(caracter == '0'){
                state = 1;
                mode = 0;
                flagSendData = true;
            } else if (caracter == '1'){
                state = 2;
                mode = 1;
                flagSendData = true;
            } else if (caracter == '2'){// Proportional constant
                state = 3;
            } else if (caracter == '3'){// Integral constant
                state = 4;
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
            pwmDutty = caracter;
            state = 0;
            break;
        case 3:
            if(!flagDecimals){
                bufferDecimals[counterDecimals] = caracter;
                counterDecimals++;
            }
            if (caracter == '$'){
                flagDecimals = true;
                constantIdentifier = false;
            }
            break;
        case 4:
            
            if(!flagDecimals){
                bufferDecimals[counterDecimals] = caracter;
                counterDecimals++;
            }
            
            if (caracter == '$'){
                flagDecimals = true;
                constantIdentifier = true;
            }
            break;
        default:
            break;
    }
}

float doDecimalExtraction(){
    calculateDecimalInput();
    float result = (acum/(potencia(10, decimal)));
    // printf("result %f \n", result);
    state = 0;
    cleanVector();
    acum = 0;
    decimal = 0;
    counterDecimals = 0;
    flag_decimal = false;
    flagDecimals = false;
    return result;
}

void cleanVector(){
    for(int i=0; i<20; i++) {
        bufferDecimals[i] = '-';
    }
}

void calculateDecimalInput(){
    float temporalVar = 0;
    uint8_t counterPower = 0;
    float potenciaTemporal = 0;
    
    while (counterDecimals>0){
        counterDecimals--;
        if (bufferDecimals[counterDecimals] != '.' && bufferDecimals[counterDecimals] != '$') {
            temporalVar = bufferDecimals[counterDecimals]&0x0F;
            potenciaTemporal = potencia(10,counterPower);
            acum = acum + potenciaTemporal*temporalVar; 
            if (!flag_decimal){
                decimal++;
            }
            counterPower++;
        }
        else if(bufferDecimals[counterDecimals] == '.'){
            flag_decimal = true;
        }
    }
    counterDecimals = 0;
}

float potencia(int numero, int potencia){
    int resultado = numero;
    if (potencia == 0){
        resultado = 1;
    }
    while (potencia > 1)
    {
        resultado = resultado * numero;
        potencia--;
    }
    return resultado;
}

void setData(float setPoint, float rpm){
    char Send[12];
    Send[0] = 's';
    uint32_t x,y;
    x = setPoint;
    for(int i=1;i<=5;i++)
    {
        if (i == 1)
        {
            y=x%10;
            Send[i] = y+0x30;
        } else {
            y = (x / 10)% 10;
            Send[i] = y+0x30;
            x = x / 10;
        }
    }
    Send[6] = 'r';
    x = rpm;
    for(int i=1;i<=5;i++)
    {
        if (i == 1)
        {
            y=x%10;
            Send[i+6] = y+0x30;
        } else {
            y = (x / 10)% 10; 
            Send[i+6] = y+0x30;
            x = x / 10;
        }
    }

    for (int i = 0; i < 12; i++){
        on_uart_tx_Char(Send[i]);
    }
}

