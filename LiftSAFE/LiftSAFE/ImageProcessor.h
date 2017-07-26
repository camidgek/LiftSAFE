//------------------------------------------------------------------------------
// <copyright file="ImageProcessing.h" company="Wentworth">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Manages the opencv processing of an image

#pragma once

#include "resource.h"

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

private:
	cv::Mat						m_mIrImage;
	cv::Mat						m_mThreshImage;
	int							m_nHeight;
	int							m_nWidth;
	int							m_nFrame;
	int							m_nKneeThresh;
	bool						m_bInBalance;
	bool						m_bFault;
	std::vector<cv::Point2f>	m_vBarPointsLeft;
	std::vector<cv::Point2f>	m_vBarPointsRight;
	std::vector<cv::Point2f>	m_vKneePointsLeft;
	std::vector<cv::Point2f>	m_vKneePointsRight;
	std::vector<bool>			m_vBarBalanceValues;

	std::vector<cv::Point2f> get_positions(cv::Mat& image);

	bool check_for_fault_bar_balance();
};