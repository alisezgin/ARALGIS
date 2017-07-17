// FreeDiskSpaceController.h: interface for the CHardDiskController class.
//
//////////////////////////////////////////////////////////////////////

#include "ARALGIS.h"
#include "Constants.h"
#include <process.h>


#define THREADEXIT_SUCCESS	  0x1234

class CErrorDisplayer  
{
public:
	bool Shutdown();
	bool Start();
	CErrorDisplayer();
	virtual ~CErrorDisplayer();

private:
	static UINT	__stdcall	ErrorDisplayThread(LPVOID);

	HANDLE			ThreadErrorDisplayer; 
	unsigned int	ThreadID;
	bool			m_bInit;
	HANDLE          ShutdownEvent;    

	//void GetErrorMessage(ERRORMESSAGETYPE* aInMess);

};
