//------------------------------------------------------------------------------
// <copyright file="ImageProcesser.cpp" company="Wentworth Institute of Technology">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "ImageProcessor.h"

/// <summary>
/// Constructor
/// </summary>
CImageProcessor::CImageProcessor() :
	m_mIrImage(),
	m_mThreshImage(),
	m_nWidth(0),
	m_nHeight(0),
	m_nFrame(0),
	m_vBarPointsLeft(),
	m_vBarPointsRight(),
	m_vBarBalanceValues(),
	m_bFault(0),
	m_bInBalance(0),
	m_nKneeThresh(45),
	m_bInThreshBar(0),
	m_bInThreshKnee(0),
	m_sFaultMessage("")
{

}

/// <summary>
/// Destructor
/// </summary>
CImageProcessor::~CImageProcessor()
{
}

/// <summary>
/// OPENCV STUFF
/// </summary>
void CImageProcessor::ProcessImage(BYTE* pImage, SerialPort* pSerialPort)
{
	// Clone the passed BYTE image into Mat format
	m_mIrImage = cv::Mat(cvSize(512, 424), CV_8UC4, pImage).clone();

	// Get height and width of image
	m_nWidth = m_mIrImage.cols; m_nHeight = m_mIrImage.rows;

	// Color correct to B&W (shouldn't actually be necessary)
	cv::Mat temp, grayImage;
	cv::cvtColor(m_mIrImage, temp, cv::COLOR_BGRA2BGR);
	cv::cvtColor(temp, grayImage, cv::COLOR_BGR2GRAY);

	// Apply Threshold so only the reflective tape is white, rest is black
	cv::threshold(grayImage, m_mThreshImage, 200, 255, cv::THRESH_BINARY);

	// Morphological opening (removes small objects from the foreground)
	cv::erode(m_mThreshImage, m_mThreshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::dilate(m_mThreshImage, m_mThreshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

	// Morphological closing (removes small holes from the foreground)
	cv::dilate(m_mThreshImage, m_mThreshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::erode(m_mThreshImage, m_mThreshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

	// Retrieve a vector of points with the (x,y) location of the objects
	std::vector<cv::Point2f> points = get_positions(m_mThreshImage);

	// For each point, assign to vector based on location
	for (unsigned int i = 0; i < points.size(); i++)
	{
		// Left Half
		if (points[i].x < (m_nWidth / 2))
		{	// Top Left Quarter
			if (points[i].y < (m_nHeight / 2))
			{
				if (m_vBarPointsLeft.size() == m_nFrame)
					m_vBarPointsLeft.push_back(points[i]);
			}
			// Bottom Left Quarter
			else
			{
				if (m_vKneePointsLeft.size() == m_nFrame)
					m_vKneePointsLeft.push_back(points[i]);
			}
		}
		// Right Half
		else
		{	// Top Right Quarter
			if (points[i].y < (m_nHeight / 2))
			{
				if (m_vBarPointsRight.size() == m_nFrame)
					m_vBarPointsRight.push_back(points[i]);
			}
			// Bottom Right Quarter
			else
			{
				if (m_vKneePointsRight.size() == m_nFrame)
					m_vKneePointsRight.push_back(points[i]);
			}
		}
	}

	// Check for, and fill in, zeros if no points were added in previous step
	if (m_vBarPointsLeft.size() < m_nFrame + 1)
	{
		m_vBarPointsLeft.push_back(cv::Point2f(0, 0));
	}
	if (m_vBarPointsRight.size() < m_nFrame + 1)
	{
		m_vBarPointsRight.push_back(cv::Point2f(0, 0));
	}
	if (m_vKneePointsLeft.size() < m_nFrame + 1)
	{
		m_vKneePointsLeft.push_back(cv::Point2f(0, 0));
	}
	if (m_vKneePointsRight.size() < m_nFrame + 1)
	{
		m_vKneePointsRight.push_back(cv::Point2f(0, 0));
	}

	// Calculate angle of two points from the vectors **MOVE INTO OWN FUNCTION**
	int adjacent = m_vBarPointsRight[m_nFrame].x - m_vBarPointsLeft[m_nFrame].x;
	int opposite = m_vBarPointsRight[m_nFrame].y - m_vBarPointsLeft[m_nFrame].y;
	float radians = atan2(opposite,adjacent);
	float angle = abs((180 * radians) / PI);

	// Calculate knee distance
	int distance = abs(m_vKneePointsLeft[m_nFrame].x - m_vKneePointsRight[m_nFrame].x);

	if (distance < m_nKneeThresh)
	{
		m_bInThreshKnee = 0;
	}
	else
	{
		m_bInThreshKnee = 1;
	}
	m_vKneeThreshValues.push_back(m_bInThreshKnee);

	// Check calculated angle against threshold and add to own vector
	if (angle > 10)
	{
		m_bInBalance = 0;
	}
	else
	{
		m_bInBalance = 1;
	}
	m_vBarBalanceValues.push_back(m_bInBalance);

	// Get values from Arduino
	read_result = pSerialPort->readSerialPort(incomingData, MAX_DATA_LENGTH);
	str = incomingData;
	std::stringstream str_stream(str);
	std::getline(str_stream, new_str, '\r');
	value = atoi(new_str.c_str());

	// Check calculated angle against threshold and add to own vector
	if (value < 80)
	{
		m_bInThreshBar = 0;
	}
	else
	{
		m_bInThreshBar = 1;
	}
	m_vBackThreshValues.push_back(m_bInThreshBar);

	// Check last 5 frames for balace fault
	m_bFault = check_for_fault();

	/* BELOW IS ALL VISUAL */
	// Convert variable to string for visual purposes
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << angle;
	std::string strAngle = stream.str();
	std::stringstream stream2;
	stream2 << std::fixed << std::setprecision(2) << distance;
	std::string strDistance = stream2.str();
	
	// Display Back Value
	cv::putText(m_mIrImage,									// Input Image
		"Back Value: " + new_str,							// Text
		cv::Point2f(30, 30),								// Position
		cv::FONT_HERSHEY_COMPLEX_SMALL,						// Font
		1,													// Scale
		cv::Scalar(0, 0, 255));								// Color

	// Display Bar Value
	cv::putText(m_mIrImage,									// Input Image
		"Bar Angle: " + strAngle,							// Text
		cv::Point2f(30,60),									// Position
		cv::FONT_HERSHEY_COMPLEX_SMALL,						// Font
		1,													// Scale
		cv::Scalar(0, 0, 255));								// Color

	// Display Knee Distance
	cv::putText(m_mIrImage,									// Input Image
		"Knee Dist: " + strDistance,						// Text
		cv::Point2f(30,90),									// Position
		cv::FONT_HERSHEY_COMPLEX_SMALL,						// Font
		1,													// Scale
		cv::Scalar(0, 0, 255));								// Color

	// Draw a small red circle at those locations
	for (unsigned int i = 0; i < points.size(); i++)
	{
		// Dirty convert point to string
		int strX = points[i].x;
		int strY = points[i].y;
		std::string final = "(" + std::to_string(strX) + "," + std::to_string(strY) + ")";

		// Overlay coords on image
		cv::putText(m_mIrImage,											// Input Image
					final,	// Text
					points[i],											// Position
					cv::FONT_HERSHEY_COMPLEX_SMALL,						// Font
					1,													// Scale
					cv::Scalar(0, 0, 255));								// Color
		
		// Throw some balls on there too
		cv::circle(m_mIrImage, points[i], 3, cv::Scalar(0, 0, 255), -1);
	}

	cv::Mat faultImage(cvSize(512, 424), CV_8UC4, cv::Scalar(0, 255, 0));

	if (m_bFault)
	{
		faultImage.setTo(cv::Scalar(0, 0, 255));
		// Display fault status on screen
		cv::putText(faultImage,									// Input Image
			"Section: " + m_sFaultMessage,						// Text
			cv::Point2f(30, 30),								// Position
			cv::FONT_HERSHEY_COMPLEX_SMALL,						// Font
			1,													// Scale
			cv::Scalar(0, 0, 0));								// Color
	}
	m_nFrame++;

	cv::imshow("window", m_mIrImage);
	cv::imshow("window2", faultImage);
}

std::vector<cv::Point2f> CImageProcessor::get_positions(cv::Mat& pImage)
{
	if (pImage.channels() > 1)
	{
		return std::vector<cv::Point2f>();
	}

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(pImage, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	std::vector<cv::Point2f> center(contours.size());

	cv::Moments moment;
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		moment = cv::moments(contours[i]);
		if (moment.m00 < 500 && moment.m00 > 10)
		{
			unsigned int x = (double(moment.m10) / double(moment.m00));
			unsigned int y = (double(moment.m01) / double(moment.m00));
			center[i] = cv::Point2f(x, y);
		}
	}

	return center;
}

bool CImageProcessor::check_for_fault()
{
	int section = 0;
	bool balanced = 0;
	switch (section)
	{
		// Back
		case 0:
			if (m_vBackThreshValues.size() < 6)
				return 0;
			else if (m_bFault == 1)
				return 1;
			else
			{
				for (unsigned int i = m_nFrame; i > (m_nFrame - 5); i--)
				{
					if (m_vBackThreshValues[i] == 1)
						balanced = 1;
				}
				if (balanced == 0)
				{
					m_sFaultMessage = "Back ";
					return 1;
				}
			}
		// Bar
		case 1:
			balanced = 0;
			if (m_vBarBalanceValues.size() < 6)
				return 0;
			else if (m_bFault == 1)
				return 1;
			else
			{
				for (unsigned int i = m_nFrame; i > (m_nFrame - 5); i--)
				{
					if (m_vBarBalanceValues[i] == 1)
						balanced = 1;
				}
				if (balanced == 0)
				{
					m_sFaultMessage = "Bar ";
					return 1;
				}
			}
		// Knees
		case 2:
			balanced = 0;
			if (m_vKneeThreshValues.size() < 6)
				return 0;
			else if (m_bFault == 1)
				return 1;
			else
			{
				for (unsigned int i = m_nFrame; i > (m_nFrame - 5); i--)
				{
					if (m_vKneeThreshValues[i] == 1)
						balanced = 1;
				}
				if (balanced == 0)
				{
					m_sFaultMessage = "Knees ";
					return 1;
				}
			}
	}
	return 0;
}
