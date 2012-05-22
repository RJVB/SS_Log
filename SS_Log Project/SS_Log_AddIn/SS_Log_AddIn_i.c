/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Nov 26 15:11:49 2001
 */
/* Compiler settings for C:\VS Projects\SS_Log\SS_Log_AddIn\SS_Log_AddIn.odl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID LIBID_SS_Log_AddIn = {0xFA62656D,0xB129,0x4479,{0xB9,0xB8,0x35,0xD5,0x6F,0x80,0x05,0x8D}};


const IID IID_ICommands = {0xBA3F3B08,0x7605,0x43C2,{0x82,0x99,0xEF,0xAF,0x97,0x7E,0xBC,0xC0}};


const CLSID CLSID_Commands = {0x2546CC08,0x5803,0x471C,{0x88,0x25,0x6D,0x6E,0xB0,0x71,0xFC,0xD7}};


const CLSID CLSID_ApplicationEvents = {0x9314F28B,0x5EB6,0x450E,{0x96,0x0F,0x27,0x99,0x65,0x34,0xAE,0xE0}};


const CLSID CLSID_DebuggerEvents = {0xF7119C0F,0x1D34,0x442D,{0xA0,0xCE,0x01,0xFB,0xA7,0xC9,0xA1,0x5F}};


#ifdef __cplusplus
}
#endif

