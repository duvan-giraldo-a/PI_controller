#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/uart.h"
#include "timer.h"
#include "pwm.h"
#include "bluetooth.h"
#include "events.h"
#include "general.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "bluetooth.h"
// ----- Definition -------------
#define uart_task_PRIORITY      configMAX_PRIORITIES -1
#define Delay
#define rpm_max 17000
uint8_t counter = 0;
float rpm = 0;

SemaphoreHandle_t xSemaphore;
// Handle Task
TaskHandle_t uartHandler;
TaskHandle_t UpdatePI;
TaskHandle_t EncoderValue;
TaskHandle_t SendVelocity;

void on_uart(){
    irq_set_enabled(UART0_IRQ, false);
    BaseType_t CheckIfYieldRequired;
    CheckIfYieldRequired = xTaskResumeFromISR(uartHandler);
    portYIELD_FROM_ISR(CheckIfYieldRequired);
}


void vEncoderValue(void *pvParameters){
    while(1){
        vTaskDelay(20);
        xSemaphoreTake(xSemaphore, portMAX_DELAY);
        rpm = ((60*measure_duty_cycle(MEASURE_PIN))/(15*0.001*10));
        printf("velocidad (rpm): %1.f \n", rpm);
        xSemaphoreGive(xSemaphore);
    }
}

void vUpdatePI(void *pvParameters){
    int16_t error = 0;
    int16_t error_ant = 0;
    int acc = 0;
    int acc_ant = 0;
    float q1 = 0.188;
    float q2 = -0.17;

    while (1){   
        vTaskDelay(20);        
        if(mode == 1) {
            xSemaphoreTake(xSemaphore, portMAX_DELAY);
            uint16_t set_point = (pwmDutty*rpm_max)/100;
            error_ant = error;
            error = set_point - rpm;
            printf("Error: %u \n", error);
            acc = ((q1*error)+(q1*error_ant)+(256*acc_ant))/256;
            xSemaphoreGive(xSemaphore);
            if(acc>100) acc = 100;
            if(acc<0) acc = 0;
            acc_ant = acc;
            pwm_change_level(PWM_GPIO_MOTOR, acc_ant);    
        } else {
            pwm_change_level(PWM_GPIO_MOTOR, pwmDutty);    
        }
    }
}

void vSendVelocity(void *pvParameters){
    while(1){
        vTaskDelay(500);
        char Send[4] = "Hola";
        for (int i = 0; i < 4; i++){
            on_uart_tx_Char(Send[i]);
        }
    }
}

void uart_task(void *pvParameters){
    uartInit();
    vTaskSuspend(NULL);
    while(1){
        caracter = uart_getc(UART_ID);
        getData();
        irq_set_enabled(UART0_IRQ, true);
        vTaskSuspend(NULL);
    }
}
int main() {

    // STDIO: inicialización de USB-UART para entrada/salida
    stdio_init_all();
    set_sys_clock_48mhz();
    pwm_myinit();
    
    // Mensaje de bienvenida
    while (!stdio_usb_connected());    
    printf("\nControlador  Motor DC!\n\n");
    sleep_ms(1000);

    xSemaphore = xSemaphoreCreateMutex();


    // --------- Create Task -----------------------

    // Uart Task
    if(xTaskCreate(uart_task, "uart_task", configMINIMAL_STACK_SIZE + 100, NULL, uart_task_PRIORITY, &uartHandler) != pdPASS){
        printf("%s uart_task created faild!\n");
        while (1);
    }

    // Controller PI Task
    if(xTaskCreate(vUpdatePI, "PI_task", configMINIMAL_STACK_SIZE + 100, NULL, 20, &UpdatePI) != pdPASS){
        printf("%s PI_task created faild!\n");
        while (1);
    }

    // Encoder Task
    if(xTaskCreate(vEncoderValue, "Encoder_task", configMINIMAL_STACK_SIZE + 100, NULL, 20, &EncoderValue) != pdPASS){
        printf("%s Encoder_task created faild!\n");
        while (1);
    }

    // velocity Task
    if(xTaskCreate(vSendVelocity, "velocity_task", configMINIMAL_STACK_SIZE + 100, NULL, 10, &SendVelocity) != pdPASS){
        printf("%s velocity_task created faild!\n");
        while (1);
    }

    if(xSemaphore == NULL){
        printf("%s xSemaphore_productor created faild!\n");
        vTaskSuspend(NULL);
    }

    /* Start the tasks and timer running. */
    vTaskStartScheduler();
    /* should never reach here */
    panic_unsupported();

    // // Inicializar el TIMER (Para disparar los eventos de ADC)
    // if (!timer_init(TIMER_PERIOD_MS)) {
    //     printf("Error inicializando el timer.\n");
    //     SYSTEM_HALT();
    // }



    // // Inicializar el PWM
    // if (!pwm_myinit()) {
    //     printf("Error inicializando el PWM: los canales pertenecen a diferentes slices de PWM.\n");
    //     SYSTEM_HALT();
    // }

    // //Inicializacion UART
    
    // // Esperar por eventos
    // eventsController();

}