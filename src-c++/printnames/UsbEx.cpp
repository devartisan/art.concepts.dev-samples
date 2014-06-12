////////////////////////////////////////////////////////////////////////////////
//
// TpeUsbEx.cpp : Defines the entry point for the DLL application.
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"		//
#include "UsbEx.h"	//

#include <objbase.h>	// 
#include <initguid.h>	// 
#include <Winioctl.h>	//
#include <usbioctl.h>	//
#include <Setupapi.h>	//
//#include <hidsdi.h>		//

/*
USB specific GUIDs
*/


/* f18a0e88-c30c-11d0-8815-00a0c906bed8 */
DEFINE_GUID(GUID_DEVINTERFACE_USB_HUB, 0xf18a0e88, 0xc30c, 0x11d0, 0x88, 0x15, 0x00, \
0xa0, 0xc9, 0x06, 0xbe, 0xd8);

/* A5DCBF10-6530-11D2-901F-00C04FB951ED */
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, \
0xC0, 0x4F, 0xB9, 0x51, 0xED);

/* 3ABF6F2D-71C4-462a-8A92-1E6861E6AF27 */
DEFINE_GUID(GUID_DEVINTERFACE_USB_HOST_CONTROLLER, 0x3abf6f2d, 0x71c4, 0x462a, 0x8a, 0x92, 0x1e, \
0x68, 0x61, 0xe6, 0xaf, 0x27);

/* 4E623B20-CB14-11D1-B331-00A0C959BBD2 */
DEFINE_GUID(GUID_USB_WMI_STD_DATA, 0x4E623B20L, 0xCB14, 0x11D1, 0xB3, 0x31, 0x00,\
0xA0, 0xC9, 0x59, 0xBB, 0xD2);

/* 4E623B20-CB14-11D1-B331-00A0C959BBD2 */
DEFINE_GUID(GUID_USB_WMI_STD_NOTIFICATION, 0x4E623B20L, 0xCB14, 0x11D1, 0xB3, 0x31, 0x00,\
0xA0, 0xC9, 0x59, 0xBB, 0xD2);


/*
Obsolete device interface class GUID names.
(use of above GUID_DEVINTERFACE_* names is recommended).
--*/

#define GUID_CLASS_USBHUB GUID_DEVINTERFACE_USB_HUB
#define GUID_CLASS_USB_DEVICE GUID_DEVINTERFACE_USB_DEVICE
#define GUID_CLASS_USB_HOST_CONTROLLER GUID_DEVINTERFACE_USB_HOST_CONTROLLER

#define FILE_DEVICE_USB FILE_DEVICE_UNKNOWN

#define IOCTL_USBPRINT_GET_1284_ID     CTL_CODE(FILE_DEVICE_UNKNOWN, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)


////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
static void InitError( USB_ERROR_INFO *lpUsbErrorInfo );
static void AssignPtr( char *dst, char *src );
static void RTrim( char *s );

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//	G e t U S B D e v i c e s
////////////////////////////////////////////////////////////////////////////////
INT GetUSBDevices( INT nMaxDevices, USB_DEVICE_NAME *lpUsbDevices, USB_ERROR_INFO *lpUsbErrorInfo )
{
	int									ii;
	DWORD								dwBytes;
	INT									nDeviceFound;
	HDEVINFO							hDevInfo;
	LPTSTR								lpToken, lpWorkToken;
	GUID								guidUSBDeviceSupport;
	HANDLE								hUsbPrinter;
	SP_DEVICE_INTERFACE_DATA			deviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	deviceInterfaceDetailData;
	char								szUsb1284[1024];
	int									nDeviceIndex;
	HINSTANCE							pSetupAPI					= NULL;

	FAR HDEVINFO ( FAR WINAPI *SetupDiGetClassDevs )(CONST GUID *, PCTSTR, HWND, DWORD);
	FAR BOOL ( FAR WINAPI *SetupDiEnumDeviceInterfaces )(HDEVINFO, PSP_DEVINFO_DATA, CONST GUID *,DWORD, PSP_DEVICE_INTERFACE_DATA);
	FAR BOOL ( FAR WINAPI *SetupDiGetDeviceInterfaceDetail )(HDEVINFO, PSP_DEVICE_INTERFACE_DATA, PSP_DEVICE_INTERFACE_DETAIL_DATA, DWORD, PDWORD, PSP_DEVINFO_DATA);
	FAR BOOL ( FAR WINAPI *SetupDiDestroyDeviceInfoList )(HDEVINFO);



	//
	// The functions SetupDiGetClassDevs, SetupDiEnumDeviceInterfaces , 
	// SetupDiGetDeviceInterfaceDetail, SetupDiDestroyDeviceInfoList
	// are supported only on Windows 2000 and above and Windows 98 and above.
	// So link them dynamically to avoid windows errors.
	//
	pSetupAPI = LoadLibrary( "Setupapi" );

	if ( pSetupAPI != NULL )
	{

		SetupDiGetClassDevs = ( HDEVINFO ( WINAPI * )(CONST GUID *, PCTSTR, HWND, DWORD)) GetProcAddress( pSetupAPI, "SetupDiGetClassDevsA" );

		if ( SetupDiGetClassDevs == NULL )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_FUNCTION;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to get a pointer to SetupDiGetClassDevs");
		    FreeLibrary( pSetupAPI );
			return 0;
		}

		SetupDiEnumDeviceInterfaces = ( BOOL ( WINAPI * )(HDEVINFO, PSP_DEVINFO_DATA, CONST GUID *,DWORD, PSP_DEVICE_INTERFACE_DATA)) GetProcAddress( pSetupAPI, "SetupDiEnumDeviceInterfaces" );

		if ( SetupDiEnumDeviceInterfaces == NULL )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_FUNCTION;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to get a pointer to SetupDiEnumDeviceInterfaces");
		    FreeLibrary( pSetupAPI );
			return 0;
		}

		SetupDiGetDeviceInterfaceDetail = ( BOOL ( WINAPI * )(HDEVINFO, PSP_DEVICE_INTERFACE_DATA, PSP_DEVICE_INTERFACE_DETAIL_DATA, DWORD, PDWORD, PSP_DEVINFO_DATA)) GetProcAddress( pSetupAPI, "SetupDiGetDeviceInterfaceDetailA" );

		if ( SetupDiGetDeviceInterfaceDetail == NULL )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_FUNCTION;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to get a pointer to SetupDiGetDeviceInterfaceDetailA");
		    FreeLibrary( pSetupAPI );
			return 0;
		}

		SetupDiDestroyDeviceInfoList = ( BOOL ( WINAPI * )(HDEVINFO)) GetProcAddress( pSetupAPI, "SetupDiDestroyDeviceInfoList" );

		if ( SetupDiDestroyDeviceInfoList == NULL )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_FUNCTION;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to get a pointer to SetupDiDestroyDeviceInfoList");
		    FreeLibrary( pSetupAPI );
			return 0;
		}
	}
	else
	{
		lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_HANDLE;
		strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to load Setupapi!");
		return 0;
	}



	//
	// initialize local variables and initialize error
	// struct if a non NULL pointer was passed.
	// 
	nDeviceFound = 0;
	if ( NULL != lpUsbErrorInfo )
		InitError( lpUsbErrorInfo );


	//
	// validate that the user passed in the number of
	// available USE_DEVICE_NAMES to fill with inofrmation.
	//
	if ( 0 == nMaxDevices )
	{	
		if ( NULL != lpUsbErrorInfo )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_DATA;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : nMaxDevices has a value of zero!");
		}
		return 0;
	};

	
	
	//
	// check that we can pass information back to the calling application!
	//
	if ( NULL == lpUsbDevices )
	{
		if ( NULL != lpUsbErrorInfo )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_DATA;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : lpUsbDevices is NULL!" );
		}
		return 0;
	}



	//
	// get a handle to the set of device information returned by the Win32 API
	// functions.  This handle points to an object that is then operated on 
	// by the SetupDiEnumDeviceInterfaces function.
	//
	guidUSBDeviceSupport = GUID_CLASS_USB_DEVICE;
	hDevInfo = SetupDiGetClassDevs(	&guidUSBDeviceSupport,
									0,
									0,
									0x12);	// TODO : Determine the constant
	if ( INVALID_HANDLE_VALUE == hDevInfo )
	{
		if ( NULL != lpUsbErrorInfo )
		{
			// get the last error from system
			lpUsbErrorInfo->dwSysLastError = GetLastError();

			// get the system defined message
			FormatMessage(	FORMAT_MESSAGE_FROM_SYSTEM			|
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL,
							lpUsbErrorInfo->dwSysLastError,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
							lpUsbErrorInfo->szSysErrorMsg,
							sizeof(lpUsbErrorInfo->szSysErrorMsg) / sizeof(lpUsbErrorInfo->szSysErrorMsg[0]),
							NULL );
		}
	}


	//
	//
	//
	nDeviceFound = nDeviceIndex = 0;	
	deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
	for (nDeviceIndex = 0; SetupDiEnumDeviceInterfaces( hDevInfo,
									  NULL,
									  &guidUSBDeviceSupport,
									  nDeviceIndex,
									  &deviceInterfaceData );
		nDeviceIndex++) 
	{



		//
		// determine the amout of memory we need to allocate for the next
		// call to this function
		//
		dwBytes= 0;
		ii = SetupDiGetDeviceInterfaceDetail(	hDevInfo,
												&deviceInterfaceData,
												NULL,
												0,
												&dwBytes,
												NULL );
		if ( 0 == dwBytes )
		{
			//
			// Reinitialize the data for next pass
			// through this loop.
			//
			deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
			continue;
		}


		
		//Set cbSize in the detailData structure.
		deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc (dwBytes);
		deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			
		//
		// get the device path for the current device we are enumerating over.
		//
		ii = SetupDiGetDeviceInterfaceDetail(	hDevInfo,
												&deviceInterfaceData,
												deviceInterfaceDetailData,
												dwBytes, 
												&dwBytes,
												NULL );
		if ( 0 == ii )
		{
			//
			// Free DetailData struct and reinitialize the data for next pass
			//	through this loop.
			//
			free(deviceInterfaceDetailData);
			deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
			continue;
		}


		//
		// get handle to the device so that we can 
		//
		hUsbPrinter = CreateFile(	deviceInterfaceDetailData->DevicePath,
									GENERIC_READ		| 
										GENERIC_WRITE,
									FILE_SHARE_WRITE	|
										FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									0,
									NULL );


		//
		//
		//
		if ( INVALID_HANDLE_VALUE == hUsbPrinter )
		{
			//
			// Free DetailData struct and reinitialize the data for next pass
			// through this loop.
			//
			free(deviceInterfaceDetailData);
			deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
			continue;
		}



		//
		//
		//
		dwBytes = 0;
		memset( szUsb1284, 0, sizeof(szUsb1284) );
		ii = DeviceIoControl(	hUsbPrinter,
								IOCTL_USBPRINT_GET_1284_ID,
								NULL,
								0,
								szUsb1284,
								sizeof(szUsb1284),
								&dwBytes,
								NULL );

		//
		//
		//
		CloseHandle( hUsbPrinter );



		//
		// parse the PNP string returned from the device.
		//		
		lpToken = strtok( szUsb1284, PNP_DELIMITOR );
		while ( NULL != lpToken )
		{
			// have the manfacturer.
			lpWorkToken = strstr(lpToken, MFG_TOKEN);
			if (lpWorkToken != NULL)
			{
				strcpy(	lpUsbDevices[nDeviceFound].szDeviceMfg, lpWorkToken + MFG_TOKEN_LEN);
				RTrim( lpUsbDevices[nDeviceFound].szDeviceMfg );
			}

			//	model of the printer
			lpWorkToken = strstr(lpToken, MDL_TOKEN);
			if (lpWorkToken != NULL)
			{
				strcpy(	lpUsbDevices[nDeviceFound].szDevicePdt, lpWorkToken + MDL_TOKEN_LEN );
				RTrim( lpUsbDevices[nDeviceFound].szDevicePdt );
			}


			// next token
			lpToken = strtok( NULL, PNP_DELIMITOR );
		}



		//
		//  if the we don't manage to get BOTH the manufacturer and the product
		// of the USB printer then don't return information back the calling code.
		//
		if (	strlen(lpUsbDevices[nDeviceFound].szDeviceMfg) + 
				strlen(lpUsbDevices[nDeviceFound].szDevicePdt) > 1 )
		{
			nDeviceFound++;
			
			//break after getting the number of devices requested.
			if (nMaxDevices == nDeviceFound)
			{
		        free(deviceInterfaceDetailData);
				break;

			}

		}
		else
		{
			lpUsbDevices[nDeviceFound].szDeviceMfg[0] = 
			lpUsbDevices[nDeviceFound].szDevicePdt[0] = 0;
		}



		//
		// Free DetailData struct and reinitialize the data for next pass
		// through this loop.
		//
        free(deviceInterfaceDetailData);
		deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);

	}

	//
	//Destroy device information set and free all associated memory. 
	//
	SetupDiDestroyDeviceInfoList(hDevInfo);

	//
	//Free setupapi library
	//
    FreeLibrary( pSetupAPI );

	return nDeviceFound;

}


////////////////////////////////////////////////////////////////////////////////
//	O p e n U S B D e v i c e
////////////////////////////////////////////////////////////////////////////////
HANDLE OpenUSBDevice( LPCSTR lpMfg, LPCSTR lpPdt, USB_ERROR_INFO *lpUsbErrorInfo )
{	
	int									ii;
	DWORD								dwBytes;
	BOOL								bFoundMfg;
	BOOL								bFoundPdt;
	LPTSTR								lpToken, lpWorkToken;
	int									nDeviceIndex;
	HDEVINFO							hDevInfo;
	HANDLE								hUsbPrinter;
	char								szUsb1284[1024];
	GUID								guidUSBDeviceSupport;
	SP_DEVICE_INTERFACE_DATA			deviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	deviceInterfaceDetailData;
	HINSTANCE							pSetupAPI					= NULL;

	FAR HDEVINFO ( FAR WINAPI *SetupDiGetClassDevs )(CONST GUID *, PCTSTR, HWND, DWORD);
	FAR BOOL ( FAR WINAPI *SetupDiEnumDeviceInterfaces )(HDEVINFO, PSP_DEVINFO_DATA, CONST GUID *,DWORD, PSP_DEVICE_INTERFACE_DATA);
	FAR BOOL ( FAR WINAPI *SetupDiGetDeviceInterfaceDetail )(HDEVINFO, PSP_DEVICE_INTERFACE_DATA, PSP_DEVICE_INTERFACE_DETAIL_DATA, DWORD, PDWORD, PSP_DEVINFO_DATA);
	FAR BOOL ( FAR WINAPI *SetupDiDestroyDeviceInfoList )(HDEVINFO);



	//
	// The functions SetupDiGetClassDevs, SetupDiEnumDeviceInterfaces , 
	// SetupDiGetDeviceInterfaceDetail, SetupDiDestroyDeviceInfoList
	// are supported only on Windows 2000 and above and Windows 98 and above.
	// So link them dynamically to avoid windows errors.
	//
	pSetupAPI = LoadLibrary( "Setupapi" );

	if ( pSetupAPI != NULL )
	{

		SetupDiGetClassDevs = ( HDEVINFO ( WINAPI * )(CONST GUID *, PCTSTR, HWND, DWORD)) GetProcAddress( pSetupAPI, "SetupDiGetClassDevsA" );

		if ( SetupDiGetClassDevs == NULL )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_FUNCTION;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to get a pointer to SetupDiGetClassDevs");
		    FreeLibrary( pSetupAPI );
			return 0;
		}

		SetupDiEnumDeviceInterfaces = ( BOOL ( WINAPI * )(HDEVINFO, PSP_DEVINFO_DATA, CONST GUID *,DWORD, PSP_DEVICE_INTERFACE_DATA)) GetProcAddress( pSetupAPI, "SetupDiEnumDeviceInterfaces" );

		if ( SetupDiEnumDeviceInterfaces == NULL )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_FUNCTION;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to get a pointer to SetupDiEnumDeviceInterfaces");
		    FreeLibrary( pSetupAPI );
			return 0;
		}

		SetupDiGetDeviceInterfaceDetail = ( BOOL ( WINAPI * )(HDEVINFO, PSP_DEVICE_INTERFACE_DATA, PSP_DEVICE_INTERFACE_DETAIL_DATA, DWORD, PDWORD, PSP_DEVINFO_DATA)) GetProcAddress( pSetupAPI, "SetupDiGetDeviceInterfaceDetailA" );

		if ( SetupDiGetDeviceInterfaceDetail == NULL )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_FUNCTION;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to get a pointer to SetupDiGetDeviceInterfaceDetailA");
		    FreeLibrary( pSetupAPI );
			return 0;
		}

		SetupDiDestroyDeviceInfoList = ( BOOL ( WINAPI * )(HDEVINFO)) GetProcAddress( pSetupAPI, "SetupDiDestroyDeviceInfoList" );

		if ( SetupDiDestroyDeviceInfoList == NULL )
		{
			lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_FUNCTION;
			strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to get a pointer to SetupDiDestroyDeviceInfoList");
		    FreeLibrary( pSetupAPI );
			return 0;
		}
	}
	else
	{
		lpUsbErrorInfo->dwSysLastError = ERROR_INVALID_HANDLE;
		strcpy( lpUsbErrorInfo->szSysErrorMsg, "ERROR : Unable to load Setupapi!");
		return 0;
	}


	//
	// get a handle to the set of device information returned by the Win32 API
	// functions.  This handle points to an object that is then operated on 
	// by the SetupDiEnumDeviceInterfaces function.
	//
	InitError( lpUsbErrorInfo );
	guidUSBDeviceSupport = GUID_CLASS_USB_DEVICE;
	hDevInfo = SetupDiGetClassDevs(	&guidUSBDeviceSupport,
									NULL,
									NULL,
									0x12 );	// TODO : Determine the constant
	if ( INVALID_HANDLE_VALUE == hDevInfo )
	{
		if ( NULL != lpUsbErrorInfo )
		{
			// get the last error from system
			lpUsbErrorInfo->dwSysLastError = GetLastError();

			// get the system defined message
			FormatMessage(	FORMAT_MESSAGE_FROM_SYSTEM			|
								FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL,
							lpUsbErrorInfo->dwSysLastError,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
							lpUsbErrorInfo->szSysErrorMsg,
							sizeof(lpUsbErrorInfo->szSysErrorMsg) / sizeof(lpUsbErrorInfo->szSysErrorMsg[0]),
							NULL );
		}
	}


	//
	//
	//	
	nDeviceIndex = 0;
	bFoundMfg = bFoundPdt = FALSE;
	hUsbPrinter = INVALID_HANDLE_VALUE;
	deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
	for ( nDeviceIndex = 0; SetupDiEnumDeviceInterfaces(	hDevInfo,
											NULL,
											&guidUSBDeviceSupport,
											nDeviceIndex,
											&deviceInterfaceData ); 
		nDeviceIndex++ ) 
	{




		//
		// determine the amout of memory we need to allocate for the next
		// call to this function
		//
		dwBytes= 0;
		ii = SetupDiGetDeviceInterfaceDetail(	hDevInfo,
												&deviceInterfaceData,
												NULL,
												0,
												&dwBytes,
												NULL );
		if ( 0 == dwBytes )
		{
			//
			// Reinitialize the data for next pass
			// through this loop.
			//
			deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
			continue;
		}


		
		deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(dwBytes);
		deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);


		//
		// get the device path for the current device we are enumerating over.
		//
		ii = SetupDiGetDeviceInterfaceDetail(	hDevInfo,
												&deviceInterfaceData,
												deviceInterfaceDetailData,
												dwBytes,
												&dwBytes,
												NULL );
		if ( 0 == ii )
		{
			//
			// Free DetailData struct and reinitialize the data for next pass
			//through this loop.
			//
			free(deviceInterfaceDetailData);
			deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
			continue;
		}


		//
		// get handle to the device so that we can 
		//
		hUsbPrinter = CreateFile(	deviceInterfaceDetailData->DevicePath,
									GENERIC_READ		| 
										GENERIC_WRITE,
									FILE_SHARE_WRITE	|
										FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									0,
									NULL );


		//
		//
		//
		if ( INVALID_HANDLE_VALUE == hUsbPrinter )
		{
			//
			// Free DetailData struct and reinitialize the data for next pass
			// through this loop.
			//
			free(deviceInterfaceDetailData);
			deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
			continue;
		}



		//
		//
		//
		dwBytes = 0;
		memset( szUsb1284, 0, sizeof(szUsb1284) );
		ii = DeviceIoControl(	hUsbPrinter,
								IOCTL_USBPRINT_GET_1284_ID,
								NULL,
								0,
								szUsb1284,
								sizeof(szUsb1284),
								&dwBytes,
								NULL );

		//
		// parse the PNP string returned from the device.
		//		
		lpToken = strtok( szUsb1284, PNP_DELIMITOR );
		while ( NULL != lpToken )
		{
			// have the manfacturer.
			lpWorkToken = strstr(lpToken,MFG_TOKEN);
			if ( lpWorkToken != NULL )
			{
				RTrim(lpWorkToken);
				if ( strcmp( lpMfg, lpWorkToken + MFG_TOKEN_LEN ) == 0 )
					bFoundMfg = TRUE;
			}

			//	model of the printer
			lpWorkToken = strstr(lpToken,MDL_TOKEN);
			if ( lpWorkToken != NULL )
			{
				RTrim(lpWorkToken);
				if ( strcmp( lpPdt, lpWorkToken + MDL_TOKEN_LEN ) == 0 )
					bFoundPdt = TRUE;
			}

			// next token
			lpToken = strtok( NULL, PNP_DELIMITOR );
		}


	
		//
		//
		//
		if ( bFoundMfg && bFoundPdt )
		{
	        free(deviceInterfaceDetailData);
			break;
		}
	

		//
		//
		//
		CloseHandle( hUsbPrinter );
		hUsbPrinter = INVALID_HANDLE_VALUE;


		
		//
		// Free DetailData struct and reinitialize the data for next pass
		// through this loop.
		//
		free(deviceInterfaceDetailData);
		deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);

	}

	//
	//Destroy device information set and free all associated memory. 
	//
	SetupDiDestroyDeviceInfoList(hDevInfo);

	//
	//Free setupapi library
	//
    FreeLibrary( pSetupAPI );

	//
	//
	//
	return hUsbPrinter;
}



////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void InitError( USB_ERROR_INFO *lpUsbErrorInfo )
{
	
	//
	//
	//
	if ( NULL != lpUsbErrorInfo )
	{
		lpUsbErrorInfo->dwSysLastError = 0;
		memset( lpUsbErrorInfo->szSysErrorMsg, 0, sizeof(lpUsbErrorInfo->szSysErrorMsg) );	
	}
}


void AssignPtr( char *dst, char *src )
{
	dst = src;
}


void RTrim( char *s )
{
	int ii;
	for ( ii = strlen(s)-1; ii > 0; ii-- )
	{
		if ( ' ' == s[ii] )
			s[ii] = 0;
		else
			break;
	}
}
