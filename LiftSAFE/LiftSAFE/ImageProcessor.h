//------------------------------------------------------------------------------
// <copyright file="ImageProcessing.h" company="Wentworth">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// Manages the opencv processing of an image

#pragma once

#include <opencv2/opencv.hpp>

class ImageProcessor
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	ImageProcessor();

	/// <summary>
	/// Destructor
	/// </summary>
	virtual ~ImageProcessor();

	/// <summary>
	/// OPENCV STUFF
	/// </summary>
	void ProcessImage(BYTE* pImage);

private:
	int nFrame;
};