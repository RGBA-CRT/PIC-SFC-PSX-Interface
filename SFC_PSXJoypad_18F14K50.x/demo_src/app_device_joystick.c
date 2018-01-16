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

#ifndef USBJOYSTICK_C
#define USBJOYSTICK_C

/** INCLUDES *******************************************************/
#include "usb.h"
#include "usb_device_hid.h"
#include "app_device_joystick.h"

#include "system.h"
#include <string.h>
#include "stdint.h"

#include "SNESPad.h"
#include "PSX.h"

typedef struct _INTPUT_CONTROLS_TYPEDEF_SNES {
    uint8_t reportID;

    union {

        struct {

            struct {
                uint8_t A : 1;
                uint8_t B : 1;
                uint8_t X : 1;
                uint8_t Y : 1;
                uint8_t START : 1;
                uint8_t SELECT : 1;
                uint8_t R : 1;
                uint8_t L : 1;
            } buttons;
            uint8_t ArrowX;
            uint8_t ArrowY;
        } members;
        uint8_t val[3];
    };
} INPUT_CONTROLS_SNES;

typedef struct _INTPUT_CONTROLS_TYPEDEF_PSX {
    uint8_t reportID;
    PSX_PAD_BUTTONS buttons;
    uint8_t analog[4];
} INPUT_CONTROLS_PSX;

typedef union _JOYSTICK_DATA {
    uint8_t raw[1];
    INPUT_CONTROLS_SNES snes;
    INPUT_CONTROLS_PSX psx;
    PSX_MEM_PROCOTOL mem;
} JOYSTICK_DATA;

JOYSTICK_DATA txbuf @ HID_OUT_DATA_BUFFER_ADDRESS;
USB_VOLATILE USB_HANDLE lastTransmission = 0;

volatile USB_HANDLE USBOutHandle;

//mem_rxbufをCtrlTrfDataと同じアドレスに配置
PSX_MEM_PROCOTOL mem_rxbuf @ HID_IN_DATA_BUFFER_ADDRESS; //MEMCARD_RX_DATA_ADDRESS;

//txbufはjoystickdataに殺されてしまうのでバックアップ
#define PSX_MEM_TX_BACKUP_SIZE sizeof(INPUT_CONTROLS_PSX)
uint8_t tx_backup[PSX_MEM_TX_BACKUP_SIZE];

uint8_t tx_count;   //たまに送信する


void APP_DeviceJoystickInitialize(void) {
    lastTransmission = 0;

    //enable the HID endpoint
    //USBEnableEndpoint(JOYSTICK_EP, USB_IN_ENABLED | USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);
    USBEnableEndpoint(JOYSTICK_EP, USB_IN_ENABLED | USB_OUT_ENABLED | USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);

    //USBOutHandle = HIDRxPacket(JOYSTICK_EP, (uint8_t*)&txbuf.mem, sizeof(txbuf.mem));
    //memcpy(memcard_buf+16, "[Initial] MEM BANK0 ", 16);
    //tx_backup[0]=tx_backup;
}

void sendJoysticSnes(uint8_t padID) {
    txbuf.snes.reportID = padID + 1;
    if (isSNESPadConnected(padID) == true) {
        //buttons
        txbuf.snes.members.buttons.B = SNESPad[padID].bits.B;
        txbuf.snes.members.buttons.A = SNESPad[padID].bits.A;
        txbuf.snes.members.buttons.X = SNESPad[padID].bits.X;
        txbuf.snes.members.buttons.Y = SNESPad[padID].bits.Y;
        txbuf.snes.members.buttons.START = SNESPad[padID].bits.START;
        txbuf.snes.members.buttons.SELECT = SNESPad[padID].bits.SELECT;
        txbuf.snes.members.buttons.R = SNESPad[padID].bits.R;
        txbuf.snes.members.buttons.L = SNESPad[padID].bits.L;

        // Arrow to Analog
        txbuf.snes.val[1] = 0x80 - SNESPad[padID].bits.LEFT * 0x80 + SNESPad[padID].bits.RIGHT * 0x7f;
        txbuf.snes.val[2] = 0x80 - SNESPad[padID].bits.UP * 0x80 + SNESPad[padID].bits.DOWN * 0x7f;
    } else {
        txbuf.snes.val[0] = 0x00;
        txbuf.snes.val[1] = 0x7f;
        txbuf.snes.val[2] = 0x7f;
    }
    lastTransmission = HIDTxPacket(JOYSTICK_EP, (uint8_t*) & txbuf, sizeof (INPUT_CONTROLS_SNES));
}

void sendJoysticPSX(uint8_t padID) {
    txbuf.psx.reportID = padID + 3;
    if (isPSXPadConnected(padID) == true) {
        //buttons
        txbuf.psx.buttons.raw = PSXPad[padID].val.BUTTONS.raw;
        if (PSXPad[padID].val.deviceID == 0x73 || PSXPad[padID].val.deviceID == 0x53) {
            //analog mode
            memcpy(txbuf.psx.analog, PSXPad[padID].val.Analog, 4);
        } else {
            //digital mode
            txbuf.psx.buttons.arrow.raw = 0;
            txbuf.psx.analog[2] = 0x80 - PSXPad[padID].val.BUTTONS.bits.LEFT * 0x80 + PSXPad[padID].val.BUTTONS.bits.RIGHT * 0x7f;
            txbuf.psx.analog[3] = 0x80 - PSXPad[padID].val.BUTTONS.bits.UP * 0x80 + PSXPad[padID].val.BUTTONS.bits.DOWN * 0x7f;
            goto SET_DEFAULT_ANALOG_LEFT;
        }
    } else {
        txbuf.psx.buttons.raw = 0x0000;
        txbuf.psx.analog[2] = 0x7f;
        txbuf.psx.analog[3] = 0x7f;
SET_DEFAULT_ANALOG_LEFT:
        txbuf.psx.analog[0] = 0x7f;
        txbuf.psx.analog[1] = 0x7f;
    }
    lastTransmission = HIDTxPacket(JOYSTICK_EP, (uint8_t*) & txbuf, sizeof (INPUT_CONTROLS_PSX));
}

void processPSXMemCmd() {
    BYTE cmd = mem_rxbuf.cmd;
    BYTE targetAdr;    

    switch (cmd) {
        case PM_LED_BLINK:
            led = ~led;
            break;
            
        //アドレスセット。PORT_STATEを返す。
        case PM_SET_PARAMS:
            //アドレス
            memcard_address = mem_rxbuf.data[0] | mem_rxbuf.data[1] << 8;
            //ポート番号
            memcard_port = mem_rxbuf.data[2];
            break;
            
        //ポート情報を返す
        case PM_GET_MEM_PORT_STATE:
            txbuf.mem.data[0] = psx_mem_is_insert(0);
            txbuf.mem.data[1] = psx_mem_is_insert(1);
            txbuf.mem.data[2] = memcard_address;
            txbuf.mem.data[3] = memcard_address >> 8;
            txbuf.mem.data[4] = memcard_port;
            break;

        //メモカからPICのメモリに転送する（ついでにバンク0を転送する）
        case PM_READ_TO_BUFFER:
            //if read function returns false, MSB to be high
            //psx_mem_read_frame(memcard_port, memcard_address)
            cmd |= (psx_mem_read_frame(memcard_port, memcard_address)==false)<<7;
            memcard_address++;
            targetAdr=0;
            goto PPMC_TRANSFER_SET;  //BANK0を転送
            
        case PM_WRITE_TO_MEMCARD:
            memcard_buf[128] = mem_rxbuf.checksum;
            cmd |= (psx_mem_write_frame(memcard_port, memcard_address)==false)<<7;            
            memcard_address++;
            break;
            
        default:
            targetAdr = (cmd & 0x03) * sizeof(txbuf.mem.data);
            //READ BANK0~3 
            if ((cmd >> 4) == 0x4) {
PPMC_TRANSFER_SET:
                memcpy((void *) &txbuf.mem.data[0],
                        (void *) &memcard_buf[targetAdr],
                        sizeof(txbuf.mem.data)
                        );

                //WRITE BANK0~3
            } else if ((cmd >> 4) == 0x5) {
                memcpy((void *) &memcard_buf[targetAdr],
                        (void *) &mem_rxbuf.data[0],
                        sizeof(txbuf.mem.data)
                        );
                goto PPMC_EXIT;
            } else {
                cmd = PM_INVALID_COMMAND;
            }      
            break;
    }

    txbuf.mem.reportID = PSX_MEM_REPORT_ID;
    //retturn checksum
    txbuf.mem.checksum = memcard_buf[128];
    //ここでコマンド終了ステートを書き込み
    txbuf.mem.cmd = cmd;
    
    lastTransmission = HIDTxPacket(JOYSTICK_EP, (uint8_t*) &txbuf.mem, sizeof(txbuf.mem));
    memcpy(tx_backup, &txbuf.mem, PSX_MEM_TX_BACKUP_SIZE);
PPMC_EXIT:
    return;
}

void APP_DeviceJoystickTasks(JOYSTIC_ID joysticID) {
    //ホストとの接続が確立していない場合 or スリープ中
    if (USBIsDeviceSuspended() == true || USBGetDeviceState() < CONFIGURED_STATE)
        return;

    if (!HIDTxHandleBusy(lastTransmission)) {
        if (joysticID <= SNES2) {
            sendJoysticSnes(joysticID);
        } else if (joysticID <= PSX2) {
            sendJoysticPSX(joysticID - PSX1);
        } else if (joysticID == PSX_MEM) {
            //memcpy(&txbuf.mem, tx_backup, PSX_MEM_TX_BACKUP_SIZE);
            //lastTransmission = HIDTxPacket(JOYSTICK_EP, (uint8_t*) & txbuf.mem, sizeof (txbuf.mem));
        }
    }
/*
    //Check if we have received an OUT data packet from the host
    if (HIDRxHandleBusy(USBOutHandle) == false) {
        //We just received a packet of data from the USB host.
        //Check the first uint8_t of the packet to see what command the host
        //application software wants us to fulfill.
        processPSXMemCmd();

        //Re-arm the OUT endpoint, so we can receive the next OUT data packet 
        //that the host may try to send us.
        USBOutHandle = HIDRxPacket(JOYSTICK_EP, (uint8_t*) &mem_rxbuf, USB_EP0_BUFF_SIZE);
    }*/
}

static void USBHIDCBSetReportComplete(void) {
    /* 1 byte of LED state data should now be in the CtrlTrfData buffer.  Copy
     * it to the OUTPUT report buffer for processing */
    //outputReport.value = CtrlTrfData[0];
    //memcpy(txbuf.mem, CtrlTrfData, PSX_MEM_PROTOCOL_SIZE);

    if (mem_rxbuf.reportID == PSX_MEM_REPORT_ID) {
        /* Process the OUTPUT report. */
        processPSXMemCmd();
    }
}

//受信 ---> CtrlTrfData(rxbuf))
void USBHIDCBSetReportHandler(void) {
    /* Prepare to receive the keyboard LED state data through a SET_REPORT
     * control transfer on endpoint 0.  The host should only send 1 byte,
     * since this is all that the report descriptor allows it to send. */
    USBEP0Receive((uint8_t*) &mem_rxbuf, USB_EP0_BUFF_SIZE, USBHIDCBSetReportComplete);
}


//Callback function called by the USB stack, whenever the host sends a new SET_IDLE
//command.

void USBHIDCBSetIdleRateHandler(uint8_t reportID, uint8_t newIdleRate) {
    //Make sure the report ID matches the keyboard input report id number.
    //If however the firmware doesn't implement/use report ID numbers,
    //then it should be == 0.
    /*   if (reportID == 0) {
           keyboardIdleRate = newIdleRate;
       }*/
}
#endif
