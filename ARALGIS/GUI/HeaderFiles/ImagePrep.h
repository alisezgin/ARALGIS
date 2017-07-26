#pragma once

#include <string>

class CImagePrep
{


public:
	~CImagePrep();

	static CImagePrep* GetInstance();
	
	std::string PrepareChassisFilename();
	std::string PrepareChassisFilename(const CString & _lp);
	std::string PrepareChassisFilename(const CString & _lp, const CTime & _time);
	std::string PrepareChassisFilename(const CString & _lp, const CString & _suffix);
	std::string PrepareFrontalFilename();
	std::string PrepareFrontalFilename(const CString & _lp);
	std::string PrepareFrontalFilename(const CString & _lp, const CTime & _time);
	std::string PrepareFrontalFilename(const CString & _lp, const CString & _suffix);

protected:
	CImagePrep();
	
	CString m_strPathToCars;
	CString m_strPathToChassis;
	CString m_strPathToFrontal;
	
	static CImagePrep* m_instance;
};

