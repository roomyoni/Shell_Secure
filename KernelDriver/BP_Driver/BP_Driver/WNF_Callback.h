#pragma once
#include "WNF_Structs.h"
#include "Functions.h"

namespace Settings
{
	bool AudioChange = false;
	bool DesktopFile = false;
	bool Geolocation = false;
	bool TimeZone = false;
}


uint64_t dtb = 0;
bool overflow = false;
NTSTATUS SubscribeCallback(
	PVOID wnfStruct,
	PCWNF_STATE_NAME stateName,
	ULONG eventMask,
	ULONG changeStamp,
	PVOID typeId,
	PVOID callbackContext
) {
	UNREFERENCED_PARAMETER(stateName);
	UNREFERENCED_PARAMETER(eventMask);
	UNREFERENCED_PARAMETER(changeStamp);
	UNREFERENCED_PARAMETER(typeId);
	UNREFERENCED_PARAMETER(callbackContext);

	PUCHAR buf = (PUCHAR)ExAllocatePool(PagedPool, PAGE_SIZE);
	ULONG bufSize = PAGE_SIZE;
	ULONG cStamp;

	auto status = ExQueryWnfStateData(wnfStruct, &cStamp, buf, &bufSize);

	if (status != STATUS_SUCCESS) {
		DbgPrint("Failed to query from WNF. NTSTATUS: 0x%X\n", status);
		return STATUS_SUCCESS;
	}

	DbgPrint("New Application Called!\n");
	DbgPrint("Buffer size: %lu bytes\n", bufSize);  // Print the size of the buffer


	UNICODE_STRING regPath, keyName;
	WCHAR datastring[] = L"APPLAUNCH";

	RtlInitUnicodeString(&regPath, L"\\Registry\\Machine\\Software\\ShellSecure");
	RtlInitUnicodeString(&keyName, L"Alert1");


	if (Settings::DesktopFile)
	{
		bool result = WriteRegistry(regPath, keyName, datastring, REG_SZ, sizeof(datastring));

		if (result) {
			DbgPrint("Registry write successful!\n");
		}
		else {
			DbgPrint("Failed to write to registry.\n");
		}
	}

	//WriteToFile(L"\\??\\C:\\trigger.txt", L"APPLAUNCH");

	if (bufSize != sizeof(uint64_t))
	{
		return STATUS_SUCCESS;
	}

	return STATUS_SUCCESS;
}

NTSTATUS SubscribeCallback2(
	PVOID wnfStruct,
	PCWNF_STATE_NAME stateName,
	ULONG eventMask,
	ULONG changeStamp,
	PVOID typeId,
	PVOID callbackContext
) {
	UNREFERENCED_PARAMETER(stateName);
	UNREFERENCED_PARAMETER(eventMask);
	UNREFERENCED_PARAMETER(changeStamp);
	UNREFERENCED_PARAMETER(typeId);
	UNREFERENCED_PARAMETER(callbackContext);

	PUCHAR buf = (PUCHAR)ExAllocatePool(PagedPool, PAGE_SIZE);
	ULONG bufSize = PAGE_SIZE;
	ULONG cStamp;

	auto status = ExQueryWnfStateData(wnfStruct, &cStamp, buf, &bufSize);

	if (status != STATUS_SUCCESS) {
		DbgPrint("Failed to query from WNF. NTSTATUS: 0x%X\n", status);
		return STATUS_SUCCESS;
	}

	DbgPrint("New Audio Called!\n");
	DbgPrint("Buffer size: %lu bytes\n", bufSize);  // Print the size of the buffer

	UNICODE_STRING regPath, keyName;
	WCHAR datastring[] = L"AUDIOLAUNCH";

	RtlInitUnicodeString(&regPath, L"\\Registry\\Machine\\Software\\ShellSecure");
	RtlInitUnicodeString(&keyName, L"Alert2");

	if (Settings::AudioChange)
	{
		bool result = WriteRegistry(regPath, keyName, datastring, REG_SZ, sizeof(datastring));

		if (result) {
			DbgPrint("Registry write successful!\n");
		}
		else {
			DbgPrint("Failed to write to registry.\n");
		}
	}

	//WriteToFile(L"\\??\\C:\\trigger.txt", L"AUDIOLAUNCH");

	if (bufSize != sizeof(uint64_t))
	{
		return STATUS_SUCCESS;
	}

	return STATUS_SUCCESS;
}

NTSTATUS SubscribeCallback3(
	PVOID wnfStruct,
	PCWNF_STATE_NAME stateName,
	ULONG eventMask,
	ULONG changeStamp,
	PVOID typeId,
	PVOID callbackContext
) {
	UNREFERENCED_PARAMETER(stateName);
	UNREFERENCED_PARAMETER(eventMask);
	UNREFERENCED_PARAMETER(changeStamp);
	UNREFERENCED_PARAMETER(typeId);
	UNREFERENCED_PARAMETER(callbackContext);

	PUCHAR buf = (PUCHAR)ExAllocatePool(PagedPool, PAGE_SIZE);
	ULONG bufSize = PAGE_SIZE;
	ULONG cStamp;

	auto status = ExQueryWnfStateData(wnfStruct, &cStamp, buf, &bufSize);

	if (status != STATUS_SUCCESS) {
		DbgPrint("Failed to query from WNF. NTSTATUS: 0x%X\n", status);
		return STATUS_SUCCESS;
	}

	DbgPrint("New Geolocation Called!\n");
	DbgPrint("Buffer size: %lu bytes\n", bufSize);  // Print the size of the buffer

	UNICODE_STRING regPath, keyName;
	WCHAR datastring[] = L"GEOLOCATION";

	RtlInitUnicodeString(&regPath, L"\\Registry\\Machine\\Software\\ShellSecure");
	RtlInitUnicodeString(&keyName, L"Alert3");

	if (Settings::Geolocation)
	{
		bool result = WriteRegistry(regPath, keyName, datastring, REG_SZ, sizeof(datastring));

		if (result) {
			DbgPrint("Registry write successful!\n");
		}
		else {
			DbgPrint("Failed to write to registry.\n");
		}
	}

	//WriteToFile(L"\\??\\C:\\trigger.txt", L"GEOLOCATION");

	if (bufSize != sizeof(uint64_t))
	{
		return STATUS_SUCCESS;
	}

	return STATUS_SUCCESS;
}

NTSTATUS SubscribeCallback4(
	PVOID wnfStruct,
	PCWNF_STATE_NAME stateName,
	ULONG eventMask,
	ULONG changeStamp,
	PVOID typeId,
	PVOID callbackContext
) {
	UNREFERENCED_PARAMETER(stateName);
	UNREFERENCED_PARAMETER(eventMask);
	UNREFERENCED_PARAMETER(changeStamp);
	UNREFERENCED_PARAMETER(typeId);
	UNREFERENCED_PARAMETER(callbackContext);

	PUCHAR buf = (PUCHAR)ExAllocatePool(PagedPool, PAGE_SIZE);
	ULONG bufSize = PAGE_SIZE;
	ULONG cStamp;

	auto status = ExQueryWnfStateData(wnfStruct, &cStamp, buf, &bufSize);

	if (status != STATUS_SUCCESS) {
		DbgPrint("Failed to query from WNF. NTSTATUS: 0x%X\n", status);
		return STATUS_SUCCESS;
	}

	DbgPrint("New TimeZone Called!\n");
	DbgPrint("Buffer size: %lu bytes\n", bufSize);  // Print the size of the buffer

	UNICODE_STRING regPath, keyName;
	WCHAR datastring[] = L"TIMEZONE";

	RtlInitUnicodeString(&regPath, L"\\Registry\\Machine\\Software\\ShellSecure");
	RtlInitUnicodeString(&keyName, L"Alert4");

	if (Settings::TimeZone)
	{
		bool result = WriteRegistry(regPath, keyName, datastring, REG_SZ, sizeof(datastring));

		if (result) {
			DbgPrint("Registry write successful!\n");
		}
		else {
			DbgPrint("Failed to write to registry.\n");
		}
	}

	//WriteToFile(L"\\??\\C:\\trigger.txt", L"TIMEZONE");

	if (bufSize != sizeof(uint64_t))
	{
		return STATUS_SUCCESS;
	}

	return STATUS_SUCCESS;
}