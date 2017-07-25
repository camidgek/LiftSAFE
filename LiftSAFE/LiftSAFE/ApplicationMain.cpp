#include "ApplicationMain.h"

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

	CApplication application;
	application.Run(hInstance, nShowCmd);
}

/// <summary>
/// Constructor
/// </summary>
CApplication::CApplication()
{
}

/// <summary>
/// Destructor
/// </summary>
CApplication::~CApplication()
{
}

int CApplication::Run(HINSTANCE hInstance, int nCmdShow)
{
	CImageGrabber* grabber = new CImageGrabber;
	CImageProcessor* processor = new CImageProcessor;

	int irHeight = grabber->getInfraredHeight();
	int irWidth = grabber->getInfraredWidth();

	RGBQUAD* pInfraredImage = new RGBQUAD[irHeight * irWidth];
	RGBQUAD* pOriginal = pInfraredImage;

	MSG       msg = { 0 };
	/*
	// Main message loop
	while (msg.message != WM_QUIT)
	{
		grabber->GetInfraredImage(&pInfraredImage);
		if (pInfraredImage != pOriginal)
			processor->ProcessImage(reinterpret_cast<BYTE*>(pInfraredImage));
		
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	*/
	return static_cast<int>(msg.wParam);
}

void connectArduino()
{
	/*Portname must contain these backslashes, and remember to
	replace the following com port*/
	char *port_name = "\\\\.\\COM20";

	//String for incoming data
	//char incomingData[MAX_DATA_LENGTH];

	SerialPort arduino(port_name);
	char output[MAX_DATA_LENGTH];

	while (arduino.isConnected()) {
		//Writing string to arduino
		arduino.writeSerialPort("fault\n", MAX_DATA_LENGTH);
		//Getting reply from arduino
		arduino.readSerialPort(output, MAX_DATA_LENGTH);
		//printing the output
		puts(output);
	}
}