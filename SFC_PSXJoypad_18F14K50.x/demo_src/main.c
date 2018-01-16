/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

/** INCLUDES *******************************************************/
#include "system.h"

#include "usb.h"
#include "usb_device_hid.h"
#include "app_device_joystick.h"
#include "SNESPad.h"
#include "PSX.h"

WORD lpc=0;

MAIN_RETURN main(void)
{
    JOYSTIC_ID jid=0;
	 SYSTEM_Initialize(SYSTEM_STATE_USB_START);
     led=0;

	 USBDeviceInit();
	 USBDeviceAttach();
	 
	 initSNESPad();	 
     psx_init();
		  
	 while(1)
	 {
		SYSTEM_Tasks();

		#if defined(USB_POLLING)
		  // Interrupt or polling method.  If using polling, must call
		  // this function periodically.  This function will take care
		  // of processing and responding to SETUP transactions
		  // (such as during the enumeration process when you first
		  // plug in).  USB hosts require that USB devices should accept
		  // and process SETUP packets in a timely fashion.  Therefore,
		  // when using polling, this function should be called
		  // regularly (such as once every 1.8ms or faster** [see
		  // inline code comments in usb_device.c for explanation when
		  // "or faster" applies])  In most cases, the USBDeviceTasks()
		  // function does not take very long to execute (ex: <100
		  // instruction cycles) before it returns.
		  USBDeviceTasks();
		#endif

		/* If the USB device isn't configured yet, we can't really do anything
		 * else since we don't have a host to talk to.  So jump back to the
		 * top of the while loop. */
		if( USBGetDeviceState() < CONFIGURED_STATE ){
		  /* Jump back to the top of the while loop. */
		  continue;
		}

		/* If we are currently suspended, then we need to see if we need to
		 * issue a remote wakeup.  In either case, we shouldn't process any
		 * keyboard commands since we aren't currently communicating to the host
		 * thus just continue back to the start of the while loop. */
		if( USBIsDeviceSuspended() == true )
		{
		  /* Jump back to the top of the while loop. */
		  continue;
		}
        
        //処理パッドIDを進める
        jid++;
        if (jid > JOYSTICK_ID_MAX) jid = JOYSTICK_ID_MIN;

        if (lpc++ > 50) {
            updateSNESPad();
            lpc = 0;
        } else if (lpc % 50 == 0) { //120fpsくらいにはなる
            updatePSXpad(0);
            updatePSXpad(1);
        }

        LATCbits.LATC3=isSNESPadConnected(0) | isPSXPadConnected(0);
		LATCbits.LATC4=isSNESPadConnected(1) | isPSXPadConnected(1) | led;
		
		//jidに割当たってるパッドの送信
		APP_DeviceJoystickTasks(jid);       
	 }//end while
}//end main

/*******************************************************************************
 End of File
*/
