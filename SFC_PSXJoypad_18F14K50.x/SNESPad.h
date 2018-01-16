//#ifndef SNESPAD_H
//#define	SNESPAD_H

typedef unsigned short int WORD;
typedef unsigned char BYTE;

typedef union _SPU{
    struct _SPB{
        unsigned char B:1;
        unsigned char Y:1;
        unsigned char SELECT:1;
        unsigned char START:1;
        unsigned char UP:1;
        unsigned char DOWN:1;
        unsigned char LEFT:1;
        unsigned char RIGHT:1;
        unsigned char A:1;
        unsigned char X:1;
        unsigned char L:1;
        unsigned char R:1;
        unsigned char DeviceID:4;
    } bits;
    struct _SPARR{
        unsigned char :4;
        unsigned char value:4;
        unsigned char :8;
    }arrow;
    WORD value;
} SNESPADbits;

#define SNES_PAD_NUM 2

BYTE SNESPadConnected=0;
SNESPADbits SNESPad[SNES_PAD_NUM];  //チャタリングが抜けたきれいな奴

extern void initSNESPad();
extern void updateSNESPad();
//extern bool isSNESPadConnected(BYTE num);


#define SNESPAD_SHVC005 0x00	//コントローラのデータはNOTしてあるので0b0000
#define isSNESPadConnected(num) (SNESPad[num].bits.DeviceID==SNESPAD_SHVC005)