// printnames.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <winspool.h>

#include <iostream>
#include <string>
using namespace std;



void EnumeratePrinters( DWORD flags, LPSTR name, DWORD level );
void ListPrinterInformation( char* printerName );


void DisplayLastErrorMessage( LPSTR errorMessage, DWORD	errorId );


int main(int argc, char* argv[])
{

	if( argc == 1 )
	{
		printf( "printnames : utility to list printer resources\n" );
		printf( "    usage:\n" );
		printf( "           printnames -l [name]             -> list local printers on local machine or in name[\\\\serverName]\n" );
		printf( "           printnames -n                    -> list network printers on user's domain\n" );
		printf( "           printnames -r [name]             -> list printers on user's domain or in in name[\\\\serverName, domain, print provider]\n" );
		printf( "           printnames -a                    -> list all available printers\n" );
		printf( "           printnames -i [printer name]     -> open printer and list information about the printer\n" );
	}

	if( argc > 1 )
	{
		if( strcmp( argv[1], "-l" ) == 0 )
		{
			if( argv[2] != NULL )
			{
				printf( "LOCAL Printers on Server=[%s]\n", argv[2] );
				EnumeratePrinters( PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS| PRINTER_ENUM_NAME, argv[2], 2 );
			}
			else
			{
				printf( "LOCAL Printers\n" );
				EnumeratePrinters( PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2 );
			}
		}
		if( strcmp( argv[1], "-n" ) == 0 )
		{
			printf( "NETWORK Printers On User's Domain\n" );
			EnumeratePrinters( PRINTER_ENUM_NETWORK, NULL, 1 );
		}
		if( strcmp( argv[1], "-r" ) == 0 )
		{
			if( argv[2] != NULL )
			{
				printf( "REMOTE Printers on =[%s]\n", argv[2] );
				EnumeratePrinters( PRINTER_ENUM_NETWORK | PRINTER_ENUM_NAME | PRINTER_ENUM_CONNECTIONS, argv[2], 2 );
			}
			else
			{
				printf( "REMOTE Printers in the user's domain\n" );
				EnumeratePrinters( PRINTER_ENUM_REMOTE | PRINTER_ENUM_CONNECTIONS, NULL, 1 );
			}
		}
		if( strcmp( argv[1], "-a" ) == 0 )
		{
			printf( "LOCAL Printers\n" );
			EnumeratePrinters( PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 2 );
			printf( "REMOTE Printers\n" );
			EnumeratePrinters( PRINTER_ENUM_REMOTE, NULL, 1 );
		}
		if( strcmp( argv[1], "-i" ) == 0 )
		{
			if( argv[2] != NULL )
			{
				printf( "%s [%s]\n", "PRINTER Status - Printer Name=", argv[2] );
				ListPrinterInformation( argv[2] );
			}
			else
			{
				printf( "PRINTER Status - Failed because of missing Printer Name parameter\n" );
			}
		}
	}


	return 0;
}


void EnumeratePrinters( DWORD flags, LPSTR name, DWORD level )
{
	BOOL			bRet;
	DWORD			needed;
	DWORD			returned;
	PRINTER_INFO_1  *prninfo1;
	PRINTER_INFO_2  *prninfo2;
	// find required size for the buffer
	EnumPrinters( flags, name, level, NULL, 0, &needed, &returned );

	// allocate array ofr PRINTER_INFO structures
	if( level == 1 )
	{
		prninfo1 = (PRINTER_INFO_1 *)GlobalAlloc( GPTR, needed );
		bRet = EnumPrinters( flags, name, level, (LPBYTE)prninfo1, needed, &needed, &returned );
	}
	else
	{
		prninfo2 = (PRINTER_INFO_2 *)GlobalAlloc( GPTR, needed );
		bRet = EnumPrinters( flags, name, level, (LPBYTE)prninfo2, needed, &needed, &returned );
	}
	// call again
	if( !bRet )
	{
		DisplayLastErrorMessage( "Printer Enumeration failed", GetLastError() );
	}
	else
	{
		cout << "Objects found: " << returned << endl << endl;

		for( int counter = 0; counter < returned; counter++ )
		{
			if( level == 1 )
			{
				cout << "Printer Name=[" << prninfo1[ counter ].pName << "]" << endl;
				cout << " Description=" << prninfo1[ counter ].pDescription << endl;
				cout << " Comments   =" << prninfo1[ counter ].pComment << endl;
				cout << "       Flags=" << prninfo1[ counter ].Flags << endl << endl;
			}
			else
			{
				cout << "Printer Name=[" << prninfo2[ counter ].pPrinterName << "]" << endl;
				cout << "        Port=[" << prninfo2[ counter ].pPortName << "]" << endl;
				cout << "  Share name=[" << prninfo2[ counter ].pShareName << "]";
				if( prninfo2[ counter ].pServerName != NULL )
					cout << " on Server Name=[" << prninfo2[ counter ].pServerName << "]" << endl;
				else
					cout << endl;
				cout << "      Status=" << prninfo2[ counter ].Status << endl;
				cout << "        Jobs=" << prninfo2[ counter ].cJobs << endl << endl;
			}
		}
	}

	if( level == 1 )
		GlobalFree( prninfo1 );
	else
		GlobalFree( prninfo2 );
}


void ListPrinterInformation( char* printerName )
{
	BOOL		bRet;
	HANDLE		handle;

	bRet = OpenPrinter( printerName, &handle, NULL );

	if( bRet )
	{
		PRINTER_INFO_2  *prninfo2;
		DWORD			sizeNeeded;
		DWORD			sizeNeeded2;


		bRet = GetPrinter( handle, 2, NULL, 0, &sizeNeeded );
		
		if( bRet == 0 )
		{
			prninfo2 = (PRINTER_INFO_2 *)GlobalAlloc( GPTR, sizeNeeded  );

			bRet = GetPrinter( handle, 2, (LPBYTE)prninfo2, sizeNeeded, &sizeNeeded2 );

			if( bRet )
			{
				cout << "=======================================================================================" << endl;
				printf( "  Printer Name                                            =[%s]\n", prninfo2->pPrinterName );
				printf( "  Server Name                                             =[%s]\n", prninfo2->pServerName );
				printf( "  Driver Name                                             =[%s]\n", prninfo2->pDriverName );
				printf( "  Port Name                                               =[%s]\n", prninfo2->pPortName );
				printf( "  Share Name                                              =[%s]\n", prninfo2->pShareName );
				printf( "  Comment                                                 =[%s]\n", prninfo2->pComment );
				printf( "  Location                                                =[%s]\n", prninfo2->pLocation );
				printf( "  Separator Page File                                     =[%s]\n", prninfo2->pLocation );
				printf( "  Print Processor                                         =[%s]\n", prninfo2->pPrintProcessor );
				printf( "  Default Print Processor Parameters                      =[%s]\n", prninfo2->pParameters );
				printf( "  Printer Data Type                                       =[%s]\n", prninfo2->pDatatype );
				printf( "  Spooler Priority Value                                  =[%d]\n", prninfo2->Priority );
				printf( "  Default Priority Value                                  =[%d]\n", prninfo2->DefaultPriority );
				printf( "  Average Number of Pages Per Minute                      =[%d]\n", prninfo2->AveragePPM );
				printf( "  Number of Jobs Queued to Printer                        =[%d]\n", prninfo2->cJobs );
				printf( "  Attribute -> Printer is Shared                          =[%s]\n", prninfo2->Attributes & PRINTER_ATTRIBUTE_SHARED ? "yes" : "no" );
				printf( "  Attribute -> Local Printer                              =[%s]\n", prninfo2->Attributes & PRINTER_ATTRIBUTE_LOCAL ? "yes" : "no" );
				printf( "  Attribute -> Network Printer                            =[%s]\n", prninfo2->Attributes & PRINTER_ATTRIBUTE_NETWORK ? "yes" : "no" );
				printf( "  Attribute -> Direct Print                               =[%s]\n", prninfo2->Attributes & PRINTER_ATTRIBUTE_DIRECT ? "yes" : "no" );
				printf( "  Attribute -> Printer Visible in Directory Services      =[%s]\n", prninfo2->Attributes & PRINTER_ATTRIBUTE_PUBLISHED ? "yes" : "no" );
				printf( "  Attribute -> Print Job Queued                           =[%s]\n", prninfo2->Attributes & PRINTER_ATTRIBUTE_QUEUED ? "yes" : "no" );
				printf( "  Attribute -> Print Raw Job Queued                       =[%s]\n", prninfo2->Attributes & PRINTER_ATTRIBUTE_RAW_ONLY ? "yes" : "no" );
				printf( "  Attribute -> Print While Spooling                       =[%s]\n", prninfo2->Attributes & PRINTER_ATTRIBUTE_DO_COMPLETE_FIRST ? "yes" : "no" );
				printf( "  Attribute -> Keep Jobs In Spool After Print             =[%s]\n", prninfo2->Attributes & PRINTER_ATTRIBUTE_KEEPPRINTEDJOBS ? "yes" : "no" );
				printf( "  Status -> Printer is Busy                               =[%s]\n", prninfo2->Status & PRINTER_STATUS_BUSY ? "yes" : "no" );
				printf( "  Status -> Printer is Printing                           =[%s]\n", prninfo2->Status & PRINTER_STATUS_PRINTING ? "yes" : "no" );
				printf( "  Status -> Printer is Processing Print Job               =[%s]\n", prninfo2->Status & PRINTER_STATUS_PROCESSING ? "yes" : "no" );
				printf( "  Status -> Printer is in Error                           =[%s]\n", prninfo2->Status & PRINTER_STATUS_ERROR ? "yes" : "no" );
				printf( "  Status -> Printer is Waiting                            =[%s]\n", prninfo2->Status & PRINTER_STATUS_WAITING ? "yes" : "no" );
				printf( "  Status -> Printer is Paused                             =[%s]\n", prninfo2->Status & PRINTER_STATUS_PAUSED ? "yes" : "no" );
				printf( "  Status -> Printer is in Power Save Mode                 =[%s]\n", prninfo2->Status & PRINTER_STATUS_POWER_SAVE ? "yes" : "no" );
				printf( "  Status -> Printer is Offline                            =[%s]\n", prninfo2->Status & PRINTER_STATUS_OFFLINE ? "yes" : "no" );
				printf( "  Status -> Printer is not Available                      =[%s]\n", prninfo2->Status & PRINTER_STATUS_NOT_AVAILABLE ? "yes" : "no" );
				printf( "  Status -> Printer is warming up                         =[%s]\n", prninfo2->Status & PRINTER_STATUS_WARMING_UP ? "yes" : "no" );
				printf( "  Status -> Printer Door is Open                          =[%s]\n", prninfo2->Status & PRINTER_STATUS_DOOR_OPEN ? "yes" : "no" );
				printf( "  Status -> Printer is Initializing                       =[%s]\n", prninfo2->Status & PRINTER_STATUS_INITIALIZING ? "yes" : "no" );
				printf( "  Status -> Printer is in IO state                        =[%s]\n", prninfo2->Status & PRINTER_STATUS_IO_ACTIVE ? "yes" : "no" );
				printf( "  Status -> Printer is in Manual Feed state               =[%s]\n", prninfo2->Status & PRINTER_STATUS_MANUAL_FEED ? "yes" : "no" );
				printf( "  Status -> Printer Toner Level is low                    =[%s]\n", prninfo2->Status & PRINTER_STATUS_TONER_LOW ? "yes" : "no" );
				printf( "  Status -> Printer is out of Toner                       =[%s]\n", prninfo2->Status & PRINTER_STATUS_NO_TONER ? "yes" : "no" );
				printf( "  Status -> Printer run out of Memory                     =[%s]\n", prninfo2->Status & PRINTER_STATUS_OUT_OF_MEMORY ? "yes" : "no" );
				printf( "  Status -> Printer bin is Full                           =[%s]\n", prninfo2->Status & PRINTER_STATUS_OUTPUT_BIN_FULL ? "yes" : "no" );
				printf( "  Status -> Printer is out of Paper                       =[%s]\n", prninfo2->Status & PRINTER_STATUS_PAPER_OUT ? "yes" : "no" );
				printf( "  Status -> Printer has a Paper Problem                   =[%s]\n", prninfo2->Status & PRINTER_STATUS_PAPER_PROBLEM ? "yes" : "no" );
				printf( "  Status -> Paper is jammed in the Printer                =[%s]\n", prninfo2->Status & PRINTER_STATUS_PAPER_JAM ? "yes" : "no" );
				printf( "  Status -> Printer is being Deleted                      =[%s]\n", prninfo2->Status & PRINTER_STATUS_PENDING_DELETION ? "yes" : "no" );
				printf( "  Status -> Printer Status Server Unknown                 =[%s]\n", prninfo2->Status & PRINTER_STATUS_SERVER_UNKNOWN ? "yes" : "no" );
				printf( "  Status -> Printer Require User Intervention             =[%s]\n", prninfo2->Status & PRINTER_STATUS_USER_INTERVENTION ? "yes" : "no" );
				cout << "=======================================================================================" << endl;
				GlobalFree( prninfo2 );
			}
			else
			{
				DisplayLastErrorMessage( "Get printer information command failed", GetLastError() );
			}
		}
		else
		{
			DisplayLastErrorMessage( "Get printer information size command failed", GetLastError() );
		}

		CloseHandle( handle );
	}
	else
	{
		DisplayLastErrorMessage( "Open printer command failed", GetLastError() );
	}

}


// utility functions

void DisplayLastErrorMessage( LPSTR errorMessage, DWORD	errorId )
{
		LPVOID	pMsg;

		FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						errorId,
						MAKELANGID( LANG_NEUTRAL,SUBLANG_DEFAULT ),
						(LPTSTR) &pMsg,
						0,
						NULL );
		printf( "%s - Message ID=%d - %s\n", errorMessage, errorId, pMsg );
		LocalFree( pMsg );
}
