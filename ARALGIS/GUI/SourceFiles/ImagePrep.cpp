#include "stdafx.h"
#include "ImagePrep.h"

CImagePrep* CImagePrep::m_instance = nullptr;

CImagePrep::CImagePrep()
{
	LPWSTR tmpPathToCars = m_strPathToCars.GetBufferSetLength(MAX_FILENAME_LENGTH);
	GetEnvironmentVariable(_T("ARALGISCarsDir"), tmpPathToCars, MAX_FILENAME_LENGTH);
	// because the contents were modified via the buffer, it must be released
	// before further modification to the contents of the CString variable.
	m_strPathToCars.ReleaseBuffer();

	/// bora2ali bug-fix begins
	// the environment variable does not contain the trailing directory separator
	m_strPathToCars += _T("\\");
	// currently we have two image subdirectories:
	// FrontalView and ChassisBottom
	// we may very well end up removing the former.
	m_strPathToChassis = m_strPathToCars + _T("ChassisBottom\\");
	m_strPathToFrontal = m_strPathToCars + _T("Frontal\\");

}
CImagePrep::~CImagePrep()
{
}


CImagePrep* CImagePrep::GetInstance()
{
	if (m_instance == nullptr)
		m_instance = new CImagePrep();

	return m_instance;
}

std::string CImagePrep::PrepareChassisFilename()
{
	CString strPrevTag = _T("default.jpg");
	CString strFilename = m_strPathToChassis + strPrevTag;
	LPCTSTR cpImgFilename = LPCTSTR(strFilename);

	std::basic_string<TCHAR, std::char_traits<TCHAR>> strStdFilename{ cpImgFilename };
	std::string strFinalFilename{ strStdFilename.begin(), strStdFilename.end() };

	return strFinalFilename;
}

std::string CImagePrep::PrepareChassisFilename(const CString & _lp)
{
	return PrepareChassisFilename(_lp, _T("default.jpg"));
}

std::string CImagePrep::PrepareChassisFilename(const CString & _lp, const CTime & _time)
{
	return PrepareChassisFilename(_lp, _time.Format(_T("%d%m%Y_%H%M%S.jpg")));
}

std::string CImagePrep::PrepareChassisFilename(const CString & _lp, const CString & _suffix)
{
	CString strPrevTag = _lp + _T("_") + _suffix;
	CString strFilename = m_strPathToChassis + strPrevTag;
	LPCTSTR cpImgFilename = LPCTSTR(strFilename);

	std::basic_string<TCHAR, std::char_traits<TCHAR>> strStdFilename{ cpImgFilename };
	std::string strFinalFilename{ strStdFilename.begin(), strStdFilename.end() };

	return strFinalFilename;
}

std::string CImagePrep::PrepareFrontalFilename()
{
	CString strPrevTag = _T("default.jpg");
	CString strFilename = m_strPathToFrontal + strPrevTag;
	LPCTSTR cpImgFilename = LPCTSTR(strFilename);

	std::basic_string<TCHAR, std::char_traits<TCHAR>> strStdFilename{ cpImgFilename };
	std::string strFinalFilename{ strStdFilename.begin(), strStdFilename.end() };

	return strFinalFilename;
}
std::string CImagePrep::PrepareFrontalFilename(const CString & _lp)
{
	return PrepareFrontalFilename(_lp, _T("default"));
}

std::string CImagePrep::PrepareFrontalFilename(const CString & _lp, const CTime & _time)
{
	return PrepareFrontalFilename(_lp, _time.Format(_T("%d%m%Y_%H%M%S.jpg")));
}

std::string CImagePrep::PrepareFrontalFilename(const CString & _lp, const CString & _suffix)
{
	CString strPrevTag = _lp + _T("_") + _suffix;
	CString strFilename = m_strPathToFrontal + strPrevTag;
	LPCTSTR cpImgFilename = LPCTSTR(strFilename);

	std::basic_string<TCHAR, std::char_traits<TCHAR>> strStdFilename{ cpImgFilename };
	std::string strFinalFilename{ strStdFilename.begin(), strStdFilename.end() };

	return strFinalFilename;
}
