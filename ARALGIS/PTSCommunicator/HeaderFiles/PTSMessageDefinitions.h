#pragma once

#include "..\..\Constants.h"

// ARALGIS - PTS Communication Messages Constants
// Information about Messages coming from TCP PTS Client
#define NUMBYTEPOS       (0)
#define MESSAGEIDPOS     (0)
#define MAGICNUMPOS      (2)
#define TOTALINFOLEN     (4) // NumByte + MessageId + CheckSum + MAgicNumber

#define IMAGE_SIZE_CALC_1 (11)
#define IMAGE_SIZE_CALC_2 (12)
#define IMAGE_SIZE_CALC_3 (13)
#define IMAGE_SIZE_CALC_4 (14)
#define DATAPOS           (15)


