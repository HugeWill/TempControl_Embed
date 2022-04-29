#ifndef _DRV_ENCODER_H_
#define _DRV_ENCODER_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "ucos_ii.h"
#include "BSP_Gpio.h"

typedef enum{
    ENCODER_1 = 0,
    ENCODER_2,
    ENCODER_3,
    ENCODER_4,
    ENCODER_MAX,
} ENCODER_ENUM;

typedef enum{
    POLARITY_CW = 0,
    POLARITY_CCW,
} POLARITY_ENUM;

typedef enum{
    ENCODER_UNDEFINE = 0,
    ENCODER_READY = 1,
}ENCODER_STATUS_ENUM;
typedef struct
{
    ENCODER_ENUM id;
    ENCODER_STATUS_ENUM status;
    BSP_PORT_ENUM en1_ss1_port;
    BSP_PIN_ENUM en1_ss1_pin;
    BSP_PORT_ENUM en1_ss2_port;
    BSP_PIN_ENUM en1_ss2_pin;
	
	  BSP_PORT_ENUM en2_ss1_port;
    BSP_PIN_ENUM en2_ss1_pin;
    BSP_PORT_ENUM en2_ss2_port;
    BSP_PIN_ENUM en2_ss2_pin;
    OS_EVENT* EncoderBox;
}ENCODER_TYPE;

typedef struct{
	ENCODER_ENUM event_id;
	POLARITY_ENUM polarity;
} MESSAGE_BOX;

extern ENCODER_TYPE* _gp_Encoder;
extern ENCODER_TYPE* Drv_Encode_Init(ENCODER_ENUM id,BSP_PORT_ENUM en1_ss1_port,BSP_PIN_ENUM en1_ss1_pin,\
                              BSP_PORT_ENUM en1_ss2_port, BSP_PIN_ENUM en1_ss2_pin,\
															BSP_PORT_ENUM en2_ss1_port, BSP_PIN_ENUM en2_ss1_pin,\
															BSP_PORT_ENUM en2_ss2_port, BSP_PIN_ENUM en2_ss2_pin);
                              
void Encode1_SS1_IRQHandler(void);
void Encode1_SS2_IRQHandler(void);
void Encode2_SS1_IRQHandler(void);
void Encode2_SS2_IRQHandler(void);
                    
#endif
