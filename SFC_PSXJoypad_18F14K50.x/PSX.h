/*
 * File:   PSX.h
 * Author: RGBA_CRT
 *
 * Created on 2017/03/22, 2:07
 */

//config
#define PSX_CLOCK_DELAY 18//16
#define PSX_SELECT_DELAY 45
#define PSX_MEM_WAIT_VALUE_US 10000

#define PSX_MEM_REPORT_ID 0x05

#define PSX_PAD_MAX_RETRY_COUNT 30


//typedef
typedef unsigned short int WORD;
typedef unsigned char BYTE;

typedef union _BUTTONS{
    struct _BUTTON_BITS{        
        BYTE SELECT:1;
        BYTE LSTICK:1;
        BYTE RSTICK:1;
        BYTE START:1;        
        BYTE UP:1;
        BYTE RIGHT:1;
        BYTE DOWN:1;
        BYTE LEFT:1;
        
        BYTE L2:1;
        BYTE R2:1;
        BYTE L1:1;
        BYTE R1:1;
        BYTE TRIANGLE:1;
        BYTE CIRCLE:1;
        BYTE X:1;
        BYTE SQUARE:1;
    } bits;
    struct _ARROW_BITS{
        BYTE :4;
        BYTE raw:4;
        BYTE :8;
    } arrow;
    WORD raw;
} PSX_PAD_BUTTONS;

typedef union _PSXPAD{
    struct _BINS{        
        BYTE deviceID;
        BYTE CHECK_Z;
        PSX_PAD_BUTTONS BUTTONS;        
        BYTE Analog[4];
        BYTE NCCount;
    }val;
    BYTE raw[8];
} PSXPADbits;

#define PSX_MEMCARD_FRAME_SIZE 128
#define PSX_MEM_PROTOCOL_SIZE 35+11
#define PSX_MEM_PROCOTOL_DATA_SECTION_SIZE 32+11
#define PSX_MEM_PROCOTOL_DATA_DIV PSX_MEMCARD_FRAME_SIZE/PSX_MEM_PROCOTOL_DATA_SECTION_SIZE

typedef struct _PSX_MEM_PROCOTOL{
    BYTE reportID;
    BYTE cmd;       //PARAMETER1
    BYTE checksum;  //PARAMATER2
    BYTE data[PSX_MEM_PROCOTOL_DATA_SECTION_SIZE];
} PSX_MEM_PROCOTOL;

enum PSX_MEM_COMMANDS{
    PM_LED_BLINK = 0x10,
    PM_SET_PARAMS,
    PM_GET_MEM_PORT_STATE,
    PM_READ_TO_BUFFER,
    PM_WRITE_TO_MEMCARD,
    PM_READ_BANK0 = 0x40,
    PM_WRITE_BANK0 = 0x50,
    PM_ERROR = 0x80, //エラー時はMSBが1になる    
    PM_INVALID_COMMAND = 0xFF
};

//prototype
extern void psx_init();
extern void updatePSXpad(BYTE port);
//extern bool isPSXPadConnected(BYTE port);
#define isPSXPadConnected(port) (PSXPad[port].val.CHECK_Z == 'Z')
extern BYTE psx_mem_read_frame(BYTE port,WORD address);
extern BYTE psx_mem_write_frame(BYTE port,WORD address);
BYTE psx_mem_is_insert(BYTE port);

//values
#define PSX_PAD_NUM 2

//本データ
PSXPADbits PSXPad[PSX_PAD_NUM]; //ボタンの値の格納は正論理にしてある
char led;

BYTE memcard_buf[PSX_MEMCARD_FRAME_SIZE+PSX_MEM_PROTOCOL_SIZE+3];//バッファの最後にXOR,転送サイズが割り切れる値で+3のマージン
WORD memcard_address;
BYTE memcard_port;


#define PSX_MEM_CMD_READ 0x52
#define PSX_MEM_CMD_WRITE 0x57