#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <functional>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "HomographyEstimator.h"  
#include "Settings.h"  

class MatchCombinerEliminator
{
private:

	// member variables to hold 
	// reference and test images
	cv::Mat m_imgRef;
	cv::Mat m_imgTest;

	// variable used for image display titles
	// increased every trial so that titles of
	// the displays are different
	int m_count;

	// member variable to count the number of added
	// features, SURF and SIFT
	int m_featCnt;

	// variable to control variance multiplier
	int m_DspCnt;

	//
	int m_loopCounter;

	int m_ThreshSelect;



	std::vector<cv::DMatch> m_matchesCombinedCleaned;
	std::vector<cv::KeyPoint> m_keypointsRefCombinedCleaned;
	std::vector<cv::KeyPoint> m_keypointsTestCombinedCleaned;

	int m_DisplayCnt;

public:

	MatchCombinerEliminator() {}

	void combineEliminateMatches(	cv::Mat& imgReference,
									cv::Mat& imgTest,
									std::vector<cv::DMatch> matchesSURF,
									std::vector<cv::KeyPoint> keypointsRefSURF,
									std::vector<cv::KeyPoint> keypointsTestSURF,
									std::vector<cv::DMatch> matchesSIFT,
									std::vector<cv::KeyPoint> keypointsRefSIFT,
									std::vector<cv::KeyPoint> keypointsTestSIFT,
									cv::Point2f offset,
									std::vector<cv::DMatch>* matchesCombinedCleaned,
									std::vector<cv::KeyPoint>* keypointsRefCombinedCleaned,
									std::vector<cv::KeyPoint>* keypointsTestCombinedCleaned,
									int aDisplayCnt = 0,
									int aThreshSelect = 0)

	{
		std::vector<cv::DMatch> matchesCombined;
		std::vector<cv::KeyPoint> keypoints1, keypoints2;

		imgReference.copyTo(m_imgRef);
		imgTest.copyTo(m_imgTest);

		m_DisplayCnt = aDisplayCnt;

		m_ThreshSelect = aThreshSelect;

		m_DspCnt = (aDisplayCnt*5) + 1;

		m_count = 0;
		m_featCnt = 0;

		int k = 0;

		// put matches and keypoints of SURF in the common variables
		for (std::vector<cv::DMatch>::iterator matchIterator = matchesSURF.begin();
			matchIterator != matchesSURF.end();
			++matchIterator)
		{
			matchesCombined.push_back(cv::DMatch(k, k,
				(*matchIterator).distance));

			keypoints1.push_back(keypointsRefSURF[matchesSURF[k].trainIdx]);
			keypoints2.push_back(keypointsTestSURF[matchesSURF[k].queryIdx]);
			k++;
		}
#ifdef  DISPLAY_PRINTS_DEBUG
		std::printf("\n processMatches # of METHOD-1 Keypoints %d", k);
#endif

		//calculateOriginalPlaceSIFT(keypointsRefSIFT, keypointsTestSIFT);

		int i = 0;

		// combine matches and nd keypoints of SIFT in the common variables
		// Now SURF and SIFT parameters are combines
		for (std::vector<cv::DMatch>::iterator matchIterator = matchesSIFT.begin();
			matchIterator != matchesSIFT.end();
			++matchIterator)
		{
			matchesCombined.push_back(cv::DMatch(k, k,
				(*matchIterator).distance));

			keypoints1.push_back(keypointsRefSIFT[matchesSIFT[i].trainIdx]);
			keypoints2.push_back(keypointsTestSIFT[matchesSIFT[i].queryIdx]);
			k++;
			i++;
		}
		// end of combination

#ifdef  DISPLAY_PRINTS_DEBUG
		std::printf("\n processMatches # of METHOD-2 Keypoints %d", i);
		std::printf("\n processMatches # of TOTAL Keypoints %d\n", k);
#endif


#ifdef  DISPLAY_IMAGES_DEBUG_FINAL_PROCESS
		char title[1000];
		strcpy_s(title, "COMBINED MATCHES  1st Step of Final Processing Processing");

		strcat_s(title, " ");
		char sNumCnt[20];
		_itoa_s(m_DisplayCnt, sNumCnt, sizeof(sNumCnt), 10);
		strcat_s(title, sNumCnt);

		DisplayMatches matchDisplayer;
		matchDisplayer.displayMatchesProcessor(m_imgRef, m_imgTest, matchesCombined,
			keypoints1, keypoints2, title);
#endif
		// end of combination

		m_matchesCombinedCleaned = matchesCombined;
		m_keypointsRefCombinedCleaned = keypoints1;
		m_keypointsTestCombinedCleaned = keypoints2;
		
		double aMean, aStdDev;

		if (m_matchesCombinedCleaned.size() > 30)
		{
			m_loopCounter = 0;
			CalculateSlopeMeanAndStdDev(m_matchesCombinedCleaned, m_keypointsRefCombinedCleaned, m_keypointsTestCombinedCleaned, offset, &aMean, &aStdDev);
			m_DspCnt++;
			m_loopCounter++;
			CalculateSlopeMeanAndStdDev(m_matchesCombinedCleaned, m_keypointsRefCombinedCleaned, m_keypointsTestCombinedCleaned, offset, &aMean, &aStdDev);

			eliminateSmallResponseRecurringKeypoints();
		}

		if (m_matchesCombinedCleaned.size() < 20)
		{
			m_matchesCombinedCleaned = matchesCombined;
			m_keypointsRefCombinedCleaned = keypoints1;
			m_keypointsTestCombinedCleaned = keypoints2;
		}


		//findExtremumPoints();

		matchesCombinedCleaned->erase(matchesCombinedCleaned->begin(), matchesCombinedCleaned->end());
		keypointsRefCombinedCleaned->erase(keypointsRefCombinedCleaned->begin(), keypointsRefCombinedCleaned->end());
		keypointsTestCombinedCleaned->erase(keypointsTestCombinedCleaned->begin(), keypointsTestCombinedCleaned->end());

		*matchesCombinedCleaned = m_matchesCombinedCleaned;
		*keypointsRefCombinedCleaned = m_keypointsRefCombinedCleaned;
		*keypointsTestCombinedCleaned = m_keypointsTestCombinedCleaned;

#ifdef PRINT_DEBUG_FP
		printf("\nRemaining %d Points in FINAL Response-Recurrence Processing!!!!\n", matchesCombinedCleaned->size());
#endif

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL_PROCESS
		char titleFINAL[1000];
		strcpy_s(titleFINAL, "FINAL MATCHES After MatchsProcessor");

		strcat_s(titleFINAL, " ");
		//char sNumCnt[20];
		_itoa_s(m_DisplayCnt, sNumCnt, sizeof(sNumCnt), 10);
		strcat_s(titleFINAL, sNumCnt);

		//DisplayMatches matchDisplayer;
		matchDisplayer.setSeqDisplay(false); ///// bORAAAAAAAAAAAAAAAAAAAA
		matchDisplayer.displayMatchesProcessor(m_imgRef, m_imgTest, m_matchesCombinedCleaned,
			m_keypointsRefCombinedCleaned, m_keypointsTestCombinedCleaned, titleFINAL);
#endif
	}

	// delete keypoints that are very close to each other.
	// select only one of them.
	// while selecting, try to select the one with big response.
	// by this way, their influences in the calculations are removed
	void eliminateSmallResponseRecurringKeypoints()
	{
		std::vector<int> outlierIndices;
		std::vector<bool> shallDeleteIndex;
		cv::Point2f Point1, Point2;
		float dResponse1, dResponse2;

		bool isBreak = false;

		for (std::vector<cv::DMatch>::iterator matchIterator = m_matchesCombinedCleaned.begin();
			matchIterator != m_matchesCombinedCleaned.end();
			++matchIterator)
		{
			shallDeleteIndex.push_back(false);

		}

		int i = 0;
		int j = 0;
		int k = 0;

#ifdef PRINT_DEBUG_FP
		printf("\n");
#endif

		float aThreshold;
		if (m_ThreshSelect == 0)
		{
			aThreshold = (float)RECURRING_THRESHOLD_0;
		}
		else
		{
			aThreshold = (float)RECURRING_THRESHOLD_1;
		}

		for (std::vector<cv::DMatch>::iterator matchIterator = m_matchesCombinedCleaned.begin();
			matchIterator != m_matchesCombinedCleaned.end();
			++matchIterator)
		{
#ifdef PRINT_DEBUG_FP
			if (shallDeleteIndex[i] == false)
				printf("\nI %d KEPT", i);
			else 
				printf("\nI %d DELETED", i);
#endif

			isBreak = false;
			if (shallDeleteIndex[i] == false)
			{

				Point1 = m_keypointsRefCombinedCleaned[matchIterator->trainIdx].pt;
				dResponse1 = m_keypointsRefCombinedCleaned[matchIterator->trainIdx].response;
				j = 0;

				for (std::vector<cv::DMatch>::iterator matchIterator1 = matchIterator + 1;
					matchIterator1 != m_matchesCombinedCleaned.end();
					++matchIterator1)
				{
					Point2 = m_keypointsRefCombinedCleaned[matchIterator1->trainIdx].pt;
					dResponse2 = m_keypointsRefCombinedCleaned[matchIterator1->trainIdx].response;

					//printf("\n KP1 X %.1f  Y %.1f KP2 X %.1f Y %.1f RESP1 %f RESP2 %f", Point1.x, Point1.y, Point2.x, Point2.y, dResponse1, dResponse2);
					if (((float)fabs(Point1.x - Point2.x) <= aThreshold) &&
						((float)fabs(Point1.y - Point2.y) <= aThreshold) &&
						(dResponse1 >= dResponse2))
					{
#ifdef PRINT_DEBUG_FP
						printf("\n%d %d  DELETING NEXT KP1 X %.1f  Y %.1f KP2 X %.1f Y %.1f RESP1 %f RESP2 %f", i, i + j + 1, Point1.x, Point1.y, Point2.x, Point2.y, dResponse1, dResponse2);
#endif
						shallDeleteIndex[i + j + 1] = true;
						k++;

						//printf("J %d ",j);

						//isBreak = true;
					}
					else if (((float)fabs(Point1.x - Point2.x) <= aThreshold) &&
							((float)fabs(Point1.y - Point2.y) <= aThreshold) &&
							(dResponse1 <= dResponse2))
					{
#ifdef PRINT_DEBUG_FP
						printf("\n%d %d  DELETING CURRENT KP1 X %.1f  Y %.1f KP2 X %.1f Y %.1f RESP1 %f RESP2 %f", i, i + j + 1, Point1.x, Point1.y, Point2.x, Point2.y, dResponse1, dResponse2);
#endif
						shallDeleteIndex[i] = true;
						k++;

						//printf("J %d ",j);

						isBreak = true;
					}
					//printf("%d ",j);
					j++;

					if (isBreak)
					{
						goto BORA_BREAK;
					}
				}

			}

			BORA_BREAK:
				i++;

		}

		//printf("\n For Reference KPs Found %d Same Points\n", k);

		i = 0;
		j = 0;
		k = 0;

		std::vector<cv::DMatch> matchesCombinedCleanedTmp;
		std::vector<cv::KeyPoint> keypointsRefCombinedCleanedTmp;
		std::vector<cv::KeyPoint> keypointsTestCombinedCleanedTmp;

		i = 0;
		j = 0;
		k = 0;

#ifdef  DISPLAY_PRINTS_DEBUG
		printf("\n");
#endif
		for (std::vector<cv::DMatch>::iterator matchIterator = m_matchesCombinedCleaned.begin();
			matchIterator != m_matchesCombinedCleaned.end();
			++matchIterator)
		{
			if (shallDeleteIndex[i] == false)
			{
				matchesCombinedCleanedTmp.push_back(cv::DMatch(j, j,
					(*matchIterator).distance));

				keypointsRefCombinedCleanedTmp.push_back(m_keypointsRefCombinedCleaned[matchIterator->trainIdx]);
				keypointsTestCombinedCleanedTmp.push_back(m_keypointsTestCombinedCleaned[matchIterator->queryIdx]);

				j++;
			}
			else
			{
				k++;
			}
			i++;
		}

#ifdef  DISPLAY_PRINTS_DEBUG
		printf("\nDeleted %d Points in FINAL Response-Recurrence Processing!!!!", k);
		printf("\nRemaining %d Points in FINAL Response-Recurrence Processing!!!!\n", j);

#endif

		m_matchesCombinedCleaned.erase(m_matchesCombinedCleaned.begin(), m_matchesCombinedCleaned.end());
		m_keypointsRefCombinedCleaned.erase(m_keypointsRefCombinedCleaned.begin(), m_keypointsRefCombinedCleaned.end());
		m_keypointsTestCombinedCleaned.erase(m_keypointsTestCombinedCleaned.begin(), m_keypointsTestCombinedCleaned.end());

		m_matchesCombinedCleaned = matchesCombinedCleanedTmp;
		m_keypointsRefCombinedCleaned = keypointsRefCombinedCleanedTmp;
		m_keypointsTestCombinedCleaned = keypointsTestCombinedCleanedTmp;


#ifdef  DISPLAY_IMAGES_DEBUG_FINAL_PROCESS
		char title1[1000];
		strcpy_s(title1, "MATCHES After FINAL Response-Recurrence Processing ");
		char sNum[20];
		//itoa((m_count - 1), sNum, 10);
		_itoa_s(m_DisplayCnt, sNum, sizeof(sNum), 10);
		strcat_s(title1, sNum);
		DisplayMatches matchDisplayer;
		matchDisplayer.setSeqDisplay(false); ///// bORAAAAAAAAAAAAAAAAAAAA
		matchDisplayer.displayMatchesProcessor(m_imgRef, m_imgTest, m_matchesCombinedCleaned,
			m_keypointsRefCombinedCleaned, m_keypointsTestCombinedCleaned, title1);
#endif

	}

	void findExtremumPoints()
	{
		std::vector<int> outlierIndices;
		std::vector<bool> shallDeleteIndex;
		float dYPoint;

		int i = 0;
		int j = 0;
		int k = 0;

		std::vector<std::pair<int, float>> keyPointRefPairVect;
		std::vector<std::pair<int, float>> keyPointTestPairVect;
		std::pair<int, float> tmpPair;

		for (std::vector<cv::DMatch>::iterator matchIterator = m_matchesCombinedCleaned.begin();
			matchIterator != m_matchesCombinedCleaned.end();
			++matchIterator)
		{
			shallDeleteIndex.push_back(true);
		}

		for (std::vector<cv::DMatch>::iterator matchIterator = m_matchesCombinedCleaned.begin();
			matchIterator != m_matchesCombinedCleaned.end();
			++matchIterator)
		{
			//printf("\n Index %d LOOP_CNT %d", matchIterator->trainIdx, m_matchesCombinedCleaned.size());
			assert(matchIterator->trainIdx < (int) m_matchesCombinedCleaned.size() + 1);
			dYPoint = m_keypointsRefCombinedCleaned[matchIterator->trainIdx].pt.y;
			tmpPair.first = i;
			tmpPair.second = dYPoint;
			keyPointRefPairVect.push_back(tmpPair);
			i++;
		}

		// sort function
		// sort in decreasing order
		struct sort_pred
		{
			bool operator()(const std::pair<int, float> &left, const std::pair<int, float> &right)
			{
				return left.second > right.second;
			}
		};

		std::sort(keyPointRefPairVect.begin(), keyPointRefPairVect.end(), sort_pred());
		//std::sort(keyPointTestPairVect.begin(), keyPointTestPairVect.end(), sort_pred());


		std::vector<cv::DMatch> matchesCombinedCleanedTmp;
		std::vector<cv::KeyPoint> keypointsRefCombinedCleanedTmp;
		std::vector<cv::KeyPoint> keypointsTestCombinedCleanedTmp;

		std::vector<cv::DMatch>::iterator matchIterator = m_matchesCombinedCleaned.begin();
		tmpPair = keyPointRefPairVect[0];
		matchIterator += tmpPair.first;
		matchesCombinedCleanedTmp.push_back(cv::DMatch(0, 0, 0.0));
		keypointsRefCombinedCleanedTmp.push_back(m_keypointsRefCombinedCleaned[matchIterator->trainIdx]);
		keypointsTestCombinedCleanedTmp.push_back(m_keypointsTestCombinedCleaned[matchIterator->queryIdx]);

		matchIterator = m_matchesCombinedCleaned.begin();
		tmpPair = keyPointRefPairVect[keyPointRefPairVect.size() - 1];
		matchIterator += tmpPair.first;
		matchesCombinedCleanedTmp.push_back(cv::DMatch(1, 1, 0.0));
		keypointsRefCombinedCleanedTmp.push_back(m_keypointsRefCombinedCleaned[matchIterator->trainIdx]);
		keypointsTestCombinedCleanedTmp.push_back(m_keypointsTestCombinedCleaned[matchIterator->queryIdx]);

		matchIterator = m_matchesCombinedCleaned.begin();
		int dMedian = (int)((keyPointRefPairVect.size() / 2) + 0.5);
		tmpPair = keyPointRefPairVect[dMedian];
		matchIterator += tmpPair.first;
		matchesCombinedCleanedTmp.push_back(cv::DMatch(2, 2, 0.0));
		keypointsRefCombinedCleanedTmp.push_back(m_keypointsRefCombinedCleaned[matchIterator->trainIdx]);
		keypointsTestCombinedCleanedTmp.push_back(m_keypointsTestCombinedCleaned[matchIterator->queryIdx]);


		m_matchesCombinedCleaned.erase(m_matchesCombinedCleaned.begin(), m_matchesCombinedCleaned.end());
		m_keypointsRefCombinedCleaned.erase(m_keypointsRefCombinedCleaned.begin(), m_keypointsRefCombinedCleaned.end());
		m_keypointsTestCombinedCleaned.erase(m_keypointsTestCombinedCleaned.begin(), m_keypointsTestCombinedCleaned.end());

		m_matchesCombinedCleaned = matchesCombinedCleanedTmp;
		m_keypointsRefCombinedCleaned = keypointsRefCombinedCleanedTmp;
		m_keypointsTestCombinedCleaned = keypointsTestCombinedCleanedTmp;


#ifdef  DISPLAY_PRINTS_DEBUG
		//k = 0;
		//for (std::vector<cv::DMatch>::iterator matchIterator = matchesCombinedCleanedTmp.begin();
		//	matchIterator != matchesCombinedCleanedTmp.end();
		//	++matchIterator)
		//{
		//	printf("\n KP1 %d X %.5f  Y %.5f", k, keypointsRefCombinedCleanedTmp[k].pt.x, keypointsRefCombinedCleanedTmp[k].pt.y);
		//	k++;
		//}
		//k = 0;
		//for (std::vector<cv::DMatch>::iterator matchIterator = matchesCombinedCleanedTmp.begin();
		//	matchIterator != matchesCombinedCleanedTmp.end();
		//	++matchIterator)
		//{
		//	printf("\n KP2 %d X %.5f  Y %.5f", k, keypointsTestCombinedCleanedTmp[k].pt.x, keypointsTestCombinedCleanedTmp[k].pt.y);
		//	k++;
		//}
#endif

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL
		char title1[1000];
		strcpy_s(title1, "MATCHES After RESPONSE Processing ");
		char sNum[20];
		_itoa_s((m_count - 1), sNum, sizeof(sNum), 10);
		strcat_s(title1, sNum);
		DisplayMatches matchDisplayer;
		matchDisplayer.setSeqDisplay(false); ///// bORAAAAAAAAAAAAAAAAAAAA
		matchDisplayer.displayMatchesProcessor(m_imgRef, m_imgTest, m_matchesCombinedCleaned,
			m_keypointsRefCombinedCleaned, m_keypointsTestCombinedCleaned, title1);
#endif
	}

	// function calculating the slope of a line.
	// this line joins a RefKeypoint and TestKeypoint
	double calculateSlope(cv::Point2f Point1, cv::Point2f Point2, cv::Point2f offset)
	{
		double slope = 1.0;
		double xDiff = 0.0;
		double yDiff = 0;

		cv::Point2f point2Offset = Point2 + offset;

		xDiff = point2Offset.x - Point1.x;
		yDiff = point2Offset.y - Point1.y;

		if (xDiff != 0)
		{
			slope = yDiff / xDiff;
		}
		else
		{
			throw std::invalid_argument("calculated undefine slope");
		}

		return slope;
	}

	// Function Calculating Slopes of a whole given set of matches
	// Then calls another function to refine the calculated Mean and StandartDeviation
	void CalculateSlopeMeanAndStdDev(std::vector<cv::DMatch> matches,
		std::vector<cv::KeyPoint> keypoints1,
		std::vector<cv::KeyPoint> keypoints2,
		cv::Point2f offset,
		double* aMean,
		double* aStdDev)
	{
		std::vector<double> slopes;
		std::vector<int> outlierIndices;
		cv::Point2f Point1, Point2;
		float slope;

		int i = 0;

		for (std::vector<cv::DMatch>::iterator matchIterator = matches.begin();
			matchIterator != matches.end();
			++matchIterator)
		{
			Point1 = keypoints1[matchIterator->trainIdx].pt;
			Point2 = keypoints2[matchIterator->queryIdx].pt;

			try
			{
				slope = (float)calculateSlope(Point1, Point2, offset);
				//std::printf("\nFeature %d Slope %.5f", i, slope);
			}
			catch (const std::invalid_argument& e)
			{
				// do stuff with exception... 
				std::printf("\nException in slope calculation\n");
			}

			slopes.push_back(slope);

			i++;
		}

		eliminateOutlierSlopesForCalculation(slopes, matches, keypoints1, keypoints2, aMean, aStdDev);
	}

	// this function eliminates keypoints and matches
	// if their slopes are different then Mean +/- StdDev*Multiplier
	void eliminateOutlierSlopesForCalculation(std::vector<double> inputSlopes,
		std::vector<cv::DMatch> initialMatches,
		std::vector<cv::KeyPoint> keypoints1,
		std::vector<cv::KeyPoint> keypoints2,
		double* aMean,
		double* aStdDev)
	{
		// calculate initial mean value of slopes
		double sum = std::accumulate(inputSlopes.begin(), inputSlopes.end(), 0.0);
		double fMean = sum / inputSlopes.size();
		// end of mean calculation

		// calculate initial standart deviation value of slopes
		std::vector<double> diff(inputSlopes.size());
		std::transform(inputSlopes.begin(), inputSlopes.end(), diff.begin(),
			std::bind2nd(std::minus<double>(), fMean));

		double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		double fStDev = std::sqrt(sq_sum / inputSlopes.size());
		// end of standart deviation calculation

#ifdef DISPLAY_PRINTS_DEBUG
		std::printf("\nstarting Slope MEAN %.5f STDEV %.5f", fMean, fStDev);
#endif

		// create temporary variables
		std::vector<cv::DMatch> matchesCombinedCleaned;
		std::vector<cv::KeyPoint> keypointsRefCombinedCleaned;
		std::vector<cv::KeyPoint> keypointsTestCombinedCleaned;
		std::vector<double> inputSlopes1;


		std::vector<cv::DMatch>::iterator matchIterator = initialMatches.begin();
		int i = 0;
		int k = 0;

		double aThrsh;
		
		if (m_loopCounter == 0)
			aThrsh = SLOPE_STD_MULTIPLIER_1 * fStDev;
		else
			aThrsh = SLOPE_STD_MULTIPLIER_2 * fStDev;

		for (std::vector<double>::iterator slopeIterator = inputSlopes.begin();
			slopeIterator != inputSlopes.end();
			++slopeIterator)
		{
			double dDiff = fabs(inputSlopes[i] - fMean);

#ifdef DISPLAY_PRINTS_DEBUG
			//std::printf("\nFEATURE %d SLOPE %.5f DIFFERENCE %.5f THRESHOLd %.5f", i, inputSlopes[i], dDiff, aThrsh);
#endif
			if (dDiff <= aThrsh)
			{

				matchesCombinedCleaned.push_back(cv::DMatch(k, k,
					(*matchIterator).distance));

				keypointsRefCombinedCleaned.push_back(keypoints1[initialMatches[i].trainIdx]);
				keypointsTestCombinedCleaned.push_back(keypoints2[initialMatches[i].queryIdx]);
				inputSlopes1.push_back(*slopeIterator);
				k++;
			}
			i++;
		}

#ifdef DISPLAY_PRINTS_DEBUG
		std::printf("\n%d Features will be used for SLOPE Calculation", k);
#endif

#ifdef  DISPLAY_IMAGES_DEBUG_FINAL
		char title1[1000];
		strcpy_s(title1, "MATCHES in SURF-SIFT Slope Processing First Pass");

		strcat_s(title1, " ");
		char sNumCnt[20];
		_itoa_s(m_DspCnt, sNumCnt, sizeof(sNumCnt), 10);
		strcat_s(title1, sNumCnt);

		DisplayMatches matchDisplayer;
		matchDisplayer.setSeqDisplay(false); // set true if what to display one by one
		matchDisplayer.displayMatchesProcessor(m_imgRef, m_imgTest, matchesCombinedCleaned,
			keypointsRefCombinedCleaned, keypointsTestCombinedCleaned, title1);
#endif

		m_matchesCombinedCleaned.erase(m_matchesCombinedCleaned.begin(), m_matchesCombinedCleaned.end());
		m_keypointsRefCombinedCleaned.erase(m_keypointsRefCombinedCleaned.begin(), m_keypointsRefCombinedCleaned.end());
		m_keypointsTestCombinedCleaned.erase(m_keypointsTestCombinedCleaned.begin(), m_keypointsTestCombinedCleaned.end());

		m_matchesCombinedCleaned = matchesCombinedCleaned;
		m_keypointsRefCombinedCleaned = keypointsRefCombinedCleaned;
		m_keypointsTestCombinedCleaned = keypointsTestCombinedCleaned;

//		// calculate the refined mean value of slopes
//		double sum1 = std::accumulate(inputSlopes1.begin(), inputSlopes1.end(), 0.0);
//		double fMean1 = sum1 / inputSlopes1.size();
//		// end of mean calculation
//
//		// calculate the refined standart deviation value of slopes
//		std::vector<double> diff1(inputSlopes1.size());
//		std::transform(inputSlopes1.begin(), inputSlopes1.end(), diff1.begin(),
//			std::bind2nd(std::minus<double>(), fMean1));
//
//		double sq_sum1 = std::inner_product(diff1.begin(), diff1.end(), diff1.begin(), 0.0);
//		double fStDev1 = std::sqrt(sq_sum1 / inputSlopes1.size());
//		// end of standart deviation calculation
//
//		*aMean = fMean1;
//		*aStdDev = fStDev1;
//
//#ifdef DISPLAY_PRINTS_DEBUG
//		std::printf("\nFINAL Slope MEAN %.5f STDEV %.5f", fMean1, fStDev1);
//#endif
	}

};