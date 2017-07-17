#include "stdafx.h"
#include "ImageGrabber.h"
#include "ImageProcessor.h"

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

	CImageGrabber* application = new CImageGrabber;
	CImageProcessor* ImageProcessor = new CImageProcessor;

	int test = application->getInfraredHeight();
	int test2 = application->getInfraredWidth();

	RGBQUAD* pInfraredImage = new RGBQUAD[test * test2];

	MSG       msg = { 0 };
	// Main message loop
	while (msg.message != WM_QUIT)
	{
		application->GetInfraredImage(pInfraredImage);
		ImageProcessor->ProcessImage(reinterpret_cast<BYTE*>(pInfraredImage));

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return static_cast<int>(msg.wParam);


}