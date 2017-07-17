#pragma once

// maximum number of ARALGIS programs run on machine
#define MAX_PROG_INSTANCE (1)

#define WM_UPDATE_CONTROLS        (WM_USER + 1)  // User message for updating controls
#define WM_BIMPDLG_CLOSED         (WM_USER + 2)
#define WM_BIMPDLG_DESTROYED      (WM_USER + 3)
#define WM_FILTER1_READY		  (WM_USER + 4)
#define WM_FILTER2_READY		  (WM_USER + 5)
#define WM_FILTER3_READY		  (WM_USER + 6)
#define WM_FILTER4_READY		  (WM_USER + 7)
#define WM_FILTER5_READY		  (WM_USER + 8)
#define WM_FILTER6_READY		  (WM_USER + 9)
#define PTS_DISPLAY_IMAGE         (WM_USER + 10)
#define PTS_DISPLAY_PLAKA         (WM_USER + 11)
#define PTS_CONNECTION_LOST       (WM_USER + 12)
#define PTS_CONNECTION_OK	      (WM_USER + 13)
#define PTS_CONNECTION_NOK	      (WM_USER + 14)
#define ODROID_CONNECTION_LOST    (WM_USER + 15)
#define ODROID_CONNECTION_OK      (WM_USER + 16)
#define WM_CAMERA_DATA_READY      (WM_USER + 17)
#define WM_DBASE_FILE_WRITTEN     (WM_USER + 18)
#define WM_CHANGEDETECT_FINISHED  (WM_USER + 19)
#define WM_VEHICLEDETECT_FINISHED (WM_USER + 20)

#define WM_LMOUSE_DOWN   (WM_USER + 21)
#define WM_LMOUSE_UP     (WM_USER + 22)
#define WM_LMOUSE_MOVE   (WM_USER + 23)
#define WM_MOUSE_WHEEL   (WM_USER + 24)
#define WM_LMOUSE_DBLCLK (WM_USER + 25)

#define MAX_DIR_PATH_LENGTH (MAX_PATH)
#define MAX_FILENAME_LENGTH (MAX_PATH)
#define PLAKA_CHAR_LENGTH (8)

// Camera Related Parameters
#define MAX_BMP_WINDOW (250)
#define CAM_WIDTH  (320)
#define CAM_HEIGHT (480) // 960 or 480
#define CAM_BITS   (24)
#define CAM_SIZE   (2048)
#define CAM_MAX_BUFFER   (90)

// ARALGIS TCP PORTS
#define PORT_BYTE_LEN  (4)
#define IP_BYTE_LEN (15)

#define WARNINGWINDOW_TITLE  (L"ARALGÝS Uyarý")

#define NUM_INTERMEDIATE_TEST_IMAGE (90)

#define PTS_BY_DIVIT      0x0001
#define PTS_BY_ISSD       0x0002

#define PTS_MODE_TRIGGER      0x0001
#define PTS_MODE_CONTINUOUS    0x0002

#define NO_AUTO_VEHICLE_DETECT        0x0000
#define AUTO_VEHICLEDETECT_WITH_ISSD  0x0001
#define AUTO_VEHICLEDETECT_WITH_DIVIT 0x0002

#define CRITICAL_FREE_SPACE_AMOUNT    (100)  // 100 Mbyes = 100 * 1024 *1024

// timer ID
#define DISPLAY_TIMER_ID 123

// timer period = 10 seconds
#define TIMER_PERIOD_IN_MS  (10000)

// timer period for date timee control = 1 second
#define TIMER_PERIOD_IN_MS_FOR_DATETIME  (1000)
#define TIMER_PERIOD_IN_MS_FOR_ODROID    (1000)

#define TIMER_SECONDS_ERROR_FOR_PTS          (10)
#define TIMER_SECONDS_ERROR_FOR_CAR_FINISHED (25) 

#define COLOUR_RED   (240)
#define COLOUR_GREEN (240)
#define COLOUR_BLUE  (240)