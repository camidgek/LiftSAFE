//------------------------------------------------------------------------------
// <copyright file="ImageProcessing.h" company="Wentworth">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Manages the opencv processing of an image

#pragma once

#include "resource.h"
#include "SerialPort.h"

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
	void ProcessImage(BYTE* pImage, SerialPort* pSerialPort);

private:
	// Arduino Stuff
	char *port_name = "\\\\.\\COM3";
	char incomingData[MAX_DATA_LENGTH];
	int read_result = 0;
	std::string str;
	std::string new_str;
	int value;

	// Kinect Stuff
	cv::Mat						m_mIrImage;
	cv::Mat						m_mThreshImage;
	int							m_nHeight;
	int							m_nWidth;
	int							m_nFrame;
	int							m_nKneeThresh;
	bool						m_bInBalance;
	bool						m_bInThreshBar;
	bool						m_bInThreshKnee;
	bool						m_bFault;
	std::string					m_sFaultMessage;
	std::vector<cv::Point2f>	m_vBarPointsLeft;
	std::vector<cv::Point2f>	m_vBarPointsRight;
	std::vector<cv::Point2f>	m_vKneePointsLeft;
	std::vector<cv::Point2f>	m_vKneePointsRight;
	std::vector<bool>			m_vBarBalanceValues;
	std::vector<bool>			m_vBackThreshValues;
	std::vector<bool>			m_vKneeThreshValues;


	std::vector<cv::Point2f> get_positions(cv::Mat& image);

	bool check_for_fault();
};