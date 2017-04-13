#pragma once



#define WM_UPDATE_CONTROLS    (WM_USER + 1)  // User message for updating controls



// Odroid comm send to main frame that client socket closed
#define WM_ODROID_LOST (WM_USER + 3)

#define MAX_BORA (5)

#define MAX_STRINGTABLE_STR_SIZE (256)


// timer period = 250 miliseconds
#define TIMER_PERIOD_IN_MS  (250)

#define MAX_BUFFER            (30)
#define IMAGE_HEIGHT (480) // 960 or 480
#define MAX_BMP_WINDOW (250)



#define WM_BIMPDLG_CLOSED (WM_APP + 100)
#define  WM_BIMPDLG_DESTROYED (WM_APP + 101)
#define WM_CAMERA_DATA_READY (WM_APP + 102)


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

// timer period = 250 miliseconds
#define TIMER_PERIOD_IN_MS  (250)

// timer interrupt generated for threads every 5 second
#define THREADS_TIMER_INTERRUPT_PERIOD  (5)

// timer interrupt to display 5 seconds of warning messages
#define WARNING_MESSAGE_DURATION        (5)

// wait duration to send find server message
#define SERVER_FINDER_WAIT_DURATION     (1000) // 1000 msec = 1 sec

// number of trials to send a message to server
#define MAX_MESSAGE_TRIALS  (10)

#define WARNINGWINDOW_TITLE  (L"ARALGÝS Uyarý")

#define PLAKA_CHAR_LENGTH (8)
