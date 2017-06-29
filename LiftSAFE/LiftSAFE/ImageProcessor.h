//------------------------------------------------------------------------------
// <copyright file="ImageProcessing.h" company="Wentworth">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Manages the opencv processing of an image

#pragma once

#include <opencv2/opencv.hpp>

class CImageProcessor
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	CImageProcessor();

	/// <summary>
	/// Destructor
	/// </summary>
	~CImageProcessor();

	/// <summary>
	/// OPENCV STUFF
	/// </summary>
	void ProcessImage(BYTE* pImage);

	std::vector<cv::Point2f> get_positions(cv::Mat& image);

private:
	cv::Mat						m_mIrImage;
	cv::Mat						m_mThreshImage;
	int							m_nHeight;
	int							m_nWidth;
	int							m_nFrame;
	std::vector<cv::Point2f>	m_vCenterPoints;
};