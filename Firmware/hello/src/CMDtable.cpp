#include <cyg/kernel/diag.h>
#include <TermCMD.h>
#include <string.h>

#include "stm32cpu.h"

void ccm(cTerm & term, int argc,char * argv[]);

TermCMD::cmd_table_t TermCMD::mCmdTable [] =
{
	{"SYSTEM"	,0,0,0},
    {"h"	, "",			"Show this help info", TermCMD::help},
    {"help"	, "",			"Show this help info", TermCMD::help},
    {"dt"	, "",			"Dump Thread Info", System::handle},
    {"time"	, "",			"Current system time", System::handle},
    {"settime"	, "<yyyy mm dd HH MM SS>",			"Set system time", System::setTime},
    {"reset"	, "",			"Reset processor", System::reset},
	{"STM32"	,0,0,0},
	{"pstatus"	, "",			"Reset processor", stm32cpu::pStatus},
	{"ccm"	, "",			"Write read stuff to ccm", ccm},

    {0, 0, 0, 0}
};

cyg_uint8 data[20] __attribute__ ((section (".ccm")));

void ccm(cTerm & term, int argc,char * argv[])
{
	term << "CCM access\n";

	//memset(data, 16, 0xAA);
	for(cyg_uint8 k =0;k < 16; k++)
		term << data[k] << "";

}
