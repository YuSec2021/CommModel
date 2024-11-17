#include "comm.h"

HANDLE ghDevice = NULL;

BOOLEAN DeviceInit() {
	if (ghDevice == NULL) {
		ghDevice = CreateFileW(
			SYM_NAME,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_SYSTEM,
			0
		);
	}

	if (ghDevice == NULL || ghDevice == INVALID_HANDLE_VALUE) {
		ghDevice = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOLEAN DeviceComm(ULONG code, PVOID inData, ULONG inLen, PVOID outData, ULONG outLen) {
	if (!ghDevice) {
		DeviceInit();
	}

	if (ghDevice) {
		CommPackage package = { 0 };
		package.code = code;
		package.inData = (ULONG64)inData;
		package.inLen = (ULONG64)inLen;
		package.outData = (ULONG64)outData;
		package.outLen = (ULONG64)outLen;
		package.id = COMM_ID;
		SIZE_T p = 0;
		return WriteFile(ghDevice, &package, sizeof(package), &p, NULL);
	}

	return FALSE;
}
