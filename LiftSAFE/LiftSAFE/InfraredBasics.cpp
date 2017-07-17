//------------------------------------------------------------------------------
// <copyright file="InfraredBasics.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include <strsafe.h>
#include "resource.h"
#include "InfraredBasics.h"

/// <summary>
/// Entry point for the application
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="hPrevInstance">always 0</param>
/// <param name="lpCmdLine">command line arguments</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
/// <returns>status</returns>
int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CInfraredBasics application;
	application.Run(hInstance, nShowCmd);
}

/// <summary>
/// Constructor
/// </summary>
CInfraredBasics::CInfraredBasics() :
	m_hWnd(NULL),
	m_nStartTime(0),
	m_nLastCounter(0),
	m_nFramesSinceUpdate(0),
	m_fFreq(0),
	m_nNextStatusTime(0LL),
	m_bSaveScreenshot(false),
	m_pKinectSensor(NULL),
	m_pInfraredFrameReader(NULL),
	m_pD2DFactory(NULL),
	m_pInfraredRGBX(NULL),
	m_pImageProcessor(new CImageProcessor)
{
	LARGE_INTEGER qpf = { 0 };
	if (QueryPerformanceFrequency(&qpf))
	{
		m_fFreq = double(qpf.QuadPart);
	}

	// create heap storage for infrared pixel data in RGBX format
	m_pInfraredRGBX = new RGBQUAD[cInfraredWidth * cInfraredHeight];
}


/// <summary>
/// Destructor
/// </summary>
CInfraredBasics::~CInfraredBasics()
{
	if (m_pInfraredRGBX)
	{
		delete[] m_pInfraredRGBX;
		m_pInfraredRGBX = NULL;
	}

	// clean up Direct2D
	SafeRelease(m_pD2DFactory);

	// done with infrared frame reader
	SafeRelease(m_pInfraredFrameReader);

	// close the Kinect Sensor
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
int CInfraredBasics::Run(HINSTANCE hInstance, int nCmdShow)
{
	MSG       msg = { 0 };
	
	InitializeDefaultSensor();

	// Main message loop
	while (msg.message != WM_QUIT)
	{
		Update();

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return static_cast<int>(msg.wParam);
}

/// <summary>
/// Main processing function
/// </summary>
void CInfraredBasics::Update()
{
	if (!m_pInfraredFrameReader)
	{
		return;
	}

	IInfraredFrame* pInfraredFrame = NULL;

	HRESULT hr = m_pInfraredFrameReader->AcquireLatestFrame(&pInfraredFrame);

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		UINT nBufferSize = 0;
		UINT16 *pBuffer = NULL;

		hr = pInfraredFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pInfraredFrame->get_FrameDescription(&pFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pInfraredFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		}

		if (SUCCEEDED(hr))
		{
			ProcessInfrared(nTime, pBuffer, nWidth, nHeight);
		}

		SafeRelease(pFrameDescription);
	}

	SafeRelease(pInfraredFrame);
}

/// <summary>
/// Handle new infrared data
/// <param name="nTime">timestamp of frame</param>
/// <param name="pBuffer">pointer to frame data</param>
/// <param name="nWidth">width (in pixels) of input image data</param>
/// <param name="nHeight">height (in pixels) of input image data</param>
/// </summary>
void CInfraredBasics::ProcessInfrared(INT64 nTime, const UINT16* pBuffer, int nWidth, int nHeight)
{
	if (m_hWnd)
	{
		if (!m_nStartTime)
		{
			m_nStartTime = nTime;
		}

		double fps = 0.0;

		LARGE_INTEGER qpcNow = { 0 };
		if (m_fFreq)
		{
			if (QueryPerformanceCounter(&qpcNow))
			{
				if (m_nLastCounter)
				{
					m_nFramesSinceUpdate++;
					fps = m_fFreq * m_nFramesSinceUpdate / double(qpcNow.QuadPart - m_nLastCounter);
				}
			}
		}

		WCHAR szStatusMessage[64];
		StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L" FPS = %0.2f    Time = %I64d", fps, (nTime - m_nStartTime));

		if (SetStatusMessage(szStatusMessage, 1000, false))
		{
			m_nLastCounter = qpcNow.QuadPart;
			m_nFramesSinceUpdate = 0;
		}
	}

	if (m_pInfraredRGBX && pBuffer && (nWidth == cInfraredWidth) && (nHeight == cInfraredHeight))
	{
		RGBQUAD* pDest = m_pInfraredRGBX;

		// end pixel is start + width*height - 1
		const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

		while (pBuffer < pBufferEnd)
		{
			// normalize the incoming infrared data (ushort) to a float ranging from 
			// [InfraredOutputValueMinimum, InfraredOutputValueMaximum] by
			// 1. dividing the incoming value by the source maximum value
			float intensityRatio = static_cast<float>(*pBuffer) / InfraredSourceValueMaximum;

			// 2. dividing by the (average scene value * standard deviations)
			intensityRatio /= InfraredSceneValueAverage * InfraredSceneStandardDeviations;

			// 3. limiting the value to InfraredOutputValueMaximum
			intensityRatio = std::min(InfraredOutputValueMaximum, intensityRatio);

			// 4. limiting the lower value InfraredOutputValueMinimym
			intensityRatio = std::max(InfraredOutputValueMinimum, intensityRatio);

			// 5. converting the normalized value to a byte and using the result
			// as the RGB components required by the image
			byte intensity = static_cast<byte>(intensityRatio * 255.0f);
			pDest->rgbRed = intensity;
			pDest->rgbGreen = intensity;
			pDest->rgbBlue = intensity;

			++pDest;
			++pBuffer;
		}

		m_pImageProcessor->ProcessImage(reinterpret_cast<BYTE*>(m_pInfraredRGBX));

	}
}

/// <summary>
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT CInfraredBasics::InitializeDefaultSensor()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get the infrared reader
		IInfraredFrameSource* pInfraredFrameSource = NULL;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_InfraredFrameSource(&pInfraredFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pInfraredFrameSource->OpenReader(&m_pInfraredFrameReader);
		}

		SafeRelease(pInfraredFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		SetStatusMessage(L"No ready Kinect found!", 10000, true);
		return E_FAIL;
	}

	return hr;
}

/// <summary>
/// Handles window messages, passes most to the class instance to handle
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CInfraredBasics::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CInfraredBasics* pThis = NULL;

	if (WM_INITDIALOG == uMsg)
	{
		pThis = reinterpret_cast<CInfraredBasics*>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else
	{
		pThis = reinterpret_cast<CInfraredBasics*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pThis)
	{
		return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

/// <summary>
/// Handle windows messages for the class instance
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CInfraredBasics::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
	{
		// Bind application window handle
		m_hWnd = hWnd;

		// Init Direct2D
		D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

		// Get and initialize the default Kinect sensor
		InitializeDefaultSensor();
	}
	break;

	// If the titlebar X is clicked, destroy app
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		// Quit the main message pump
		PostQuitMessage(0);
		break;

		// Handle button press
	case WM_COMMAND:
		// If it was for the screenshot control and a button clicked event, save a screenshot next frame 
		if (IDC_BUTTON_SCREENSHOT == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))
		{
			m_bSaveScreenshot = true;
		}
		break;
	}

	return FALSE;
}

/// <summary>
/// Set the status bar message
/// </summary>
/// <param name="szMessage">message to display</param>
/// <param name="showTimeMsec">time in milliseconds to ignore future status messages</param>
/// <param name="bForce">force status update</param>
bool CInfraredBasics::SetStatusMessage(_In_z_ WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce)
{
	INT64 now = GetTickCount64();

	if (m_hWnd && (bForce || (m_nNextStatusTime <= now)))
	{
		SetDlgItemText(m_hWnd, IDC_STATUS, szMessage);
		m_nNextStatusTime = now + nShowTimeMsec;

		return true;
	}

	return false;
}
