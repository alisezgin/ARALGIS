#ifndef IMAGE_H
#define IMAGE_H
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* DEFINE CONSTANTS */

#define MAX_LINE_LEN 80	    /**< Max. line length */

#define NEW_LINE '\n'	    /**< New line character */

#define NUM_GRAY 256	    /**< Number of gray levels in an 8-bit gray-scale image */

#define MAX_GRAY 255	    /**< Max. gray level in an 8-bit gray-scale image */

#define OBJECT 1

#define BACKGROUND 0

#define PI ( 3.1415926535897932384626433832795 )

#define TWO_PI ( 6.283185307179586476925286766559 )

#define PI_OVER_3 ( 1.0471975511965977461542144610932 )

#define FIVE_PI_OVER_3 ( 5.2359877559829887307710723054658 )

#define SQRT3 ( 1.7320508075688772935274463415059 )


/* MACROS */

/**
* @brief Sets the function name
*/

#define SET_FUNC_NAME( f_name ) static const char func_name[] = f_name

/**
* @brief Determines whether or not a given pointer is NULL
*/

#define IS_NULL( x ) ( ( x ) == NULL )

/**
* @brief Determines whether or not a given number is in the range of [0,255]
* @warning This is not a safe macro
*/

#define IS_BYTE( x ) ( ( 0 <= ( x ) ) && ( ( x ) < 256 ) )

/**
* @brief Determines whether or not a given floating-point number is close to 0.0
*/

#define IS_ZERO( x ) ( fabs ( ( x ) ) < DBL_EPSILON )

/**
* @brief Determines whether or not a given floating-point number is positive
*/

#define IS_POS( x ) ( ( x ) > DBL_EPSILON )

/**
* @brief Determines whether or not a given floating-point number is negative
*/

#define IS_NEG( x ) ( ( x ) < -DBL_EPSILON )

#define IS_ODD( x ) ( ( x ) % 2 )
#define IS_EVEN( x ) ( !IS_ODD ( ( x ) ) )
#define IS_POS_ODD( x ) ( ( ( x ) > 0 ) && IS_ODD ( x ) )

/**
* @brief Determines whether or not a given number is in the range of [0,1]
* @warning This is not a safe macro
*/
#define IS_IN_0_1( x ) ( ! ( IS_NEG ( ( x )  ) || IS_POS ( ( x ) - 1.0 ) ) )

#define SAFE_LOG( x ) ( ( ( x ) > 0.0 ) ? log ( ( x ) ) : ( 0.0 ) )

#define IS_VALID_OBJ( x ) ( !IS_NULL ( ( x ) ) && ( x )->type )

#define GET_NUM_ROWS( x ) ( ( x )->num_rows )

#define GET_NUM_COLS( x ) ( ( x )->num_cols )

#define L1_DIST_3D( x1, y1, z1, x2, y2, z2 )\
 ( abs ( ( x1 ) - ( x2 ) ) + abs ( ( y1 ) - ( y2 ) )  + abs ( ( z1 ) - ( z2 ) ) )

#define L2_NORM_3D_SQR( x, y, z )\
 ( ( x ) * ( x ) + ( y ) * ( y ) + ( z ) * ( z ) )

#define L2_NORM_3D( x, y, z )\
 sqrt ( L2_NORM_3D_SQR ( ( x ), ( y ), ( z ) ) )

#define L2_NORM_2D_SQR( x, y )\
 ( ( x ) * ( x ) + ( y ) * ( y ) )

#define L2_NORM_2D( x, y )\
 sqrt ( L2_NORM_2D_SQR ( ( x ), ( y ) ) )

#define L2_DIST_3D_SQR( x1, y1, z1, x2, y2, z2 )\
 ( ( ( x1 ) - ( x2 ) ) * ( ( x1 ) - ( x2 ) )\
 + ( ( y1 ) - ( y2 ) ) * ( ( y1 ) - ( y2 ) )\
 + ( ( z1 ) - ( z2 ) ) * ( ( z1 ) - ( z2 ) ) )

#define L2_DIST_3D( x1, y1, z1, x2, y2, z2 )\
 sqrt ( L2_DIST_3D_SQR ( ( x1 ), ( y1 ), ( z1 ), ( x2 ), ( y2 ), ( z2 ) ) )

#define L2_DIST_2D_SQR( x1, y1, x2, y2 )\
 ( ( ( x1 ) - ( x2 ) ) * ( ( x1 ) - ( x2 ) )\
 + ( ( y1 ) - ( y2 ) ) * ( ( y1 ) - ( y2 ) ) )\

#define L2_DIST_2D( x1, y1, x2, y2 )\
 sqrt ( L2_DIST_2D_SQR( ( x1 ), ( y1 ), ( x2 ), ( y2 ) ) )\

#define ROUND( X )      ( ( X ) < 0.0 ? ( X ) - 0.5 : ( X ) + 0.5 )

#define DEG_TO_RAD( D ) ( 0.017453292519943295769236907684886 * ( D ) )

#define CLAMP_BYTE( X ) ( ( X ) < 0 ? 0 : ( ( X ) > MAX_GRAY ) ? MAX_GRAY : ( X ) )

#define MIN_2( A, B ) ( ( A ) < ( B ) ? ( A ) : ( B ) )

#define MAX_2( A, B ) ( ( A ) < ( B ) ? ( B ) : ( A ) )

#define MIN_3( A, B, C ) MIN_2 ( ( A ), MIN_2 ( ( B ), ( C ) ) )

#define MAX_3( A, B, C ) MAX_2 ( ( A ), MAX_2 ( ( B ), ( C ) ) )

/** @cond INTERNAL_MACRO */
#define SWAP_INT( a, b ) { tmp_var = ( a ); ( a ) = ( b ); ( b ) = tmp_var; }

/** @endcond INTERNAL_MACRO */

/** @cond INTERNAL_MACRO */
#define SORT_INT( a, b ) { if ( ( a ) > ( b ) ) SWAP_INT ( a, b ); }

/** @endcond INTERNAL_MACRO */

#define DIST_FUNC L2_DIST_3D
#define DIST_FUNC_SQR L2_DIST_3D_SQR
#define ACOS acos

/**
* @brief If DEBUG_ON is defined, prints file name and line number and then
*        calls the variable-argument debug function; otherwise does nothing
*/

#ifdef DEBUG_ON
# define DEBUG fprintf ( stderr, "Debug in %s: ", func_name ), debug
//#else
//# define DEBUG do_nothing
#endif

/**
* @brief Prints file name and line number and then
*        calls the variable-argument warning function
*/

#define WARNING fprintf ( stderr, "Warning in %s: ", func_name ), warning

/**
* @brief Prints file name and line number and then
*        calls the variable-argument error function
*/

//#define ERROR fprintf ( stderr, "Error in %s: ", func_name ), error

/**
* @brief Prints file name and line number and then
*        calls the variable-argument fatal function
*/

#define FATAL fprintf ( stderr, "Fatal error in %s: ", func_name ), fatal

/**
* @brief Prints file name, line number, and the given error message;
*        conditionally aborts the program and then returns the value of
*        the given error number
*/

#define ERROR_RET( err_msg, err_no ) \
         do \
		           { \
           fprintf ( stderr, "Error in %s: %s\n", func_name, ( err_msg ) ); \
           fflush ( stderr ); \
           if ( get_err_mode ( ) ) abort ( ); \
           return ( err_no ) ; \
		           } while ( 0 )

#define MALLOC_STRUCT( x ) ( ( x * ) malloc ( sizeof ( x ) ) )

#define CALLOC_STRUCT( x ) ( ( x * ) calloc ( 1, sizeof ( x ) ) )

#define NEG_ONE_POW( x ) ( IS_EVEN ( ( x ) ) ? ( 1 ) : ( -1 ) )

/* ENUMERATIONS/TYPEDEFS */

enum
{

	E_SUCCESS = 0,	 /**< success */

	E_FAILURE,	 /**< failure */

	E_INVARG,	 /**< invalid argument */

	E_INVOBJ,	 /**< invalid object */

	E_INVBPP,	 /**< invalid pixel depth */

	E_NULL,	 /**< null pointer */

	E_NOMEM,	 /**< insufficient memory */

	E_DIVZERO,	 /**< divide by zero */

	E_UNIMPL,	 /**< feature not implemented */

	E_UNFMT,	 /**< unknown file format */

	E_FOPEN,	 /**< file open error */

	E_FREAD,	 /**< file read error */

	E_FEOF		 /**< end of file reached */

}; /**< Error Codes */

typedef unsigned char byte; /**< Byte Type */

//typedef enum
//{
//
//	false = 0,
//
//	true = 1
//
//} Bool;	/**< Boolean Type */

#define IS_BOOL( x ) ( ( ( x ) == false ) || ( ( x ) == true ) )

typedef enum
{

	PIX_INVALID = 0,  /**< invalid */

	PIX_BIN,	   /**< binary */

	PIX_GRAY,	   /**< gray-scale */

	PIX_RGB,	   /**< RGB color */

	PIX_INT_1B,	   /**< single-band integer */

	PIX_INT_3B,	   /**< 3-band integer */

	PIX_DBL_1B,	   /**< single-band double */

	PIX_DBL_3B	   /**< 3-band double */

} PixelType; /**< Pixel Type Enumeration */

typedef enum
{

	FMT_UNKNOWN = 0, /**< Unknown */

	FMT_BMP,	     /**< BMP */

	FMT_GIF,	     /**< GIF */

	FMT_JPG,	     /**< JPG */

	FMT_PBMA,	     /**< Plain/ASCII PBM */

	FMT_PBM,	     /**< Raw/Binary PBM */

	FMT_PCX,	     /**< PCX */

	FMT_PGMA,	     /**< Plain/ASCII PGM */

	FMT_PGM,	     /**< Raw/Binary PGM */

	FMT_PNG,	     /**< PNG */

	FMT_PPMA,	     /**< Plain/ASCII PPM */

	FMT_PPM,	     /**< Raw/Binary PPM */

	FMT_PSD,	     /**< PSD */

	FMT_RAS,	     /**< RAS */

	FMT_TGA,	     /**< TGA */

	FMT_TIFF	     /**< TIFF */

} ImageFormat; /**< Image Format Enumeration */

typedef struct
{

	PixelType type;     /**< Pixel Type */

	int num_bands;	     /**< Number of Bands */

	int num_rows;	     /**< Number of Rows */

	int num_cols;	     /**< Number of Columns */

	int max_pix_val;    /**< Max. Pixel Value (Defined only for PIX_BIN, PIX_GRAY, and PIX_RGB) */

	int num_cc;	     /**< Number of Connected Components (Defined only for PIX_INT_1B) */

	union
	{

		byte *byte_data;	 /**< For PIX_BIN, PIX_GRAY, PIX_RGB */

		int *int_data;	 /**< For PIX_INT_1B, PIX_INT_3B */

		double *double_data;	 /**< For PIX_DBL_1B, PIX_DBL_3B */

	} data_1d;  /**< 1-dimensional contiguous pixel array */

	union
	{

		byte **byte_data_1b;	      /**< For PIX_BIN, PIX_GRAY */

		byte ***byte_data_3b;	      /**< For PIX_RGB */

		int **int_data_1b;	      /**< For PIX_INT_1B */

		int ***int_data_3b;	      /**< For PIX_INT_3B */

		double **double_data_1b;    /**< For PIX_DBL_1B */

		double ***double_data_3b;   /**< For PIX_DBL_3B */

	} data_nd;  /**< 2 or 3-dimensional pixel array */

} Image; /**< Image Structure */


////////////////// boraN !!!!!!!!!!!!!!!!111
//// moved to Histogram.h

//typedef enum
//{
//
//	HISTO_INVALID = 0,	/**< invalid */
//
//	HISTO_INT,	/**< unnormalized */
//
//	HISTO_DBL	/**< normalized */
//
//} HistoType; /**< Histogram Type Enumeration */
//
//typedef struct
//{
//
//	HistoType type;
//
//	int num_bins;	      /**< Number of Bins */
//
//	int num_pixels;      /**< Number of Pixels in the Source Image */
//
//	union
//	{
//
//		int *int_data;	 /**< For HISTO_INT */
//
//		double *double_data;	 /**< For HISTO_DBL */
//
//	} data;   /**< 1-dimensional contiguous histogram data array */
//
//} Histo; /**< Histogram Structure */
//////////////////////////  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!11111

typedef enum
{

	ErrM_INVALID = 0, /**< invalid */

	EM_MAE,     /**< Mean Absolute Error */

	EM_MSE,     /**< Mean Squared Error */

	EM_RMSE,    /**< Root Mean Squared Error */

	EM_PSNR,    /**< Peak Signal-to-Noise Ratio */

	EM_NMSE,    /**< Normalized Mean Squared Error */

	EM_NCD	     /**< Normalized Color Distance */

} ErrorMeasure;	/**< Error Measure Enumeration */

typedef enum
{

	GEN_INVALID = 0,    /**< invalid */

	GEN_VALID	     /**< valid */

} GenericType; /**< Generic Type Enumeration */

typedef struct
{

	GenericType type;

	int length;			/* length (# points) of the chain */
	int *row;			/* row coordinates */
	int *col;			/* column coordinates */
	int *dir;			/* Freeman chaincode directions */

} Chain; /**< Chain Code Structure */

typedef struct
{

	GenericType type;

	int num_chains;		/* number of chains (objects) in the list */
	int max_length;		/* max. length of an object chain */
	int num_rows;			/* # rows in the parent image */
	int num_cols;			/* # columns in the parent image */
	Chain **chain;			/* chain code pointers */

} ChainList; /**< Chain Code List Structure */

typedef struct
{

	int min_row;			/* minimum row coordinate */
	int max_row;			/* maximum row coordinate */
	int min_col;			/* minimum column coordinate */
	int max_col;			/* maximum column coordinate */

} Box; /**< Bounding-Box Structure */

typedef enum
{

	CMAP_INVALID = 0,

	CMAP_HSV,

	CMAP_INFRARED,

	CMAP_JET

} ColorMap; /**< Color Map Enumeration */

typedef struct
{
	double row;
	double col;
} Point;

typedef struct
{

	GenericType type;

	int num_pts;			/* number of points in the list */
	Point **point;			/* Point pointers */

} PointList; /**< 2-Dimensional Point List Structure */

typedef struct
{
	double maj_axis_len;
	double min_axis_len;
	double aspect_ratio;
	double eccentricity;
	double orientation;
} EllipseFeatures;

typedef struct
{
	double mean;
	double stdev;
	double entropy;
	double roughness;
} RadialDistFeatures;

typedef struct
{
	double F1;
	double F2;
	double F3;
	double F4;
} CSMoments;

typedef struct
{
	double mean;
	double variance;
	double skewness;
	double kurtosis;
} ChordLenStats;

typedef struct
{
	double m00;
	double m01;
	double m10;
	double m11;
	double m02;
	double m20;
	double m12;
	double m21;
	double m03;
	double m30;
} GeoMoments;

typedef struct
{
	double phi1;
	double phi2;
	double phi3;
	double phi4;
	double phi5;
	double phi6;
	double phi7;
} HuMoments;

typedef struct
{
	double I1;
	double I2;
	double I3;
	double I4;
} AffineMoments;

typedef struct
{
	double real;
	double imag;
} Complex;

typedef struct
{
	GenericType type;

	int max_order;
	int radius;
	int num_terms;
	Complex ***data;

} ZernikeBasis;

typedef enum
{

	STRL_INVALID = 0,  /**< invalid */

	STRL_LINE,	   /**< flat line */

	STRL_RECT,	   /**< flat rectangle */

	STRL_DISK,	   /**< flat disk */

	STRL_FLAT,	   /**< arbitrary flat */

	STRL_NONFLAT	   /**< arbitrary non-flat */

} StrelType; /**< Structuring Element Type Enumeration */

typedef struct
{

	StrelType type;

	int num_rows;

	int num_cols;

	int origin_row;

	int origin_col;

	byte *data_1d;

	byte **data_2d;

} Strel;

typedef enum
{

	SEQ_INVALID = 0,  /**< invalid */

	SEQ_OC,	   /**< */

	SEQ_CO,	   /**< */

	SEQ_OCO,	   /**< */

	SEQ_COC	   /**< */

} ASFSeq; /**< Alternating Sequential Filter Sequence */

//typedef enum
//{
//	TYPE_UNKNOWN /* unknown type */,
//	UCHAR /* unsigned char */,
//	SCHAR /* signed char */,
//	USHORT /* unsigned short int */,
//	SSHORT /* signed short int */,
//	INT /* signed int */,
//	ULINT /* unsigned long int */,
//	FLOAT /* float */,
//	DOUBLE				/* double */
//} bufferType;

typedef enum
{
	UNKNOWN_FILTER = 0 /* unknown filter type */,
	ALPHA_DERICHE = 1 /* Deriche's filter (exponential (- alpha |X|)) */,
	GAUSSIAN_DERICHE = 2		/* gaussian approximation (Deriche's coefficients) */
} recursiveFilterType;

static bool abort_on_err = true;

bool get_err_mode(void)
{
	return abort_on_err;
}

/**
* @brief Sets the error mode
*
* @param[in] err_mode Error mode
*
* @return none
*
* @see #get_err_mode
*
* @author M. Emre Celebi
* @date 10.15.2006
*/

void
set_err_mode(const bool err_mode)
{
	abort_on_err = err_mode ? true : false;
}

#endif
