#include "Comm/comm.h"

NTSTATUS NTAPI Dispatch(PCommPackage package) {
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	switch (package->code) {
	case TEST:
	{
		DbgBreakPoint();
		PTest t = (PTest)package->inData;
		KdPrintEx((77, 0, "[db]----x: %x -----y: %x\r\n", t->x, t->y));
		t->x = 400;
		t->y = 10000;
		status = STATUS_SUCCESS;
		break;
	}
	}
	return status;
}

VOID DriverUnload(PDRIVER_OBJECT pDriver) {
	DriverDestroyComm(pDriver);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg) {
	DriverRegisterComm(pDriver, Dispatch);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}