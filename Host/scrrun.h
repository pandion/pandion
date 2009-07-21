
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Fri Aug 30 03:41:43 2002
 */
/* Compiler settings for scrrun.idl:
    Os, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __scrrun_h__
#define __scrrun_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDictionary_FWD_DEFINED__
#define __IDictionary_FWD_DEFINED__
typedef interface IDictionary IDictionary;
#endif 	/* __IDictionary_FWD_DEFINED__ */


#ifndef __IFileSystem_FWD_DEFINED__
#define __IFileSystem_FWD_DEFINED__
typedef interface IFileSystem IFileSystem;
#endif 	/* __IFileSystem_FWD_DEFINED__ */


#ifndef __IDriveCollection_FWD_DEFINED__
#define __IDriveCollection_FWD_DEFINED__
typedef interface IDriveCollection IDriveCollection;
#endif 	/* __IDriveCollection_FWD_DEFINED__ */


#ifndef __IDrive_FWD_DEFINED__
#define __IDrive_FWD_DEFINED__
typedef interface IDrive IDrive;
#endif 	/* __IDrive_FWD_DEFINED__ */


#ifndef __IFolder_FWD_DEFINED__
#define __IFolder_FWD_DEFINED__
typedef interface IFolder IFolder;
#endif 	/* __IFolder_FWD_DEFINED__ */


#ifndef __IFolderCollection_FWD_DEFINED__
#define __IFolderCollection_FWD_DEFINED__
typedef interface IFolderCollection IFolderCollection;
#endif 	/* __IFolderCollection_FWD_DEFINED__ */


#ifndef __IFileCollection_FWD_DEFINED__
#define __IFileCollection_FWD_DEFINED__
typedef interface IFileCollection IFileCollection;
#endif 	/* __IFileCollection_FWD_DEFINED__ */


#ifndef __IFile_FWD_DEFINED__
#define __IFile_FWD_DEFINED__
typedef interface IFile IFile;
#endif 	/* __IFile_FWD_DEFINED__ */


#ifndef __ITextStream_FWD_DEFINED__
#define __ITextStream_FWD_DEFINED__
typedef interface ITextStream ITextStream;
#endif 	/* __ITextStream_FWD_DEFINED__ */


#ifndef __IFileSystem3_FWD_DEFINED__
#define __IFileSystem3_FWD_DEFINED__
typedef interface IFileSystem3 IFileSystem3;
#endif 	/* __IFileSystem3_FWD_DEFINED__ */


#ifndef __IScriptEncoder_FWD_DEFINED__
#define __IScriptEncoder_FWD_DEFINED__
typedef interface IScriptEncoder IScriptEncoder;
#endif 	/* __IScriptEncoder_FWD_DEFINED__ */


#ifndef __Dictionary_FWD_DEFINED__
#define __Dictionary_FWD_DEFINED__

#ifdef __cplusplus
typedef class Dictionary Dictionary;
#else
typedef struct Dictionary Dictionary;
#endif /* __cplusplus */

#endif 	/* __Dictionary_FWD_DEFINED__ */


#ifndef __FileSystemObject_FWD_DEFINED__
#define __FileSystemObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class FileSystemObject FileSystemObject;
#else
typedef struct FileSystemObject FileSystemObject;
#endif /* __cplusplus */

#endif 	/* __FileSystemObject_FWD_DEFINED__ */


#ifndef __Drive_FWD_DEFINED__
#define __Drive_FWD_DEFINED__

#ifdef __cplusplus
typedef class Drive Drive;
#else
typedef struct Drive Drive;
#endif /* __cplusplus */

#endif 	/* __Drive_FWD_DEFINED__ */


#ifndef __Drives_FWD_DEFINED__
#define __Drives_FWD_DEFINED__

#ifdef __cplusplus
typedef class Drives Drives;
#else
typedef struct Drives Drives;
#endif /* __cplusplus */

#endif 	/* __Drives_FWD_DEFINED__ */


#ifndef __Folder_FWD_DEFINED__
#define __Folder_FWD_DEFINED__

#ifdef __cplusplus
typedef class Folder Folder;
#else
typedef struct Folder Folder;
#endif /* __cplusplus */

#endif 	/* __Folder_FWD_DEFINED__ */


#ifndef __Folders_FWD_DEFINED__
#define __Folders_FWD_DEFINED__

#ifdef __cplusplus
typedef class Folders Folders;
#else
typedef struct Folders Folders;
#endif /* __cplusplus */

#endif 	/* __Folders_FWD_DEFINED__ */


#ifndef __File_FWD_DEFINED__
#define __File_FWD_DEFINED__

#ifdef __cplusplus
typedef class File File;
#else
typedef struct File File;
#endif /* __cplusplus */

#endif 	/* __File_FWD_DEFINED__ */


#ifndef __Files_FWD_DEFINED__
#define __Files_FWD_DEFINED__

#ifdef __cplusplus
typedef class Files Files;
#else
typedef struct Files Files;
#endif /* __cplusplus */

#endif 	/* __Files_FWD_DEFINED__ */


#ifndef __TextStream_FWD_DEFINED__
#define __TextStream_FWD_DEFINED__

#ifdef __cplusplus
typedef class TextStream TextStream;
#else
typedef struct TextStream TextStream;
#endif /* __cplusplus */

#endif 	/* __TextStream_FWD_DEFINED__ */


#ifndef __Encoder_FWD_DEFINED__
#define __Encoder_FWD_DEFINED__

#ifdef __cplusplus
typedef class Encoder Encoder;
#else
typedef struct Encoder Encoder;
#endif /* __cplusplus */

#endif 	/* __Encoder_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __Scripting_LIBRARY_DEFINED__
#define __Scripting_LIBRARY_DEFINED__

/* library Scripting */
/* [custom][custom][helpcontext][helpfile][helpstring][version][uuid] */ 












typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_scrrun_0000_0001
    {	BinaryCompare	= 0,
	TextCompare	= 1,
	DatabaseCompare	= 2
    } 	CompareMethod;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_scrrun_0000_0002
    {	ForReading	= 1,
	ForWriting	= 2,
	ForAppending	= 8
    } 	IOMode;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_scrrun_0000_0003
    {	TristateTrue	= 0xffffffff,
	TristateFalse	= 0,
	TristateUseDefault	= 0xfffffffe,
	TristateMixed	= 0xfffffffe
    } 	Tristate;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_scrrun_0000_0004
    {	WindowsFolder	= 0,
	SystemFolder	= 1,
	TemporaryFolder	= 2
    } 	SpecialFolderConst;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_scrrun_0000_0005
    {	UnknownType	= 0,
	Removable	= 1,
	Fixed	= 2,
	Remote	= 3,
	CDRom	= 4,
	RamDisk	= 5
    } 	DriveTypeConst;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_scrrun_0000_0006
    {	StdIn	= 0,
	StdOut	= 1,
	StdErr	= 2
    } 	StandardStreamTypes;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_scrrun_0000_0007
    {	Normal	= 0,
	ReadOnly	= 1,
	Hidden	= 2,
	System	= 4,
	Volume	= 8,
	Directory	= 16,
	Archive	= 32,
	Alias	= 1024,
	Compressed	= 2048
    } 	FileAttribute;

EXTERN_C const IID LIBID_Scripting;

#ifndef __IDictionary_INTERFACE_DEFINED__
#define __IDictionary_INTERFACE_DEFINED__

/* interface IDictionary */
/* [object][oleautomation][dual][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IDictionary;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("42C642C1-97E1-11CF-978F-00A02463E06F")
    IDictionary : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][propputref][id] */ HRESULT STDMETHODCALLTYPE putref_Item( 
            /* [in] */ VARIANT *Key,
            /* [in] */ VARIANT *pRetItem) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Item( 
            /* [in] */ VARIANT *Key,
            /* [in] */ VARIANT *pRetItem) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT *Key,
            /* [retval][out] */ VARIANT *pRetItem) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ VARIANT *Key,
            /* [in] */ VARIANT *Item) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *pCount) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Exists( 
            /* [in] */ VARIANT *Key,
            /* [retval][out] */ VARIANT_BOOL *pExists) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Items( 
            /* [retval][out] */ VARIANT *pItemsArray) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Key( 
            /* [in] */ VARIANT *Key,
            /* [in] */ VARIANT *rhs) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Keys( 
            /* [retval][out] */ VARIANT *pKeysArray) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT *Key) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveAll( void) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_CompareMode( 
            /* [in] */ CompareMethod pcomp) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_CompareMode( 
            /* [retval][out] */ CompareMethod *pcomp) = 0;
        
        virtual /* [restricted][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown **ppunk) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_HashVal( 
            /* [in] */ VARIANT *Key,
            /* [retval][out] */ VARIANT *HashVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDictionaryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDictionary * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDictionary * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDictionary * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDictionary * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDictionary * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDictionary * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDictionary * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][propputref][id] */ HRESULT ( STDMETHODCALLTYPE *putref_Item )( 
            IDictionary * This,
            /* [in] */ VARIANT *Key,
            /* [in] */ VARIANT *pRetItem);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Item )( 
            IDictionary * This,
            /* [in] */ VARIANT *Key,
            /* [in] */ VARIANT *pRetItem);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IDictionary * This,
            /* [in] */ VARIANT *Key,
            /* [retval][out] */ VARIANT *pRetItem);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IDictionary * This,
            /* [in] */ VARIANT *Key,
            /* [in] */ VARIANT *Item);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IDictionary * This,
            /* [retval][out] */ long *pCount);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Exists )( 
            IDictionary * This,
            /* [in] */ VARIANT *Key,
            /* [retval][out] */ VARIANT_BOOL *pExists);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Items )( 
            IDictionary * This,
            /* [retval][out] */ VARIANT *pItemsArray);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Key )( 
            IDictionary * This,
            /* [in] */ VARIANT *Key,
            /* [in] */ VARIANT *rhs);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Keys )( 
            IDictionary * This,
            /* [retval][out] */ VARIANT *pKeysArray);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IDictionary * This,
            /* [in] */ VARIANT *Key);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveAll )( 
            IDictionary * This);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_CompareMode )( 
            IDictionary * This,
            /* [in] */ CompareMethod pcomp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_CompareMode )( 
            IDictionary * This,
            /* [retval][out] */ CompareMethod *pcomp);
        
        /* [restricted][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            IDictionary * This,
            /* [retval][out] */ IUnknown **ppunk);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_HashVal )( 
            IDictionary * This,
            /* [in] */ VARIANT *Key,
            /* [retval][out] */ VARIANT *HashVal);
        
        END_INTERFACE
    } IDictionaryVtbl;

    interface IDictionary
    {
        CONST_VTBL struct IDictionaryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDictionary_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDictionary_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDictionary_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDictionary_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDictionary_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDictionary_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDictionary_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDictionary_putref_Item(This,Key,pRetItem)	\
    (This)->lpVtbl -> putref_Item(This,Key,pRetItem)

#define IDictionary_put_Item(This,Key,pRetItem)	\
    (This)->lpVtbl -> put_Item(This,Key,pRetItem)

#define IDictionary_get_Item(This,Key,pRetItem)	\
    (This)->lpVtbl -> get_Item(This,Key,pRetItem)

#define IDictionary_Add(This,Key,Item)	\
    (This)->lpVtbl -> Add(This,Key,Item)

#define IDictionary_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IDictionary_Exists(This,Key,pExists)	\
    (This)->lpVtbl -> Exists(This,Key,pExists)

#define IDictionary_Items(This,pItemsArray)	\
    (This)->lpVtbl -> Items(This,pItemsArray)

#define IDictionary_put_Key(This,Key,rhs)	\
    (This)->lpVtbl -> put_Key(This,Key,rhs)

#define IDictionary_Keys(This,pKeysArray)	\
    (This)->lpVtbl -> Keys(This,pKeysArray)

#define IDictionary_Remove(This,Key)	\
    (This)->lpVtbl -> Remove(This,Key)

#define IDictionary_RemoveAll(This)	\
    (This)->lpVtbl -> RemoveAll(This)

#define IDictionary_put_CompareMode(This,pcomp)	\
    (This)->lpVtbl -> put_CompareMode(This,pcomp)

#define IDictionary_get_CompareMode(This,pcomp)	\
    (This)->lpVtbl -> get_CompareMode(This,pcomp)

#define IDictionary__NewEnum(This,ppunk)	\
    (This)->lpVtbl -> _NewEnum(This,ppunk)

#define IDictionary_get_HashVal(This,Key,HashVal)	\
    (This)->lpVtbl -> get_HashVal(This,Key,HashVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][propputref][id] */ HRESULT STDMETHODCALLTYPE IDictionary_putref_Item_Proxy( 
    IDictionary * This,
    /* [in] */ VARIANT *Key,
    /* [in] */ VARIANT *pRetItem);


void __RPC_STUB IDictionary_putref_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IDictionary_put_Item_Proxy( 
    IDictionary * This,
    /* [in] */ VARIANT *Key,
    /* [in] */ VARIANT *pRetItem);


void __RPC_STUB IDictionary_put_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDictionary_get_Item_Proxy( 
    IDictionary * This,
    /* [in] */ VARIANT *Key,
    /* [retval][out] */ VARIANT *pRetItem);


void __RPC_STUB IDictionary_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IDictionary_Add_Proxy( 
    IDictionary * This,
    /* [in] */ VARIANT *Key,
    /* [in] */ VARIANT *Item);


void __RPC_STUB IDictionary_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDictionary_get_Count_Proxy( 
    IDictionary * This,
    /* [retval][out] */ long *pCount);


void __RPC_STUB IDictionary_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IDictionary_Exists_Proxy( 
    IDictionary * This,
    /* [in] */ VARIANT *Key,
    /* [retval][out] */ VARIANT_BOOL *pExists);


void __RPC_STUB IDictionary_Exists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IDictionary_Items_Proxy( 
    IDictionary * This,
    /* [retval][out] */ VARIANT *pItemsArray);


void __RPC_STUB IDictionary_Items_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IDictionary_put_Key_Proxy( 
    IDictionary * This,
    /* [in] */ VARIANT *Key,
    /* [in] */ VARIANT *rhs);


void __RPC_STUB IDictionary_put_Key_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IDictionary_Keys_Proxy( 
    IDictionary * This,
    /* [retval][out] */ VARIANT *pKeysArray);


void __RPC_STUB IDictionary_Keys_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IDictionary_Remove_Proxy( 
    IDictionary * This,
    /* [in] */ VARIANT *Key);


void __RPC_STUB IDictionary_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IDictionary_RemoveAll_Proxy( 
    IDictionary * This);


void __RPC_STUB IDictionary_RemoveAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IDictionary_put_CompareMode_Proxy( 
    IDictionary * This,
    /* [in] */ CompareMethod pcomp);


void __RPC_STUB IDictionary_put_CompareMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDictionary_get_CompareMode_Proxy( 
    IDictionary * This,
    /* [retval][out] */ CompareMethod *pcomp);


void __RPC_STUB IDictionary_get_CompareMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][id] */ HRESULT STDMETHODCALLTYPE IDictionary__NewEnum_Proxy( 
    IDictionary * This,
    /* [retval][out] */ IUnknown **ppunk);


void __RPC_STUB IDictionary__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE IDictionary_get_HashVal_Proxy( 
    IDictionary * This,
    /* [in] */ VARIANT *Key,
    /* [retval][out] */ VARIANT *HashVal);


void __RPC_STUB IDictionary_get_HashVal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDictionary_INTERFACE_DEFINED__ */


#ifndef __IFileSystem_INTERFACE_DEFINED__
#define __IFileSystem_INTERFACE_DEFINED__

/* interface IFileSystem */
/* [object][oleautomation][nonextensible][dual][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IFileSystem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0AB5A3D0-E5B6-11D0-ABF5-00A0C90FFFC0")
    IFileSystem : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Drives( 
            /* [retval][out] */ IDriveCollection **ppdrives) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE BuildPath( 
            /* [in] */ BSTR Path,
            /* [in] */ BSTR Name,
            /* [retval][out] */ BSTR *pbstrResult) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDriveName( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParentFolderName( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFileName( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBaseName( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetExtensionName( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAbsolutePathName( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTempName( 
            /* [retval][out] */ BSTR *pbstrResult) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE DriveExists( 
            /* [in] */ BSTR DriveSpec,
            /* [retval][out] */ VARIANT_BOOL *pfExists) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE FileExists( 
            /* [in] */ BSTR FileSpec,
            /* [retval][out] */ VARIANT_BOOL *pfExists) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE FolderExists( 
            /* [in] */ BSTR FolderSpec,
            /* [retval][out] */ VARIANT_BOOL *pfExists) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDrive( 
            /* [in] */ BSTR DriveSpec,
            /* [retval][out] */ IDrive **ppdrive) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFile( 
            /* [in] */ BSTR FilePath,
            /* [retval][out] */ IFile **ppfile) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFolder( 
            /* [in] */ BSTR FolderPath,
            /* [retval][out] */ IFolder **ppfolder) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSpecialFolder( 
            /* [in] */ SpecialFolderConst SpecialFolder,
            /* [retval][out] */ IFolder **ppfolder) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteFile( 
            /* [in] */ BSTR FileSpec,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force = 0) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteFolder( 
            /* [in] */ BSTR FolderSpec,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force = 0) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveFile( 
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveFolder( 
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyFile( 
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles = -1) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyFolder( 
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles = -1) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateFolder( 
            /* [in] */ BSTR Path,
            /* [retval][out] */ IFolder **ppfolder) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateTextFile( 
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Overwrite,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Unicode,
            /* [retval][out] */ ITextStream **ppts) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenTextFile( 
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ IOMode IOMode,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Create,
            /* [defaultvalue][optional][in] */ Tristate Format,
            /* [retval][out] */ ITextStream **ppts) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFileSystemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileSystem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileSystem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileSystem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFileSystem * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFileSystem * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFileSystem * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFileSystem * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Drives )( 
            IFileSystem * This,
            /* [retval][out] */ IDriveCollection **ppdrives);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BuildPath )( 
            IFileSystem * This,
            /* [in] */ BSTR Path,
            /* [in] */ BSTR Name,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDriveName )( 
            IFileSystem * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParentFolderName )( 
            IFileSystem * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFileName )( 
            IFileSystem * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBaseName )( 
            IFileSystem * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetExtensionName )( 
            IFileSystem * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAbsolutePathName )( 
            IFileSystem * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTempName )( 
            IFileSystem * This,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DriveExists )( 
            IFileSystem * This,
            /* [in] */ BSTR DriveSpec,
            /* [retval][out] */ VARIANT_BOOL *pfExists);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FileExists )( 
            IFileSystem * This,
            /* [in] */ BSTR FileSpec,
            /* [retval][out] */ VARIANT_BOOL *pfExists);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FolderExists )( 
            IFileSystem * This,
            /* [in] */ BSTR FolderSpec,
            /* [retval][out] */ VARIANT_BOOL *pfExists);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDrive )( 
            IFileSystem * This,
            /* [in] */ BSTR DriveSpec,
            /* [retval][out] */ IDrive **ppdrive);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFile )( 
            IFileSystem * This,
            /* [in] */ BSTR FilePath,
            /* [retval][out] */ IFile **ppfile);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFolder )( 
            IFileSystem * This,
            /* [in] */ BSTR FolderPath,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSpecialFolder )( 
            IFileSystem * This,
            /* [in] */ SpecialFolderConst SpecialFolder,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteFile )( 
            IFileSystem * This,
            /* [in] */ BSTR FileSpec,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteFolder )( 
            IFileSystem * This,
            /* [in] */ BSTR FolderSpec,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveFile )( 
            IFileSystem * This,
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveFolder )( 
            IFileSystem * This,
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyFile )( 
            IFileSystem * This,
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyFolder )( 
            IFileSystem * This,
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFolder )( 
            IFileSystem * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateTextFile )( 
            IFileSystem * This,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Overwrite,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Unicode,
            /* [retval][out] */ ITextStream **ppts);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenTextFile )( 
            IFileSystem * This,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ IOMode IOMode,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Create,
            /* [defaultvalue][optional][in] */ Tristate Format,
            /* [retval][out] */ ITextStream **ppts);
        
        END_INTERFACE
    } IFileSystemVtbl;

    interface IFileSystem
    {
        CONST_VTBL struct IFileSystemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileSystem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFileSystem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFileSystem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFileSystem_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFileSystem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFileSystem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFileSystem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFileSystem_get_Drives(This,ppdrives)	\
    (This)->lpVtbl -> get_Drives(This,ppdrives)

#define IFileSystem_BuildPath(This,Path,Name,pbstrResult)	\
    (This)->lpVtbl -> BuildPath(This,Path,Name,pbstrResult)

#define IFileSystem_GetDriveName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetDriveName(This,Path,pbstrResult)

#define IFileSystem_GetParentFolderName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetParentFolderName(This,Path,pbstrResult)

#define IFileSystem_GetFileName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetFileName(This,Path,pbstrResult)

#define IFileSystem_GetBaseName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetBaseName(This,Path,pbstrResult)

#define IFileSystem_GetExtensionName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetExtensionName(This,Path,pbstrResult)

#define IFileSystem_GetAbsolutePathName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetAbsolutePathName(This,Path,pbstrResult)

#define IFileSystem_GetTempName(This,pbstrResult)	\
    (This)->lpVtbl -> GetTempName(This,pbstrResult)

#define IFileSystem_DriveExists(This,DriveSpec,pfExists)	\
    (This)->lpVtbl -> DriveExists(This,DriveSpec,pfExists)

#define IFileSystem_FileExists(This,FileSpec,pfExists)	\
    (This)->lpVtbl -> FileExists(This,FileSpec,pfExists)

#define IFileSystem_FolderExists(This,FolderSpec,pfExists)	\
    (This)->lpVtbl -> FolderExists(This,FolderSpec,pfExists)

#define IFileSystem_GetDrive(This,DriveSpec,ppdrive)	\
    (This)->lpVtbl -> GetDrive(This,DriveSpec,ppdrive)

#define IFileSystem_GetFile(This,FilePath,ppfile)	\
    (This)->lpVtbl -> GetFile(This,FilePath,ppfile)

#define IFileSystem_GetFolder(This,FolderPath,ppfolder)	\
    (This)->lpVtbl -> GetFolder(This,FolderPath,ppfolder)

#define IFileSystem_GetSpecialFolder(This,SpecialFolder,ppfolder)	\
    (This)->lpVtbl -> GetSpecialFolder(This,SpecialFolder,ppfolder)

#define IFileSystem_DeleteFile(This,FileSpec,Force)	\
    (This)->lpVtbl -> DeleteFile(This,FileSpec,Force)

#define IFileSystem_DeleteFolder(This,FolderSpec,Force)	\
    (This)->lpVtbl -> DeleteFolder(This,FolderSpec,Force)

#define IFileSystem_MoveFile(This,Source,Destination)	\
    (This)->lpVtbl -> MoveFile(This,Source,Destination)

#define IFileSystem_MoveFolder(This,Source,Destination)	\
    (This)->lpVtbl -> MoveFolder(This,Source,Destination)

#define IFileSystem_CopyFile(This,Source,Destination,OverWriteFiles)	\
    (This)->lpVtbl -> CopyFile(This,Source,Destination,OverWriteFiles)

#define IFileSystem_CopyFolder(This,Source,Destination,OverWriteFiles)	\
    (This)->lpVtbl -> CopyFolder(This,Source,Destination,OverWriteFiles)

#define IFileSystem_CreateFolder(This,Path,ppfolder)	\
    (This)->lpVtbl -> CreateFolder(This,Path,ppfolder)

#define IFileSystem_CreateTextFile(This,FileName,Overwrite,Unicode,ppts)	\
    (This)->lpVtbl -> CreateTextFile(This,FileName,Overwrite,Unicode,ppts)

#define IFileSystem_OpenTextFile(This,FileName,IOMode,Create,Format,ppts)	\
    (This)->lpVtbl -> OpenTextFile(This,FileName,IOMode,Create,Format,ppts)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_get_Drives_Proxy( 
    IFileSystem * This,
    /* [retval][out] */ IDriveCollection **ppdrives);


void __RPC_STUB IFileSystem_get_Drives_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_BuildPath_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Path,
    /* [in] */ BSTR Name,
    /* [retval][out] */ BSTR *pbstrResult);


void __RPC_STUB IFileSystem_BuildPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetDriveName_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ BSTR *pbstrResult);


void __RPC_STUB IFileSystem_GetDriveName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetParentFolderName_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ BSTR *pbstrResult);


void __RPC_STUB IFileSystem_GetParentFolderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetFileName_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ BSTR *pbstrResult);


void __RPC_STUB IFileSystem_GetFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetBaseName_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ BSTR *pbstrResult);


void __RPC_STUB IFileSystem_GetBaseName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetExtensionName_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ BSTR *pbstrResult);


void __RPC_STUB IFileSystem_GetExtensionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetAbsolutePathName_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ BSTR *pbstrResult);


void __RPC_STUB IFileSystem_GetAbsolutePathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetTempName_Proxy( 
    IFileSystem * This,
    /* [retval][out] */ BSTR *pbstrResult);


void __RPC_STUB IFileSystem_GetTempName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_DriveExists_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR DriveSpec,
    /* [retval][out] */ VARIANT_BOOL *pfExists);


void __RPC_STUB IFileSystem_DriveExists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_FileExists_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR FileSpec,
    /* [retval][out] */ VARIANT_BOOL *pfExists);


void __RPC_STUB IFileSystem_FileExists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_FolderExists_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR FolderSpec,
    /* [retval][out] */ VARIANT_BOOL *pfExists);


void __RPC_STUB IFileSystem_FolderExists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetDrive_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR DriveSpec,
    /* [retval][out] */ IDrive **ppdrive);


void __RPC_STUB IFileSystem_GetDrive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetFile_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR FilePath,
    /* [retval][out] */ IFile **ppfile);


void __RPC_STUB IFileSystem_GetFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetFolder_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR FolderPath,
    /* [retval][out] */ IFolder **ppfolder);


void __RPC_STUB IFileSystem_GetFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_GetSpecialFolder_Proxy( 
    IFileSystem * This,
    /* [in] */ SpecialFolderConst SpecialFolder,
    /* [retval][out] */ IFolder **ppfolder);


void __RPC_STUB IFileSystem_GetSpecialFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_DeleteFile_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR FileSpec,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);


void __RPC_STUB IFileSystem_DeleteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_DeleteFolder_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR FolderSpec,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);


void __RPC_STUB IFileSystem_DeleteFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_MoveFile_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Source,
    /* [in] */ BSTR Destination);


void __RPC_STUB IFileSystem_MoveFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_MoveFolder_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Source,
    /* [in] */ BSTR Destination);


void __RPC_STUB IFileSystem_MoveFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_CopyFile_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Source,
    /* [in] */ BSTR Destination,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);


void __RPC_STUB IFileSystem_CopyFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_CopyFolder_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Source,
    /* [in] */ BSTR Destination,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);


void __RPC_STUB IFileSystem_CopyFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_CreateFolder_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR Path,
    /* [retval][out] */ IFolder **ppfolder);


void __RPC_STUB IFileSystem_CreateFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_CreateTextFile_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR FileName,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL Overwrite,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL Unicode,
    /* [retval][out] */ ITextStream **ppts);


void __RPC_STUB IFileSystem_CreateTextFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem_OpenTextFile_Proxy( 
    IFileSystem * This,
    /* [in] */ BSTR FileName,
    /* [defaultvalue][optional][in] */ IOMode IOMode,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL Create,
    /* [defaultvalue][optional][in] */ Tristate Format,
    /* [retval][out] */ ITextStream **ppts);


void __RPC_STUB IFileSystem_OpenTextFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFileSystem_INTERFACE_DEFINED__ */


#ifndef __IDriveCollection_INTERFACE_DEFINED__
#define __IDriveCollection_INTERFACE_DEFINED__

/* interface IDriveCollection */
/* [object][oleautomation][nonextensible][dual][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IDriveCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7C3F5A1-88A3-11D0-ABCB-00A0C90FFFC0")
    IDriveCollection : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Key,
            /* [retval][out] */ IDrive **ppdrive) = 0;
        
        virtual /* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppenum) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *plCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDriveCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDriveCollection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDriveCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDriveCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDriveCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDriveCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDriveCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDriveCollection * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IDriveCollection * This,
            /* [in] */ VARIANT Key,
            /* [retval][out] */ IDrive **ppdrive);
        
        /* [hidden][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IDriveCollection * This,
            /* [retval][out] */ IUnknown **ppenum);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IDriveCollection * This,
            /* [retval][out] */ long *plCount);
        
        END_INTERFACE
    } IDriveCollectionVtbl;

    interface IDriveCollection
    {
        CONST_VTBL struct IDriveCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDriveCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDriveCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDriveCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDriveCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDriveCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDriveCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDriveCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDriveCollection_get_Item(This,Key,ppdrive)	\
    (This)->lpVtbl -> get_Item(This,Key,ppdrive)

#define IDriveCollection_get__NewEnum(This,ppenum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppenum)

#define IDriveCollection_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDriveCollection_get_Item_Proxy( 
    IDriveCollection * This,
    /* [in] */ VARIANT Key,
    /* [retval][out] */ IDrive **ppdrive);


void __RPC_STUB IDriveCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IDriveCollection_get__NewEnum_Proxy( 
    IDriveCollection * This,
    /* [retval][out] */ IUnknown **ppenum);


void __RPC_STUB IDriveCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDriveCollection_get_Count_Proxy( 
    IDriveCollection * This,
    /* [retval][out] */ long *plCount);


void __RPC_STUB IDriveCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDriveCollection_INTERFACE_DEFINED__ */


#ifndef __IDrive_INTERFACE_DEFINED__
#define __IDrive_INTERFACE_DEFINED__

/* interface IDrive */
/* [object][oleautomation][nonextensible][dual][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IDrive;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7C3F5A0-88A3-11D0-ABCB-00A0C90FFFC0")
    IDrive : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *pbstrPath) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DriveLetter( 
            /* [retval][out] */ BSTR *pbstrLetter) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ShareName( 
            /* [retval][out] */ BSTR *pbstrShareName) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DriveType( 
            /* [retval][out] */ DriveTypeConst *pdt) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_RootFolder( 
            /* [retval][out] */ IFolder **ppfolder) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AvailableSpace( 
            /* [retval][out] */ VARIANT *pvarAvail) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FreeSpace( 
            /* [retval][out] */ VARIANT *pvarFree) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TotalSize( 
            /* [retval][out] */ VARIANT *pvarTotal) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_VolumeName( 
            /* [retval][out] */ BSTR *pbstrName) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_VolumeName( 
            /* [in] */ BSTR pbstrName) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FileSystem( 
            /* [retval][out] */ BSTR *pbstrFileSystem) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SerialNumber( 
            /* [retval][out] */ long *pulSerialNumber) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_IsReady( 
            /* [retval][out] */ VARIANT_BOOL *pfReady) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDriveVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDrive * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDrive * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDrive * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDrive * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDrive * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDrive * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDrive * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IDrive * This,
            /* [retval][out] */ BSTR *pbstrPath);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DriveLetter )( 
            IDrive * This,
            /* [retval][out] */ BSTR *pbstrLetter);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ShareName )( 
            IDrive * This,
            /* [retval][out] */ BSTR *pbstrShareName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DriveType )( 
            IDrive * This,
            /* [retval][out] */ DriveTypeConst *pdt);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RootFolder )( 
            IDrive * This,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AvailableSpace )( 
            IDrive * This,
            /* [retval][out] */ VARIANT *pvarAvail);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FreeSpace )( 
            IDrive * This,
            /* [retval][out] */ VARIANT *pvarFree);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TotalSize )( 
            IDrive * This,
            /* [retval][out] */ VARIANT *pvarTotal);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_VolumeName )( 
            IDrive * This,
            /* [retval][out] */ BSTR *pbstrName);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_VolumeName )( 
            IDrive * This,
            /* [in] */ BSTR pbstrName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FileSystem )( 
            IDrive * This,
            /* [retval][out] */ BSTR *pbstrFileSystem);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_SerialNumber )( 
            IDrive * This,
            /* [retval][out] */ long *pulSerialNumber);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IsReady )( 
            IDrive * This,
            /* [retval][out] */ VARIANT_BOOL *pfReady);
        
        END_INTERFACE
    } IDriveVtbl;

    interface IDrive
    {
        CONST_VTBL struct IDriveVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDrive_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDrive_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDrive_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDrive_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDrive_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDrive_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDrive_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDrive_get_Path(This,pbstrPath)	\
    (This)->lpVtbl -> get_Path(This,pbstrPath)

#define IDrive_get_DriveLetter(This,pbstrLetter)	\
    (This)->lpVtbl -> get_DriveLetter(This,pbstrLetter)

#define IDrive_get_ShareName(This,pbstrShareName)	\
    (This)->lpVtbl -> get_ShareName(This,pbstrShareName)

#define IDrive_get_DriveType(This,pdt)	\
    (This)->lpVtbl -> get_DriveType(This,pdt)

#define IDrive_get_RootFolder(This,ppfolder)	\
    (This)->lpVtbl -> get_RootFolder(This,ppfolder)

#define IDrive_get_AvailableSpace(This,pvarAvail)	\
    (This)->lpVtbl -> get_AvailableSpace(This,pvarAvail)

#define IDrive_get_FreeSpace(This,pvarFree)	\
    (This)->lpVtbl -> get_FreeSpace(This,pvarFree)

#define IDrive_get_TotalSize(This,pvarTotal)	\
    (This)->lpVtbl -> get_TotalSize(This,pvarTotal)

#define IDrive_get_VolumeName(This,pbstrName)	\
    (This)->lpVtbl -> get_VolumeName(This,pbstrName)

#define IDrive_put_VolumeName(This,pbstrName)	\
    (This)->lpVtbl -> put_VolumeName(This,pbstrName)

#define IDrive_get_FileSystem(This,pbstrFileSystem)	\
    (This)->lpVtbl -> get_FileSystem(This,pbstrFileSystem)

#define IDrive_get_SerialNumber(This,pulSerialNumber)	\
    (This)->lpVtbl -> get_SerialNumber(This,pulSerialNumber)

#define IDrive_get_IsReady(This,pfReady)	\
    (This)->lpVtbl -> get_IsReady(This,pfReady)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_Path_Proxy( 
    IDrive * This,
    /* [retval][out] */ BSTR *pbstrPath);


void __RPC_STUB IDrive_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_DriveLetter_Proxy( 
    IDrive * This,
    /* [retval][out] */ BSTR *pbstrLetter);


void __RPC_STUB IDrive_get_DriveLetter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_ShareName_Proxy( 
    IDrive * This,
    /* [retval][out] */ BSTR *pbstrShareName);


void __RPC_STUB IDrive_get_ShareName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_DriveType_Proxy( 
    IDrive * This,
    /* [retval][out] */ DriveTypeConst *pdt);


void __RPC_STUB IDrive_get_DriveType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_RootFolder_Proxy( 
    IDrive * This,
    /* [retval][out] */ IFolder **ppfolder);


void __RPC_STUB IDrive_get_RootFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_AvailableSpace_Proxy( 
    IDrive * This,
    /* [retval][out] */ VARIANT *pvarAvail);


void __RPC_STUB IDrive_get_AvailableSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_FreeSpace_Proxy( 
    IDrive * This,
    /* [retval][out] */ VARIANT *pvarFree);


void __RPC_STUB IDrive_get_FreeSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_TotalSize_Proxy( 
    IDrive * This,
    /* [retval][out] */ VARIANT *pvarTotal);


void __RPC_STUB IDrive_get_TotalSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_VolumeName_Proxy( 
    IDrive * This,
    /* [retval][out] */ BSTR *pbstrName);


void __RPC_STUB IDrive_get_VolumeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IDrive_put_VolumeName_Proxy( 
    IDrive * This,
    /* [in] */ BSTR pbstrName);


void __RPC_STUB IDrive_put_VolumeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_FileSystem_Proxy( 
    IDrive * This,
    /* [retval][out] */ BSTR *pbstrFileSystem);


void __RPC_STUB IDrive_get_FileSystem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_SerialNumber_Proxy( 
    IDrive * This,
    /* [retval][out] */ long *pulSerialNumber);


void __RPC_STUB IDrive_get_SerialNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IDrive_get_IsReady_Proxy( 
    IDrive * This,
    /* [retval][out] */ VARIANT_BOOL *pfReady);


void __RPC_STUB IDrive_get_IsReady_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDrive_INTERFACE_DEFINED__ */


#ifndef __IFolder_INTERFACE_DEFINED__
#define __IFolder_INTERFACE_DEFINED__

/* interface IFolder */
/* [object][oleautomation][nonextensible][dual][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7C3F5A2-88A3-11D0-ABCB-00A0C90FFFC0")
    IFolder : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *pbstrPath) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pbstrName) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR pbstrName) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ShortPath( 
            /* [retval][out] */ BSTR *pbstrPath) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ShortName( 
            /* [retval][out] */ BSTR *pbstrName) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Drive( 
            /* [retval][out] */ IDrive **ppdrive) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ParentFolder( 
            /* [retval][out] */ IFolder **ppfolder) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ FileAttribute *pfa) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Attributes( 
            /* [in] */ FileAttribute pfa) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DateCreated( 
            /* [retval][out] */ DATE *pdate) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DateLastModified( 
            /* [retval][out] */ DATE *pdate) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DateLastAccessed( 
            /* [retval][out] */ DATE *pdate) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR *pbstrType) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force = 0) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles = -1) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Move( 
            /* [in] */ BSTR Destination) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_IsRootFolder( 
            /* [retval][out] */ VARIANT_BOOL *pfRootFolder) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ VARIANT *pvarSize) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SubFolders( 
            /* [retval][out] */ IFolderCollection **ppfolders) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Files( 
            /* [retval][out] */ IFileCollection **ppfiles) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateTextFile( 
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Overwrite,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Unicode,
            /* [retval][out] */ ITextStream **ppts) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFolder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFolder * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFolder * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFolder * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFolder * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IFolder * This,
            /* [retval][out] */ BSTR *pbstrPath);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IFolder * This,
            /* [retval][out] */ BSTR *pbstrName);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IFolder * This,
            /* [in] */ BSTR pbstrName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ShortPath )( 
            IFolder * This,
            /* [retval][out] */ BSTR *pbstrPath);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ShortName )( 
            IFolder * This,
            /* [retval][out] */ BSTR *pbstrName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Drive )( 
            IFolder * This,
            /* [retval][out] */ IDrive **ppdrive);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ParentFolder )( 
            IFolder * This,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Attributes )( 
            IFolder * This,
            /* [retval][out] */ FileAttribute *pfa);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Attributes )( 
            IFolder * This,
            /* [in] */ FileAttribute pfa);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DateCreated )( 
            IFolder * This,
            /* [retval][out] */ DATE *pdate);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DateLastModified )( 
            IFolder * This,
            /* [retval][out] */ DATE *pdate);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DateLastAccessed )( 
            IFolder * This,
            /* [retval][out] */ DATE *pdate);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IFolder * This,
            /* [retval][out] */ BSTR *pbstrType);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IFolder * This,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Copy )( 
            IFolder * This,
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Move )( 
            IFolder * This,
            /* [in] */ BSTR Destination);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IsRootFolder )( 
            IFolder * This,
            /* [retval][out] */ VARIANT_BOOL *pfRootFolder);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            IFolder * This,
            /* [retval][out] */ VARIANT *pvarSize);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_SubFolders )( 
            IFolder * This,
            /* [retval][out] */ IFolderCollection **ppfolders);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Files )( 
            IFolder * This,
            /* [retval][out] */ IFileCollection **ppfiles);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateTextFile )( 
            IFolder * This,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Overwrite,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Unicode,
            /* [retval][out] */ ITextStream **ppts);
        
        END_INTERFACE
    } IFolderVtbl;

    interface IFolder
    {
        CONST_VTBL struct IFolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFolder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFolder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFolder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFolder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFolder_get_Path(This,pbstrPath)	\
    (This)->lpVtbl -> get_Path(This,pbstrPath)

#define IFolder_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)

#define IFolder_put_Name(This,pbstrName)	\
    (This)->lpVtbl -> put_Name(This,pbstrName)

#define IFolder_get_ShortPath(This,pbstrPath)	\
    (This)->lpVtbl -> get_ShortPath(This,pbstrPath)

#define IFolder_get_ShortName(This,pbstrName)	\
    (This)->lpVtbl -> get_ShortName(This,pbstrName)

#define IFolder_get_Drive(This,ppdrive)	\
    (This)->lpVtbl -> get_Drive(This,ppdrive)

#define IFolder_get_ParentFolder(This,ppfolder)	\
    (This)->lpVtbl -> get_ParentFolder(This,ppfolder)

#define IFolder_get_Attributes(This,pfa)	\
    (This)->lpVtbl -> get_Attributes(This,pfa)

#define IFolder_put_Attributes(This,pfa)	\
    (This)->lpVtbl -> put_Attributes(This,pfa)

#define IFolder_get_DateCreated(This,pdate)	\
    (This)->lpVtbl -> get_DateCreated(This,pdate)

#define IFolder_get_DateLastModified(This,pdate)	\
    (This)->lpVtbl -> get_DateLastModified(This,pdate)

#define IFolder_get_DateLastAccessed(This,pdate)	\
    (This)->lpVtbl -> get_DateLastAccessed(This,pdate)

#define IFolder_get_Type(This,pbstrType)	\
    (This)->lpVtbl -> get_Type(This,pbstrType)

#define IFolder_Delete(This,Force)	\
    (This)->lpVtbl -> Delete(This,Force)

#define IFolder_Copy(This,Destination,OverWriteFiles)	\
    (This)->lpVtbl -> Copy(This,Destination,OverWriteFiles)

#define IFolder_Move(This,Destination)	\
    (This)->lpVtbl -> Move(This,Destination)

#define IFolder_get_IsRootFolder(This,pfRootFolder)	\
    (This)->lpVtbl -> get_IsRootFolder(This,pfRootFolder)

#define IFolder_get_Size(This,pvarSize)	\
    (This)->lpVtbl -> get_Size(This,pvarSize)

#define IFolder_get_SubFolders(This,ppfolders)	\
    (This)->lpVtbl -> get_SubFolders(This,ppfolders)

#define IFolder_get_Files(This,ppfiles)	\
    (This)->lpVtbl -> get_Files(This,ppfiles)

#define IFolder_CreateTextFile(This,FileName,Overwrite,Unicode,ppts)	\
    (This)->lpVtbl -> CreateTextFile(This,FileName,Overwrite,Unicode,ppts)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_Path_Proxy( 
    IFolder * This,
    /* [retval][out] */ BSTR *pbstrPath);


void __RPC_STUB IFolder_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_Name_Proxy( 
    IFolder * This,
    /* [retval][out] */ BSTR *pbstrName);


void __RPC_STUB IFolder_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IFolder_put_Name_Proxy( 
    IFolder * This,
    /* [in] */ BSTR pbstrName);


void __RPC_STUB IFolder_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_ShortPath_Proxy( 
    IFolder * This,
    /* [retval][out] */ BSTR *pbstrPath);


void __RPC_STUB IFolder_get_ShortPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_ShortName_Proxy( 
    IFolder * This,
    /* [retval][out] */ BSTR *pbstrName);


void __RPC_STUB IFolder_get_ShortName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_Drive_Proxy( 
    IFolder * This,
    /* [retval][out] */ IDrive **ppdrive);


void __RPC_STUB IFolder_get_Drive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_ParentFolder_Proxy( 
    IFolder * This,
    /* [retval][out] */ IFolder **ppfolder);


void __RPC_STUB IFolder_get_ParentFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_Attributes_Proxy( 
    IFolder * This,
    /* [retval][out] */ FileAttribute *pfa);


void __RPC_STUB IFolder_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IFolder_put_Attributes_Proxy( 
    IFolder * This,
    /* [in] */ FileAttribute pfa);


void __RPC_STUB IFolder_put_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_DateCreated_Proxy( 
    IFolder * This,
    /* [retval][out] */ DATE *pdate);


void __RPC_STUB IFolder_get_DateCreated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_DateLastModified_Proxy( 
    IFolder * This,
    /* [retval][out] */ DATE *pdate);


void __RPC_STUB IFolder_get_DateLastModified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_DateLastAccessed_Proxy( 
    IFolder * This,
    /* [retval][out] */ DATE *pdate);


void __RPC_STUB IFolder_get_DateLastAccessed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_Type_Proxy( 
    IFolder * This,
    /* [retval][out] */ BSTR *pbstrType);


void __RPC_STUB IFolder_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFolder_Delete_Proxy( 
    IFolder * This,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);


void __RPC_STUB IFolder_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFolder_Copy_Proxy( 
    IFolder * This,
    /* [in] */ BSTR Destination,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);


void __RPC_STUB IFolder_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFolder_Move_Proxy( 
    IFolder * This,
    /* [in] */ BSTR Destination);


void __RPC_STUB IFolder_Move_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_IsRootFolder_Proxy( 
    IFolder * This,
    /* [retval][out] */ VARIANT_BOOL *pfRootFolder);


void __RPC_STUB IFolder_get_IsRootFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_Size_Proxy( 
    IFolder * This,
    /* [retval][out] */ VARIANT *pvarSize);


void __RPC_STUB IFolder_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_SubFolders_Proxy( 
    IFolder * This,
    /* [retval][out] */ IFolderCollection **ppfolders);


void __RPC_STUB IFolder_get_SubFolders_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolder_get_Files_Proxy( 
    IFolder * This,
    /* [retval][out] */ IFileCollection **ppfiles);


void __RPC_STUB IFolder_get_Files_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFolder_CreateTextFile_Proxy( 
    IFolder * This,
    /* [in] */ BSTR FileName,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL Overwrite,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL Unicode,
    /* [retval][out] */ ITextStream **ppts);


void __RPC_STUB IFolder_CreateTextFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFolder_INTERFACE_DEFINED__ */


#ifndef __IFolderCollection_INTERFACE_DEFINED__
#define __IFolderCollection_INTERFACE_DEFINED__

/* interface IFolderCollection */
/* [object][oleautomation][nonextensible][dual][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IFolderCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7C3F5A3-88A3-11D0-ABCB-00A0C90FFFC0")
    IFolderCollection : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ IFolder **ppfolder) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Key,
            /* [retval][out] */ IFolder **ppfolder) = 0;
        
        virtual /* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppenum) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *plCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFolderCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFolderCollection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFolderCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFolderCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFolderCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFolderCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFolderCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFolderCollection * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IFolderCollection * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IFolderCollection * This,
            /* [in] */ VARIANT Key,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [hidden][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IFolderCollection * This,
            /* [retval][out] */ IUnknown **ppenum);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IFolderCollection * This,
            /* [retval][out] */ long *plCount);
        
        END_INTERFACE
    } IFolderCollectionVtbl;

    interface IFolderCollection
    {
        CONST_VTBL struct IFolderCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFolderCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFolderCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFolderCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFolderCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFolderCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFolderCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFolderCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFolderCollection_Add(This,Name,ppfolder)	\
    (This)->lpVtbl -> Add(This,Name,ppfolder)

#define IFolderCollection_get_Item(This,Key,ppfolder)	\
    (This)->lpVtbl -> get_Item(This,Key,ppfolder)

#define IFolderCollection_get__NewEnum(This,ppenum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppenum)

#define IFolderCollection_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFolderCollection_Add_Proxy( 
    IFolderCollection * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ IFolder **ppfolder);


void __RPC_STUB IFolderCollection_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolderCollection_get_Item_Proxy( 
    IFolderCollection * This,
    /* [in] */ VARIANT Key,
    /* [retval][out] */ IFolder **ppfolder);


void __RPC_STUB IFolderCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IFolderCollection_get__NewEnum_Proxy( 
    IFolderCollection * This,
    /* [retval][out] */ IUnknown **ppenum);


void __RPC_STUB IFolderCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFolderCollection_get_Count_Proxy( 
    IFolderCollection * This,
    /* [retval][out] */ long *plCount);


void __RPC_STUB IFolderCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFolderCollection_INTERFACE_DEFINED__ */


#ifndef __IFileCollection_INTERFACE_DEFINED__
#define __IFileCollection_INTERFACE_DEFINED__

/* interface IFileCollection */
/* [object][oleautomation][nonextensible][dual][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IFileCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7C3F5A5-88A3-11D0-ABCB-00A0C90FFFC0")
    IFileCollection : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ VARIANT Key,
            /* [retval][out] */ IFile **ppfile) = 0;
        
        virtual /* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown **ppenum) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *plCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFileCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileCollection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileCollection * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFileCollection * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFileCollection * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFileCollection * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFileCollection * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Item )( 
            IFileCollection * This,
            /* [in] */ VARIANT Key,
            /* [retval][out] */ IFile **ppfile);
        
        /* [hidden][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            IFileCollection * This,
            /* [retval][out] */ IUnknown **ppenum);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IFileCollection * This,
            /* [retval][out] */ long *plCount);
        
        END_INTERFACE
    } IFileCollectionVtbl;

    interface IFileCollection
    {
        CONST_VTBL struct IFileCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFileCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFileCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFileCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFileCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFileCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFileCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFileCollection_get_Item(This,Key,ppfile)	\
    (This)->lpVtbl -> get_Item(This,Key,ppfile)

#define IFileCollection_get__NewEnum(This,ppenum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppenum)

#define IFileCollection_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFileCollection_get_Item_Proxy( 
    IFileCollection * This,
    /* [in] */ VARIANT Key,
    /* [retval][out] */ IFile **ppfile);


void __RPC_STUB IFileCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IFileCollection_get__NewEnum_Proxy( 
    IFileCollection * This,
    /* [retval][out] */ IUnknown **ppenum);


void __RPC_STUB IFileCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFileCollection_get_Count_Proxy( 
    IFileCollection * This,
    /* [retval][out] */ long *plCount);


void __RPC_STUB IFileCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFileCollection_INTERFACE_DEFINED__ */


#ifndef __IFile_INTERFACE_DEFINED__
#define __IFile_INTERFACE_DEFINED__

/* interface IFile */
/* [object][oleautomation][nonextensible][dual][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IFile;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7C3F5A4-88A3-11D0-ABCB-00A0C90FFFC0")
    IFile : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *pbstrPath) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pbstrName) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR pbstrName) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ShortPath( 
            /* [retval][out] */ BSTR *pbstrPath) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ShortName( 
            /* [retval][out] */ BSTR *pbstrName) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Drive( 
            /* [retval][out] */ IDrive **ppdrive) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ParentFolder( 
            /* [retval][out] */ IFolder **ppfolder) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
            /* [retval][out] */ FileAttribute *pfa) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Attributes( 
            /* [in] */ FileAttribute pfa) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DateCreated( 
            /* [retval][out] */ DATE *pdate) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DateLastModified( 
            /* [retval][out] */ DATE *pdate) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DateLastAccessed( 
            /* [retval][out] */ DATE *pdate) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ VARIANT *pvarSize) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR *pbstrType) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force = 0) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Copy( 
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles = -1) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Move( 
            /* [in] */ BSTR Destination) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenAsTextStream( 
            /* [defaultvalue][optional][in] */ IOMode IOMode,
            /* [defaultvalue][optional][in] */ Tristate Format,
            /* [retval][out] */ ITextStream **ppts) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFile * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFile * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFile * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFile * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFile * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFile * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFile * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IFile * This,
            /* [retval][out] */ BSTR *pbstrPath);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IFile * This,
            /* [retval][out] */ BSTR *pbstrName);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IFile * This,
            /* [in] */ BSTR pbstrName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ShortPath )( 
            IFile * This,
            /* [retval][out] */ BSTR *pbstrPath);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ShortName )( 
            IFile * This,
            /* [retval][out] */ BSTR *pbstrName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Drive )( 
            IFile * This,
            /* [retval][out] */ IDrive **ppdrive);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ParentFolder )( 
            IFile * This,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Attributes )( 
            IFile * This,
            /* [retval][out] */ FileAttribute *pfa);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Attributes )( 
            IFile * This,
            /* [in] */ FileAttribute pfa);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DateCreated )( 
            IFile * This,
            /* [retval][out] */ DATE *pdate);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DateLastModified )( 
            IFile * This,
            /* [retval][out] */ DATE *pdate);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DateLastAccessed )( 
            IFile * This,
            /* [retval][out] */ DATE *pdate);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            IFile * This,
            /* [retval][out] */ VARIANT *pvarSize);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IFile * This,
            /* [retval][out] */ BSTR *pbstrType);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IFile * This,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Copy )( 
            IFile * This,
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Move )( 
            IFile * This,
            /* [in] */ BSTR Destination);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenAsTextStream )( 
            IFile * This,
            /* [defaultvalue][optional][in] */ IOMode IOMode,
            /* [defaultvalue][optional][in] */ Tristate Format,
            /* [retval][out] */ ITextStream **ppts);
        
        END_INTERFACE
    } IFileVtbl;

    interface IFile
    {
        CONST_VTBL struct IFileVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFile_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFile_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFile_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFile_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFile_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFile_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFile_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFile_get_Path(This,pbstrPath)	\
    (This)->lpVtbl -> get_Path(This,pbstrPath)

#define IFile_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)

#define IFile_put_Name(This,pbstrName)	\
    (This)->lpVtbl -> put_Name(This,pbstrName)

#define IFile_get_ShortPath(This,pbstrPath)	\
    (This)->lpVtbl -> get_ShortPath(This,pbstrPath)

#define IFile_get_ShortName(This,pbstrName)	\
    (This)->lpVtbl -> get_ShortName(This,pbstrName)

#define IFile_get_Drive(This,ppdrive)	\
    (This)->lpVtbl -> get_Drive(This,ppdrive)

#define IFile_get_ParentFolder(This,ppfolder)	\
    (This)->lpVtbl -> get_ParentFolder(This,ppfolder)

#define IFile_get_Attributes(This,pfa)	\
    (This)->lpVtbl -> get_Attributes(This,pfa)

#define IFile_put_Attributes(This,pfa)	\
    (This)->lpVtbl -> put_Attributes(This,pfa)

#define IFile_get_DateCreated(This,pdate)	\
    (This)->lpVtbl -> get_DateCreated(This,pdate)

#define IFile_get_DateLastModified(This,pdate)	\
    (This)->lpVtbl -> get_DateLastModified(This,pdate)

#define IFile_get_DateLastAccessed(This,pdate)	\
    (This)->lpVtbl -> get_DateLastAccessed(This,pdate)

#define IFile_get_Size(This,pvarSize)	\
    (This)->lpVtbl -> get_Size(This,pvarSize)

#define IFile_get_Type(This,pbstrType)	\
    (This)->lpVtbl -> get_Type(This,pbstrType)

#define IFile_Delete(This,Force)	\
    (This)->lpVtbl -> Delete(This,Force)

#define IFile_Copy(This,Destination,OverWriteFiles)	\
    (This)->lpVtbl -> Copy(This,Destination,OverWriteFiles)

#define IFile_Move(This,Destination)	\
    (This)->lpVtbl -> Move(This,Destination)

#define IFile_OpenAsTextStream(This,IOMode,Format,ppts)	\
    (This)->lpVtbl -> OpenAsTextStream(This,IOMode,Format,ppts)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_Path_Proxy( 
    IFile * This,
    /* [retval][out] */ BSTR *pbstrPath);


void __RPC_STUB IFile_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_Name_Proxy( 
    IFile * This,
    /* [retval][out] */ BSTR *pbstrName);


void __RPC_STUB IFile_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IFile_put_Name_Proxy( 
    IFile * This,
    /* [in] */ BSTR pbstrName);


void __RPC_STUB IFile_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_ShortPath_Proxy( 
    IFile * This,
    /* [retval][out] */ BSTR *pbstrPath);


void __RPC_STUB IFile_get_ShortPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_ShortName_Proxy( 
    IFile * This,
    /* [retval][out] */ BSTR *pbstrName);


void __RPC_STUB IFile_get_ShortName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_Drive_Proxy( 
    IFile * This,
    /* [retval][out] */ IDrive **ppdrive);


void __RPC_STUB IFile_get_Drive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_ParentFolder_Proxy( 
    IFile * This,
    /* [retval][out] */ IFolder **ppfolder);


void __RPC_STUB IFile_get_ParentFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_Attributes_Proxy( 
    IFile * This,
    /* [retval][out] */ FileAttribute *pfa);


void __RPC_STUB IFile_get_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IFile_put_Attributes_Proxy( 
    IFile * This,
    /* [in] */ FileAttribute pfa);


void __RPC_STUB IFile_put_Attributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_DateCreated_Proxy( 
    IFile * This,
    /* [retval][out] */ DATE *pdate);


void __RPC_STUB IFile_get_DateCreated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_DateLastModified_Proxy( 
    IFile * This,
    /* [retval][out] */ DATE *pdate);


void __RPC_STUB IFile_get_DateLastModified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_DateLastAccessed_Proxy( 
    IFile * This,
    /* [retval][out] */ DATE *pdate);


void __RPC_STUB IFile_get_DateLastAccessed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_Size_Proxy( 
    IFile * This,
    /* [retval][out] */ VARIANT *pvarSize);


void __RPC_STUB IFile_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFile_get_Type_Proxy( 
    IFile * This,
    /* [retval][out] */ BSTR *pbstrType);


void __RPC_STUB IFile_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFile_Delete_Proxy( 
    IFile * This,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);


void __RPC_STUB IFile_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFile_Copy_Proxy( 
    IFile * This,
    /* [in] */ BSTR Destination,
    /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);


void __RPC_STUB IFile_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFile_Move_Proxy( 
    IFile * This,
    /* [in] */ BSTR Destination);


void __RPC_STUB IFile_Move_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IFile_OpenAsTextStream_Proxy( 
    IFile * This,
    /* [defaultvalue][optional][in] */ IOMode IOMode,
    /* [defaultvalue][optional][in] */ Tristate Format,
    /* [retval][out] */ ITextStream **ppts);


void __RPC_STUB IFile_OpenAsTextStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFile_INTERFACE_DEFINED__ */


#ifndef __ITextStream_INTERFACE_DEFINED__
#define __ITextStream_INTERFACE_DEFINED__

/* interface ITextStream */
/* [object][oleautomation][nonextensible][dual][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_ITextStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("53BAD8C1-E718-11CF-893D-00A0C9054228")
    ITextStream : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Line( 
            /* [retval][out] */ long *Line) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Column( 
            /* [retval][out] */ long *Column) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AtEndOfStream( 
            /* [retval][out] */ VARIANT_BOOL *EOS) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AtEndOfLine( 
            /* [retval][out] */ VARIANT_BOOL *EOL) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Read( 
            /* [in] */ long Characters,
            /* [retval][out] */ BSTR *Text) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadLine( 
            /* [retval][out] */ BSTR *Text) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadAll( 
            /* [retval][out] */ BSTR *Text) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Write( 
            /* [in] */ BSTR Text) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteLine( 
            /* [defaultvalue][optional][in] */ BSTR Text = L"") = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteBlankLines( 
            /* [in] */ long Lines) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ long Characters) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE SkipLine( void) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITextStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITextStream * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITextStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITextStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ITextStream * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ITextStream * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ITextStream * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ITextStream * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Line )( 
            ITextStream * This,
            /* [retval][out] */ long *Line);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Column )( 
            ITextStream * This,
            /* [retval][out] */ long *Column);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AtEndOfStream )( 
            ITextStream * This,
            /* [retval][out] */ VARIANT_BOOL *EOS);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AtEndOfLine )( 
            ITextStream * This,
            /* [retval][out] */ VARIANT_BOOL *EOL);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Read )( 
            ITextStream * This,
            /* [in] */ long Characters,
            /* [retval][out] */ BSTR *Text);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadLine )( 
            ITextStream * This,
            /* [retval][out] */ BSTR *Text);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadAll )( 
            ITextStream * This,
            /* [retval][out] */ BSTR *Text);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Write )( 
            ITextStream * This,
            /* [in] */ BSTR Text);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteLine )( 
            ITextStream * This,
            /* [defaultvalue][optional][in] */ BSTR Text);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteBlankLines )( 
            ITextStream * This,
            /* [in] */ long Lines);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Skip )( 
            ITextStream * This,
            /* [in] */ long Characters);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SkipLine )( 
            ITextStream * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            ITextStream * This);
        
        END_INTERFACE
    } ITextStreamVtbl;

    interface ITextStream
    {
        CONST_VTBL struct ITextStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITextStream_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITextStream_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITextStream_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITextStream_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITextStream_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITextStream_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITextStream_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITextStream_get_Line(This,Line)	\
    (This)->lpVtbl -> get_Line(This,Line)

#define ITextStream_get_Column(This,Column)	\
    (This)->lpVtbl -> get_Column(This,Column)

#define ITextStream_get_AtEndOfStream(This,EOS)	\
    (This)->lpVtbl -> get_AtEndOfStream(This,EOS)

#define ITextStream_get_AtEndOfLine(This,EOL)	\
    (This)->lpVtbl -> get_AtEndOfLine(This,EOL)

#define ITextStream_Read(This,Characters,Text)	\
    (This)->lpVtbl -> Read(This,Characters,Text)

#define ITextStream_ReadLine(This,Text)	\
    (This)->lpVtbl -> ReadLine(This,Text)

#define ITextStream_ReadAll(This,Text)	\
    (This)->lpVtbl -> ReadAll(This,Text)

#define ITextStream_Write(This,Text)	\
    (This)->lpVtbl -> Write(This,Text)

#define ITextStream_WriteLine(This,Text)	\
    (This)->lpVtbl -> WriteLine(This,Text)

#define ITextStream_WriteBlankLines(This,Lines)	\
    (This)->lpVtbl -> WriteBlankLines(This,Lines)

#define ITextStream_Skip(This,Characters)	\
    (This)->lpVtbl -> Skip(This,Characters)

#define ITextStream_SkipLine(This)	\
    (This)->lpVtbl -> SkipLine(This)

#define ITextStream_Close(This)	\
    (This)->lpVtbl -> Close(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ITextStream_get_Line_Proxy( 
    ITextStream * This,
    /* [retval][out] */ long *Line);


void __RPC_STUB ITextStream_get_Line_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ITextStream_get_Column_Proxy( 
    ITextStream * This,
    /* [retval][out] */ long *Column);


void __RPC_STUB ITextStream_get_Column_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ITextStream_get_AtEndOfStream_Proxy( 
    ITextStream * This,
    /* [retval][out] */ VARIANT_BOOL *EOS);


void __RPC_STUB ITextStream_get_AtEndOfStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ITextStream_get_AtEndOfLine_Proxy( 
    ITextStream * This,
    /* [retval][out] */ VARIANT_BOOL *EOL);


void __RPC_STUB ITextStream_get_AtEndOfLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ITextStream_Read_Proxy( 
    ITextStream * This,
    /* [in] */ long Characters,
    /* [retval][out] */ BSTR *Text);


void __RPC_STUB ITextStream_Read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ITextStream_ReadLine_Proxy( 
    ITextStream * This,
    /* [retval][out] */ BSTR *Text);


void __RPC_STUB ITextStream_ReadLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ITextStream_ReadAll_Proxy( 
    ITextStream * This,
    /* [retval][out] */ BSTR *Text);


void __RPC_STUB ITextStream_ReadAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ITextStream_Write_Proxy( 
    ITextStream * This,
    /* [in] */ BSTR Text);


void __RPC_STUB ITextStream_Write_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ITextStream_WriteLine_Proxy( 
    ITextStream * This,
    /* [defaultvalue][optional][in] */ BSTR Text);


void __RPC_STUB ITextStream_WriteLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ITextStream_WriteBlankLines_Proxy( 
    ITextStream * This,
    /* [in] */ long Lines);


void __RPC_STUB ITextStream_WriteBlankLines_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ITextStream_Skip_Proxy( 
    ITextStream * This,
    /* [in] */ long Characters);


void __RPC_STUB ITextStream_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ITextStream_SkipLine_Proxy( 
    ITextStream * This);


void __RPC_STUB ITextStream_SkipLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ITextStream_Close_Proxy( 
    ITextStream * This);


void __RPC_STUB ITextStream_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITextStream_INTERFACE_DEFINED__ */


#ifndef __IFileSystem3_INTERFACE_DEFINED__
#define __IFileSystem3_INTERFACE_DEFINED__

/* interface IFileSystem3 */
/* [object][oleautomation][nonextensible][dual][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IFileSystem3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A0B9D10-4B87-11D3-A97A-00104B365C9F")
    IFileSystem3 : public IFileSystem
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetStandardStream( 
            /* [in] */ StandardStreamTypes StandardStreamType,
            /* [optional][in] */ VARIANT_BOOL Unicode,
            /* [retval][out] */ ITextStream **ppts) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFileVersion( 
            /* [in] */ BSTR FileName,
            /* [retval][out] */ BSTR *FileVersion) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFileSystem3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileSystem3 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileSystem3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileSystem3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFileSystem3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFileSystem3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFileSystem3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFileSystem3 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Drives )( 
            IFileSystem3 * This,
            /* [retval][out] */ IDriveCollection **ppdrives);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BuildPath )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Path,
            /* [in] */ BSTR Name,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDriveName )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParentFolderName )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFileName )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBaseName )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetExtensionName )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAbsolutePathName )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetTempName )( 
            IFileSystem3 * This,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DriveExists )( 
            IFileSystem3 * This,
            /* [in] */ BSTR DriveSpec,
            /* [retval][out] */ VARIANT_BOOL *pfExists);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FileExists )( 
            IFileSystem3 * This,
            /* [in] */ BSTR FileSpec,
            /* [retval][out] */ VARIANT_BOOL *pfExists);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FolderExists )( 
            IFileSystem3 * This,
            /* [in] */ BSTR FolderSpec,
            /* [retval][out] */ VARIANT_BOOL *pfExists);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDrive )( 
            IFileSystem3 * This,
            /* [in] */ BSTR DriveSpec,
            /* [retval][out] */ IDrive **ppdrive);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFile )( 
            IFileSystem3 * This,
            /* [in] */ BSTR FilePath,
            /* [retval][out] */ IFile **ppfile);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFolder )( 
            IFileSystem3 * This,
            /* [in] */ BSTR FolderPath,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSpecialFolder )( 
            IFileSystem3 * This,
            /* [in] */ SpecialFolderConst SpecialFolder,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteFile )( 
            IFileSystem3 * This,
            /* [in] */ BSTR FileSpec,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteFolder )( 
            IFileSystem3 * This,
            /* [in] */ BSTR FolderSpec,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Force);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveFile )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveFolder )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyFile )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyFolder )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Source,
            /* [in] */ BSTR Destination,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL OverWriteFiles);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateFolder )( 
            IFileSystem3 * This,
            /* [in] */ BSTR Path,
            /* [retval][out] */ IFolder **ppfolder);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateTextFile )( 
            IFileSystem3 * This,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Overwrite,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Unicode,
            /* [retval][out] */ ITextStream **ppts);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenTextFile )( 
            IFileSystem3 * This,
            /* [in] */ BSTR FileName,
            /* [defaultvalue][optional][in] */ IOMode IOMode,
            /* [defaultvalue][optional][in] */ VARIANT_BOOL Create,
            /* [defaultvalue][optional][in] */ Tristate Format,
            /* [retval][out] */ ITextStream **ppts);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetStandardStream )( 
            IFileSystem3 * This,
            /* [in] */ StandardStreamTypes StandardStreamType,
            /* [optional][in] */ VARIANT_BOOL Unicode,
            /* [retval][out] */ ITextStream **ppts);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFileVersion )( 
            IFileSystem3 * This,
            /* [in] */ BSTR FileName,
            /* [retval][out] */ BSTR *FileVersion);
        
        END_INTERFACE
    } IFileSystem3Vtbl;

    interface IFileSystem3
    {
        CONST_VTBL struct IFileSystem3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileSystem3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFileSystem3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFileSystem3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFileSystem3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFileSystem3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFileSystem3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFileSystem3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFileSystem3_get_Drives(This,ppdrives)	\
    (This)->lpVtbl -> get_Drives(This,ppdrives)

#define IFileSystem3_BuildPath(This,Path,Name,pbstrResult)	\
    (This)->lpVtbl -> BuildPath(This,Path,Name,pbstrResult)

#define IFileSystem3_GetDriveName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetDriveName(This,Path,pbstrResult)

#define IFileSystem3_GetParentFolderName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetParentFolderName(This,Path,pbstrResult)

#define IFileSystem3_GetFileName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetFileName(This,Path,pbstrResult)

#define IFileSystem3_GetBaseName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetBaseName(This,Path,pbstrResult)

#define IFileSystem3_GetExtensionName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetExtensionName(This,Path,pbstrResult)

#define IFileSystem3_GetAbsolutePathName(This,Path,pbstrResult)	\
    (This)->lpVtbl -> GetAbsolutePathName(This,Path,pbstrResult)

#define IFileSystem3_GetTempName(This,pbstrResult)	\
    (This)->lpVtbl -> GetTempName(This,pbstrResult)

#define IFileSystem3_DriveExists(This,DriveSpec,pfExists)	\
    (This)->lpVtbl -> DriveExists(This,DriveSpec,pfExists)

#define IFileSystem3_FileExists(This,FileSpec,pfExists)	\
    (This)->lpVtbl -> FileExists(This,FileSpec,pfExists)

#define IFileSystem3_FolderExists(This,FolderSpec,pfExists)	\
    (This)->lpVtbl -> FolderExists(This,FolderSpec,pfExists)

#define IFileSystem3_GetDrive(This,DriveSpec,ppdrive)	\
    (This)->lpVtbl -> GetDrive(This,DriveSpec,ppdrive)

#define IFileSystem3_GetFile(This,FilePath,ppfile)	\
    (This)->lpVtbl -> GetFile(This,FilePath,ppfile)

#define IFileSystem3_GetFolder(This,FolderPath,ppfolder)	\
    (This)->lpVtbl -> GetFolder(This,FolderPath,ppfolder)

#define IFileSystem3_GetSpecialFolder(This,SpecialFolder,ppfolder)	\
    (This)->lpVtbl -> GetSpecialFolder(This,SpecialFolder,ppfolder)

#define IFileSystem3_DeleteFile(This,FileSpec,Force)	\
    (This)->lpVtbl -> DeleteFile(This,FileSpec,Force)

#define IFileSystem3_DeleteFolder(This,FolderSpec,Force)	\
    (This)->lpVtbl -> DeleteFolder(This,FolderSpec,Force)

#define IFileSystem3_MoveFile(This,Source,Destination)	\
    (This)->lpVtbl -> MoveFile(This,Source,Destination)

#define IFileSystem3_MoveFolder(This,Source,Destination)	\
    (This)->lpVtbl -> MoveFolder(This,Source,Destination)

#define IFileSystem3_CopyFile(This,Source,Destination,OverWriteFiles)	\
    (This)->lpVtbl -> CopyFile(This,Source,Destination,OverWriteFiles)

#define IFileSystem3_CopyFolder(This,Source,Destination,OverWriteFiles)	\
    (This)->lpVtbl -> CopyFolder(This,Source,Destination,OverWriteFiles)

#define IFileSystem3_CreateFolder(This,Path,ppfolder)	\
    (This)->lpVtbl -> CreateFolder(This,Path,ppfolder)

#define IFileSystem3_CreateTextFile(This,FileName,Overwrite,Unicode,ppts)	\
    (This)->lpVtbl -> CreateTextFile(This,FileName,Overwrite,Unicode,ppts)

#define IFileSystem3_OpenTextFile(This,FileName,IOMode,Create,Format,ppts)	\
    (This)->lpVtbl -> OpenTextFile(This,FileName,IOMode,Create,Format,ppts)


#define IFileSystem3_GetStandardStream(This,StandardStreamType,Unicode,ppts)	\
    (This)->lpVtbl -> GetStandardStream(This,StandardStreamType,Unicode,ppts)

#define IFileSystem3_GetFileVersion(This,FileName,FileVersion)	\
    (This)->lpVtbl -> GetFileVersion(This,FileName,FileVersion)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem3_GetStandardStream_Proxy( 
    IFileSystem3 * This,
    /* [in] */ StandardStreamTypes StandardStreamType,
    /* [optional][in] */ VARIANT_BOOL Unicode,
    /* [retval][out] */ ITextStream **ppts);


void __RPC_STUB IFileSystem3_GetStandardStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSystem3_GetFileVersion_Proxy( 
    IFileSystem3 * This,
    /* [in] */ BSTR FileName,
    /* [retval][out] */ BSTR *FileVersion);


void __RPC_STUB IFileSystem3_GetFileVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFileSystem3_INTERFACE_DEFINED__ */


#ifndef __IScriptEncoder_INTERFACE_DEFINED__
#define __IScriptEncoder_INTERFACE_DEFINED__

/* interface IScriptEncoder */
/* [object][oleautomation][dual][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IScriptEncoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AADC65F6-CFF1-11D1-B747-00C04FC2B085")
    IScriptEncoder : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE EncodeScriptFile( 
            /* [in] */ BSTR szExt,
            /* [in] */ BSTR bstrStreamIn,
            /* [in] */ long cFlags,
            /* [in] */ BSTR bstrDefaultLang,
            /* [retval][out] */ BSTR *pbstrStreamOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScriptEncoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScriptEncoder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScriptEncoder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScriptEncoder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScriptEncoder * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScriptEncoder * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScriptEncoder * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScriptEncoder * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EncodeScriptFile )( 
            IScriptEncoder * This,
            /* [in] */ BSTR szExt,
            /* [in] */ BSTR bstrStreamIn,
            /* [in] */ long cFlags,
            /* [in] */ BSTR bstrDefaultLang,
            /* [retval][out] */ BSTR *pbstrStreamOut);
        
        END_INTERFACE
    } IScriptEncoderVtbl;

    interface IScriptEncoder
    {
        CONST_VTBL struct IScriptEncoderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScriptEncoder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScriptEncoder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScriptEncoder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScriptEncoder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScriptEncoder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScriptEncoder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScriptEncoder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScriptEncoder_EncodeScriptFile(This,szExt,bstrStreamIn,cFlags,bstrDefaultLang,pbstrStreamOut)	\
    (This)->lpVtbl -> EncodeScriptFile(This,szExt,bstrStreamIn,cFlags,bstrDefaultLang,pbstrStreamOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IScriptEncoder_EncodeScriptFile_Proxy( 
    IScriptEncoder * This,
    /* [in] */ BSTR szExt,
    /* [in] */ BSTR bstrStreamIn,
    /* [in] */ long cFlags,
    /* [in] */ BSTR bstrDefaultLang,
    /* [retval][out] */ BSTR *pbstrStreamOut);


void __RPC_STUB IScriptEncoder_EncodeScriptFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScriptEncoder_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Dictionary;

#ifdef __cplusplus

class DECLSPEC_UUID("EE09B103-97E0-11CF-978F-00A02463E06F")
Dictionary;
#endif

EXTERN_C const CLSID CLSID_FileSystemObject;

#ifdef __cplusplus

class DECLSPEC_UUID("0D43FE01-F093-11CF-8940-00A0C9054228")
FileSystemObject;
#endif

EXTERN_C const CLSID CLSID_Drive;

#ifdef __cplusplus

class DECLSPEC_UUID("C7C3F5B1-88A3-11D0-ABCB-00A0C90FFFC0")
Drive;
#endif

EXTERN_C const CLSID CLSID_Drives;

#ifdef __cplusplus

class DECLSPEC_UUID("C7C3F5B2-88A3-11D0-ABCB-00A0C90FFFC0")
Drives;
#endif

EXTERN_C const CLSID CLSID_Folder;

#ifdef __cplusplus

class DECLSPEC_UUID("C7C3F5B3-88A3-11D0-ABCB-00A0C90FFFC0")
Folder;
#endif

EXTERN_C const CLSID CLSID_Folders;

#ifdef __cplusplus

class DECLSPEC_UUID("C7C3F5B4-88A3-11D0-ABCB-00A0C90FFFC0")
Folders;
#endif

EXTERN_C const CLSID CLSID_File;

#ifdef __cplusplus

class DECLSPEC_UUID("C7C3F5B5-88A3-11D0-ABCB-00A0C90FFFC0")
File;
#endif

EXTERN_C const CLSID CLSID_Files;

#ifdef __cplusplus

class DECLSPEC_UUID("C7C3F5B6-88A3-11D0-ABCB-00A0C90FFFC0")
Files;
#endif

EXTERN_C const CLSID CLSID_TextStream;

#ifdef __cplusplus

class DECLSPEC_UUID("0BB02EC0-EF49-11CF-8940-00A0C9054228")
TextStream;
#endif

EXTERN_C const CLSID CLSID_Encoder;

#ifdef __cplusplus

class DECLSPEC_UUID("32DA2B15-CFED-11D1-B747-00C04FC2B085")
Encoder;
#endif
#endif /* __Scripting_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


