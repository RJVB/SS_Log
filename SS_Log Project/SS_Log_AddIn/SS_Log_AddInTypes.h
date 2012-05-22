/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Nov 26 15:11:49 2001
 */
/* Compiler settings for C:\VS Projects\SS_Log\SS_Log_AddIn\SS_Log_AddIn.odl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __SS_Log_AddInTypes_h__
#define __SS_Log_AddInTypes_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ICommands_FWD_DEFINED__
#define __ICommands_FWD_DEFINED__
typedef interface ICommands ICommands;
#endif 	/* __ICommands_FWD_DEFINED__ */


#ifndef __Commands_FWD_DEFINED__
#define __Commands_FWD_DEFINED__

#ifdef __cplusplus
typedef class Commands Commands;
#else
typedef struct Commands Commands;
#endif /* __cplusplus */

#endif 	/* __Commands_FWD_DEFINED__ */


#ifndef __ApplicationEvents_FWD_DEFINED__
#define __ApplicationEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class ApplicationEvents ApplicationEvents;
#else
typedef struct ApplicationEvents ApplicationEvents;
#endif /* __cplusplus */

#endif 	/* __ApplicationEvents_FWD_DEFINED__ */


#ifndef __DebuggerEvents_FWD_DEFINED__
#define __DebuggerEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class DebuggerEvents DebuggerEvents;
#else
typedef struct DebuggerEvents DebuggerEvents;
#endif /* __cplusplus */

#endif 	/* __DebuggerEvents_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __SS_Log_AddIn_LIBRARY_DEFINED__
#define __SS_Log_AddIn_LIBRARY_DEFINED__

/* library SS_Log_AddIn */
/* [helpstring][version][uuid] */ 


DEFINE_GUID(LIBID_SS_Log_AddIn,0xFA62656D,0xB129,0x4479,0xB9,0xB8,0x35,0xD5,0x6F,0x80,0x05,0x8D);

#ifndef __ICommands_INTERFACE_DEFINED__
#define __ICommands_INTERFACE_DEFINED__

/* interface ICommands */
/* [object][dual][oleautomation][uuid] */ 


DEFINE_GUID(IID_ICommands,0xBA3F3B08,0x7605,0x43C2,0x82,0x99,0xEF,0xAF,0x97,0x7E,0xBC,0xC0);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BA3F3B08-7605-43C2-8299-EFAF977EBCC0")
    ICommands : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SS_Log_AddInCommandMethod( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICommandsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICommands __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICommands __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICommands __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICommands __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICommands __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICommands __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICommands __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SS_Log_AddInCommandMethod )( 
            ICommands __RPC_FAR * This);
        
        END_INTERFACE
    } ICommandsVtbl;

    interface ICommands
    {
        CONST_VTBL struct ICommandsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICommands_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICommands_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICommands_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICommands_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICommands_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICommands_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICommands_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICommands_SS_Log_AddInCommandMethod(This)	\
    (This)->lpVtbl -> SS_Log_AddInCommandMethod(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ICommands_SS_Log_AddInCommandMethod_Proxy( 
    ICommands __RPC_FAR * This);


void __RPC_STUB ICommands_SS_Log_AddInCommandMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICommands_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_Commands,0x2546CC08,0x5803,0x471C,0x88,0x25,0x6D,0x6E,0xB0,0x71,0xFC,0xD7);

#ifdef __cplusplus

class DECLSPEC_UUID("2546CC08-5803-471C-8825-6D6EB071FCD7")
Commands;
#endif

DEFINE_GUID(CLSID_ApplicationEvents,0x9314F28B,0x5EB6,0x450E,0x96,0x0F,0x27,0x99,0x65,0x34,0xAE,0xE0);

#ifdef __cplusplus

class DECLSPEC_UUID("9314F28B-5EB6-450E-960F-27996534AEE0")
ApplicationEvents;
#endif

DEFINE_GUID(CLSID_DebuggerEvents,0xF7119C0F,0x1D34,0x442D,0xA0,0xCE,0x01,0xFB,0xA7,0xC9,0xA1,0x5F);

#ifdef __cplusplus

class DECLSPEC_UUID("F7119C0F-1D34-442D-A0CE-01FBA7C9A15F")
DebuggerEvents;
#endif
#endif /* __SS_Log_AddIn_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
