#pragma once


//#include <stdlib.h>
//#include <stdio.h>


#include "ARALGIS.h"
#include "Constants.h"

#include "..\\..\\Implementation\\HeaderFiles\\miniini.h"


class IniFileProcessing
{
public:
	IniFileProcessing();
	virtual ~IniFileProcessing();


public:
	BOOL ReadIniFile();


private:
	void CreateDefaultParameters();

};

