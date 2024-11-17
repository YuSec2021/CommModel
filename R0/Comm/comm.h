#pragma once
#include "commstruct.h"

typedef NTSTATUS(NTAPI* CommCallback)(PCommPackage package);

//
// Init Dispatch : For IRP_MJ_CREATE and IRP_MJ_CLOSE
//
NTSTATUS initDispatch(IN PDEVICE_OBJECT pDevice, IN PIRP pIrp);

//
// Read Dispatch: For IRP_MJ_READ
// 
NTSTATUS readDispatch(IN PDEVICE_OBJECT pDevice, IN PIRP pIrp);

//
// Write Dispatch: For IRP_MJ_WRITE
//
NTSTATUS writeDispatch(IN PDEVICE_OBJECT pDevice, IN PIRP pIrp);

//
// Register Driver
//
NTSTATUS DriverRegisterComm(IN PDRIVER_OBJECT pDriver, IN CommCallback commCallback);

//
// Destroy Driver
//
VOID DriverDestroyComm(IN PDRIVER_OBJECT pDriver);