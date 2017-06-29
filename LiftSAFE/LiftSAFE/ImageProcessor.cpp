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
	m_nHeight(0)
{
	m_nFrame = 0;
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
void CImageProcessor::ProcessImage(BYTE* pImage)
{
	m_nFrame++;

	m_mIrImage = cv::Mat(cvSize(512, 424), CV_8UC4, pImage).clone();
	
	m_nWidth = m_mIrImage.cols; m_nHeight = m_mIrImage.rows;

	cv::Mat temp, grayImage;
	cv::cvtColor(m_mIrImage, temp, cv::COLOR_BGRA2BGR);
	cv::cvtColor(temp, grayImage, cv::COLOR_BGR2GRAY);

	cv::threshold(grayImage, m_mThreshImage, 200, 255, cv::THRESH_BINARY);

	// Morphological opening (removes small objects from the foreground)
	cv::erode(m_mThreshImage, m_mThreshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::dilate(m_mThreshImage, m_mThreshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

	// Morphological closing (removes small holes from the foreground)
	cv::dilate(m_mThreshImage, m_mThreshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::erode(m_mThreshImage, m_mThreshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

	// Retrieve a vector of points with the (x,y) location of the objects
	std::vector<cv::Point2f> points = get_positions(m_mThreshImage);

	// Draw a small green circle at those locations for educational purposes
	for (unsigned int i = 0; i < points.size(); i++)
	{
		// Dirty convert point to string
		int strX = points[i].x;
		int strY = points[i].y;
		std::string final = "(" + std::to_string(strX) + "," + std::to_string(strY) + ")";

		// Overlay point on image
		cv::putText(m_mIrImage,											// Input Image
					final,	// Text
					points[i],											// Position
					cv::FONT_HERSHEY_COMPLEX_SMALL,						// Font
					1,													// Scale
					cv::Scalar(0, 0, 255));								// Color

		// Throw some balls on there too
		cv::circle(m_mIrImage, points[i], 3, cv::Scalar(0, 0, 255), -1);
	}


	cv::imshow("window", m_mIrImage);
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
		unsigned int x = (double(moment.m10) / double(moment.m00));
		unsigned int y = (double(moment.m01) / double(moment.m00));
		center[i] = cv::Point2f(x, y);
	}

	return center;
}