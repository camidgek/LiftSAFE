//------------------------------------------------------------------------------
// <copyright file="ImageProcessing.h" company="Wentworth">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Manages the opencv processing of an image

#pragma once

#include "resource.h"

#include <opencv2/opencv.hpp>

class CUser
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	CUser();

	/// <summary>
	/// Destructor
	/// </summary>
	~CUser();

	// Getter functions
	cv::Point getBarReferencePoints() { return m_pBarRefPoints; };
	cv::Point getKneeReferencePoints() { return m_pKneeRefPoints; };

private:
	// User data
	std::string					m_sFirstName;
	std::string					m_sLastName;
	std::string					m_sFullName;
	int							m_nWeight = 170; //lb
	int							m_nHeight = 72;	 //in

	// Reference points
	cv::Point					m_pBarRefPoints = cv::Point(0,0);
	cv::Point					m_pKneeRefPoints = cv::Point(0,0);

};