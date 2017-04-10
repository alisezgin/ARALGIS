#pragma once

/**
* @file histogram.c
* Routines for histogram creation/manipulation
*/

#include "Image.h"
#include "Settings.h"  


typedef enum
{

	HISTO_INVALID = 0,	/**< invalid */

	HISTO_INT,	/**< unnormalized */

	HISTO_DBL	/**< normalized */

} HistoType; /**< Histogram Type Enumeration */

typedef struct
{

	HistoType type;

	int num_bins;	      /**< Number of Bins */

	int num_pixels;      /**< Number of Pixels in the Source Image */

	union
	{

		int *int_data;	 /**< For HISTO_INT */

		double *double_data;	 /**< For HISTO_DBL */

	} data;   /**< 1-dimensional contiguous histogram data array */

} Histo; /**< Histogram Structure */

/**
* @brief Returns the type of a histogram
*
* @param[in] histo Histogram pointer
*
* @return Histogram type if the histogram is valid;
*         HISTO_INVALID otherwise
*
*/

HistoType get_histo_type(const Histo * histo)
{
	SET_FUNC_NAME("get_histo_type");

	if (!IS_VALID_OBJ(histo))
	{
		ERROR_RET("Invalid histogram object !", HISTO_INVALID);
	}

	return histo->type;
}

/**
* @brief Checks whether or not a histogram is of integer type
*
* @param[in] histo Histogram pointer
*
* @return true if the histogram is of integer type;
*         false otherwise
*
*/

bool is_int_histo(const Histo * histo)
{
	return (!IS_NULL(histo) && histo->type == HISTO_INT);
}

/**
* @brief Checks whether or not a histogram is of double type
*
* @param[in] histo Histogram pointer
*
* @return true if the histogram is of double type;
*         false otherwise
*
*/

bool is_double_histo(const Histo * histo)
{
	return (!IS_NULL(histo) && histo->type == HISTO_DBL);
}

/**
* @brief Returns the # bins of a histogram
*
* @param[in] histo Histogram pointer
*
* @return # bins if the histogram is valid;
*         INT_MIN otherwise
*
*/

int get_num_bins(const Histo * histo)
{
	SET_FUNC_NAME("get_num_bins");

	if (!IS_VALID_OBJ(histo))
	{
		ERROR_RET("Invalid histogram object !", INT_MIN);
	}

	return histo->num_bins;
}

/**
* @brief Returns the # pixels in histogram
*
* @param[in] histo Histogram pointer
*
* @return # pixels if the histogram is valid;
*         INT_MIN otherwise
*
*/

int get_num_pixels(const Histo * histo)
{
	SET_FUNC_NAME("get_num_pixels");

	if (!IS_VALID_OBJ(histo))
	{
		ERROR_RET("Invalid histogram object !", INT_MIN);
	}

	return histo->num_pixels;
}

/**
* @brief Returns the data associated with a histogram
*
* @param[in] histo Histogram pointer
*
* @return Data if the histogram is valid;
*         NULL otherwise
*
*/

void* get_histo_data(const Histo * histo)
{
	SET_FUNC_NAME("get_histo_data");

	if (!IS_VALID_OBJ(histo))
	{
		ERROR_RET("Invalid histogram object !", NULL);
	}

	if (get_histo_type(histo) == HISTO_INT)
	{
		return histo->data.int_data;
	}

	return histo->data.double_data;
}

/**
* @brief Allocates a histogram object
*
* @param[in] type Histogram data type
* @param[in] num_bins # bins { positive }
*
* @return Pointer to the allocated histogram or NULL
*
* @see #free_histo
*
*/

Histo* alloc_histo(const HistoType type, const int num_bins)
{
	SET_FUNC_NAME("alloc_histo");
	Histo *histo = NULL;

	if (!type)
	{
		ERROR_RET("Invalid histogram type !", NULL);
	}

	if (num_bins <= 0)
	{
		//ERROR("Number of bins ( %d ) must be positive !", num_bins);
		return NULL;
	}

	histo = MALLOC_STRUCT(Histo);
	histo->type = type;
	histo->num_bins = num_bins;
	histo->num_pixels = INT_MIN;	/* Illegal value */

	/* Allocate storage for histogram data */
	if (type == HISTO_INT)
	{
		histo->data.int_data = (int *)calloc(num_bins, sizeof(int));
		if (IS_NULL(histo->data.int_data))
		{
			ERROR_RET("Insufficient memory !", NULL);
		}
	}
	else
	{
		histo->data.double_data =
			(double *)calloc(num_bins, sizeof(double));
		if (IS_NULL(histo->data.double_data))
		{
			ERROR_RET("Insufficient memory !", NULL);
		}
	}

	return histo;
}

/**
* @brief Deallocates a histogram object
*
* @param[in,out] histo Histogram pointer
*
* @return none
*
* @note nothing happens if the histogram object is invalid
* @see #alloc_histo
*
*/

void free_histo(Histo * histo)
{
	if (IS_VALID_OBJ(histo))
	{
		if (get_histo_type(histo) == HISTO_INT)
		{
			free(histo->data.int_data);
			histo->data.int_data = NULL;
		}
		else
		{
			free(histo->data.double_data);
			histo->data.double_data = NULL;
		}

		histo->type = HISTO_INVALID;
		histo->num_bins = INT_MIN;
		histo->num_pixels = INT_MIN;
	}
}

/**
* @brief Creates the histogram of a grayscale image
*
* @param[in] img Image pointer { grayscale }
*
* @return Pointer to the histogram or NULL
*
*/

Histo* create_histo(const cv::Mat& img)
{
	SET_FUNC_NAME("create_histo");
	byte *img_data;
	int i;
	int num_pixels;
	int *histo_data;
	Histo *histo = NULL;



	num_pixels = img.rows * img.cols;

	img_data = (byte *) &(img.at<byte>(0, 0));

	/* Allocate storage for the histogram */
	histo = alloc_histo(HISTO_INT, NUM_GRAY);
	if (IS_NULL(histo))
	{
		ERROR_RET("alloc_histo() failed !", NULL);
	}

	histo->num_pixels = num_pixels;

	histo_data = (int *) get_histo_data(histo);

	/* Populate the histogram */
	for (i = 0; i < num_pixels; i++)
	{
		histo_data[img_data[i]]++;
	}

	return histo;
}

/**
* @brief Normalizes an histogram
*
* @param[in] in_histo Histogram pointer
*
* @return Pointer to the normalized histogram or NULL
*
*/

Histo* normalize_histo(const Histo * in_histo)
{
	SET_FUNC_NAME("normalize_histo");
	int ih;
	int num_pixels;
	int num_bins;			/* number of bins in the histogram */
	int *in_data;			/* input histogram data */
	double term;
	double *out_data;		/* output histogram data */
	Histo *out_histo;

	if (!is_int_histo(in_histo))
	{
		ERROR_RET("Not an integer histogram !", NULL);
	}

	num_pixels = get_num_pixels(in_histo);
	num_bins = get_num_bins(in_histo);
	in_data = (int *) get_histo_data(in_histo);

	/* Allocate storage for the histogram */
	out_histo = alloc_histo(HISTO_DBL, num_bins);
	if (IS_NULL(out_histo))
	{
		ERROR_RET("alloc_histo() failed !", NULL);
	}

	out_histo->num_pixels = num_pixels;
	out_data = (double *) get_histo_data(out_histo);

	term = 1.0 / (double)num_pixels;

	/* Calculate the normalized histogram */
	for (ih = 0; ih < num_bins; ih++)
	{
		out_data[ih] = term * in_data[ih];
	}

	return out_histo;
}

/**
* @brief Calculates the cumulative histogram of an histogram
*
* @param[in] histo Histogram pointer
*
* @return Pointer to the cumulative histogram data or NULL
*
*/

void* accumulate_histo(const Histo * histo)
{
	SET_FUNC_NAME("accumulate_histo");
	int ih;
	int num_bins;

	if (!IS_VALID_OBJ(histo))
	{
		ERROR_RET("Invalid histogram object !", NULL);
	}

	num_bins = get_num_bins(histo);

	/* Codes for the INT and DBL histograms are nearly identical */

	if (get_histo_type(histo) == HISTO_INT)
	{
		int *histo_data;
		int *accum_data;

		histo_data = (int *) get_histo_data(histo);

		accum_data = (int *)malloc(num_bins * sizeof(int));
		if (IS_NULL(accum_data))
		{
			ERROR_RET("Insufficient memory !", NULL);
		}

		accum_data[0] = histo_data[0];
		for (ih = 1; ih < num_bins; ih++)
		{
			accum_data[ih] = accum_data[ih - 1] + histo_data[ih];
		}

		return accum_data;
	}
	else
	{
		double *histo_data;
		double *accum_data;

		histo_data = (double *) get_histo_data(histo);

		accum_data = (double *)malloc(num_bins * sizeof(double));
		if (IS_NULL(accum_data))
		{
			ERROR_RET("Insufficient memory !", NULL);
		}

		accum_data[0] = histo_data[0];
		for (ih = 1; ih < num_bins; ih++)
		{
			accum_data[ih] = accum_data[ih - 1] + histo_data[ih];
		}

		return accum_data;
	}

	/*@notreached@ */
}









