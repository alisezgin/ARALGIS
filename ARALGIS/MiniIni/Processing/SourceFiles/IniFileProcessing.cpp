#include "stdafx.h"


#include "ARALGIS.h"
#include "..\HeaderFiles\IniFileProcessing.h"

#include "MainFrm.h"
#include "ARALGISView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	IniFileProcessing::IniFileProcessing
// 
// DESCRIPTION:	C'tor 
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
IniFileProcessing::IniFileProcessing()
{

}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	IniFileProcessing::IniFileProcessing
// 
// DESCRIPTION:	D'tor
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
IniFileProcessing::~IniFileProcessing()
{

}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	IniFileProcessing::ReadIniFile
// 
// DESCRIPTION:	Reads the ini file to initialize some parameters
//
//  const char * appname;
//  int appbuild;
//  float brightness;
//  bool fullscreen;
//
//   if (!general->ReadString("AppName", appname))
//      std::cout << "ERROR: Missing tag AppName= in section [general] in example.ini"
//      << std::endl;
//
//   if (!general->ReadInt("AppBuild", appbuild))
//      std::cout << "ERROR: Missing or invalid tag AppBuild= in section [general] in example.ini"
//      << std::endl;
//
//   if (!general->ReadFloat("Brightness", brightness))
//      std::cout << "ERROR: Missing or invalid tag Brightness= in section [general] in example.ini"
//      << std::endl;
//
//   if (!general->ReadBool("FullScreen", fullscreen))
//      std::cout << "ERROR: Missing or invalid tag FullScreen= in section [general] in example.ini"
//      << std::endl;
//
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
BOOL IniFileProcessing::ReadIniFile()
{
	int dCameraWidth;
	int	dCameraHeight;
	int dCameraPixelBits;
	int dCameraBuffer;
	const char* dCameraConfigFile;
	
	const char* cOdroidPort;
	const char* cPTSPort;
	const char* cPTSIP;

	const char* cRefImageFilePath;

	int dImageOnScreen;

	INIFile ini;
	if (!ini.OpenFile("C:\\Users\\bora\\Desktop\\ARALGISConfig.ini"))
	{
		::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasý Bulunamadý.\nProgram Kapanýyor!!!!!",
			         WARNINGWINDOW_TITLE, MB_ICONSTOP);

		return FALSE;
	}

	INISection* camera_parameters = ini.GetSection("CAMERA_PARAMETERS");
	{
		if (!camera_parameters)
		{
			::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nKamera Parametreleri.\nProgram Kapanýyor!!!!!",
				WARNINGWINDOW_TITLE, MB_ICONSTOP);
			return FALSE;
		}
		else
		{
			if (!camera_parameters->ReadInt("Camera_Width", dCameraWidth))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nKamera Parametreleri: Camera_Width.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}

			if (!camera_parameters->ReadInt("Camera_Height", dCameraHeight))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nKamera Parametreleri: Camera_Height.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}

			if (!camera_parameters->ReadInt("Camera_Pixel_Bits", dCameraPixelBits))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nKamera Parametreleri: Camera_Pixel_Bits.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}

			if (!camera_parameters->ReadInt("Camera_Buffer", dCameraBuffer))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nKamera Parametreleri: Camera_Buffer.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}

			if (!camera_parameters->ReadString("Camera_Config_File", dCameraConfigFile))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nKamera Parametreleri: Camera_Buffer.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	INISection* ethernet_parameters = ini.GetSection("ETHERNET_COMMUNICATION");
	{
		if (!ethernet_parameters)
		{
			::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nEthernet Parametreleri.\nProgram Kapanýyor!!!!!",
				WARNINGWINDOW_TITLE, MB_ICONSTOP);
			return FALSE;
		}
		else
		{
			if (!ethernet_parameters->ReadString("Odroid_Port", cOdroidPort))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nEthernet Parametreleri: Odroid_Port.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}

			if (!ethernet_parameters->ReadString("PTS_Port", cPTSPort))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nKamera Parametreleri: PTS_Port.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}

			if (!ethernet_parameters->ReadString("PTS_IP", cPTSIP))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nKamera Parametreleri: PTS_Port.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	INISection* database_parameters = ini.GetSection("DATABASE");
	{
		if (!database_parameters)
		{
			::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nVeritabaný Parametreleri.\nProgram Kapanýyor!!!!!",
				WARNINGWINDOW_TITLE, MB_ICONSTOP);
			return FALSE;
		}
		else
		{
			if (!database_parameters->ReadString("Reference_File_Path", cRefImageFilePath))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nVeritabaný Parametreleri: Reference_File_Path.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	INISection* various_parameters = ini.GetSection("VARIOUS");
	{
		if (!various_parameters)
		{
			::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nDiðer Parametreler.\nProgram Kapanýyor!!!!!",
				WARNINGWINDOW_TITLE, MB_ICONSTOP);
			return FALSE;
		}
		else
		{
			if (!various_parameters->ReadInt("Image_On_Screen_Duration", dImageOnScreen))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nDiðer Parametreler: Image_On_Screen.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	g_CameraWidth = dCameraWidth;
	g_CameraHeight = dCameraHeight;
	g_CameraPixelBits = dCameraPixelBits;
	g_CameraBufferSize = dCameraBuffer;
	strncpy_s(g_ConfigFilename, (size_t)(MAX_FILENAME_LENGTH + 1), dCameraConfigFile, (size_t)(MAX_FILENAME_LENGTH));

	strncpy_s(g_Odroid_Port, (size_t)(PORT_BYTE_LEN + 1), cOdroidPort, (size_t)(PORT_BYTE_LEN));
	strncpy_s(g_PTSPort, (size_t)(PORT_BYTE_LEN + 1), cPTSPort, (size_t)(PORT_BYTE_LEN));

	strncpy_s(g_PTSIP, (size_t)(IP_BYTE_LEN + 1), cPTSIP, (size_t)(IP_BYTE_LEN));


	strncpy_s(g_ReferenceFilePath, (size_t)(MAX_DIR_PATH_LENGTH + 1), cRefImageFilePath, (size_t)(MAX_DIR_PATH_LENGTH));

	g_ImageOnScreenDuration = dImageOnScreen;


	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	IniFileProcessing::CreateDefaultParameters
// 
// DESCRIPTION:	Initializes parameters with default values if reading fails
// 
// INPUTS:		
// 
// NOTES:	
// 
// MODIFICATIONS:
// 
////////////////////////////////////////////////////////////////////////////////
void IniFileProcessing::CreateDefaultParameters()
{

}


