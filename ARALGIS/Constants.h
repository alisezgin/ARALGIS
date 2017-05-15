#pragma once

// maximum number of ARALGIS programs run on machine
#define MAX_PROG_INSTANCE (1)

#define WM_UPDATE_CONTROLS      (WM_USER + 1)  // User message for updating controls
#define WM_BIMPDLG_CLOSED       (WM_USER + 2)
#define WM_BIMPDLG_DESTROYED    (WM_USER + 3)
#define WM_CAMERA_DATA_READY    (WM_USER + 4)
#define WM_DBASE_CAR_INFO_READY (WM_USER + 5)


#define MAX_DIR_PATH_LENGTH (256)
#define MAX_FILENAME_LENGTH (512)
#define PLAKA_CHAR_LENGTH (8)


// timer period = 1000 miliseconds - 1second
#define TIMER_PERIOD_IN_MS  (1000)
#define DISPLAY_TIMER_ID (111)

// plaka info will stay 5 seconds on the screen
#define PLAKA_TIME_ON_SCREEN (10000)

// Camera Related Parameters
#define MAX_BMP_WINDOW (250)
#define CAM_WIDTH  (320)
#define CAM_HEIGHT (480) // 960 or 480
#define CAM_BITS   (24)
#define CAM_SIZE   (2048)
#define CAM_MAX_BUFFER   (90)

#define PORT_BYTE_LEN  (4)

// ARALGIS TCP PORTS
#define ARALGIS_ODROID_TCP_PORT_CHAR ("1001")
#define ARALGIS_PTS_TCP_PORT_CHAR ("1000") 
#define IP_ADDRESS_LEN       (16)
#define IP_ADDRESS_BYTE_LEN  (4)

#define WARNINGWINDOW_TITLE  (L"ARALGÝS Uyarý")

#define NUM_INTERMEDIATE_TEST_IMAGE (90)
