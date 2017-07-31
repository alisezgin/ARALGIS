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

	const char* dChangeDetectActive;

	const char* cPTSProducer;
	const char* cPTSMode;

	const char* cAutoVehicleDetect;

	INIFile ini;
	if (!ini.OpenFile("C:\\ali\\github-home\\ARALGIS\\ConfigFiles\\ARALGISConfig.ini"))
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

	INISection* changeDetect_parameters = ini.GetSection("CHANGE_DETECT");
	{
		if (!changeDetect_parameters)
		{
			::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nDeðiþiklik Tespiti Parametreleri.\nProgram Kapanýyor!!!!!",
				WARNINGWINDOW_TITLE, MB_ICONSTOP);
			return FALSE;
		}
		else
		{
			if (!changeDetect_parameters->ReadString("Use_Change_Detect", dChangeDetectActive))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nDeðiþiklik Tespiti Parametreleri: Use_Change_Detect.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	INISection* ptsmodel_parameters = ini.GetSection("PTS_PARAMETERS");
	{
		if (!ptsmodel_parameters)
		{
			::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nPTS Markasý Parametreleri.\nProgram Kapanýyor!!!!!",
				WARNINGWINDOW_TITLE, MB_ICONSTOP);
			return FALSE;
		}
		else
		{
			if (!ptsmodel_parameters->ReadString("PTS_Producer", cPTSProducer))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nPTS Markasý Parametreleri: PTS_Producer.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}

			if (!ptsmodel_parameters->ReadString("PTS_Mode", cPTSMode))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nPTS Markasý Parametreleri: PTS_Producer.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	INISection* autovehicledetect_parameters = ini.GetSection("AUTOMATIC_VEHICLE_DETECTION");
	{
		if (!autovehicledetect_parameters)
		{
			::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nOtomatik Araç Tanýma Parametreleri.\nProgram Kapanýyor!!!!!",
				WARNINGWINDOW_TITLE, MB_ICONSTOP);
			return FALSE;
		}
		else
		{
			if (!autovehicledetect_parameters->ReadString("Use_Automatic_Detection", cAutoVehicleDetect))
			{
				::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nOtomatik Araç Tanýma Parametreleri: Use_Automatic_Detection.\nProgram Kapanýyor!!!!!",
					WARNINGWINDOW_TITLE, MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	g_CameraWidth = dCameraWidth;
	g_CameraHeight = dCameraHeight;
	g_CameraPixelBits = dCameraPixelBits;
	g_CameraBufferSize = dCameraBuffer;
	strncpy_s(g_CameraConfigFilename, (size_t)(MAX_FILENAME_LENGTH + 1), dCameraConfigFile, (size_t)(MAX_FILENAME_LENGTH));

	strncpy_s(g_Odroid_Port, (size_t)(PORT_BYTE_LEN + 1), cOdroidPort, (size_t)(PORT_BYTE_LEN));
	strncpy_s(g_PTSPort, (size_t)(PORT_BYTE_LEN + 1), cPTSPort, (size_t)(PORT_BYTE_LEN));
	strncpy_s(g_PTSIP, (size_t)(IP_BYTE_LEN + 1), cPTSIP, (size_t)(IP_BYTE_LEN));

	strncpy_s(g_ReferenceFilePath, (size_t)(MAX_DIR_PATH_LENGTH + 1), cRefImageFilePath, (size_t)(MAX_DIR_PATH_LENGTH));

	if (strcmp(dChangeDetectActive, "Yes") == 0)
	{
		g_ChangeDetectActive = true;
	}
	else if (strcmp(dChangeDetectActive, "No") == 0)
	{
		g_ChangeDetectActive = false;
	}
	else
	{
		return FALSE;
	}

	if (strcmp(cPTSProducer, "ISSD") == 0)
	{
		g_PTS_Producer_ID = PTS_BY_ISSD;
	}
	else if (strcmp(cPTSProducer, "DIVIT") == 0)
	{
		g_PTS_Producer_ID = PTS_BY_DIVIT;
	}
	else
	{
		::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nPTS Markasý Parametreleri: PTS_Producer: Bilinmeyen Marka.\nProgram Kapanýyor!!!!!",
			WARNINGWINDOW_TITLE, MB_ICONSTOP);
		return FALSE;
	}

	if (strcmp(cPTSMode, "Trigger") == 0)
	{
		g_PTS_Mode = PTS_MODE_TRIGGER;
	}
	else if (strcmp(cPTSMode, "Continuous") == 0)
	{
		g_PTS_Mode = PTS_MODE_CONTINUOUS;
	}
	else
	{
		::MessageBox(NULL, (LPCWSTR)L"Baþlangýc Dosyasýnda Hata.\nPTS Markasý Parametreleri: PTS_Producer: Bilinmeyen Marka.\nProgram Kapanýyor!!!!!",
			WARNINGWINDOW_TITLE, MB_ICONSTOP);
		return FALSE;
	}

	if (strcmp(cAutoVehicleDetect, "Yes") == 0)
	{
		g_Use_Auto_VehicleDetect = true;
	}
	else if (strcmp(cAutoVehicleDetect, "No") == 0)
	{
		g_Use_Auto_VehicleDetect = false;
	}
	else
	{
		return FALSE;
	}

	if ((g_PTS_Producer_ID == PTS_BY_DIVIT) && (g_Use_Auto_VehicleDetect == true))
	{
		g_AutoDetect_Type = AUTO_VEHICLEDETECT_WITH_DIVIT;
	}
	else if ((g_PTS_Producer_ID == PTS_BY_ISSD) && (g_Use_Auto_VehicleDetect == true))
	{
		g_AutoDetect_Type = AUTO_VEHICLEDETECT_WITH_ISSD;
	}
	else if (g_Use_Auto_VehicleDetect == false)
	{
		g_AutoDetect_Type = NO_AUTO_VEHICLE_DETECT;
	}
	else
	{
		return false;
	}

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


