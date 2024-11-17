#include "comm.h"

#define DEVICE_NAME L"\\Device\\yusecDevice"
CommCallback gCommCallback = NULL;

// public
NTSTATUS DriverRegisterComm(IN PDRIVER_OBJECT pDriver, IN CommCallback commCallback) {

	UNICODE_STRING symName = RTL_CONSTANT_STRING(SYM_NAME);
	UNICODE_STRING deviceName = RTL_CONSTANT_STRING(DEVICE_NAME);

	PDEVICE_OBJECT pDevice = NULL;
	NTSTATUS status = IoCreateDevice(
		pDriver,
		0,
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&pDevice
	);


	if (!NT_SUCCESS(status)) {
		KdPrintEx((77, 0, "[db] Create Device:%x\r\n", status));
		return status;
	}

	status = IoCreateSymbolicLink(&symName, &deviceName);

	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(pDevice);
		KdPrintEx((77, 0, "[db] Create SymbolLink :%x\r\n", status));
		return status;
	}

	pDevice->Flags &= ~DO_DEVICE_INITIALIZING;
	pDevice->Flags |= DO_BUFFERED_IO;

	pDriver->MajorFunction[IRP_MJ_CREATE] = initDispatch;
	pDriver->MajorFunction[IRP_MJ_CLOSE] = initDispatch;
	pDriver->MajorFunction[IRP_MJ_WRITE] = writeDispatch;
	//pDriver->MajorFunction[IRP_MJ_READ] = readDispatch;

	if (NT_SUCCESS(status)) {
		gCommCallback = commCallback;
	}
}

// public
VOID DriverDestroyComm(IN PDRIVER_OBJECT pDriver) {
	UNICODE_STRING symName = RTL_CONSTANT_STRING(SYM_NAME);
	IoDeleteSymbolicLink(&symName);
	if (pDriver->DeviceObject) {
		IoDeleteDevice(pDriver->DeviceObject);
	}
}

// private
NTSTATUS initDispatch(IN PDEVICE_OBJECT pDevice, IN PIRP pIrp) {
	NTSTATUS status = STATUS_SUCCESS;
	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}

// private
NTSTATUS readDispatch(IN PDEVICE_OBJECT pDevice, IN PIRP pIrp) {
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(pIrp);
	LARGE_INTEGER byteOffset = ioStack->Parameters.Read.ByteOffset;
	int length = ioStack->Parameters.Read.Length;

	if (length == sizeof(CommPackage) && gCommCallback) {
		PCommPackage package = (PCommPackage)pIrp->AssociatedIrp.SystemBuffer;
		if (MmIsAddressValid(package)) {
			status = gCommCallback(package);
		}
	}

	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}

// private
NTSTATUS writeDispatch(IN PDEVICE_OBJECT pDevice, IN PIRP pIrp) {
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(pIrp);
	//LARGE_INTEGER byteOffset = ioStack->Parameters.Write.ByteOffset;
	int length = ioStack->Parameters.Write.Length;

	if (length == sizeof(CommPackage) && gCommCallback) {
		PCommPackage package = (PCommPackage)pIrp->AssociatedIrp.SystemBuffer;
		if (MmIsAddressValid(package) && package->id == COMM_ID) {
			status = gCommCallback(package);
		}
	}

	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}