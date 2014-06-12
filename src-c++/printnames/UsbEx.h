////////////////////////////////////////////////////////////////////////////////
//
// Module : UsbEx.h
//
// Description : This module contains all declaration regarding functionality of 
//		USB devices.  This functionality is exported for the purpose and use of
//		the TPE.
//
// History
// -----------------------------------------------------------------------------
// DATE			SCR		Author			Description
// -----------------------------------------------------------------------------
// 2002.04.15	XXXX	K Dwyer			Creation of module.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Defines and declarations for returning USB device information to the calling
//	application.
//
// The USB_DEVICE_NAME struct contains information about USB printer connected to 
//	the current machine. The szDeviceMfg is the manufacturer of the device and the
//	szDevicePdt is product name of the device.  This information is obtained from
//	device NOT through registry setting on the machine.
//
////////////////////////////////////////////////////////////////////////////////
#define USB_MFG_NAME_SIZE		128
#define USB_PDT_NAME_SIZE		128


typedef struct USB_DEVICE_NAME_tag
{
	char		szDeviceMfg[USB_MFG_NAME_SIZE];
	char		szDevicePdt[USB_PDT_NAME_SIZE];
} USB_DEVICE_NAME, *LPUSB_DEVICE_NAME;



////////////////////////////////////////////////////////////////////////////////
//
// Defines and declaration for returning error information to the calling 
//	application.
//
// The USB_ERROR_INFO struct contains information about errors that occurred 
//	during processing of the code within this module. The dwSysLastError is
//	value returned from the GetLastError function and the szSysErrorMsg is 
//	Windows system message generated via call to FormatMessage.  The nKewillError
//	is Kewill specific error values defined below.  The szKewillErrorMsg is a
//	Kewill defined description linked to the value contained within the nKewillError
//	member of the struct.
//
////////////////////////////////////////////////////////////////////////////////
#define USB_SYS_ERROR_MSG_SIZE		1024


typedef struct USB_ERROR_INFO_tag
{
	DWORD		dwSysLastError;
	TCHAR		szSysErrorMsg[USB_SYS_ERROR_MSG_SIZE];
} USB_ERROR_INFO, *LPUSB_ERROR_INFO;



// PNP 1284 setting tokens

#define MFG_TOKEN		"MFG:"	// Manufacturer token
#define CMD_TOKEN		"CMD:"	// Command Set token
#define MDL_TOKEN		"MDL:"	// Model token
#define CLS_TOKEN		"CLS:"	// Class token
#define DES_TOKEN		"DES:"	// Description token
#define CID_TOKEN		"CID:"	//

#define MFG_TOKEN_LEN	4		//
#define CMD_TOKEN_LEN	4		//
#define MDL_TOKEN_LEN	4		//
#define CLS_TOKEN_LEN	4		//
#define DES_TOKEN_LEN	4		//
#define CID_TOKEN_LEN	4		//

#define PNP_DELIMITOR	"!;"	//


#if !defined(PCTSTR) && !defined(PCTSTR_DEFINED)
	typedef TCHAR const*   PCTSTR;
	#define PCTSTR_DEFINED
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Function : GetUSBDevices
//
// Description : This function returns manufacturuer and product name for 
//	each USB printer known to the machine.  If the device doesn't return both
//	the manufacturuer name and product name the device doesn't get returned
//	in the list.  If any Win32 error occurr that fault processing the error
//	code and description are returned in the last argument.
//
// Arguments
//	nMaxDevices - the number of USB_DEVICE_NAME that the lpUsbDevices argument 
//		points to.
//
//	lpUsbDevices - a pointer to N USB_DEVICE_NAME structs, where N is value
//		in nMaxDevices.
//
//	lpUsbErrorInfo - struct containing Win32 error code and system deascription
//		fot that error code. The error code will be zero if no error happens.
//
// Returns : The number USB printers that the local machine knows about.
//
////////////////////////////////////////////////////////////////////////////////
INT GetUSBDevices( INT nMaxDevices, USB_DEVICE_NAME *lpUsbDevices, USB_ERROR_INFO *lpUsbErrorInfo );




////////////////////////////////////////////////////////////////////////////////
//
// Function : OpenUSBDevice
//
// Description : This function where a handle to the USB Printer specified by the 
//	manufacturer and product name.  If there are multiple printers of the same
//	make and model, this function will return a handle to the first it finds.
//	Currently there is no way specific whick printer to select.
//
// Argument(s)
//	lpMfg - The name of the manufacturer as returned via GetUSBDevices.
//
//	lpPdt - The model name as returned via GetUSBDevices.
//
//	lpUsbErrorInfo - struct containing Win32 error code and system deascription
//		fot that error code. The error code will be zero if no error happens.
//
// Returns : Handle to the USB Printer, else INVALID_HANDLE_VALUE.
//
////////////////////////////////////////////////////////////////////////////////
HANDLE OpenUSBDevice( LPCSTR lpMfg, LPCSTR lpPdt, USB_ERROR_INFO *lpUsbErrorInfo );


//void EnumerateHostControllers (USB_DEVICE_NAME *lpUsbDevices,
//    ULONG     *DevicesConnected);

