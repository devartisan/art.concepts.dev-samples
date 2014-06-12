// printout.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "malloc.h"
#include "string.h"
#include "windows.h" 


#include <winspool.h>


#define LOG_FILE		"c:\\tmp\\printout.log"



// GENERIC RIGHTS
#define GENERIC_READ	(0x80000000L)
#define GENERIC_WRITE	(0x40000000L)
#define GENERIC_EXECUTE	(0x20000000L)
#define GENERIC_ALL		(0x10000000L)


// DECLARE LOCAL FUNCTIONS
bool checkPrinterpath( char* printerPath, bool silent );
void displaySystemInfo();
void listenKPrintActiveX();
void logMessage( char* buffer );
BOOL sendFileDataToPrinter( char* printerPath, char *filename );
int  sendFileToPrinter(  char* printerPath, char *filename );
BOOL sendRawDataToPrinter(	LPSTR szPrinterName, LPBYTE lpData, DWORD dwCount );



int main(int argc, char* argv[])
{
	int iRet = 0;
	char msgBuffer[2048];

	if( argc == 1 )
	{
		printf( "printout : utility to send files to printers and identify printer ports\n" );
		printf( "    usage:\n" );
		printf( "           printout -f printerPath filename -> to send a file to a printer open as file\n" );
		printf( "           printout -p printerPath filename -> to send a file to a printer open as printer\n" );
		printf( "           printout -s printerPath          -> to indicate if a printer path is visible or not\n" );
		printf( "           printout -i                      -> to display some system related info\n" );
		printf( "           printout -a                      -> to act as a listener for the KPrintActiveX\n" );
		printf( "    Notes: if we have a c:\\tmp\\printout.log file present in the filesystem the\n" );
		printf( "           will log some information to it for printout (-p) option\n" );
	}

	if( argc > 1 )
	{
		if( strcmp( argv[1], "-s" ) == 0 )
		{
			sprintf( msgBuffer, "Command= printout -s %s\n", argv[2] );
			logMessage( msgBuffer );
			
			checkPrinterpath( argv[2], false );
		}

		if( strcmp( argv[1], "-f" ) == 0 )
		{
			sprintf( msgBuffer, "Command= printout -f %s %s\n", argv[2], argv[3] );
			logMessage( msgBuffer );

			iRet = sendFileToPrinter( argv[2], argv[3] );
		}

		if( strcmp( argv[1], "-p" ) == 0 )
		{
			sprintf( msgBuffer, "Command= printout -p %s %s\n", argv[2], argv[3] );
			logMessage( msgBuffer );

			if( !sendFileDataToPrinter( argv[2], argv[3] ) )
				iRet = -1;
		}

		if( strcmp( argv[1], "-i" ) == 0 )
		{
			displaySystemInfo();
		}

		if( strcmp( argv[1], "-a" ) == 0 )
		{
			listenKPrintActiveX();
		}
	}
	return iRet;
}

// METHODS

bool checkPrinterpath( char* printerPath, bool silent )
{
	bool bRet = false;
	char msgBuffer[2048];
	HANDLE	hDev;
	DWORD	access = GENERIC_WRITE;

//	hDev = CreateFile( sharepointName, access, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	hDev = CreateFile( printerPath, access, 0, NULL, CREATE_ALWAYS, 0, NULL );

	if( hDev == INVALID_HANDLE_VALUE )
	{
		DWORD errorID = GetLastError();

		LPTSTR errorMsg = NULL;

		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, 0, (LPTSTR)&errorMsg, 0, NULL );
		sprintf( msgBuffer, "Printer path %s not available!!!!!\n", printerPath );
		sprintf( msgBuffer, "    Error Id=%i Error Message=%s\n", errorID, errorMsg );
		printf( msgBuffer );
		logMessage( msgBuffer );

		LocalFree(errorMsg);
	}
	else
	{
		bRet = true;
		if( !silent )
		{
			printf( "SUCCESS - printer path %s available!!!!!\n", printerPath );

			if( GetLastError() == ERROR_PIPE_BUSY )
				printf( "    Share is BUSY - it could fail to answer to a connection request.\n" );
		}
		CloseHandle( hDev );
	}

	return bRet;
}


void displaySystemInfo()
{
	char Buffer[1024];

	GetTempPath( 1024, Buffer );
	printf( "Temp Path=%s\n" , Buffer );
}

void listenKPrintActiveX()
{
	char msgBuffer[2048];
	char Buffer[1024];
	char Path[300];
	char *empty = "empty";
	char PrinterPath[300];
	char FileName[300];
	char TempPath[300];
	char *kprintLog = "kprintactivex.log";
	char PathToSearch[300];
	bool alternate = false;

	
	GetTempPath( 300, TempPath );
	sprintf( PathToSearch, "%s", TempPath );

	while( true )
	{
		if( SearchPath( PathToSearch, kprintLog, NULL, 300, Path, NULL ) > 0 )
		{

			HANDLE hFile = CreateFile( Path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );

			DWORD BytesRead = 0;
			DWORD BytesWritten;
			if( hFile != INVALID_HANDLE_VALUE )
			{
				if( ReadFile( hFile, Buffer, 1024, &BytesRead, NULL ) && BytesRead > 0 )
				{
					if( Buffer[ BytesRead - 1 ] == '\n' &&
						Buffer[ BytesRead - 2 ] == '\n' && 
						Buffer[ BytesRead - 3 ] == '\n' )
					{
						SetFilePointer( hFile, 0, 0, FILE_BEGIN );
						WriteFile( hFile, empty, strlen( empty ), &BytesWritten, NULL );
						SetEndOfFile( hFile );
						CloseHandle( hFile );
						
						unsigned position;
						unsigned position2;
						for( position = 0; position < strlen( Buffer ); position++ )
						{
							if( Buffer[ position ] == '\n' )
								break;
						}
						strncpy( PrinterPath, Buffer, position );
						PrinterPath[ position ] = '\0';

						position++;
						position2 = position;

						for( ; position < strlen( Buffer ); position++ )
						{
							if( Buffer[ position ] == '\n' )
								break;
						}
						strncpy( FileName, Buffer + position2, position - position2 );
						FileName[ position - position2 ] = '\0';

						sendFileToPrinter( PrinterPath, FileName );

					}
					else
					{
						CloseHandle( hFile );
						Sleep( 1000 );
					}
				}
				else
				{
					Sleep( 1000 );
				}

			}
			else
			{
				sprintf( msgBuffer, "could not open the kprintactivex.log\n" );
				logMessage( msgBuffer );
				Sleep( 1000 );
			}
		}
		else
		{
			if( !alternate )
			{
				sprintf( msgBuffer, "could not find the kprintactivex.log on %\n", PathToSearch );
				logMessage( msgBuffer );
				alternate = true;
				sprintf( PathToSearch, "%s\\Low", TempPath );

			}
			else
			{
				sprintf( msgBuffer, "could not find the kprintactivex.log on %\n", PathToSearch );
				logMessage( msgBuffer );
				Sleep( 1000 );
			}
		}
	}
}

void logMessage( char* buffer )
{
	HANDLE hFile = CreateFile( LOG_FILE, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );

	if( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD bytesWritten;

		SetFilePointer( hFile, 0, 0, FILE_END );
			
		if( !WriteFile( hFile, buffer, strlen( buffer ), &bytesWritten, NULL )  )
		{
			DWORD errorID = GetLastError();

			LPTSTR errorMsg = NULL;

			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, 0, (LPTSTR)&errorMsg, 0, NULL );
			printf( "Error writing to c:\\tmp\\printout.log\n" );
			printf(	"    Error Id=%i Error Message=%s\n", errorID, errorMsg );

			LocalFree(errorMsg);
		}
		CloseHandle( hFile );
	}
}


BOOL sendFileDataToPrinter( char* printerPath, char *filename )
{
	BOOL bRet = FALSE;
	char msgBuffer[2048];

	HANDLE hFile = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );

	if( hFile != INVALID_HANDLE_VALUE )
	{
		LPBYTE buffer = NULL;
		DWORD bytesRead;
		DWORD nFileSize = GetFileSize( hFile, NULL );

		if( nFileSize != INVALID_FILE_SIZE )
		{
			buffer = (LPBYTE)calloc( nFileSize + 1, sizeof( BYTE ) );
			bytesRead = -1;

			ReadFile( hFile, buffer, nFileSize, &bytesRead, NULL );

			if( bytesRead == nFileSize )
			{
				if( sendRawDataToPrinter( printerPath, buffer, nFileSize ) )
				{
					bRet = TRUE;
					sprintf( msgBuffer, "Sucess on  writing to %s - Source file=%s - Total bytes written[%i]\n", printerPath, filename, nFileSize );
					logMessage( msgBuffer );
				}
				else
				{
					DWORD errorID = GetLastError();

					LPTSTR errorMsg = NULL;

					FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, 0, (LPTSTR)&errorMsg, 0, NULL );
					sprintf( msgBuffer, "Error printing %s to printer %s\n", filename, printerPath );
					sprintf( msgBuffer, "    Error Id=%i Error Message=%s\n", errorID, errorMsg );
					printf( msgBuffer );
					logMessage( msgBuffer );

					LocalFree(errorMsg);
				}
			}
			else
			{
				DWORD errorID = GetLastError();

				LPTSTR errorMsg = NULL;

				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, 0, (LPTSTR)&errorMsg, 0, NULL );
				sprintf( msgBuffer, "Error reading file %s - File Size=%d Bytes Read=%d\n", filename, nFileSize, bytesRead );
				sprintf( msgBuffer, "    Error Id=%i Error Message=%s\n", errorID, errorMsg );
				printf( msgBuffer );
				logMessage( msgBuffer );

				LocalFree(errorMsg);
			}

		}
		else
		{
			DWORD errorID = GetLastError();

			LPTSTR errorMsg = NULL;

			FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, 0, (LPTSTR)&errorMsg, 0, NULL );
			sprintf( msgBuffer, "Error getting size of file %s\n", filename );
			sprintf( msgBuffer, "    Error Id=%i Error Message=%s\n", errorID, errorMsg );
			printf( msgBuffer );
			logMessage( msgBuffer );

			LocalFree(errorMsg);
		}

		if( buffer != NULL )
		{
			free( buffer );
			buffer = NULL;
		}

		CloseHandle( hFile );
		
	}
	else
	{

		DWORD errorID = GetLastError();

		LPTSTR errorMsg = NULL;

		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, 0, (LPTSTR)&errorMsg, 0, NULL );
		sprintf( msgBuffer, "File %s not available!!!!!\n", filename );
		sprintf( msgBuffer, "    Error Id=%i Error Message=%s\n", errorID, errorMsg );
		printf( msgBuffer );
		logMessage( msgBuffer );

		LocalFree(errorMsg);
	}


	return bRet;
}

int sendFileToPrinter(  char* printerPath, char* filename )
{
	int iRet = -1;
	char msgBuffer[2048];
	HANDLE	hDev;
	DWORD	access = GENERIC_WRITE;

	if( checkPrinterpath( printerPath, true ) )
	{
		hDev = CreateFile( printerPath, access, 0, NULL, CREATE_ALWAYS, 0, NULL );

		if( hDev != INVALID_HANDLE_VALUE )
		{
			HANDLE hFile = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );

			if( hFile != INVALID_HANDLE_VALUE )
			{
				char buffer[4096];
				DWORD bytesRead;
				DWORD bytesWritten;
				DWORD totalBytes;
				
				totalBytes = 0;
				bytesRead = -1;
				while( ReadFile( hFile, buffer, 4096, &bytesRead, NULL ) && bytesRead > 0 )
				{
					if( !WriteFile( hDev, buffer, bytesRead, &bytesWritten, NULL )  )
					{
						DWORD errorID = GetLastError();

						LPTSTR errorMsg = NULL;

						FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, 0, (LPTSTR)&errorMsg, 0, NULL );
						sprintf( msgBuffer, "Error writing to %s\n", printerPath );
						sprintf( msgBuffer, "    Error Id=%i Error Message=%s\n", errorID, errorMsg );
						printf( msgBuffer );
						logMessage( msgBuffer );

						LocalFree(errorMsg);
					}
					else if( bytesWritten != bytesRead )
					{
						sprintf( msgBuffer, "Error writing to %s\n    Mismatch on the number of bytes read[%i] and written[%i]\n", printerPath, bytesRead, bytesWritten );
						printf( msgBuffer );
						logMessage( msgBuffer );
						break;
					}

					totalBytes += bytesRead;
					bytesRead = -1;

				}

				if( bytesRead == -1 )
				{
					DWORD errorID = GetLastError();

					LPTSTR errorMsg = NULL;

					FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, 0, (LPTSTR)&errorMsg, 0, NULL );
					sprintf( msgBuffer, "Error reading from %s\n", filename );
					sprintf( msgBuffer, "    Error Id=%i Error Message=%s\n", errorID, errorMsg );
					printf( msgBuffer );
					logMessage( msgBuffer );

					LocalFree(errorMsg);
				}
				else
				{
					iRet = 0;
					sprintf( msgBuffer, "Sucess on  writing to %s - Source file=%s - Total bytes written[%i]\n", printerPath, filename, totalBytes );
					logMessage( msgBuffer );
				}

				CloseHandle( hFile );
			}
			else
			{

				DWORD errorID = GetLastError();

				LPTSTR errorMsg = NULL;

				FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorID, 0, (LPTSTR)&errorMsg, 0, NULL );
				sprintf( msgBuffer, "File %s not available!!!!!\n", filename );
				sprintf( msgBuffer, "    Error Id=%i Error Message=%s\n", errorID, errorMsg );
				printf( msgBuffer );
				logMessage( msgBuffer );

				LocalFree(errorMsg);
			}

			CloseHandle( hDev );
		}

	}
	return iRet;
}

// sendRawDataToPrinter - sends binary data directly to a printer
   // 
   // Params:
   //   szPrinterName - NULL terminated string specifying printer name
   //   lpData        - Pointer to raw data bytes
   //   dwCount       - Length of lpData in bytes
   // 
   // Returns: TRUE for success, FALSE for failure.
   // 
   BOOL sendRawDataToPrinter(	LPSTR szPrinterName, LPBYTE lpData, DWORD dwCount )
   {
		HANDLE     hPrinter;
		DOC_INFO_1 DocInfo;
		DWORD      dwJob;
		DWORD      dwBytesWritten;

		// Need a handle to the printer.
		if( ! OpenPrinter( szPrinterName, &hPrinter, NULL ) )
			return FALSE;

		// Fill in the structure with info about this "document."
		DocInfo.pDocName = "My Document";
		DocInfo.pOutputFile = NULL;
		DocInfo.pDatatype = "RAW";
		
		// Info2rm the spooler the document is beginning.
		if( (dwJob = StartDocPrinter( hPrinter, 1, (LPBYTE)&DocInfo )) == 0 )
		{
			ClosePrinter( hPrinter );
			return FALSE;
		}
     
		// Start a page.
		if( ! StartPagePrinter( hPrinter ) )
		{
			EndDocPrinter( hPrinter );
			ClosePrinter( hPrinter );
			return FALSE;
		}
		
		// Send the data to the printer.
		if( ! WritePrinter( hPrinter, lpData, dwCount, &dwBytesWritten ) )
		{
			EndPagePrinter( hPrinter );
			EndDocPrinter( hPrinter );
			ClosePrinter( hPrinter );
			return FALSE;
		}
     
		// End the page.
		if( ! EndPagePrinter( hPrinter ) )
		{
			EndDocPrinter( hPrinter );
			ClosePrinter( hPrinter );
			return FALSE;
		}
		
		// Inform the spooler that the document is ending.
		if( ! EndDocPrinter( hPrinter ) )
		{
			ClosePrinter( hPrinter );
			return FALSE;
		}
     
		// Tidy up the printer handle.
		ClosePrinter( hPrinter );
     
		// Check to see if correct number of bytes were written.
		if( dwBytesWritten != dwCount )
			return FALSE;
		
		return TRUE;
   }

