#ifndef USBTHREAD_H_
#define USBTHREAD_H_
#include <cyg/kernel/kapi.h>

#include "definitions.h"

#define USBRXBUFF_SIZE 1024

class usbDevice 
{
public:
	enum eUSBstate
	{
		unknown,
		reset,
		configured,
		suspended,
		resumed
	};

private:
	static usbDevice * __instance;

	cyg_uint8 mRXStack[USB_STACK_SIZE];
	cyg_thread mRXThread;
	cyg_handle_t mRXThreadHandle;


	eUSBstate mUSBstatus;

	cyg_cond_t mUSBrxCond;
	cyg_mutex_t mUSBmutex;

	cyg_uint8 mUSBRXbuff[USBRXBUFF_SIZE];
	cyg_uint32 mUSBRXlen;

	usbDevice();
	static void rx_thread_func(cyg_addrword_t arg);
	void handleData(cyg_uint8* Buf, cyg_uint32 Len);

public:
	static void init();
	static usbDevice* get(){ return __instance; };

	//USB device function call backs
	static void     USBD_USR_Init(void);
	static void     USBD_USR_DeviceReset (cyg_uint8 speed);
	static void     USBD_USR_DeviceConfigured (void);
	static void     USBD_USR_DeviceSuspended(void);
	static void     USBD_USR_DeviceResumed(void);

	static void     USBD_USR_DeviceConnected(void);
	static void     USBD_USR_DeviceDisconnected(void);


	//Communication device class functions
	static cyg_uint16 cdc_Init();
	static cyg_uint16 cdc_DeInit(void);
	static cyg_uint16 cdc_Ctrl(cyg_uint32 Cmd, cyg_uint8* Buf, cyg_uint32 Len);
	static cyg_uint16 cdc_DataTx (cyg_uint8* Buf, cyg_uint32 Len);
	static cyg_uint16 cdc_rxData(cyg_uint8* Buf, cyg_uint32 Len);

	void setUSBstate(eUSBstate state);
	eUSBstate getUSBstate(){ return mUSBstatus; };

	cyg_uint32 send(cyg_uint8* buff, cyg_uint32 len);


	virtual ~usbDevice();
};

#endif /* USBTHREAD_H_ */
