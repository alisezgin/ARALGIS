// FreeDiskSpaceController.h: interface for the CHardDiskController class.
//
//////////////////////////////////////////////////////////////////////

#include "ARALGIS.h"
#include "Constants.h"
#include <process.h>


#define THREADEXIT_SUCCESS	  0x1234

class CFreeDiskSpaceController  
{
public:
	bool Shutdown();
	bool Start();
	CFreeDiskSpaceController();
	virtual ~CFreeDiskSpaceController();

private:
	static UINT	__stdcall	FreeDiskSpaceControllerThread(LPVOID);

	HANDLE			ThreadFreeDiskSpaceController; 
	unsigned int	ThreadID;
	bool			m_bInit;
	HANDLE          ShutdownEvent;    
};
