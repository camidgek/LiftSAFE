#pragma once

#include "stdafx.h"
#include "resource.h"

#include "ImageGrabber.h"
#include "ImageProcessor.h"
#include "User.h"
#include "SerialPort.h"

class CApplication
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	CApplication();

	/// <summary>
	/// Destructor
	/// </summary>
	~CApplication();

	int CApplication::Run(HINSTANCE hInstance, int nCmdShow);

private:
	
};