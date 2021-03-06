#pragma once

#include "..\..\Constants.h"

// ARALGIS - Odroid Communication Messages Constants
// Information about Messages going/coming to/from TCP client
#define MESSAGE_LENGTH         (16)

#define ODROID_MESSAGE_INDICATOR (1)
#define ARALGIS_MESSAGE_INDICATOR (2)

#define MESSAGE_FROM_POS       (0)

#define MESSAGE_ID_POS         (1)

#define MESSAGE_RESERVED1_POS  (2)

#define MESSAGE_RESERVED1_SIZE (2) // 2 bytes reserved

#define MESSAGE_VAL_POS (4)
#define MESSAGE_VAL_SIZE (2)

#define MESSAGE_RESERVED2_POS  (6)
#define MESSAGE_RESERVED2_SIZE (2) // 2 bytes reserved

#define TIME_POS           (8)
#define TIME_SIZE          (8)

#define MESSAGE_CAR_DETECTED_NO (1)
#define MESSAGE_CAR_FINISHED_NO (2)
#define MESSAGE_CAR_SPEED_NO    (3)

#define MESSAGE_OPEN_BARRIER_NO   (4)
#define MESSAGE_CLOSE_BARRIRER_NO (5)
#define MESSAGE_START_HEATING_NO  (6)
#define MESSAGE_STOP_HEATING_NO   (7)
#define MESSAGE_START_ALARM_NO    (8)
#define MESSAGE_STOP_ALARM_NO     (9)


typedef struct _MessageOdroid {
	unsigned char messageFrom;
	unsigned char messageID;
	unsigned char reserved1[MESSAGE_RESERVED1_SIZE];
	unsigned char valueContent[MESSAGE_VAL_SIZE];
	unsigned char reserved2[MESSAGE_RESERVED2_SIZE];	
	unsigned char messageTime[TIME_SIZE];
} MessageOdroid;

