#include <cyg/kernel/diag.h>
#include <TermCMD.h>

#include "stm32cpu.h"
#include "input_port.h"
#include "init.h"
#include "F4_RTC.h"
#include "pwm_port.h"
#include "output_port.h"
#include "motor.h"

TermCMD::cmd_table_t TermCMD::mCmdTable [] =
{
	{"SYSTEM"	,0,0,0},
    {"h"	, "",			"Show this help info", TermCMD::help},
    {"help"	, "",			"Show this help info", TermCMD::help},
    {"dt"	, "",			"Dump Thread Info", System::handle},
    {"time"	, "",			"Current system time", System::handle},
    {"settime"	, "<yyyy mm dd HH MM SS>",			"Set system time", System::setTime},
	{"STM32"	,0,0,0},
	{"reset"	, "",			"Reset processor", System::reset},
	{"pstat"	, "",			"List powered peripherals", stm32cpu::pStatus},
	{"sync"	, "",			"sync F4RTC time", F4RTC::syncEcosTime},
	{"OUTPUT"	,0,0,0},
	{"o"	, "",			"List output ports", cOutputDriver::setOutput},
	{"r"	, "",			"Rotate motor", cMotor::debugMotor},
	{"SYSMON"	,0,0,0},
	{"status"	, "",			"Show status", cInit::debug},



    {0, 0, 0, 0}
};

