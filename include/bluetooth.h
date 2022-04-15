#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H



#define PICO_UART_TX_PIN  16
#define PICO_UART_RX_PIN  17
#define BOUD_RATE         9600
#define UART_ID           uart0
#define DATA_BITS         0
#define STOP_BITS         1
#define PARITY            UART_PARITY_NONE
# define TamFIFO  4
# define TamBUFFER  2

extern char caracter;
extern uint8_t pwmDutty;
extern uint8_t fifo[TamFIFO];
extern uint8_t buffer[TamBUFFER];
extern bool emptyBuffer;
extern uint8_t Getptr;
extern uint8_t Putptr;
extern char bufferDecimals[20];
extern uint8_t mode;
extern bool flagDecimals;
extern bool flagSendData;

void uartInit(void);
void on_uart(void);
void on_uart_tx_String(char *msg);
void on_uart_tx_Char(char msg);
void getData();
void setData(float setPoint, float rpm);
float potencia(int numero, int potencia);
void calculateDecimalInput(void);
void cleanVector(void);
void doDecimalExtraction(void);


#endif