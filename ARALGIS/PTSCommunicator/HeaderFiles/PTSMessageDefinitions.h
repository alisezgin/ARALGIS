#pragma once

#include "..\..\Constants.h"

// ARALGIS - PTS Communication Messages Constants
// Information about Messages coming from TCP PTS Client
#define MESSAGE_LENGTH    (6)
#define CLIENT_ID_POS     (6)
#define DATA_SIZE_LENGTH  (4)
#define PLAKA_NO_MESSAGE  (1)
#define CAR_IMAGE_MESSAGE (2)
#define UVSS_IMAGE        (3)
#define MESSAGE_TYPE_POS  (4)
#define REGISTRATION_CONSTANT (0x2F)

#define IMAGE_SIZE_CALC_1 (0)
#define IMAGE_SIZE_CALC_2 (1)
#define IMAGE_SIZE_CALC_3 (2)
#define IMAGE_SIZE_CALC_4 (3)


typedef struct _MessageToAralgis {
	unsigned char DataLenght[DATA_SIZE_LENGTH];
	unsigned char messageID;
	unsigned char PlakaNo[PLAKA_CHAR_LENGTH];

} MessageToAralgis;

typedef struct _MessageAralgisID {
	unsigned char DataLenght[MESSAGE_LENGTH];
} MessageAralgisID;










