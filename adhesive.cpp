#include <iostream>
#include <Windows.h>
 
auto ExceptionHandler(PEXCEPTION_POINTERS exception) -> LONG
{
	// return EXCEPTION_CONTINUE_EXECUTION so we never crash
	printf("[*] Exception Information: \n");
 
	printf("\tRCX = 0x%llx\n", exception->ContextRecord->Rcx);
	printf("\tR8 = 0x%llx\n", exception->ContextRecord->R8);
 
	return EXCEPTION_CONTINUE_EXECUTION;
}
 
auto Main() -> void
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
 
	// create exception handler
	AddVectoredExceptionHandler(0, ExceptionHandler);
 
	DWORD old;
 
	constexpr auto INT3 = 0xCC;
	constexpr auto size = 0x3;
 
	// get the base address of adhesive
	const uint64_t adhesive = reinterpret_cast<uint64_t>(GetModuleHandleW(L"adhesive.dll"));
	// get the address to set the exception to get triggered at
	void* address = (void*)(adhesive + 0x378921); // mov rcx, [r8] (0x49, 0x8B, 0x08 )
 
	// change page protections and place INT3 instructions
	// to get the exception triggered
	VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old);
	memset(address, INT3, size);
	VirtualProtect(address, size, old, &old);
}
 
auto DllMain(void*, int reason, void*)->bool
{
	if (reason != 1)
		return false;
 
	Main();
	return true;
}