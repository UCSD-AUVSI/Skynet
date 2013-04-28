#pragma once

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue May 26 13:48:02 2009
 */
/* Compiler settings for .\AComport.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __AComport_h__
#define __AComport_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IComPort_FWD_DEFINED__
#define __IComPort_FWD_DEFINED__
typedef interface IComPort IComPort;
#endif 	/* __IComPort_FWD_DEFINED__ */


#ifndef __ComPort_FWD_DEFINED__
#define __ComPort_FWD_DEFINED__

#ifdef __cplusplus
typedef class ComPort ComPort;
#else
typedef struct ComPort ComPort;
#endif /* __cplusplus */

#endif 	/* __ComPort_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IComPort_INTERFACE_DEFINED__
#define __IComPort_INTERFACE_DEFINED__

/* interface IComPort */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IComPort;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("38014DE4-7967-4F71-A6C1-CDB62E7DD269")
    IComPort : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LastError( 
            /* [retval][out] */ long *LastError) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ BSTR *Version) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExpirationDate( 
            /* [retval][out] */ BSTR *Date) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Build( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Reserved( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DistributionID( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DistributionKey( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BaudRate( 
            /* [retval][out] */ long *BaudRate) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BaudRate( 
            /* [in] */ long BaudRate) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Parity( 
            /* [retval][out] */ short *Parity) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Parity( 
            /* [in] */ short Parity) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DataBits( 
            /* [retval][out] */ short *DataBits) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DataBits( 
            /* [in] */ short DataBits) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StopBits( 
            /* [retval][out] */ short *StopBits) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StopBits( 
            /* [in] */ short StopBits) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HardwareFlowControl( 
            /* [retval][out] */ LONG *HardwareFlowControl) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HardwareFlowControl( 
            /* [in] */ LONG HardwareFlowControl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SoftwareFlowControl( 
            /* [retval][out] */ LONG *SoftwareFlowControl) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SoftwareFlowControl( 
            /* [in] */ LONG SoftwareFlowControl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ComTimeout( 
            /* [retval][out] */ long *Millisecs) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ComTimeout( 
            /* [in] */ long Millisecs) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsOpened( 
            /* [retval][out] */ BOOL *Openedl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Device( 
            /* [retval][out] */ BSTR *Device) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Device( 
            /* [in] */ BSTR Device) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NewLine( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_NewLine( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PreCommandDelay( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PreCommandDelay( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InterCharDelay( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_InterCharDelay( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogFile( 
            /* [retval][out] */ BSTR *LogFile) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogFile( 
            /* [in] */ BSTR LogFile) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RTSFlowControl( 
            /* [retval][out] */ LONG *RTSl) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RTSFlowControl( 
            /* [in] */ LONG RTS) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DTRFlowControl( 
            /* [retval][out] */ LONG *DTR) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DTRFlowControl( 
            /* [in] */ LONG DTR) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CTSFlowControl( 
            /* [retval][out] */ LONG *CTS) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CTSFlowControl( 
            /* [in] */ LONG CTS) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DSRFlowControl( 
            /* [retval][out] */ LONG *DSR) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DSRFlowControl( 
            /* [in] */ LONG DSR) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetErrorDescription( 
            /* [in] */ long Error,
            /* [retval][out] */ BSTR *Description) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Activate( 
            BSTR Key,
            /* [defaultvalue][optional][in] */ BOOL bPersistent = 1) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Sleep( 
            long Millisecs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Open( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateCom( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDeviceCount( 
            /* [retval][out] */ LONG *Count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDevice( 
            LONG Device,
            /* [retval][out] */ BSTR *Name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteString( 
            BSTR String) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteByte( 
            short Byte) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteBytes( 
            BSTR Bytes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadString( 
            /* [retval][out] */ BSTR *String) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadByte( 
            /* [retval][out] */ short *__MIDL__IComPort0000) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadBytes( 
            /* [retval][out] */ BSTR *__MIDL__IComPort0001) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearTX( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearRX( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RaiseRTS( 
            /* [in] */ BOOL Raise) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RaiseDTR( 
            /* [in] */ BOOL Raise) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryCTS( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryDSR( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryDCD( 
            /* [retval][out] */ BOOL *Active) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryRI( 
            /* [retval][out] */ BOOL *Active) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asFLOWCONTROL_DEFAULT( 
            /* [retval][out] */ LONG *FlowControl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asFLOWCONTROL_DISABLE( 
            /* [retval][out] */ LONG *FlowControl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asFLOWCONTROL_ENABLE( 
            /* [retval][out] */ LONG *FlowControl) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asDATABITS_DEFAULT( 
            /* [retval][out] */ LONG *DataBits) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asDATABITS_7( 
            /* [retval][out] */ LONG *DataBits) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asDATABITS_8( 
            /* [retval][out] */ LONG *DataBits) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asPARITY_DEFAULT( 
            /* [retval][out] */ LONG *Parity) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asPARITY_NONE( 
            /* [retval][out] */ LONG *Parity) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asPARITY_ODD( 
            /* [retval][out] */ LONG *Parity) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asPARITY_EVEN( 
            /* [retval][out] */ LONG *Parity) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asPARITY_MARK( 
            /* [retval][out] */ LONG *Parity) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asPARITY_SPACE( 
            /* [retval][out] */ LONG *Parity) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asSTOPBITS_DEFAULT( 
            /* [retval][out] */ LONG *StopBits) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asSTOPBITS_1( 
            /* [retval][out] */ LONG *StopBits) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asSTOPBITS_2( 
            /* [retval][out] */ LONG *StopBits) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_asSTOPBITS_15( 
            /* [retval][out] */ LONG *StopBits) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Module( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComPortVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IComPort * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IComPort * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IComPort * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IComPort * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IComPort * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IComPort * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IComPort * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LastError )( 
            IComPort * This,
            /* [retval][out] */ long *LastError);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            IComPort * This,
            /* [retval][out] */ BSTR *Version);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExpirationDate )( 
            IComPort * This,
            /* [retval][out] */ BSTR *Date);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Build )( 
            IComPort * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Reserved )( 
            IComPort * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DistributionID )( 
            IComPort * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DistributionKey )( 
            IComPort * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BaudRate )( 
            IComPort * This,
            /* [retval][out] */ long *BaudRate);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BaudRate )( 
            IComPort * This,
            /* [in] */ long BaudRate);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parity )( 
            IComPort * This,
            /* [retval][out] */ short *Parity);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Parity )( 
            IComPort * This,
            /* [in] */ short Parity);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DataBits )( 
            IComPort * This,
            /* [retval][out] */ short *DataBits);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DataBits )( 
            IComPort * This,
            /* [in] */ short DataBits);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StopBits )( 
            IComPort * This,
            /* [retval][out] */ short *StopBits);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StopBits )( 
            IComPort * This,
            /* [in] */ short StopBits);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HardwareFlowControl )( 
            IComPort * This,
            /* [retval][out] */ LONG *HardwareFlowControl);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_HardwareFlowControl )( 
            IComPort * This,
            /* [in] */ LONG HardwareFlowControl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SoftwareFlowControl )( 
            IComPort * This,
            /* [retval][out] */ LONG *SoftwareFlowControl);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SoftwareFlowControl )( 
            IComPort * This,
            /* [in] */ LONG SoftwareFlowControl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ComTimeout )( 
            IComPort * This,
            /* [retval][out] */ long *Millisecs);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ComTimeout )( 
            IComPort * This,
            /* [in] */ long Millisecs);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsOpened )( 
            IComPort * This,
            /* [retval][out] */ BOOL *Openedl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Device )( 
            IComPort * This,
            /* [retval][out] */ BSTR *Device);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Device )( 
            IComPort * This,
            /* [in] */ BSTR Device);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_NewLine )( 
            IComPort * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_NewLine )( 
            IComPort * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PreCommandDelay )( 
            IComPort * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PreCommandDelay )( 
            IComPort * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InterCharDelay )( 
            IComPort * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InterCharDelay )( 
            IComPort * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_LogFile )( 
            IComPort * This,
            /* [retval][out] */ BSTR *LogFile);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_LogFile )( 
            IComPort * This,
            /* [in] */ BSTR LogFile);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RTSFlowControl )( 
            IComPort * This,
            /* [retval][out] */ LONG *RTSl);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RTSFlowControl )( 
            IComPort * This,
            /* [in] */ LONG RTS);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DTRFlowControl )( 
            IComPort * This,
            /* [retval][out] */ LONG *DTR);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DTRFlowControl )( 
            IComPort * This,
            /* [in] */ LONG DTR);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CTSFlowControl )( 
            IComPort * This,
            /* [retval][out] */ LONG *CTS);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CTSFlowControl )( 
            IComPort * This,
            /* [in] */ LONG CTS);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DSRFlowControl )( 
            IComPort * This,
            /* [retval][out] */ LONG *DSR);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DSRFlowControl )( 
            IComPort * This,
            /* [in] */ LONG DSR);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IComPort * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetErrorDescription )( 
            IComPort * This,
            /* [in] */ long Error,
            /* [retval][out] */ BSTR *Description);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Activate )( 
            IComPort * This,
            BSTR Key,
            /* [defaultvalue][optional][in] */ BOOL bPersistent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Sleep )( 
            IComPort * This,
            long Millisecs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IComPort * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IComPort * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateCom )( 
            IComPort * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDeviceCount )( 
            IComPort * This,
            /* [retval][out] */ LONG *Count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDevice )( 
            IComPort * This,
            LONG Device,
            /* [retval][out] */ BSTR *Name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteString )( 
            IComPort * This,
            BSTR String);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteByte )( 
            IComPort * This,
            short Byte);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteBytes )( 
            IComPort * This,
            BSTR Bytes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadString )( 
            IComPort * This,
            /* [retval][out] */ BSTR *String);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadByte )( 
            IComPort * This,
            /* [retval][out] */ short *__MIDL__IComPort0000);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadBytes )( 
            IComPort * This,
            /* [retval][out] */ BSTR *__MIDL__IComPort0001);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearTX )( 
            IComPort * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearRX )( 
            IComPort * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RaiseRTS )( 
            IComPort * This,
            /* [in] */ BOOL Raise);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RaiseDTR )( 
            IComPort * This,
            /* [in] */ BOOL Raise);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryCTS )( 
            IComPort * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryDSR )( 
            IComPort * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryDCD )( 
            IComPort * This,
            /* [retval][out] */ BOOL *Active);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryRI )( 
            IComPort * This,
            /* [retval][out] */ BOOL *Active);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asFLOWCONTROL_DEFAULT )( 
            IComPort * This,
            /* [retval][out] */ LONG *FlowControl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asFLOWCONTROL_DISABLE )( 
            IComPort * This,
            /* [retval][out] */ LONG *FlowControl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asFLOWCONTROL_ENABLE )( 
            IComPort * This,
            /* [retval][out] */ LONG *FlowControl);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asDATABITS_DEFAULT )( 
            IComPort * This,
            /* [retval][out] */ LONG *DataBits);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asDATABITS_7 )( 
            IComPort * This,
            /* [retval][out] */ LONG *DataBits);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asDATABITS_8 )( 
            IComPort * This,
            /* [retval][out] */ LONG *DataBits);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asPARITY_DEFAULT )( 
            IComPort * This,
            /* [retval][out] */ LONG *Parity);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asPARITY_NONE )( 
            IComPort * This,
            /* [retval][out] */ LONG *Parity);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asPARITY_ODD )( 
            IComPort * This,
            /* [retval][out] */ LONG *Parity);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asPARITY_EVEN )( 
            IComPort * This,
            /* [retval][out] */ LONG *Parity);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asPARITY_MARK )( 
            IComPort * This,
            /* [retval][out] */ LONG *Parity);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asPARITY_SPACE )( 
            IComPort * This,
            /* [retval][out] */ LONG *Parity);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asSTOPBITS_DEFAULT )( 
            IComPort * This,
            /* [retval][out] */ LONG *StopBits);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asSTOPBITS_1 )( 
            IComPort * This,
            /* [retval][out] */ LONG *StopBits);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asSTOPBITS_2 )( 
            IComPort * This,
            /* [retval][out] */ LONG *StopBits);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_asSTOPBITS_15 )( 
            IComPort * This,
            /* [retval][out] */ LONG *StopBits);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Module )( 
            IComPort * This,
            /* [retval][out] */ BSTR *pVal);
        
        END_INTERFACE
    } IComPortVtbl;

    interface IComPort
    {
        CONST_VTBL struct IComPortVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComPort_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IComPort_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IComPort_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IComPort_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IComPort_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IComPort_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IComPort_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IComPort_get_LastError(This,LastError)	\
    ( (This)->lpVtbl -> get_LastError(This,LastError) ) 

#define IComPort_get_Version(This,Version)	\
    ( (This)->lpVtbl -> get_Version(This,Version) ) 

#define IComPort_get_ExpirationDate(This,Date)	\
    ( (This)->lpVtbl -> get_ExpirationDate(This,Date) ) 

#define IComPort_get_Build(This,pVal)	\
    ( (This)->lpVtbl -> get_Build(This,pVal) ) 

#define IComPort_put_Reserved(This,newVal)	\
    ( (This)->lpVtbl -> put_Reserved(This,newVal) ) 

#define IComPort_put_DistributionID(This,newVal)	\
    ( (This)->lpVtbl -> put_DistributionID(This,newVal) ) 

#define IComPort_put_DistributionKey(This,newVal)	\
    ( (This)->lpVtbl -> put_DistributionKey(This,newVal) ) 

#define IComPort_get_BaudRate(This,BaudRate)	\
    ( (This)->lpVtbl -> get_BaudRate(This,BaudRate) ) 

#define IComPort_put_BaudRate(This,BaudRate)	\
    ( (This)->lpVtbl -> put_BaudRate(This,BaudRate) ) 

#define IComPort_get_Parity(This,Parity)	\
    ( (This)->lpVtbl -> get_Parity(This,Parity) ) 

#define IComPort_put_Parity(This,Parity)	\
    ( (This)->lpVtbl -> put_Parity(This,Parity) ) 

#define IComPort_get_DataBits(This,DataBits)	\
    ( (This)->lpVtbl -> get_DataBits(This,DataBits) ) 

#define IComPort_put_DataBits(This,DataBits)	\
    ( (This)->lpVtbl -> put_DataBits(This,DataBits) ) 

#define IComPort_get_StopBits(This,StopBits)	\
    ( (This)->lpVtbl -> get_StopBits(This,StopBits) ) 

#define IComPort_put_StopBits(This,StopBits)	\
    ( (This)->lpVtbl -> put_StopBits(This,StopBits) ) 

#define IComPort_get_HardwareFlowControl(This,HardwareFlowControl)	\
    ( (This)->lpVtbl -> get_HardwareFlowControl(This,HardwareFlowControl) ) 

#define IComPort_put_HardwareFlowControl(This,HardwareFlowControl)	\
    ( (This)->lpVtbl -> put_HardwareFlowControl(This,HardwareFlowControl) ) 

#define IComPort_get_SoftwareFlowControl(This,SoftwareFlowControl)	\
    ( (This)->lpVtbl -> get_SoftwareFlowControl(This,SoftwareFlowControl) ) 

#define IComPort_put_SoftwareFlowControl(This,SoftwareFlowControl)	\
    ( (This)->lpVtbl -> put_SoftwareFlowControl(This,SoftwareFlowControl) ) 

#define IComPort_get_ComTimeout(This,Millisecs)	\
    ( (This)->lpVtbl -> get_ComTimeout(This,Millisecs) ) 

#define IComPort_put_ComTimeout(This,Millisecs)	\
    ( (This)->lpVtbl -> put_ComTimeout(This,Millisecs) ) 

#define IComPort_get_IsOpened(This,Openedl)	\
    ( (This)->lpVtbl -> get_IsOpened(This,Openedl) ) 

#define IComPort_get_Device(This,Device)	\
    ( (This)->lpVtbl -> get_Device(This,Device) ) 

#define IComPort_put_Device(This,Device)	\
    ( (This)->lpVtbl -> put_Device(This,Device) ) 

#define IComPort_get_NewLine(This,pVal)	\
    ( (This)->lpVtbl -> get_NewLine(This,pVal) ) 

#define IComPort_put_NewLine(This,newVal)	\
    ( (This)->lpVtbl -> put_NewLine(This,newVal) ) 

#define IComPort_get_PreCommandDelay(This,pVal)	\
    ( (This)->lpVtbl -> get_PreCommandDelay(This,pVal) ) 

#define IComPort_put_PreCommandDelay(This,newVal)	\
    ( (This)->lpVtbl -> put_PreCommandDelay(This,newVal) ) 

#define IComPort_get_InterCharDelay(This,pVal)	\
    ( (This)->lpVtbl -> get_InterCharDelay(This,pVal) ) 

#define IComPort_put_InterCharDelay(This,newVal)	\
    ( (This)->lpVtbl -> put_InterCharDelay(This,newVal) ) 

#define IComPort_get_LogFile(This,LogFile)	\
    ( (This)->lpVtbl -> get_LogFile(This,LogFile) ) 

#define IComPort_put_LogFile(This,LogFile)	\
    ( (This)->lpVtbl -> put_LogFile(This,LogFile) ) 

#define IComPort_get_RTSFlowControl(This,RTSl)	\
    ( (This)->lpVtbl -> get_RTSFlowControl(This,RTSl) ) 

#define IComPort_put_RTSFlowControl(This,RTS)	\
    ( (This)->lpVtbl -> put_RTSFlowControl(This,RTS) ) 

#define IComPort_get_DTRFlowControl(This,DTR)	\
    ( (This)->lpVtbl -> get_DTRFlowControl(This,DTR) ) 

#define IComPort_put_DTRFlowControl(This,DTR)	\
    ( (This)->lpVtbl -> put_DTRFlowControl(This,DTR) ) 

#define IComPort_get_CTSFlowControl(This,CTS)	\
    ( (This)->lpVtbl -> get_CTSFlowControl(This,CTS) ) 

#define IComPort_put_CTSFlowControl(This,CTS)	\
    ( (This)->lpVtbl -> put_CTSFlowControl(This,CTS) ) 

#define IComPort_get_DSRFlowControl(This,DSR)	\
    ( (This)->lpVtbl -> get_DSRFlowControl(This,DSR) ) 

#define IComPort_put_DSRFlowControl(This,DSR)	\
    ( (This)->lpVtbl -> put_DSRFlowControl(This,DSR) ) 

#define IComPort_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IComPort_GetErrorDescription(This,Error,Description)	\
    ( (This)->lpVtbl -> GetErrorDescription(This,Error,Description) ) 

#define IComPort_Activate(This,Key,bPersistent)	\
    ( (This)->lpVtbl -> Activate(This,Key,bPersistent) ) 

#define IComPort_Sleep(This,Millisecs)	\
    ( (This)->lpVtbl -> Sleep(This,Millisecs) ) 

#define IComPort_Open(This)	\
    ( (This)->lpVtbl -> Open(This) ) 

#define IComPort_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#define IComPort_UpdateCom(This)	\
    ( (This)->lpVtbl -> UpdateCom(This) ) 

#define IComPort_GetDeviceCount(This,Count)	\
    ( (This)->lpVtbl -> GetDeviceCount(This,Count) ) 

#define IComPort_GetDevice(This,Device,Name)	\
    ( (This)->lpVtbl -> GetDevice(This,Device,Name) ) 

#define IComPort_WriteString(This,String)	\
    ( (This)->lpVtbl -> WriteString(This,String) ) 

#define IComPort_WriteByte(This,Byte)	\
    ( (This)->lpVtbl -> WriteByte(This,Byte) ) 

#define IComPort_WriteBytes(This,Bytes)	\
    ( (This)->lpVtbl -> WriteBytes(This,Bytes) ) 

#define IComPort_ReadString(This,String)	\
    ( (This)->lpVtbl -> ReadString(This,String) ) 

#define IComPort_ReadByte(This,__MIDL__IComPort0000)	\
    ( (This)->lpVtbl -> ReadByte(This,__MIDL__IComPort0000) ) 

#define IComPort_ReadBytes(This,__MIDL__IComPort0001)	\
    ( (This)->lpVtbl -> ReadBytes(This,__MIDL__IComPort0001) ) 

#define IComPort_ClearTX(This)	\
    ( (This)->lpVtbl -> ClearTX(This) ) 

#define IComPort_ClearRX(This)	\
    ( (This)->lpVtbl -> ClearRX(This) ) 

#define IComPort_RaiseRTS(This,Raise)	\
    ( (This)->lpVtbl -> RaiseRTS(This,Raise) ) 

#define IComPort_RaiseDTR(This,Raise)	\
    ( (This)->lpVtbl -> RaiseDTR(This,Raise) ) 

#define IComPort_QueryCTS(This,pVal)	\
    ( (This)->lpVtbl -> QueryCTS(This,pVal) ) 

#define IComPort_QueryDSR(This,pVal)	\
    ( (This)->lpVtbl -> QueryDSR(This,pVal) ) 

#define IComPort_QueryDCD(This,Active)	\
    ( (This)->lpVtbl -> QueryDCD(This,Active) ) 

#define IComPort_QueryRI(This,Active)	\
    ( (This)->lpVtbl -> QueryRI(This,Active) ) 

#define IComPort_get_asFLOWCONTROL_DEFAULT(This,FlowControl)	\
    ( (This)->lpVtbl -> get_asFLOWCONTROL_DEFAULT(This,FlowControl) ) 

#define IComPort_get_asFLOWCONTROL_DISABLE(This,FlowControl)	\
    ( (This)->lpVtbl -> get_asFLOWCONTROL_DISABLE(This,FlowControl) ) 

#define IComPort_get_asFLOWCONTROL_ENABLE(This,FlowControl)	\
    ( (This)->lpVtbl -> get_asFLOWCONTROL_ENABLE(This,FlowControl) ) 

#define IComPort_get_asDATABITS_DEFAULT(This,DataBits)	\
    ( (This)->lpVtbl -> get_asDATABITS_DEFAULT(This,DataBits) ) 

#define IComPort_get_asDATABITS_7(This,DataBits)	\
    ( (This)->lpVtbl -> get_asDATABITS_7(This,DataBits) ) 

#define IComPort_get_asDATABITS_8(This,DataBits)	\
    ( (This)->lpVtbl -> get_asDATABITS_8(This,DataBits) ) 

#define IComPort_get_asPARITY_DEFAULT(This,Parity)	\
    ( (This)->lpVtbl -> get_asPARITY_DEFAULT(This,Parity) ) 

#define IComPort_get_asPARITY_NONE(This,Parity)	\
    ( (This)->lpVtbl -> get_asPARITY_NONE(This,Parity) ) 

#define IComPort_get_asPARITY_ODD(This,Parity)	\
    ( (This)->lpVtbl -> get_asPARITY_ODD(This,Parity) ) 

#define IComPort_get_asPARITY_EVEN(This,Parity)	\
    ( (This)->lpVtbl -> get_asPARITY_EVEN(This,Parity) ) 

#define IComPort_get_asPARITY_MARK(This,Parity)	\
    ( (This)->lpVtbl -> get_asPARITY_MARK(This,Parity) ) 

#define IComPort_get_asPARITY_SPACE(This,Parity)	\
    ( (This)->lpVtbl -> get_asPARITY_SPACE(This,Parity) ) 

#define IComPort_get_asSTOPBITS_DEFAULT(This,StopBits)	\
    ( (This)->lpVtbl -> get_asSTOPBITS_DEFAULT(This,StopBits) ) 

#define IComPort_get_asSTOPBITS_1(This,StopBits)	\
    ( (This)->lpVtbl -> get_asSTOPBITS_1(This,StopBits) ) 

#define IComPort_get_asSTOPBITS_2(This,StopBits)	\
    ( (This)->lpVtbl -> get_asSTOPBITS_2(This,StopBits) ) 

#define IComPort_get_asSTOPBITS_15(This,StopBits)	\
    ( (This)->lpVtbl -> get_asSTOPBITS_15(This,StopBits) ) 

#define IComPort_get_Module(This,pVal)	\
    ( (This)->lpVtbl -> get_Module(This,pVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IComPort_INTERFACE_DEFINED__ */



#ifndef __ACOMPORTLib_LIBRARY_DEFINED__
#define __ACOMPORTLib_LIBRARY_DEFINED__

/* library ACOMPORTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ACOMPORTLib;

EXTERN_C const CLSID CLSID_ComPort;

#ifdef __cplusplus

class DECLSPEC_UUID("07ECB42B-322A-40B9-A8A9-3815AF3C4F60")
ComPort;
#endif
#endif /* __ACOMPORTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


