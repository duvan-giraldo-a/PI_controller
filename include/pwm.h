#ifndef _PWM_H
#define _PWM_H

#define PWM_GPIO_LED    0
#define PWM_GPIO_MOTOR  1
// Pin para medir los rpm del motor
#define MEASURE_PIN		5

#define PWM_WRAP_VALUE      12500
#define PWM_LEVEL_VALUE     0
#define PWM_ADC_MIN_LEVEL   50*100/4095

bool pwm_myinit(void);
void pwm_change_level(uint, uint16_t);
uint16_t measure_duty_cycle(uint gpio);

#endif