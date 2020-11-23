#include <stdio.h>
#include <stdlib.h>

#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "libtcc.h"

typedef void (*loadFunc)(const char* request, char** contentPtr, int* lengthPtr);

loadFunc compile(const char* filename) {
	TCCState *s = tcc_new();

	if (!s) {
		fprintf(stderr, "Can't create tcc state.\n");
		return NULL;
	}

	tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

	if (tcc_add_file(s, filename) == -1) {
		fprintf(stderr, "Can't add file.\n");
		tcc_delete(s);
		return NULL;
	}

	if (tcc_relocate(s, TCC_RELOCATE_AUTO) == -1) {
		fprintf(stderr, "Can't relocate.\n");
		tcc_delete(s);
		return NULL;
	}
	
	loadFunc func = (loadFunc) tcc_get_symbol(s, "load");
	if (func == NULL) {
		fprintf(stderr, "Can't find 'load' symbol.\n");
	}
	tcc_delete(s);
	return func;
}

// Using Windows fileapi.h to preserve \r\n
void getFileContent(const char* filename, char** contentPtr, int* lengthPtr) {
	HANDLE fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		*lengthPtr = -1;
		return;
	}
	
	DWORD length = GetFileSize(fileHandle, NULL);
	*lengthPtr = (int)length;
	
	*contentPtr = (char*)malloc(*lengthPtr);
	DWORD bytesRead;
	ReadFile(fileHandle, (void*)*contentPtr, length, &bytesRead, NULL);
	CloseHandle(fileHandle);
}

struct mimeDef {
	const char* ext;
	const char* mime;
};
// List is from https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types, incomplete
struct mimeDef mimes[23] = {
	{"html" , "text/html"      },
	{"c"    , "text/html"      },
	{"txt"  , "text/plain"     },
	{"css"  , "text/css"       },
	{"js"   , "text/javascript"},
	{"apng" , "image/apng"     },
	{"bmp"  , "image/bmp"      },
	{"gif"  , "image/gif"      },
	{"ico"  , "image/x-icon"   },
	{"cur"  , "image/x-icon"   },
	{"jpg"  , "image/jpeg"     },
	{"jpeg" , "image/jpeg"     },
	{"jfif" , "image/jpeg"     },
	{"pjpeg", "image/jpeg"     },
	{"pjp"  , "image/jpeg"     },
	{"png"  , "image/png"      },
	{"svg"  , "image/svg+xml"  },
	{"tif"  , "image/tiff"     },
	{"tiff" , "image/tiff"     },
	{"webp" , "image/webp"     },
	{"wav"  , "audio/wav"      },
	{"webm" , "video/webm"     },
	{"ogg"  , "video/ogg"      }
};
const char* getMime(const char* path) {
	int pos = 0;
	char ext[12] = {};
	int extPos = 0;
	int gotDot = 0;
	while (path[pos] != 0) {
		if (gotDot) {
			if (path[pos] == '.') {
				extPos = 0;
			} else {
				ext[extPos++] = path[pos];
			}
		}
		if (path[pos] == '.') {
			gotDot = 1;
		}
		pos++;
	}
	ext[extPos] = 0;
	if (gotDot) { // Only process the one with extension
		int i;
		for (i = 0; i < (sizeof(mimes) / sizeof(struct mimeDef)); i++) {
			for (pos = 0; pos <= extPos; pos++) {
				if (ext[pos] != mimes[i].ext[pos]) {
					break;
				}
			}
			if (pos > extPos) {
				return mimes[i].mime;
			}
		}
	}
	return "application/octet-stream";
}

const char* notFoundStr = "Not found!";
const char* compileErrorStr = "Compilation error!";
const char* okHeader =
"HTTP/1.1 200 OK\r\n"
"Server: tinyweb/0.0.1 (Win64)\r\n"
"Content-Length: %d\r\n"
"Content-Type: %s\r\n"
"Connection: Closed\r\n"
"\r\n";
DWORD handleClient(void* socket) {
	SOCKET sock = (SOCKET)socket;
	int connected = 1;
	char buffer[0xffff];
	while (connected) {
		int receiveLength = recv(sock, buffer, 0xffff, 0);
		if (receiveLength > 0) {
			buffer[receiveLength] = 0;
			
			char path[1024] = "www";
			int readPos = 0;
			int writePos = 3;
			while (buffer[readPos] != ' ') {
				readPos++;
			}
			readPos++;
			while (buffer[readPos] != ' ') {
				path[writePos++] = buffer[readPos++];
			}
			path[writePos] = 0;
			
			char* content;
			int contentLength;
			if (path[writePos - 2] == '.' && path[writePos - 1] == 'c') {
				loadFunc func = compile(path);
				if (func == NULL) {
					contentLength = 0;
					content = (char*)malloc(18);
					while (compileErrorStr[contentLength] != 0) {
						content[contentLength] = compileErrorStr[contentLength];
						contentLength++;
					}
				} else {
					func("", &content, &contentLength);
				}
			} else {
				getFileContent(path, &content, &contentLength);
				if (contentLength == -1) {
					contentLength = 0;
					content = (char*)malloc(10);
					while (notFoundStr[contentLength] != 0) {
						content[contentLength] = notFoundStr[contentLength];
						contentLength++;
					}
				}
			}
			
			// Send header
			char sendBuffer[1024];
			sprintf(sendBuffer, okHeader, contentLength, getMime(path));
			int sendBufferLength = 0;
			while (sendBuffer[sendBufferLength] != 0) {sendBufferLength++;}
			send(sock, sendBuffer, sendBufferLength, 0);
			
			// Send content
			send(sock, content, contentLength, 0);
			
			free(content);
			
			closesocket(sock);
		} else {
			connected = 0;
		}
	}
}

int main(void) {
	WSADATA wsaData;
	
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
	
	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	
	bind(server, (SOCKADDR*)&addr, sizeof(addr));
	
	while (1) {
		listen(server, 0);
		SOCKET client;
		SOCKADDR_IN clientAddr;
		int clientAddrSize = sizeof(clientAddr);
		
		if ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET) {
			HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handleClient, (void*)client, 0, 0);
		}
	}
	
	/*
	loadFunc test = compile("test.c");
	if (test != NULL) {
		printf("Result: %d\n", test(NULL));
	}*/
	
	WSACleanup();
	return 0;
}