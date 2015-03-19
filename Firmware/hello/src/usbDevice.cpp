#include <cyg/kernel/diag.h>
#include <stdio.h>

#include "../usb_dev/inc/usb_conf.h"
#include "../usb_dev/inc/usbd_desc.h"
#include "../usb_otg/inc/usbd_cdc_core.h"

#include "usbDevice.h"

usbDevice * usbDevice::__instance = 0;

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
extern  USBD_DEVICE USR_desc;

extern cyg_uint8  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern cyg_uint32 APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                    in the buffer APP_Rx_Buffer. */

CDC_IF_Prop_TypeDef cdc_fops =
{
		usbDevice::cdc_Init,
		usbDevice::cdc_DeInit,
		usbDevice::cdc_Ctrl,
		usbDevice::cdc_DataTx,
		usbDevice::cdc_rxData
};

USBD_Usr_cb_TypeDef USR_cb =
{
		usbDevice::USBD_USR_Init,
		usbDevice::USBD_USR_DeviceReset,
		usbDevice::USBD_USR_DeviceConfigured,
		usbDevice::USBD_USR_DeviceSuspended,
		usbDevice::USBD_USR_DeviceResumed,

		usbDevice::USBD_USR_DeviceConnected,
		usbDevice::USBD_USR_DeviceDisconnected,

};


void usbDevice::init()
{
	if(!__instance)
	{
		__instance = new usbDevice();
	}
}

usbDevice::usbDevice()
{
	cyg_thread_create(USB_PRIORITY,
			usbDevice::rx_thread_func,
			(cyg_addrword_t)this,
			(char *)"usbThread",
			mRXStack,
			USB_STACK_SIZE,
			&mRXThreadHandle,
			&mRXThread);

	cyg_thread_resume(mRXThreadHandle);


	mUSBRXlen = 0;
	mUSBstatus = unknown;

	cyg_mutex_init(&mUSBmutex);
	cyg_cond_init(&mUSBrxCond, &mUSBmutex);

	USBD_Init(&USB_OTG_dev,
			USB_OTG_FS_CORE_ID,
			&USR_desc,
			&USBD_CDC_cb,
			&USR_cb);

}

void usbDevice::rx_thread_func(cyg_addrword_t arg)
{
	diag_printf("USB started\n");

	for(;;)
	{
		cyg_mutex_lock(&(__instance->mUSBmutex));
		if(cyg_cond_timed_wait(&(__instance->mUSBrxCond), cyg_current_time() + 5000)) //keep the thread running to prevent lock ups
		{
			if(__instance->mUSBRXlen)
			{
				__instance->handleData(__instance->mUSBRXbuff, __instance->mUSBRXlen);
			}
		}

		cyg_mutex_unlock(&(__instance->mUSBmutex));
	}
}


/**
 * @brief  USBD_USR_Init
 *         Displays the message on LCD for host lib initialization
 * @param  None
 * @retval None
 */
void  usbDevice::USBD_USR_Init(void)
{
	diag_printf("USB: USR init\n");
}

/**
 * @brief  USBD_USR_DeviceReset
 *         Displays the message on LCD on device Reset Event
 * @param  speed : device speed
 * @retval None
 */
void  usbDevice::USBD_USR_DeviceReset(cyg_uint8 speed )
{
	//	switch (speed)
	//	{
	//	case USB_OTG_SPEED_HIGH:
	//		diag_printf ("     USB Device Library v1.0.0 [HS]\n" );
	//		break;
	//
	//	case USB_OTG_SPEED_FULL:
	//		diag_printf("     USB Device Library v1.0.0 [FS]\n" );
	//		break;
	//	default:
	//		diag_printf("     USB Device Library v1.0.0 [??]\n" );
	//	}

	if(__instance)
		__instance->setUSBstate(reset);
}


/**
 * @brief  USBD_USR_DeviceConfigured
 *         Displays the message on LCD on device configuration Event
 * @param  None
 * @retval Staus
 */
void  usbDevice::USBD_USR_DeviceConfigured (void)
{
	if(__instance)
		__instance->setUSBstate(configured);
}

/**
 * @brief  USBD_USR_DeviceSuspended
 *         Displays the message on LCD on device suspend Event
 * @param  None
 * @retval None
 */
void  usbDevice::USBD_USR_DeviceSuspended(void)
{
	if(__instance)
		__instance->setUSBstate(suspended);
}


/**
 * @brief  USBD_USR_DeviceResumed
 *         Displays the message on LCD on device resume Event
 * @param  None
 * @retval None
 */
void  usbDevice::USBD_USR_DeviceResumed(void)
{
	if(__instance)
		__instance->setUSBstate(resumed);
}

/**
 * @brief  USBD_USR_DeviceConnected
 *         Displays the message on LCD on device connection Event
 * @param  None
 * @retval Staus
 */
void  usbDevice::USBD_USR_DeviceConnected (void)
{
	diag_printf("USB: Device Connected.\n");
}


/**
 * @brief  USBD_USR_DeviceDisonnected
 *         Displays the message on LCD on device disconnection Event
 * @param  None
 * @retval Staus
 */
void  usbDevice::USBD_USR_DeviceDisconnected (void)
{
	diag_printf("USB: Device Disconnected.\n");
}

/**
 * @brief  cdc_Init
 *         Initializes the Media on the STM32
 * @param  None
 * @retval Result of the opeartion (USBD_OK in all cases)
 */
cyg_uint16 usbDevice::cdc_Init()
{
	diag_printf("USB: CDC initialized\n");

	return USBD_OK;
}


cyg_uint16 usbDevice::cdc_DeInit(void)
{
	diag_printf("CDC De- initialized\n");

	return USBD_OK;
}

typedef struct
{
  cyg_uint32 bitrate;
  cyg_uint8  format;
  cyg_uint8  paritytype;
  cyg_uint8  datatype;
}LINE_CODING;

LINE_CODING linecoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };

/**
 * @brief  cdc_Ctrl
 *         Manage the CDC class requests
 * @param  Cmd: Command code
 * @param  Buf: Buffer containing command data (request parameters)
 * @param  Len: Number of data to be sent (in bytes)
 * @retval Result of the opeartion (USBD_OK in all cases
 */
//not really necessary for this example
cyg_uint16 usbDevice::cdc_Ctrl (cyg_uint32 Cmd, cyg_uint8* Buf, cyg_uint32 Len)
{
	diag_printf("cdc_Ctrl %02X, len %d\n", Cmd, Len);


	switch (Cmd)
	{
	case SEND_ENCAPSULATED_COMMAND:
		/* Not  needed for this driver */
		break;

	case GET_ENCAPSULATED_RESPONSE:
		/* Not  needed for this driver */
		break;

	case SET_COMM_FEATURE:
		/* Not  needed for this driver */
		break;

	case GET_COMM_FEATURE:
		/* Not  needed for this driver */
		break;

	case CLEAR_COMM_FEATURE:
		/* Not  needed for this driver */
		break;

	case SET_LINE_CODING:
		diag_printf("set line coding\n");
		linecoding.bitrate = (cyg_uint32)(Buf[0] | (Buf[1] << 8) | (Buf[2] << 16) | (Buf[3] << 24));
		linecoding.format = Buf[4];
		linecoding.paritytype = Buf[5];
		linecoding.datatype = Buf[6];
		/* Set the new configuration */

		break;

	case GET_LINE_CODING:
		diag_printf("get line coding\n");
		Buf[0] = (cyg_uint8)(linecoding.bitrate);
		Buf[1] = (cyg_uint8)(linecoding.bitrate >> 8);
		Buf[2] = (cyg_uint8)(linecoding.bitrate >> 16);
		Buf[3] = (cyg_uint8)(linecoding.bitrate >> 24);
		Buf[4] = linecoding.format;
		Buf[5] = linecoding.paritytype;
		Buf[6] = linecoding.datatype;
		break;

	case SET_CONTROL_LINE_STATE:
		/* Not  needed for this driver */
		break;

	case SEND_BREAK:
		/* Not  needed for this driver */
		break;

	default:
		break;
	}

	return USBD_OK;
}


/**
 * @brief  cdc_DataTx
 *         CDC received data to be send over USB IN endpoint are managed in
 *         this function.
 * @param  Buf: Buffer of data to be sent
 * @param  Len: Number of data to be sent (in bytes)
 * @retval Result of the opeartion: USBD_OK
 */
cyg_uint16 usbDevice::cdc_DataTx (cyg_uint8* Buf, cyg_uint32 Len)
{
	cyg_uint32 i;
	//loop through buffer
	for( i = 0; i < Len; i++ )
	{
		//push data into transfer buffer
		APP_Rx_Buffer[APP_Rx_ptr_in] = Buf[i] ;
		//increase pointer value
		APP_Rx_ptr_in++;
		/* To avoid buffer overflow */
		if(APP_Rx_ptr_in == APP_RX_DATA_SIZE)
		{
			APP_Rx_ptr_in = 0;
		}
	}

	return USBD_OK;
}

void usbDevice::handleData(cyg_uint8* buff, cyg_uint32 len)
{
	//Handle data accordingly

	diag_printf("USB: %d\n", len);
	diag_dump_buf(buff, len);

	//echo received data to UART
}

cyg_uint32 usbDevice::send(cyg_uint8* buff, cyg_uint32 len)
{
	if(mUSBstatus != configured)
		return 0;


	//diag_printf("USB TX: %d\n", len);
	//diag_dump_buf(sFrame_buff, sFrame_len);

	if( cdc_DataTx(buff, len) == USBD_OK)
		return len;

	diag_printf("Could not send data\n");

	return 0;
}

cyg_uint16  usbDevice::cdc_rxData(cyg_uint8* buff, cyg_uint32 len)
{
	//diag_printf("RX %d\n", len);
	if(!__instance)
		return USBD_FAIL;

	if(!(len && (len < 512)))
		return USBD_FAIL;

	cyg_mutex_lock(&(__instance->mUSBmutex));
	memcpy(__instance->mUSBRXbuff, buff, len);
	__instance->mUSBRXlen = len;
	cyg_mutex_unlock(&(__instance->mUSBmutex));

	cyg_cond_signal(&(__instance->mUSBrxCond));


	return USBD_OK;
}

//void setUSBstate(cyg_uint8 state)
//{
//	if(usbThread::get())
//		usbThread::get()->setUSBstate((usbThread::eUSBstate)state);
//}

void usbDevice::setUSBstate(usbDevice::eUSBstate state)
{
	//diag_printf("Setting state %d\n", state);
	switch(state)
	{

	case reset:
	{
		diag_printf("USB: Reset\n");
	}
	break;
	case configured:
	{
		diag_printf("USB: Configured.\n");
	}
	break;
	case suspended:
	{
		diag_printf("USB: Suspend Mode.\n");
	}
	break;
	case resumed:
	{
		diag_printf("USB: Resumed\n");
	}
	break;

	default:
		break;
	}
	mUSBstatus = state;
}


usbDevice::~usbDevice()
{
}

