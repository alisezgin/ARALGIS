#pragma once

#include "..\..\Constants.h"

// AccountingPC - Server Communication Messages Constants
// Information about Messages coming from UDP client/JAMES/SitePlayer/CMD
#define NUMBYTEPOS       (0)
#define MESSAGEIDPOS     (0)
#define MAGICNUMPOS      (2)
#define TOTALINFOLEN     (4) // NumByte + MessageId + CheckSum + MAgicNumber

#define IMAGE_SIZE_CALC_1 (11)
#define IMAGE_SIZE_CALC_2 (12)
#define IMAGE_SIZE_CALC_3 (13)
#define IMAGE_SIZE_CALC_4 (14)
#define DATAPOS           (15)




// value showing messages coming from our !!solar!! system
#define MAGICNUMBER     (0xEB)

typedef struct _MessageTrigger {
	unsigned char numberOfBytes;
	unsigned char messageID;
	unsigned char MagicNumber;
	unsigned char checkSum;
	double        totalMoney;
	double        activeMoney;
} MessageB4;

typedef struct _ReceivedMessage{
	unsigned char NumberOfBytes;
	unsigned char MessageId;
	unsigned char MagicNumber;
	unsigned char Data[MAX_MESSAGE_LENGTH_FROM_PTS];
	unsigned char Checksum;
} ReceivedMessage;

