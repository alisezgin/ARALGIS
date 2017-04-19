#pragma once



#define WM_UPDATE_CONTROLS      (WM_USER + 1)  // User message for updating controls
#define WM_BIMPDLG_CLOSED       (WM_USER + 2)
#define WM_BIMPDLG_DESTROYED    (WM_USER + 3)
#define WM_CAMERA_DATA_READY    (WM_USER + 4)
#define WM_DBASE_CAR_INFO_READY (WM_USER + 5)


#define MAX_BORA (5)

#define MAX_DIR_PATH_LENGTH (400)
#define MAX_FILENAME_LENGTH (2356)


// timer period = 1000 miliseconds - 1second
#define TIMER_PERIOD_IN_MS  (1000)
#define DISPLAY_TIMER_ID (111)

// plaka info will stay 5 seconds on the screen
#define PLAKA_TIME_ON_SCREEN (5)

#define MAX_BUFFER            (90)
#define IMAGE_HEIGHT (480) // 960 or 480
#define MAX_BMP_WINDOW (250)






#define CAM_WIDTH  (320)
#define CAM_HEIGHT (240)
#define CAM_BITS   (24)

// length of messages coming from PTS System
#define MAX_MESSAGE_LENGTH_FROM_PTS   (4096) //unused now


// ARALGIS TCP PORTS
#define ARALGIS_ODROID_TCP_PORT_INT (1001)
#define ARALGIS_ODROID_TCP_PORT_CHAR ("1001")
#define ARALGIS_PTS_TCP_PORT_CHAR ("1000") 
#define ARALGIS_PTS_TCP_PORT_INT (1000) 



#define IP_ADDRESS_LEN       (16)
#define IP_ADDRESS_BYTE_LEN  (4)

#define PTS_IP_ADDRESS ("127.0.0.1")


// timer interrupt generated for threads every 5 second
#define THREADS_TIMER_INTERRUPT_PERIOD  (5)

// timer interrupt to display 5 seconds of warning messages
#define WARNING_MESSAGE_DURATION        (5)


// number of trials to send a message to server
#define MAX_MESSAGE_TRIALS  (10)

#define WARNINGWINDOW_TITLE  (L"ARALGÝS Uyarý")

#define PLAKA_CHAR_LENGTH (8)
