#include <cyg/kernel/diag.h>
#include <TermCMD.h>

#include "stm32cpu.h"
#include "input_port.h"
#include "init.h"

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
	{"pstatus"	, "",			"List powered peripherals", stm32cpu::pStatus},
	{"MENU"	,0,0,0},
	{"i"	, "",			"List input ports", cInput::showInputs},
	{"u"	, "",			"UP", cInit::handleNavigation},
	{"d"	, "",			"DOWN", cInit::handleNavigation},
	{"e"	, "",			"ENTER", cInit::handleNavigation},
	{"c"	, "",			"CANCEL", cInit::handleNavigation},


    {0, 0, 0, 0}
};

