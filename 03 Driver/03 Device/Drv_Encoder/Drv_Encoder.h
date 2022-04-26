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
    POLARITY_CW = 0;
    POLARITY_CCW,
} POLARITY_ENUM;

typedef enum{
    ENCODER_UNDEFINE = 0,
    ENCODER_READY = 1,
}ENCODER_STATUS_ENUM;
typedef struct
{

    OS_Event* EncoderBox;
};

#endif
