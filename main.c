
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

typedef struct{
    uint8_t id;
    uint32_t delay;
    SemaphoreHandle_t *dmutex;
}delayTask_t;

static void HardwareInit(){
    set_sys_clock_48mhz();
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN,true);
}

void sleepingTask(void * pvParameters){
    uint8_t ID = ((delayTask_t *)pvParameters)->id;
    uint32_t delay = ((delayTask_t *)pvParameters)->delay;
    //SemaphoreHandle_t * xMtx = ((delayTask_t *)pvParameters)->dmutex;
    for(;;){
        printf("Task %s executing on core %d\n",pcTaskGetName(NULL),get_core_num());
        vTaskDelay(delay);
        if(ID==0)
            gpio_put(PICO_DEFAULT_LED_PIN,false);
        else if(ID==1)
            gpio_put(PICO_DEFAULT_LED_PIN,true);
    }
}

int main(void) {
    BaseType_t xReturned;

    HardwareInit();
    while(!stdio_usb_connected());
    sleep_ms(1000);
    TaskHandle_t taskHandleArray[10];
    uint32_t taskDelays[10]={500,500,1000,1000,2000,2000,3000,5000,10000,20000};
    char taskName[]="T0\0T1\0T2\0T3\0T4\0T5\0T6\0T7\0T8\0T9\0";
    delayTask_t delayTaskArray[10];

    for(int i=0;i<10;i++){
        delayTaskArray[i].id=i;
        delayTaskArray[i].delay=taskDelays[i];
        delayTaskArray[i].dmutex=NULL;
        xReturned = xTaskCreate(sleepingTask,&taskName[3*i],1000,&delayTaskArray[i],2,&taskHandleArray[i]);
        if (xReturned == pdPASS)
            printf("%s created succesfuly\n",&taskName[3*i]);
    }

    printf("Core %d: Launching FreeRTOS scheduler\n", get_core_num());
    /* Start the tasks and timer running. */
    vTaskStartScheduler();
    /* should never reach here */
    panic_unsupported();

}