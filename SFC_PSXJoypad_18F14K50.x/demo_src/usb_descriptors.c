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

/*********************************************************************
 * Descriptor specific type definitions are defined in:
 * usb_device.h
 *
 * Configuration options are defined in:
 * usb_config.h
 ********************************************************************/

/** INCLUDES *******************************************************/
#include "usb.h"
#include "usb_device_hid.h"
#include "PSX.h"

/** CONSTANTS ******************************************************/
#if defined(COMPILER_MPLAB_C18)
#pragma romdata
#endif

/* Device Descriptor */
const USB_DEVICE_DESCRIPTOR device_dsc=
{
    0x12,    // Size of this descriptor in bytes
    USB_DESCRIPTOR_DEVICE,                // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0x00,                   // Class Code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    USB_EP0_BUFF_SIZE,      // Max packet size for EP0, see usb_config.h
    0x04D8,                 // Vendor ID, see usb_config.h
    0x005E,                 // Product ID, see usb_config.h
    0x0001,                 // Device release number in BCD format
    0x01,                   // Manufacturer string index
    0x02,                   // Product string index
    0x00,                   // Device serial number string index
    01                    // Number of possible configurations
};


/* Configuration 1 Descriptor */
const uint8_t configDescriptor1[]={
    /* Configuration Descriptor */
    0x09,//sizeof(USB_CFG_DSC),    // Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,                // CONFIGURATION descriptor type
    DESC_CONFIG_WORD(0x0022),                   // Total length of data for this cfg
    1,                      // Number of interfaces in this cfg
    1,                      // Index value of this configuration
    0,                      // Configuration string index
    _DEFAULT/* | _SELF*/,               // Attributes, see usb_device.h
    96,                     // Max power consumption (2X mA)

    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    0,                      // Interface Number
    0,                      // Alternate Setting Number
    1,                      // Number of endpoints in this intf
    HID_INTF,               // Class code
    0,     // Subclass code
    0,     // Protocol code
    2,                      // Interface string index

    /* HID Class-Specific Descriptor */
    0x09,//sizeof(USB_HID_DSC)+3,    // Size of this descriptor in bytes RRoj hack
    DSC_HID,                // HID descriptor type
    DESC_CONFIG_WORD(0x0111),                 // HID Spec Release Number in BCD format (1.11)
    0x00,                   // Country Code (0x00 for Not supported)
    HID_NUM_OF_DSC,         // Number of class descriptors, see usbcfg.h
    DSC_RPT,                // Report descriptor type
    DESC_CONFIG_WORD(HID_RPT01_SIZE),   //sizeof(hid_rpt01),      // Size of the report descriptor
    
    /* Endpoint Descriptor */
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    JOYSTICK_EP | _EP_IN,            //EndpointAddress
    _INTERRUPT,                       //Attributes
    DESC_CONFIG_WORD(64),        //size
    0x01,                        //Interval
    
};


//Language code string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[1];}sd000={
sizeof(sd000),USB_DESCRIPTOR_STRING,{0x0409
}};

//Manufacturer string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[19];}sd001={
sizeof(sd001),USB_DESCRIPTOR_STRING,
{'R','G','B','A','_','C','R','T',' ',
'S','o','f','t','w','a','r','e','s','.'
}};


//Product string descriptor
const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[21];}sd002={
sizeof(sd002),USB_DESCRIPTOR_STRING,
{'P','I','C',' ',
		'G','a','m','e','p','a','d',' ','I','n','t','e','r','f','a','c','e'
}};


const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[4];}sd003={
sizeof(sd003),USB_DESCRIPTOR_STRING,{'S','F','C','1'}};

const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[4];}sd004={
sizeof(sd003),USB_DESCRIPTOR_STRING,{'S','F','C','2'}};

const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[4];}sd005={
sizeof(sd005),USB_DESCRIPTOR_STRING,{'P','S','X','1'}};

const struct{uint8_t bLength;uint8_t bDscType;uint16_t string[4];}sd006={
sizeof(sd006),USB_DESCRIPTOR_STRING,{'P','S','X','2'}};


//Array of configuration descriptors
const uint8_t *const USB_CD_Ptr[]=
{
    (const uint8_t *const)&configDescriptor1
};

//Array of string descriptors
const uint8_t *const USB_SD_Ptr[]=
{
    (const uint8_t *const)&sd000,	//言語コード
    (const uint8_t *const)&sd001,	//製造元
    (const uint8_t *const)&sd002,	//デバイスの説明
    (const uint8_t *const)&sd003,
    (const uint8_t *const)&sd004,
    (const uint8_t *const)&sd005,
    (const uint8_t *const)&sd006
};

const struct{uint8_t report[HID_RPT01_SIZE];}hid_rpt01={{
	//--------------- SNES PAD1 START ---------------------------//
	0x05,0x01,				//USAGE_PAGE (Generic Desktop)
	0x09,0x05,				//USAGE (Game Pad)
	0xA1,0x01,				//COLLECTION (Application)
	0x85, 0x01,				//  Report ID = 1
	0x79, 0x03,				//  PadName = SFC1
	
		//8ボタン
		0x15,0x00,				//	LOGICAL_MINIMUM(0)	//ボタンの値は0か
		0x25,0x01,				//	LOGICAL_MAXIMUM(1)	//				1
		0x35,0x00,				//	PHYSICAL_MINIMUM(0)
		0x45,0x01,				//	PHYSICAL_MAXIMUM(1)
		0x75,0x01,				//	REPORT_SIZE(1)		//1bit*13
		0x95,0x08,				//	REPORT_COUNT(8)
		0x05,0x09,				//	USAGE_PAGE(Button)		
		0x19,0x01,				//	USAGE_MINIMUM(Button 1)	//先頭ボタン
		0x29,0x08,				//	USAGE_MAXIMUM(Button 8)	//最後尾のボタン
		0x81,0x02,				//	INPUT(Data,Var,Abs)		//PCへ入力		
			
		//アナログ十字キー
		0x05,0x01,				//	USAGE_PAGE(Generic Desktop)
		0x25,0xFF,              //	LOGICAL_MAXIMUM(255)
		0x45,0xFF,              //	PHYSICAL_MAXIMUM(255)
		0x09,0x30,				//	USAGE(X)
		0x09,0x31,				//	USAGE(Y)
		0x75,0x08,				//	REPORT_SIZE(8)
		0x95,0x02,				//	REPORT_COUNT(2)
		0x81,0x02,				//	INPUT(Data,Var,Abs)
			
	0xC0,							//END_COLLECTION
			
	//--------------- SNES PAD2 START ---------------------------//
			
	0x05,0x01,				//USAGE_PAGE (Generic Desktop)
	0x09,0x05,				//USAGE (Game Pad)
	0xA1,0x01,				//COLLECTION (Application)
	0x85, 0x02,				//  Report ID = 2
    0x79, 0x04,				//  PadName = SFC1
	
		//8ボタン
		0x15,0x00,				//	LOGICAL_MINIMUM(0)	//ボタンの値は0か
		0x25,0x01,				//	LOGICAL_MAXIMUM(1)	//				1
		0x35,0x00,				//	PHYSICAL_MINIMUM(0)
		0x45,0x01,				//	PHYSICAL_MAXIMUM(1)
		0x75,0x01,				//	REPORT_SIZE(1)		//1bit*13
		0x95,0x08,				//	REPORT_COUNT(8)
		0x05,0x09,				//	USAGE_PAGE(Button)		
		0x19,0x01,				//	USAGE_MINIMUM(Button 1)	//先頭ボタン
		0x29,0x08,				//	USAGE_MAXIMUM(Button 8)	//最後尾のボタン
		0x81,0x02,				//	INPUT(Data,Var,Abs)		//PCへ入力		
			
		//アナログ十字キー
		0x05,0x01,				//	USAGE_PAGE(Generic Desktop)
		0x25,0xFF,		 //	LOGICAL_MAXIMUM(255)
		0x45,0xFF,		 //	PHYSICAL_MAXIMUM(255)
		0x09,0x30,				//	USAGE(X)
		0x09,0x31,				//	USAGE(Y)
		0x75,0x08,				//	REPORT_SIZE(8)
		0x95,0x02,				//	REPORT_COUNT(2)
		0x81,0x02,				//	INPUT(Data,Var,Abs)
			
	0xC0,							//END_COLLECTION	

	//--------------- PSX PAD1 START ---------------------------//
			
	0x05,0x01,				//USAGE_PAGE (Generic Desktop)
	0x09,0x05,				//USAGE (Game Pad)
	0xA1,0x01,				//COLLECTION (Application)
	0x85, 0x03,				//  Report ID = 3
    0x79, 0x05,				//  PadName = PSX6
	
		//16ボタン
		0x15,0x00,				//	LOGICAL_MINIMUM(0)	//ボタンの値は0か
		0x25,0x01,				//	LOGICAL_MAXIMUM(1)	//				1
		0x35,0x00,				//	PHYSICAL_MINIMUM(0)
		0x45,0x01,				//	PHYSICAL_MAXIMUM(1)
		0x75,0x01,				//	REPORT_SIZE(1)		//1bit*13
		0x95,0x10,				//	REPORT_COUNT(8)
		0x05,0x09,				//	USAGE_PAGE(Button)		
		0x19,0x01,				//	USAGE_MINIMUM(Button 1)	//先頭ボタン
		0x29,0x10,				//	USAGE_MAXIMUM(Button 8)	//最後尾のボタン
		0x81,0x02,				//	INPUT(Data,Var,Abs)		//PCへ入力		
			
		//アナログ
		0x05,0x01,				//	USAGE_PAGE(Generic Desktop)
		0x25,0xFF,              //	LOGICAL_MAXIMUM(255)
		0x45,0xFF,              //	PHYSICAL_MAXIMUM(255)
		0x09,0x32,				//	USAGE(Z)
		0x09,0x35,				//	USAGE(Rz)
		0x09,0x30,				//	USAGE(X)
		0x09,0x31,				//	USAGE(Y)
		0x75,0x08,				//	REPORT_SIZE(8bit)
		0x95,0x04,				//	REPORT_COUNT(4)
		0x81,0x02,				//	INPUT(Data,Var,Abs)	
			
		0xC0,							//END_COLLECTION	

	//--------------- PSX PAD2 START ---------------------------//
			
	0x05,0x01,				//USAGE_PAGE (Generic Desktop)
	0x09,0x05,				//USAGE (Game Pad)
	0xA1,0x01,				//COLLECTION (Application)
	0x85, 0x04,				//  Report ID = 4
    0x79, 0x06,				//  PadName = PSX2
	
		//16ボタン
		0x15,0x00,				//	LOGICAL_MINIMUM(0)	//ボタンの値は0か
		0x25,0x01,				//	LOGICAL_MAXIMUM(1)	//				1
		0x35,0x00,				//	PHYSICAL_MINIMUM(0)
		0x45,0x01,				//	PHYSICAL_MAXIMUM(1)
		0x75,0x01,				//	REPORT_SIZE(1)	//1bit*16
		0x95,0x10,				//	REPORT_COUNT(16)
		0x05,0x09,				//	USAGE_PAGE(Button)		
		0x19,0x01,				//	USAGE_MINIMUM(Button 1)	//先頭ボタン
		0x29,0x10,				//	USAGE_MAXIMUM(Button 8)	//最後尾のボタン
		0x81,0x02,				//	INPUT(Data,Var,Abs)		//PCへ入力		
			
		//アナログ
		0x05,0x01,				//	USAGE_PAGE(Generic Desktop)
		0x25,0xFF,              //	LOGICAL_MAXIMUM(255)
		0x45,0xFF,              //	PHYSICAL_MAXIMUM(255)
		0x09,0x32,				//	USAGE(Z)
		0x09,0x35,				//	USAGE(Rz)
		0x09,0x30,				//	USAGE(X)
		0x09,0x31,				//	USAGE(Y)
		0x75,0x08,				//	REPORT_SIZE(8bit)
		0x95,0x04,				//	REPORT_COUNT(4)
		0x81,0x02,				//	INPUT(Data,Var,Abs)	
			
	0xC0,							//END_COLLECTION	
        
    //--------------- MEMORY CARD INTERFACE ----------------
    0x06, 0x00, 0xff,                // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                     // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, PSX_MEM_REPORT_ID,            //   REPORT_ID (5)
    0x19, 0x01,                            //   USAGE_MINIMUM (Vendor Usage 1)
    0x29, PSX_MEM_PROTOCOL_SIZE - 1,      //   USAGE_MAXIMUM (Vendor Usage 2)
    0x15, 0x00,                          //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,                   //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                        //   REPORT_SIZE (8)
    0x95, PSX_MEM_PROTOCOL_SIZE - 1,  //   REPORT_COUNT (46)
    0x81, 0x00,                      //   INPUT (Data,Ary,Abs)
    0x19, 0x01,                        //   USAGE_MINIMUM (Vendor Usage 1)
    0x29, PSX_MEM_PROTOCOL_SIZE - 1,  //   USAGE_MAXIMUM (Vendor Usage 2)
    0x91, 0x00,                      //   OUTPUT (Data,Ary,Abs)
    0xc0                            // END_COLLECTION
}};
