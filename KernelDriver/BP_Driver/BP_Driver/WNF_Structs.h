#pragma once
#include <ntddk.h>
#pragma warning( disable : 4996)

typedef NTSYSAPI NTSTATUS(NTAPI* pfnExSubscribeWnfStateChange)(
	PVOID* wnfStruct,
	PCWNF_STATE_NAME stateName,
	ULONG eventMask,
	PULONG changeStamp,
	PVOID callback,
	PVOID callbackContext);

typedef NTSYSAPI NTSTATUS(NTAPI* pfnExUnsubscribeWnfStateChange)(
	PVOID wnfStruct);

typedef NTSYSAPI NTSTATUS(NTAPI* pfnSubscribeCallback)(
	PVOID wnfStruct,
	PCWNF_STATE_NAME stateName,
	ULONG eventMask,
	ULONG changeStamp,
	PVOID typeId,
	PVOID callbackContext
	);

typedef NTSYSAPI NTSTATUS(NTAPI* pfnExQueryWnfStateData)(
	PVOID wnfStruct,
	PULONG changeStamp,
	PVOID buf,
	PULONG bufSize
	);

typedef NTSYSAPI NTSTATUS(NTAPI* pfnNtUpdateWnfStateData)(
	PVOID StateName,
	PVOID Buffer,
	ULONG Length,
	PVOID TypeId,
	PVOID ExplicitScope,
	ULONG MatchingChangeStamp,
	ULONG CheckStamp
	);

pfnExSubscribeWnfStateChange ExSubscribeWnfStateChange = nullptr;
pfnExUnsubscribeWnfStateChange ExUnsubscribeWnfStateChange = nullptr;
pfnExQueryWnfStateData ExQueryWnfStateData = nullptr;

NTSTATUS InitExports()
{
	UNICODE_STRING fnExSubscribeWnfStateChange = RTL_CONSTANT_STRING(L"ExSubscribeWnfStateChange");
	ExSubscribeWnfStateChange = (pfnExSubscribeWnfStateChange)MmGetSystemRoutineAddress(&fnExSubscribeWnfStateChange);

	UNICODE_STRING fnExUnsubscribeWnfStateChange = RTL_CONSTANT_STRING(L"ExUnsubscribeWnfStateChange");
	ExUnsubscribeWnfStateChange = (pfnExUnsubscribeWnfStateChange)MmGetSystemRoutineAddress(&fnExUnsubscribeWnfStateChange);

	UNICODE_STRING fnExQueryWnfStateData = RTL_CONSTANT_STRING(L"ExQueryWnfStateData");
	ExQueryWnfStateData = (pfnExQueryWnfStateData)MmGetSystemRoutineAddress(&fnExQueryWnfStateData);

	if (ExSubscribeWnfStateChange == nullptr || ExUnsubscribeWnfStateChange == nullptr || ExQueryWnfStateData == nullptr)
	{
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}