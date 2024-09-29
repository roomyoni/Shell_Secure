#include <ntddk.h>
#include "WNF_Callback.h"
PVOID handle;
PVOID handle2;
PVOID handle3;
PVOID handle4;
LARGE_INTEGER cookie;
UNICODE_STRING key = { 0 };

WNF_STATE_NAME SplitWnfStateName(uint64_t fullStateName) {
	WNF_STATE_NAME stateName;

	// Extract the lower and upper 32 bits from the 64-bit state name
	stateName.Data[0] = static_cast<uint32_t>(fullStateName & 0xFFFFFFFF);     // Lower 32 bits
	stateName.Data[1] = static_cast<uint32_t>((fullStateName >> 32) & 0xFFFFFFFF); // Upper 32 bits

	return stateName;
}


NTSTATUS RegistryCallback(PVOID callbackContext, PVOID arg1, PVOID arg2) {
	UNREFERENCED_PARAMETER(callbackContext);
	if (INT(arg1) != 16) // RegNtPostSetValueKey
		return STATUS_SUCCESS;

	PREG_POST_OPERATION_INFORMATION postInfo = (PREG_POST_OPERATION_INFORMATION)arg2;
	PREG_SET_VALUE_KEY_INFORMATION preInfo = (PREG_SET_VALUE_KEY_INFORMATION)postInfo->PreInformation;

	if ((RtlEqualUnicodeString)(preInfo->ValueName, &key, TRUE) == 0)
	{
		return STATUS_SUCCESS;
	}

	Minimalist_RCMD* request = *(Minimalist_RCMD**)preInfo->Data;
	if (!request)
	{
		return STATUS_SUCCESS;
	}

	if (request->magic_code != 1337)
	{
		return STATUS_SUCCESS;
	}

	switch (request->request)
	{
		case REQUEST_SETTINGS:
		{
			DbgPrint("--------------------------------------------");
			Settings::AudioChange = (bool)request->AudioChange;
			DbgPrint("Settings::AudioChange: %u\n", Settings::AudioChange);
			Settings::DesktopFile = (bool)request->DesktopFile;
			DbgPrint("Settings::DesktopFile: %u\n", Settings::DesktopFile);
			Settings::Geolocation = (bool)request->Geolocation;
			DbgPrint("Settings::Geolocation: %u\n", Settings::Geolocation);
			Settings::TimeZone = (bool)request->TimeZone;
			DbgPrint("Settings::TimeZone: %u\n", Settings::TimeZone);
			DbgPrint("--------------------------------------------\n");
			break;
		}
	}
}
#define MB_ICONINFORMATION 0x40
extern "C" NTSTATUS NTAPI ExRaiseHardError(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);
ULONG KeMessageBox(PCWSTR title, PCWSTR text, ULONG_PTR type)
{
	UNICODE_STRING uTitle = { 0 };
	UNICODE_STRING uText = { 0 };

	RtlInitUnicodeString(&uTitle, title);
	RtlInitUnicodeString(&uText, text);

	ULONG_PTR args[] = { (ULONG_PTR)&uText, (ULONG_PTR)&uTitle, type };
	ULONG response = 0;

	ExRaiseHardError(STATUS_SERVICE_NOTIFICATION, 3, 3, args, 2, &response);
	return response;
}



void DriverUnload(PDRIVER_OBJECT drvObj) {
	UNREFERENCED_PARAMETER(drvObj);
	if (ExUnsubscribeWnfStateChange != NULL) {
		NTSTATUS status = ExUnsubscribeWnfStateChange(handle);
		ExUnsubscribeWnfStateChange(handle2);
		ExUnsubscribeWnfStateChange(handle3);
		ExUnsubscribeWnfStateChange(handle4);
		DbgPrint("Successfully unsubscribed from WNF.\n");
	}
	CmUnRegisterCallback(cookie);
	DbgPrint("Driver Unloaded.\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT drvObj, PUNICODE_STRING regPath) {
	drvObj->DriverUnload = DriverUnload;
	UNREFERENCED_PARAMETER(regPath);

	DbgPrint("Entry Called.\n");

	NTSTATUS InitStatus = InitExports();
	if (InitStatus == STATUS_SUCCESS)
	{
		DbgPrint("Succesfully Processed Exports.\n");

		WNF_STATE_NAME stateName{0};
		stateName = SplitWnfStateName(0xd83063ea3be5075);

		auto status = ExSubscribeWnfStateChange(&handle, &stateName, 3, NULL, SubscribeCallback, (PVOID)1);

		if (status == STATUS_SUCCESS) {
			DbgPrint("Callback created!\n");
		}
		else {
			DbgPrint("Failed to create callback. NTSTATUS: 0x%X\n", status);
		}

		WNF_STATE_NAME stateName2{ 0 };
		stateName2 = SplitWnfStateName(0x2821b2ca3bc4075);

		auto status2 = ExSubscribeWnfStateChange(&handle2, &stateName2, 3, NULL, SubscribeCallback2, (PVOID)1);

		if (status2 == STATUS_SUCCESS) {
			DbgPrint("Callback created!\n");
		}
		else {
			DbgPrint("Failed to create callback. NTSTATUS: 0x%X\n", status2);
		}

		WNF_STATE_NAME stateName3{ 0 };
		stateName3 = SplitWnfStateName(0x41840b3ea3bcb075);

		auto status3 = ExSubscribeWnfStateChange(&handle3, &stateName3, 3, NULL, SubscribeCallback3, (PVOID)1);

		if (status3 == STATUS_SUCCESS) {
			DbgPrint("Callback created!\n");
		}
		else {
			DbgPrint("Failed to create callback. NTSTATUS: 0x%X\n", status3);
		}

		WNF_STATE_NAME stateName4{ 0 };
		stateName4 = SplitWnfStateName(0x41840b3ea3bc3875);

		auto status4 = ExSubscribeWnfStateChange(&handle4, &stateName4, 3, NULL, SubscribeCallback4, (PVOID)1);

		if (status4 == STATUS_SUCCESS) {
			DbgPrint("Callback created!\n");
		}
		else {
			DbgPrint("Failed to create callback. NTSTATUS: 0x%X\n", status4);
		}
	}
	else
	{
		DbgPrint("Couldn't Process Exports.\n");
	}
	RtlInitUnicodeString(&key, L"MuiInit");

	UNICODE_STRING altitude{};
	RtlInitUnicodeString(&altitude, L"136800");

	if (!NT_SUCCESS(CmRegisterCallbackEx(
		RegistryCallback,
		&altitude,
		drvObj,
		nullptr,
		&cookie,
		nullptr
	)))
	{
		DbgPrint("Failed CmRegisterCallbackEx");
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		DbgPrint("Success CmRegisterCallbackEx");
		return STATUS_SUCCESS;
	}
}