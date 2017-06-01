#pragma once


// definition of the included processes
//#define PREPROCESS_RESIZE_IMAGES
//#define PREPROCESS_SCALEDOWN_IMAGES
#define PREPROCESS_HISTO_EQUALIZE  // need this
//#define PREPROCESS_HISTO_MATCH
//#define PREPROCESS_GAUSS_BLUR
//#define PREPROCESS_IMAGE_NORMALIZE

#define DIFF_MEAN
#define DIFF_GRAY

//#define INCLUDE_FAST
#define INCLUDE_STAR
#define INCLUDE_SURF
#define INCLUDE_ORB
////// end of included processes

// definition of debug display images
//#define DISPLAY_IMAGES_DEBUG 

//#define DISPLAY_IMAGES_DEBUG_INITIAL 

//#define DISPLAY_IMAGES_DEBUG_MATCHER

//#define DISPLAY_PRINTS_DEBUG 

//#define DISPLAY_IMAGES_DEBUG_FINAL

//#define DISPLAY_IMAGES_DEBUG_DIFF 

//#define  DISPLAY_IMAGES_PERSPECTIVE ///////////////

//#define  DISPLAY_IMAGES_THRESHOLD // need this //////////////////////

//#define DISPLAY_IMAGES_DEBUG_FINAL_PROCESS

//#define DISPLAY_IMAGES_MEAN_FILTER

//#define DISPLAY_IMAGES_DEBUG_FINAL_SIFT

//#define DISPLAY_SAM // need this //////////////////////

//#define DISPLAY_ROSIN  //////////////////

//#define DISPLAY_PARTITION
//#define DISPLAY_DEBUG_PARTITION
//#define DISPLAY_DEBUG_PARTITION_LESS // need this ////////
//#define DISPLAY_PARTITION_REF_KP // need this/////////
//#define DISPLAY_CONCAN_IMAGE // need this/////////////
//#define DISPLAY_CONCAN_IMAGE_INTERMEDIATE
//#define CONCAN_DISPLAY_BW//////////
// definition of debug display images ends here 


// debug prints
//#define PRINT_DEBUG_FP


// parameter definitions
#define OPEN_SIZE (3)
#define CLOSE_SIZE (9)


#define SLOPE_ELIMINATION_THRESHOLD (1)
#define DISTANCE_ELIMINATION_THRESHOLD (1)

#define MATCHES_PROCESSING_THRESHOLD (30)

#define RECURRING_THRESHOLD (15.0)

#define RECURRING_THRESHOLD_0 (100)  //25
#define RECURRING_THRESHOLD_1 (50)  // 50

#define ANGLE_DIFFERENCE_THRESHOLD (10)

#define SIZE_INDEX_THRESHOLD (50)

#define SIZE_PERCENTAGE_THRESHOLD (55)
#define RESPONSE_PERCENTAGE_THRESHOLD (70.0)
#define MATCH_DISTANCE_PERCENTAGE_THRESHOLD (75.0)

#define DISTANCE_FINAL_THRESHOLD (50)

#define SLOPE_STD_MULTIPLIER_1 (3)
#define SLOPE_STD_MULTIPLIER_2 (2)

#define HOMOGRAPHY_REPROJECT_THRESHOLD (50)

#define MAX_CLUSTERS (3)
//#define USE_ALL_KEYPOINTS
#define USE_CLUSTER_CLEAN
#define USE_CLUSTERING

#define DISPLAY_IMAGES_DEBUG_BORA