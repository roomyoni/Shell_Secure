#pragma once
#include "Structures.h"
#include <intrin.h>

// Global variable for provider handle
REGHANDLE RegHandle;


void* SavedBaseAddress = 0;

bool WriteRegistry(UNICODE_STRING RegPath, UNICODE_STRING Key, PVOID Address, ULONG Type, ULONG Size)
{
    bool Success = false;
    HANDLE hKey;
    OBJECT_ATTRIBUTES ObjAttr;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    ULONG Disposition;
    InitializeObjectAttributes(&ObjAttr, &RegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    Status = ZwCreateKey(&hKey, KEY_ALL_ACCESS, &ObjAttr, 0, NULL, REG_OPTION_NON_VOLATILE, &Disposition);

    if (NT_SUCCESS(Status))
    {
        Status = ZwSetValueKey(hKey, &Key, 0, Type, Address, Size);

        if (NT_SUCCESS(Status)) {
            Success = true;
        }
        ZwClose(hKey);
    }

    return Success;
}

