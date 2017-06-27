//------------------------------------------------------------------------------
// <copyright file="ImageProcesser.cpp" company="Wentworth Institute of Technology">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "ImageProcessor.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

/// <summary>
/// Constructor
/// </summary>
ImageProcessor::ImageProcessor()
{
	nFrame = 0;
}

/// <summary>
/// Destructor
/// </summary>
ImageProcessor::~ImageProcessor()
{
}

/// <summary>
/// OPENCV STUFF
/// </summary>
void ImageProcessor::ProcessImage(BYTE* pImage)
{
	cv::Mat irImage = cv::Mat(cvSize(512, 424), CV_8UC4, pImage).clone();	
	cv::Mat grayImage, threshImage, temp;

	cv::cvtColor(irImage, temp, cv::COLOR_BGRA2BGR);
	cv::cvtColor(temp, grayImage, cv::COLOR_BGR2GRAY);

	cv::threshold(grayImage, threshImage, 200, 255, cv::THRESH_BINARY);

	// Morphological opening (removes small objects from the foreground)
	cv::erode(threshImage, threshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::dilate(threshImage, threshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

	// Morphological closing (removes small holes from the foreground)
	cv::dilate(threshImage, threshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
	cv::erode(threshImage, threshImage, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

	cv::Mat leftFrame = threshImage(cv::Rect(0, 0, threshImage.cols / 2, threshImage.rows));

	cv::Moments leftMoments = cv::moments(leftFrame);
	double dY = leftMoments.m01;
	double dX = leftMoments.m10;
	double dArea = leftMoments.m00;
	int posX = dX / dArea;
	int posY = dY / dArea;
	
	cv::putText(threshImage,				// Input Image
				"Test",						// Text
				cv::Point(posX, posY),		// Position
				cv::FONT_HERSHEY_SIMPLEX,	// Font
				1.0,						// Scale
				cv::Scalar(255, 255, 255)); // Color

	cv::imshow("window", threshImage);
}