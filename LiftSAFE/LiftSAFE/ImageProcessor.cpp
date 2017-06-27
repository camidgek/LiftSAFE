//------------------------------------------------------------------------------
// <copyright file="ImageProcesser.cpp" company="Wentworth Institute of Technology">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "ImageProcessor.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

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

	cv::imshow("window", irImage);

	//int size = irImage.total() * irImage.elemSize();
	//BYTE* bytes = new BYTE[size];  // you will have to delete[] that later
	//std::memcpy(bytes, irImage.data, size * sizeof(byte));
	//pImage = bytes;
}