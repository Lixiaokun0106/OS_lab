#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>

#define BUFSIZE 512

DWORD WINAPI InstanceThread(void*); 
VOID GetAnswerToRequest(char*, LPDWORD); 

int main(VOID) 
{ 
	BOOL   fConnected = FALSE; 
	DWORD  dwThreadId = 0; 
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL; 
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\engintime.oslab.com1"); 

	// The main loop creates an instance of the named pipe and 
	// then waits for a client to connect to it. When the client 
	// connects, a thread is created to handle communications 
	// with that client, and this loop is free to wait for the
	// next client connect request. It is an infinite loop.

	_tprintf( TEXT("\n必须先启动本应用程序，然后再启动OS Lab调试\nSerialChat: awaiting client connection on %s\n"), lpszPipename);
	hPipe = CreateNamedPipe( 
		lpszPipename,             // pipe name 
		PIPE_ACCESS_DUPLEX,       // read/write access 
		PIPE_TYPE_MESSAGE |       // message type pipe 
		PIPE_READMODE_MESSAGE |   // message-read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // max. instances  
		BUFSIZE,                  // output buffer size 
		BUFSIZE,                  // input buffer size 
		0,                        // client time-out 
		NULL);                    // default security attribute 

	if (hPipe == INVALID_HANDLE_VALUE) 
	{
		_tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError()); 
		return -1;
	}

	// Wait for the client to connect; if it succeeds, 
	// the function returns a nonzero value. If the function
	// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

	fConnected = ConnectNamedPipe(hPipe, NULL) ? 
TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

	if (fConnected) 
	{ 
		printf("Client connected success.\n"); 

		// Create a thread for this client. 
		hThread = CreateThread( 
			NULL,              // no security attribute 
			0,                 // default stack size 
			InstanceThread,    // thread proc
			(LPVOID) hPipe,    // thread parameter 
			0,                 // not suspended 
			&dwThreadId);      // returns thread ID 

		if (hThread == NULL) 
		{
			_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError()); 
			return -1;
		}
		else CloseHandle(hThread); 
	} 
	else 
		// The client could not connect, so close the pipe. 
		CloseHandle(hPipe); 


	while(1);

	return 0; 
} 

DWORD WINAPI InstanceThread(void* lpvParam)
// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
{ 
	HANDLE hHeap      = GetProcessHeap();
	char* pchRequest = (char*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(char));
	char* pchReply   = (char*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(char));

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0; 
	BOOL fSuccess = FALSE;
	HANDLE hPipe  = NULL;

	// Do some extra error checking since the app will keep running even if this
	// thread fails.

	if (lpvParam == NULL)
	{
		printf( "\nERROR - Pipe Server Failure:\n");
		printf( "   InstanceThread got an unexpected NULL value in lpvParam.\n");
		printf( "   InstanceThread exitting.\n");
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	if (pchRequest == NULL)
	{
		printf( "\nERROR - Pipe Server Failure:\n");
		printf( "   InstanceThread got an unexpected NULL heap allocation.\n");
		printf( "   InstanceThread exitting.\n");
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		return (DWORD)-1;
	}

	if (pchReply == NULL)
	{
		printf( "\nERROR - Pipe Server Failure:\n");
		printf( "   InstanceThread got an unexpected NULL heap allocation.\n");
		printf( "   InstanceThread exitting.\n");
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	// Print verbose messages. In production code, this should be for debugging only.
	printf("Now, receiving and processing messages.\n");

	// The thread's parameter is a handle to a pipe object instance. 

	hPipe = (HANDLE) lpvParam; 

	// Loop until done reading
	while (1) 
	{ 
		// Read client requests from the pipe. This simplistic code only allows messages
		// up to BUFSIZE characters in length.
		ZeroMemory(pchRequest,BUFSIZE*sizeof(char));
		printf("<<");

		do{
			fSuccess = ReadFile( 
				hPipe,        // handle to pipe 
				pchRequest,    // buffer to receive data 
				BUFSIZE*sizeof(char), // size of buffer 
				&cbBytesRead, // number of bytes read 
				NULL);        // not overlapped I/O 

			if (!fSuccess || cbBytesRead == 0)
			{   
				if (GetLastError() == ERROR_BROKEN_PIPE)
				{
					_tprintf(TEXT("InstanceThread: client disconnected.\n"), GetLastError()); 
				}
				else
				{
					_tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError()); 
				}
				break;
			}

			printf( "%c", pchRequest[0]);

			if ('\0' == *(pchRequest)) {
				break;
			}

		}while(TRUE);

		// Process the incoming message.
		GetAnswerToRequest(pchReply, &cbReplyBytes); 

		// Write the reply to the pipe.
		fSuccess = WriteFile( 
			hPipe,        // handle to pipe 
			pchReply,     // buffer to write from 
			cbReplyBytes, // number of bytes to write 
			&cbWritten,   // number of bytes written 
			NULL);        // not overlapped I/O 
			
		if (!fSuccess || cbReplyBytes != cbWritten)
		{   
			_tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError()); 
			break;
		}
	}

	// Flush the pipe to allow the client to read the pipe's contents 
	// before disconnecting. Then disconnect the pipe, and close the 
	// handle to this pipe instance. 

	FlushFileBuffers(hPipe); 
	DisconnectNamedPipe(hPipe); 
	CloseHandle(hPipe); 

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);

	printf("InstanceThread exitting.\n");
	return 1;
}

VOID GetAnswerToRequest(char* pchReply, 
						LPDWORD pchBytes)						
{
	printf("\n>>");
	gets(pchReply);

	*pchBytes = (strlen(pchReply)+1)*sizeof(char);
}
