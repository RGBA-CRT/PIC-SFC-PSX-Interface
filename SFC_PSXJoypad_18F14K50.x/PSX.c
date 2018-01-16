/*
 * File:   PSX.c
 * Author: RGBA_CRT
 *
 * Created on 2017/03/22, 2:07
 */
#include "system.h"
#include "PSX.h"

#define _XTAL_FREQ 12000000    // 使用するPIC等により動作周波数値を設定する

//手動SPI
#define SPI_CS1 LATCbits.LATC6
#define SPI_CS2 LATCbits.LATC1
#define SPI_CLK LATBbits.LATB6
#define SPI_SDO LATCbits.LATC7
#define SPI_SDI PORTBbits.RB4//LATBbits.LATB4

#define SPI_RSV_STATE 0x00

void PSX_MEMCARD_LONGWAIT(){
    __delay_us(PSX_MEM_WAIT_VALUE_US);
}

void psx_init() {
    TRISCbits.TRISC6 = 0; // SPI_CSを出力に設定
    TRISCbits.TRISC7 = 0; // SPISDOを出力に設定
    TRISBbits.TRISB6 = 0; // SPISCKを出力に設定
    TRISBbits.RB4 = 1; // SPISCKを入力に設定

    SPI_CS1 = 1;
    SPI_CS2 = 1;

    //パッド情報初期化
    PSXPad[0].val.CHECK_Z = 0x00;
    PSXPad[1].val.CHECK_Z = 0x00;
}

char SPI_SendByte(char cmd) {
    char i, rsv = 0;
    for (i = 0; i < 8; i++) {
        SPI_SDO = (cmd >> i) & 0x01; //立下りで出力
        SPI_CLK = 0;
        __delay_us(PSX_CLOCK_DELAY);
        rsv |= SPI_SDI << i; //立ち上がりで取り込み
        SPI_CLK = 1;
        __delay_us(PSX_CLOCK_DELAY);
    }
    
    __delay_us(PSX_SELECT_DELAY);
    return rsv;
}
/*
void SPI_SendBuffer(char* buf, char length) {
    int i;
    for (i = 0; i < length; i++)
        SPI_SendByte(buf[i]);
}*/

void SPI_ReadBytes(char* buf, char length) {
    int i;
    for (i = 0; i < length; i++)
        buf[i] = SPI_SendByte(SPI_RSV_STATE);
}

#define SPI_CS(port,state) port==0 ? SPI_CS1=state : SPI_CS2=state
#define PSX_MEM_LED(port,state) port==0 ? LATCbits.LATC3=state : LATCbits.LATC4=state
/*
void SPI_CS(BYTE port,BYTE state){
    port==0 ? SPI_CS1=state : SPI_CS2=state;
}*/

//---------------- PSX AREA ---------------
void updatePSXpad(BYTE port) {
    SPI_CS(port,0);
    __delay_us(PSX_SELECT_DELAY);

    SPI_SendByte(0x01);
    
    //コントローラID0xFFは存在しないため、応答なし判定する
    if ((PSXPad[port].val.deviceID = SPI_SendByte(0x42)) == 0xFF){
        PSXPad[port].val.CHECK_Z=0x00;
        goto UPSX_END_SPI;
    }

    PSXPad[port].val.CHECK_Z = SPI_SendByte(0x00);  // 'Z'
    
    //正常応答のみパッド情報を更新する
    if(PSXPad[port].val.CHECK_Z=='Z'){
        //容量節約のため６バイト固定受信
        SPI_ReadBytes(PSXPad[port].raw + 2, 6);

        //ボタンを正論理にする
        PSXPad[port].val.BUTTONS.raw = ~PSXPad[port].val.BUTTONS.raw;
    }
 UPSX_END_SPI:
    SPI_CS(port, 1);

    if (PSXPad[port].val.CHECK_Z != 'Z') {
        //何回か連続で未接続来たら、未接続
        if (PSXPad[port].val.NCCount < PSX_PAD_MAX_RETRY_COUNT) { //まだ未接続判断するには早い
            PSXPad[port].val.CHECK_Z = 'Z';
            PSXPad[port].val.NCCount++;
        }
    } else {
        PSXPad[port].val.NCCount = 0;
    }
}

#define PSX_MEM_INVALID_ADDRESS 0xffff
BYTE psx_mem_start(BYTE mode,WORD address){
    BYTE mc_ret[2];
    //[wait] after CS Low
    PSX_MEMCARD_LONGWAIT();
    
    //start memory access
    SPI_SendByte(0x81);
    SPI_SendByte(mode);
    SPI_ReadBytes(mc_ret, 2);
    PSX_MEMCARD_LONGWAIT();
    if (mc_ret[0] == 'Z' && mc_ret[1] == ']') {
        if(address==PSX_MEM_INVALID_ADDRESS) 
            return true;
        
        //set address
        SPI_SendByte(address >> 8);
        SPI_SendByte(address);
        PSX_MEMCARD_LONGWAIT();
        return true;
    } else
        return false;


}
//read to buffer
BYTE psx_mem_read_frame(BYTE port,WORD address){
    BYTE ret=false, mc_ret[2];
    
    SPI_CS(port,0);
    PSX_MEM_LED(port,1);
    
    //send read command & Address
    if(psx_mem_start(PSX_MEM_CMD_READ,address) == false){
        memcpy(memcard_buf,"SPI START ERR\0",16);
        goto PMRF_EXIT;
    }
           
    //rsv responce
    mc_ret[0] = SPI_SendByte(SPI_RSV_STATE);    // 'Z'
    PSX_MEMCARD_LONGWAIT();
    mc_ret[1] = SPI_SendByte(SPI_RSV_STATE);       // ']'
    
    if(mc_ret[1]!=']'){
        memcpy(memcard_buf,"RSP ERR=",8);
        memcard_buf[8]=mc_ret[0];
        memcard_buf[9]=mc_ret[1];
        memcard_buf[10]=0;
        goto PMRF_EXIT;  
    }
    
    PSX_MEMCARD_LONGWAIT();
    SPI_ReadBytes(mc_ret,2);    //rsv address  
    PSX_MEMCARD_LONGWAIT();
    
    //read memcard (data, xor)
    SPI_ReadBytes(memcard_buf,PSX_MEMCARD_FRAME_SIZE+1);
    
    ret=true;
PMRF_EXIT:
    SPI_CS(port,1);
    PSX_MEM_LED(port,0);
    return ret;
}

//write buffer to memcard
BYTE psx_mem_write_frame(BYTE port,WORD address){
    BYTE ret=false, mc_ret[3], i;
    
    SPI_CS(port,0);
    PSX_MEM_LED(port,1);
    
    //send write command & Address
    if(psx_mem_start(PSX_MEM_CMD_WRITE,address) == false)
        goto PMWF_EXIT;    

    //データ128+XOR送信
    for (i = 0; i < PSX_MEMCARD_FRAME_SIZE+1; i++)
        SPI_SendByte(memcard_buf[i]);
    
    PSX_MEMCARD_LONGWAIT();
    //rsv responce 
    SPI_ReadBytes(mc_ret,3);    //rsv address  
    //SPI_ReadBytes(mc_ret,2);
    if(mc_ret[2]!='G')
        goto PMWF_EXIT;  
    
        
    ret=true;
PMWF_EXIT:
    SPI_CS(port,1);
    PSX_MEM_LED(port,0);
    return ret;
}

BYTE psx_mem_is_insert(BYTE port){
    BYTE ret;
    SPI_CS(port,0);
    ret=psx_mem_start(PSX_MEM_CMD_READ,PSX_MEM_INVALID_ADDRESS);
    SPI_CS(port,1);
    return ret;
}
        
//これだけじゃちょっと足りないかも
/*bool isPSXPadTmpConnected(BYTE port) {
    return (PSXPad[port].val.CHECK_Z == 'Z');
}*/

