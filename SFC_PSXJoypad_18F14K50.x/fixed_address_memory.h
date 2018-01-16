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

#ifndef FIXED_MEMORY_ADDRESS_H
#define FIXED_MEMORY_ADDRESS_H

#define FIXED_ADDRESS_MEMORY

//エンドポイントサイズが64バイトなので、ジョイスティックデータアドレスも64バイト分用意しなければならない?
#define HID_IN_DATA_BUFFER_ADDRESS CTRL_TRF_DATA_ADDRESS   //260 //JOYSTICK_DATA_ADDRESS+USB_EP0_BUFF_SIZE
#define HID_OUT_DATA_BUFFER_ADDRESS (HID_IN_DATA_BUFFER_ADDRESS+USB_EP0_BUFF_SIZE) //0x2A0

//↓こいつらは↑に統合
//#define MEMCARD_TX_DATA_ADDRESS HID_IN_DATA_BUFFER_ADDRESS+USB_EP0_BUFF_SIZE
//#define MEMCARD_RX_DATA_ADDRESS MEMCARD_TX_DATA_ADDRESS+35 //PSX_MEM_PROTOCOL_SIZE

#endif //FIXED_MEMORY_ADDRESS