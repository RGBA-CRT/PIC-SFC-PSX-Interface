/*
 * File:   SNESPad.c
 * Author: RGBA_CRT
 *
 * Created on 2016/12/20, 22:52
 * 
 * RC0 - SNES CLK
 * RC1 - SNES LATCH(P/S)
 * RC2 - SNES DAT
 * RC3 - SNES PAD1 CONNECTED LED
 */

#include "system.h"
#include "SNESPad.h"


void initSNESPad(){
    //IOポート設定
    LATC=0xff;			//ポートの値
    LATB=0xff;			//ポートの値
    TRISC=0b11100100;	//ポートのDDR(0=OUTPUT,1=INPUT)
	TRISBbits.RB5=1;
    
	ANSEL=0;	//全アナログ機能OFF
	ANSELH=0;
    
	for(BYTE i=0;i<SNES_PAD_NUM;i++)
		SNESPad[i].value=0xFFFF;
}

void updateSNESPad(){
    WORD ctrl[SNES_PAD_NUM]={0};  
    
    LATC |= 0b11;  //PSラッチ、　CLK HIGH
    
    for(BYTE i=0;i<16;i++){
        LATCbits.LATC0=1;   //CLK Hi
        LATCbits.LATC1=0;   //PS Lo+時間稼ぎ
        LATCbits.LATC0=0;   //CLK lo
        ctrl[0] |= PORTCbits.RC2 << i; 
        ctrl[1] |= PORTBbits.RB5 << i;    
    }
    
    LATC |= 0b10;  //PSラッチ、　CLK HIGH
    
	for(BYTE i=0;i<SNES_PAD_NUM;i++)
		SNESPad[i].value=~ctrl[i];
}
/*
bool isSNESPadConnected(BYTE num){
    return (SNESPad[num].bits.DeviceID==SNESPAD_SHVC005);
}*/