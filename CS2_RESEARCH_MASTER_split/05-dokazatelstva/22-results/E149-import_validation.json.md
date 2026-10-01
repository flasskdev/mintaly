<!-- split-part | CS2_RESEARCH_MASTER.md lines 76454-79593 | body-sha256 d7665de5416dd03d80733432d210894a1e4fe51a8b3f5dfc81c26b2a46cee9b2 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-149"></a>

## E149. `analysis/results/import_validation.json`

Bytes: 72096. SHA-256: `098f5aba3f3219d781c4d29547031c1d105ca330a5a4b0119ee759e9e1c59d24`.

```json
[
  {
    "VA": "0x212C4291D40",
    "RVA": "0x00F91D40",
    "module": "ntdll.dll",
    "name": "RtlAcquireSRWLockExclusive",
    "dump_pointer": "0x7ffd9df67810",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4291D48",
    "RVA": "0x00F91D48",
    "module": "ntdll.dll",
    "name": "RtlAcquireSRWLockShared",
    "dump_pointer": "0x7ffd9df56840",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291D50",
    "RVA": "0x00F91D50",
    "module": "KERNEL32.DLL",
    "name": "AreFileApisANSI",
    "dump_pointer": "0x7ffd9c3e9050",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291D58",
    "RVA": "0x00F91D58",
    "module": "KERNEL32.DLL",
    "name": "CloseHandle",
    "dump_pointer": "0x7ffd9c3f01e0",
    "indexed_xrefs": 60
  },
  {
    "VA": "0x212C4291D68",
    "RVA": "0x00F91D68",
    "module": "KERNEL32.DLL",
    "name": "CompareFileTime",
    "dump_pointer": "0x7ffd9c3f0410",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291D70",
    "RVA": "0x00F91D70",
    "module": "KERNEL32.DLL",
    "name": "CompareStringEx",
    "dump_pointer": "0x7ffd9c3eeeb0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291D78",
    "RVA": "0x00F91D78",
    "module": "KERNEL32.DLL",
    "name": "CompareStringW",
    "dump_pointer": "0x7ffd9c3e5ca0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291D80",
    "RVA": "0x00F91D80",
    "module": "KERNEL32.DLL",
    "name": "ConvertFiberToThread",
    "dump_pointer": "0x7ffd9c3f0b30",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291D88",
    "RVA": "0x00F91D88",
    "module": "KERNEL32.DLL",
    "name": "ConvertThreadToFiber",
    "dump_pointer": "0x7ffd9c3f0b40",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291D90",
    "RVA": "0x00F91D90",
    "module": "KERNEL32.DLL",
    "name": "CreateDirectoryW",
    "dump_pointer": "0x7ffd9c3f0430",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291D98",
    "RVA": "0x00F91D98",
    "module": "KERNEL32.DLL",
    "name": "CreateEventW",
    "dump_pointer": "0x7ffd9c3f0260",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4291DA0",
    "RVA": "0x00F91DA0",
    "module": "KERNEL32.DLL",
    "name": "CreateFiber",
    "dump_pointer": "0x7ffd9c3f0b60",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291DA8",
    "RVA": "0x00F91DA8",
    "module": "KERNEL32.DLL",
    "name": "CreateFile2",
    "dump_pointer": "0x7ffd9c3f0440",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291DB0",
    "RVA": "0x00F91DB0",
    "module": "KERNEL32.DLL",
    "name": "CreateFileA",
    "dump_pointer": "0x7ffd9c3f0450",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291DB8",
    "RVA": "0x00F91DB8",
    "module": "KERNEL32.DLL",
    "name": "CreateFileMappingA",
    "dump_pointer": "0x7ffd9c3e5030",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291DC0",
    "RVA": "0x00F91DC0",
    "module": "KERNEL32.DLL",
    "name": "CreateFileW",
    "dump_pointer": "0x7ffd9c3f0460",
    "indexed_xrefs": 8
  },
  {
    "VA": "0x212C4291DC8",
    "RVA": "0x00F91DC8",
    "module": "KERNEL32.DLL",
    "name": "CreateIoCompletionPort",
    "dump_pointer": "0x7ffd9c3e8de0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291DD0",
    "RVA": "0x00F91DD0",
    "module": "KERNEL32.DLL",
    "name": "CreatePipe",
    "dump_pointer": "0x7ffd9c3ee750",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291DD8",
    "RVA": "0x00F91DD8",
    "module": "KERNEL32.DLL",
    "name": "CreateProcessW",
    "dump_pointer": "0x7ffd9c3e61a0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291DE0",
    "RVA": "0x00F91DE0",
    "module": "KERNEL32.DLL",
    "name": "CreateThread",
    "dump_pointer": "0x7ffd9c3e44b0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291DF0",
    "RVA": "0x00F91DF0",
    "module": "KERNEL32.DLL",
    "name": "CreateTimerQueue",
    "dump_pointer": "0x7ffd9c3edd20",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291DF8",
    "RVA": "0x00F91DF8",
    "module": "KERNEL32.DLL",
    "name": "CreateTimerQueueTimer",
    "dump_pointer": "0x7ffd9c3e88e0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291E08",
    "RVA": "0x00F91E08",
    "module": "KERNEL32.DLL",
    "name": "CreateWaitableTimerW",
    "dump_pointer": "0x7ffd9c3d2270",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291E10",
    "RVA": "0x00F91E10",
    "module": "ntdll.dll",
    "name": "RtlDecodePointer",
    "dump_pointer": "0x7ffd9df9d870",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291E18",
    "RVA": "0x00F91E18",
    "module": "ntdll.dll",
    "name": "RtlDeleteCriticalSection",
    "dump_pointer": "0x7ffd9df8a770",
    "indexed_xrefs": 21
  },
  {
    "VA": "0x212C4291E20",
    "RVA": "0x00F91E20",
    "module": "KERNEL32.DLL",
    "name": "DeleteFiber",
    "dump_pointer": "0x7ffd9c3f0b80",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4291E28",
    "RVA": "0x00F91E28",
    "module": "KERNEL32.DLL",
    "name": "DeleteFileW",
    "dump_pointer": "0x7ffd9c3f0490",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291E30",
    "RVA": "0x00F91E30",
    "module": "KERNEL32.DLL",
    "name": "DeleteTimerQueue",
    "dump_pointer": "0x7ffd9c3f0920",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4291E38",
    "RVA": "0x00F91E38",
    "module": "KERNEL32.DLL",
    "name": "DeleteTimerQueueTimer",
    "dump_pointer": "0x7ffd9c3e94c0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291E50",
    "RVA": "0x00F91E50",
    "module": "KERNEL32.DLL",
    "name": "DuplicateHandle",
    "dump_pointer": "0x7ffd9c3f01f0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291E58",
    "RVA": "0x00F91E58",
    "module": "ntdll.dll",
    "name": "RtlEncodePointer",
    "dump_pointer": "0x7ffd9dfa3730",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4291E60",
    "RVA": "0x00F91E60",
    "module": "ntdll.dll",
    "name": "RtlEnterCriticalSection",
    "dump_pointer": "0x7ffd9df51690",
    "indexed_xrefs": 51
  },
  {
    "VA": "0x212C4291E68",
    "RVA": "0x00F91E68",
    "module": "KERNEL32.DLL",
    "name": "EnumSystemLocalesW",
    "dump_pointer": "0x7ffd9c40b4a0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4291E70",
    "RVA": "0x00F91E70",
    "module": "KERNEL32.DLL",
    "name": "ExitProcess",
    "dump_pointer": "0x7ffd9c3e7fa0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291E78",
    "RVA": "0x00F91E78",
    "module": "KERNEL32.DLL",
    "name": "FileTimeToSystemTime",
    "dump_pointer": "0x7ffd9c3f0990",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291E80",
    "RVA": "0x00F91E80",
    "module": "KERNEL32.DLL",
    "name": "FindClose",
    "dump_pointer": "0x7ffd9c3f04c0",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4291E88",
    "RVA": "0x00F91E88",
    "module": "KERNEL32.DLL",
    "name": "FindFirstFileA",
    "dump_pointer": "0x7ffd9c3f0500",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291E90",
    "RVA": "0x00F91E90",
    "module": "KERNEL32.DLL",
    "name": "FindFirstFileExW",
    "dump_pointer": "0x7ffd9c3f0520",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291E98",
    "RVA": "0x00F91E98",
    "module": "KERNEL32.DLL",
    "name": "FindFirstFileW",
    "dump_pointer": "0x7ffd9c3f0540",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291EA0",
    "RVA": "0x00F91EA0",
    "module": "KERNEL32.DLL",
    "name": "FindNextFileA",
    "dump_pointer": "0x7ffd9c3f0570",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291EA8",
    "RVA": "0x00F91EA8",
    "module": "KERNEL32.DLL",
    "name": "FindNextFileW",
    "dump_pointer": "0x7ffd9c3f0590",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291EB0",
    "RVA": "0x00F91EB0",
    "module": "KERNEL32.DLL",
    "name": "FlsAlloc",
    "dump_pointer": "0x7ffd9c3e8b10",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291EB8",
    "RVA": "0x00F91EB8",
    "module": "KERNEL32.DLL",
    "name": "FlsFree",
    "dump_pointer": "0x7ffd9c3e9100",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291EC0",
    "RVA": "0x00F91EC0",
    "module": "KERNEL32.DLL",
    "name": "FlsGetValue",
    "dump_pointer": "0x7ffd9c3e3310",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4291EC8",
    "RVA": "0x00F91EC8",
    "module": "KERNEL32.DLL",
    "name": "FlsSetValue",
    "dump_pointer": "0x7ffd9c3e50f0",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4291ED0",
    "RVA": "0x00F91ED0",
    "module": "KERNEL32.DLL",
    "name": "FlushFileBuffers",
    "dump_pointer": "0x7ffd9c3f05c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291ED8",
    "RVA": "0x00F91ED8",
    "module": "KERNEL32.DLL",
    "name": "FormatMessageA",
    "dump_pointer": "0x7ffd9c3e9660",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4291EE0",
    "RVA": "0x00F91EE0",
    "module": "KERNEL32.DLL",
    "name": "FormatMessageW",
    "dump_pointer": "0x7ffd9c3e7010",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291EE8",
    "RVA": "0x00F91EE8",
    "module": "KERNEL32.DLL",
    "name": "FreeEnvironmentStringsW",
    "dump_pointer": "0x7ffd9c3e8880",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4291EF0",
    "RVA": "0x00F91EF0",
    "module": "KERNEL32.DLL",
    "name": "FreeLibrary",
    "dump_pointer": "0x7ffd9c3e5420",
    "indexed_xrefs": 17
  },
  {
    "VA": "0x212C4291EF8",
    "RVA": "0x00F91EF8",
    "module": "KERNEL32.DLL",
    "name": "FreeLibraryAndExitThread",
    "dump_pointer": "0x7ffd9c3e99a0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291F00",
    "RVA": "0x00F91F00",
    "module": "KERNEL32.DLL",
    "name": "GetACP",
    "dump_pointer": "0x7ffd9c3e8130",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4291F08",
    "RVA": "0x00F91F08",
    "module": "KERNEL32.DLL",
    "name": "GetCPInfo",
    "dump_pointer": "0x7ffd9c3e7e00",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4291F10",
    "RVA": "0x00F91F10",
    "module": "KERNEL32.DLL",
    "name": "GetCommandLineA",
    "dump_pointer": "0x7ffd9c3e8ad0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291F18",
    "RVA": "0x00F91F18",
    "module": "KERNEL32.DLL",
    "name": "GetCommandLineW",
    "dump_pointer": "0x7ffd9c3e7de0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291F20",
    "RVA": "0x00F91F20",
    "module": "KERNEL32.DLL",
    "name": "GetConsoleMode",
    "dump_pointer": "0x7ffd9c3f0c40",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4291F28",
    "RVA": "0x00F91F28",
    "module": "KERNEL32.DLL",
    "name": "GetConsoleOutputCP",
    "dump_pointer": "0x7ffd9c3f0c50",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291F30",
    "RVA": "0x00F91F30",
    "module": "KERNEL32.DLL",
    "name": "GetCurrentDirectoryW",
    "dump_pointer": "0x7ffd9c3e8990",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4291F38",
    "RVA": "0x00F91F38",
    "module": "KERNEL32.DLL",
    "name": "GetCurrentProcess",
    "dump_pointer": "0x7ffd9c3f0160",
    "indexed_xrefs": 11
  },
  {
    "VA": "0x212C4291F40",
    "RVA": "0x00F91F40",
    "module": "KERNEL32.DLL",
    "name": "GetCurrentProcessId",
    "dump_pointer": "0x7ffd9c3f0170",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4291F48",
    "RVA": "0x00F91F48",
    "module": "KERNEL32.DLL",
    "name": "GetCurrentThread",
    "dump_pointer": "0x7ffd9c3e0d90",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291F50",
    "RVA": "0x00F91F50",
    "module": "KERNEL32.DLL",
    "name": "GetCurrentThreadId",
    "dump_pointer": "0x7ffd9c3d2750",
    "indexed_xrefs": 12
  },
  {
    "VA": "0x212C4291F58",
    "RVA": "0x00F91F58",
    "module": "KERNEL32.DLL",
    "name": "GetDateFormatW",
    "dump_pointer": "0x7ffd9c3e9140",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291F60",
    "RVA": "0x00F91F60",
    "module": "KERNEL32.DLL",
    "name": "GetDriveTypeW",
    "dump_pointer": "0x7ffd9c3f0620",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291F68",
    "RVA": "0x00F91F68",
    "module": "KERNEL32.DLL",
    "name": "GetEnvironmentStringsW",
    "dump_pointer": "0x7ffd9c3e8860",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291F70",
    "RVA": "0x00F91F70",
    "module": "KERNEL32.DLL",
    "name": "GetEnvironmentVariableA",
    "dump_pointer": "0x7ffd9c3e9360",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291F78",
    "RVA": "0x00F91F78",
    "module": "KERNEL32.DLL",
    "name": "GetEnvironmentVariableW",
    "dump_pointer": "0x7ffd9c3e4530",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291F80",
    "RVA": "0x00F91F80",
    "module": "KERNEL32.DLL",
    "name": "GetExitCodeProcess",
    "dump_pointer": "0x7ffd9c3e6e80",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291F88",
    "RVA": "0x00F91F88",
    "module": "KERNEL32.DLL",
    "name": "GetFileAttributesExW",
    "dump_pointer": "0x7ffd9c3f0650",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291F90",
    "RVA": "0x00F91F90",
    "module": "KERNEL32.DLL",
    "name": "GetFileAttributesW",
    "dump_pointer": "0x7ffd9c3f0660",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291F98",
    "RVA": "0x00F91F98",
    "module": "KERNEL32.DLL",
    "name": "GetFileInformationByHandle",
    "dump_pointer": "0x7ffd9c3f0670",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291FA0",
    "RVA": "0x00F91FA0",
    "module": "KERNEL32.DLL",
    "name": "GetFileInformationByHandleEx",
    "dump_pointer": "0x7ffd9c3e54d0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4291FA8",
    "RVA": "0x00F91FA8",
    "module": "KERNEL32.DLL",
    "name": "GetFileSizeEx",
    "dump_pointer": "0x7ffd9c3f0690",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291FB0",
    "RVA": "0x00F91FB0",
    "module": "KERNEL32.DLL",
    "name": "GetFileType",
    "dump_pointer": "0x7ffd9c3f06b0",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4291FB8",
    "RVA": "0x00F91FB8",
    "module": "KERNEL32.DLL",
    "name": "GetFinalPathNameByHandleW",
    "dump_pointer": "0x7ffd9c3f06d0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291FC0",
    "RVA": "0x00F91FC0",
    "module": "KERNEL32.DLL",
    "name": "GetFullPathNameW",
    "dump_pointer": "0x7ffd9c3f06f0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4291FC8",
    "RVA": "0x00F91FC8",
    "module": "KERNEL32.DLL",
    "name": "GetLastError",
    "dump_pointer": "0x7ffd9c3e0df0",
    "indexed_xrefs": 123
  },
  {
    "VA": "0x212C4291FD0",
    "RVA": "0x00F91FD0",
    "module": "KERNEL32.DLL",
    "name": "GetLocalTime",
    "dump_pointer": "0x7ffd9c3e7f80",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291FD8",
    "RVA": "0x00F91FD8",
    "module": "KERNEL32.DLL",
    "name": "GetLocaleInfoEx",
    "dump_pointer": "0x7ffd9c3e54b0",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4291FE0",
    "RVA": "0x00F91FE0",
    "module": "KERNEL32.DLL",
    "name": "GetLocaleInfoW",
    "dump_pointer": "0x7ffd9c3e9070",
    "indexed_xrefs": 11
  },
  {
    "VA": "0x212C4291FE8",
    "RVA": "0x00F91FE8",
    "module": "KERNELBASE.dll",
    "name": "GetLogicalProcessorInformationEx",
    "dump_pointer": "0x7ffd9b94e570",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4291FF0",
    "RVA": "0x00F91FF0",
    "module": "KERNEL32.DLL",
    "name": "GetModuleFileNameA",
    "dump_pointer": "0x7ffd9c3e89f0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4291FF8",
    "RVA": "0x00F91FF8",
    "module": "KERNEL32.DLL",
    "name": "GetModuleFileNameW",
    "dump_pointer": "0x7ffd9c3e7030",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292000",
    "RVA": "0x00F92000",
    "module": "KERNEL32.DLL",
    "name": "GetModuleHandleA",
    "dump_pointer": "0x7ffd9c3e8940",
    "indexed_xrefs": 14
  },
  {
    "VA": "0x212C4292008",
    "RVA": "0x00F92008",
    "module": "KERNEL32.DLL",
    "name": "GetModuleHandleExA",
    "dump_pointer": "0x7ffd9c3e9b60",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4292010",
    "RVA": "0x00F92010",
    "module": "KERNEL32.DLL",
    "name": "GetModuleHandleExW",
    "dump_pointer": "0x7ffd9c3e84a0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292018",
    "RVA": "0x00F92018",
    "module": "KERNEL32.DLL",
    "name": "GetModuleHandleW",
    "dump_pointer": "0x7ffd9c3e66b0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292020",
    "RVA": "0x00F92020",
    "module": "KERNEL32.DLL",
    "name": "GetOEMCP",
    "dump_pointer": "0x7ffd9c3e9700",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292030",
    "RVA": "0x00F92030",
    "module": "KERNEL32.DLL",
    "name": "GetProcAddress",
    "dump_pointer": "0x7ffd9c3e3c10",
    "indexed_xrefs": 29
  },
  {
    "VA": "0x212C4292038",
    "RVA": "0x00F92038",
    "module": "KERNEL32.DLL",
    "name": "GetProcessHeap",
    "dump_pointer": "0x7ffd9c3e0dd0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292040",
    "RVA": "0x00F92040",
    "module": "KERNEL32.DLL",
    "name": "GetQueuedCompletionStatus",
    "dump_pointer": "0x7ffd9c3e0db0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292048",
    "RVA": "0x00F92048",
    "module": "KERNEL32.DLL",
    "name": "GetStartupInfoW",
    "dump_pointer": "0x7ffd9c3e72c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292050",
    "RVA": "0x00F92050",
    "module": "KERNEL32.DLL",
    "name": "GetStdHandle",
    "dump_pointer": "0x7ffd9c3e7d00",
    "indexed_xrefs": 7
  },
  {
    "VA": "0x212C4292058",
    "RVA": "0x00F92058",
    "module": "KERNEL32.DLL",
    "name": "GetStringTypeW",
    "dump_pointer": "0x7ffd9c3e80a0",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292060",
    "RVA": "0x00F92060",
    "module": "KERNEL32.DLL",
    "name": "GetSystemDirectoryA",
    "dump_pointer": "0x7ffd9c40b930",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4292068",
    "RVA": "0x00F92068",
    "module": "KERNEL32.DLL",
    "name": "GetSystemInfo",
    "dump_pointer": "0x7ffd9c3e6dc0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292070",
    "RVA": "0x00F92070",
    "module": "KERNEL32.DLL",
    "name": "GetSystemTime",
    "dump_pointer": "0x7ffd9c3e8920",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292078",
    "RVA": "0x00F92078",
    "module": "KERNEL32.DLL",
    "name": "GetSystemTimeAsFileTime",
    "dump_pointer": "0x7ffd9c3e1100",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292080",
    "RVA": "0x00F92080",
    "module": "KERNEL32.DLL",
    "name": "GetSystemTimePreciseAsFileTime",
    "dump_pointer": "0x7ffd9c3f0980",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292088",
    "RVA": "0x00F92088",
    "module": "KERNEL32.DLL",
    "name": "GetTempPathW",
    "dump_pointer": "0x7ffd9c3f0760",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292090",
    "RVA": "0x00F92090",
    "module": "KERNEL32.DLL",
    "name": "GetThreadContext",
    "dump_pointer": "0x7ffd9c3e9860",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292098",
    "RVA": "0x00F92098",
    "module": "KERNEL32.DLL",
    "name": "GetTickCount",
    "dump_pointer": "0x7ffd9c3e0d60",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42920A0",
    "RVA": "0x00F920A0",
    "module": "KERNEL32.DLL",
    "name": "GetTickCount64",
    "dump_pointer": "0x7ffd9c3e0ce0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42920B0",
    "RVA": "0x00F920B0",
    "module": "KERNEL32.DLL",
    "name": "GetTimeFormatW",
    "dump_pointer": "0x7ffd9c3e98c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42920B8",
    "RVA": "0x00F920B8",
    "module": "KERNEL32.DLL",
    "name": "GetTimeZoneInformation",
    "dump_pointer": "0x7ffd9c3e9220",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42920C0",
    "RVA": "0x00F920C0",
    "module": "KERNEL32.DLL",
    "name": "GetUserDefaultLCID",
    "dump_pointer": "0x7ffd9c3e93c0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42920D8",
    "RVA": "0x00F920D8",
    "module": "KERNEL32.DLL",
    "name": "GetWindowsDirectoryW",
    "dump_pointer": "0x7ffd9c3e9ca0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42920E0",
    "RVA": "0x00F920E0",
    "module": "KERNEL32.DLL",
    "name": "GlobalAlloc",
    "dump_pointer": "0x7ffd9c3e5cc0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42920E8",
    "RVA": "0x00F920E8",
    "module": "KERNEL32.DLL",
    "name": "GlobalFree",
    "dump_pointer": "0x7ffd9c3e3c80",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42920F0",
    "RVA": "0x00F920F0",
    "module": "KERNEL32.DLL",
    "name": "GlobalWire",
    "dump_pointer": "0x7ffd9c3e9320",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C42920F8",
    "RVA": "0x00F920F8",
    "module": "KERNEL32.DLL",
    "name": "GlobalMemoryStatusEx",
    "dump_pointer": "0x7ffd9c3e8ab0",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292100",
    "RVA": "0x00F92100",
    "module": "KERNEL32.DLL",
    "name": "GlobalSize",
    "dump_pointer": "0x7ffd9c3e9ba0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292108",
    "RVA": "0x00F92108",
    "module": "KERNEL32.DLL",
    "name": "GlobalUnlock",
    "dump_pointer": "0x7ffd9c3e9300",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292110",
    "RVA": "0x00F92110",
    "module": "ntdll.dll",
    "name": "RtlAllocateHeap",
    "dump_pointer": "0x7ffd9df6c610",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292118",
    "RVA": "0x00F92118",
    "module": "KERNEL32.DLL",
    "name": "HeapFree",
    "dump_pointer": "0x7ffd9c3e0d10",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292130",
    "RVA": "0x00F92130",
    "module": "KERNELBASE.dll",
    "name": "InitOnceExecuteOnce",
    "dump_pointer": "0x7ffd9b931130",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292138",
    "RVA": "0x00F92138",
    "module": "ntdll.dll",
    "name": "RtlInitializeCriticalSection",
    "dump_pointer": "0x7ffd9df8c260",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292140",
    "RVA": "0x00F92140",
    "module": "KERNEL32.DLL",
    "name": "InitializeCriticalSectionAndSpinCount",
    "dump_pointer": "0x7ffd9c3f02e0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292148",
    "RVA": "0x00F92148",
    "module": "KERNEL32.DLL",
    "name": "InitializeCriticalSectionEx",
    "dump_pointer": "0x7ffd9c3f02f0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292158",
    "RVA": "0x00F92158",
    "module": "ntdll.dll",
    "name": "RtlInterlockedFlushSList",
    "dump_pointer": "0x7ffd9dfa2c10",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292160",
    "RVA": "0x00F92160",
    "module": "ntdll.dll",
    "name": "RtlInterlockedPushEntrySList",
    "dump_pointer": "0x7ffd9df9e360",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292168",
    "RVA": "0x00F92168",
    "module": "KERNEL32.DLL",
    "name": "IsDebuggerPresent",
    "dump_pointer": "0x7ffd9c3e7f20",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292170",
    "RVA": "0x00F92170",
    "module": "KERNEL32.DLL",
    "name": "IsProcessorFeaturePresent",
    "dump_pointer": "0x7ffd9c3e6f40",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292178",
    "RVA": "0x00F92178",
    "module": "KERNEL32.DLL",
    "name": "IsValidCodePage",
    "dump_pointer": "0x7ffd9c3e89b0",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292180",
    "RVA": "0x00F92180",
    "module": "KERNEL32.DLL",
    "name": "IsValidLocale",
    "dump_pointer": "0x7ffd9c3e91a0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292198",
    "RVA": "0x00F92198",
    "module": "KERNEL32.DLL",
    "name": "LCMapStringEx",
    "dump_pointer": "0x7ffd9c3edac0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C42921A0",
    "RVA": "0x00F921A0",
    "module": "KERNEL32.DLL",
    "name": "LCMapStringW",
    "dump_pointer": "0x7ffd9c3e3290",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42921A8",
    "RVA": "0x00F921A8",
    "module": "ntdll.dll",
    "name": "RtlLeaveCriticalSection",
    "dump_pointer": "0x7ffd9df56ab0",
    "indexed_xrefs": 63
  },
  {
    "VA": "0x212C42921B0",
    "RVA": "0x00F921B0",
    "module": "KERNEL32.DLL",
    "name": "LoadLibraryA",
    "dump_pointer": "0x7ffd9c3e92c0",
    "indexed_xrefs": 7
  },
  {
    "VA": "0x212C42921B8",
    "RVA": "0x00F921B8",
    "module": "KERNEL32.DLL",
    "name": "LoadLibraryExA",
    "dump_pointer": "0x7ffd9c3e8050",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C42921C0",
    "RVA": "0x00F921C0",
    "module": "KERNEL32.DLL",
    "name": "LoadLibraryExW",
    "dump_pointer": "0x7ffd9c3e3d90",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42921D0",
    "RVA": "0x00F921D0",
    "module": "KERNEL32.DLL",
    "name": "LocalFree",
    "dump_pointer": "0x7ffd9c3e32f0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C42921D8",
    "RVA": "0x00F921D8",
    "module": "KERNEL32.DLL",
    "name": "MapViewOfFile",
    "dump_pointer": "0x7ffd9c3e6250",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42921F0",
    "RVA": "0x00F921F0",
    "module": "KERNEL32.DLL",
    "name": "MoveFileExA",
    "dump_pointer": "0x7ffd9c3f7360",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42921F8",
    "RVA": "0x00F921F8",
    "module": "KERNEL32.DLL",
    "name": "MoveFileExW",
    "dump_pointer": "0x7ffd9c3eee50",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292200",
    "RVA": "0x00F92200",
    "module": "KERNEL32.DLL",
    "name": "MultiByteToWideChar",
    "dump_pointer": "0x7ffd9c3e0d40",
    "indexed_xrefs": 38
  },
  {
    "VA": "0x212C4292208",
    "RVA": "0x00F92208",
    "module": "KERNEL32.DLL",
    "name": "PeekNamedPipe",
    "dump_pointer": "0x7ffd9c3f6cf0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292210",
    "RVA": "0x00F92210",
    "module": "KERNEL32.DLL",
    "name": "PostQueuedCompletionStatus",
    "dump_pointer": "0x7ffd9c3e47b0",
    "indexed_xrefs": 27
  },
  {
    "VA": "0x212C4292228",
    "RVA": "0x00F92228",
    "module": "KERNEL32.DLL",
    "name": "QueryPerformanceCounter",
    "dump_pointer": "0x7ffd9c3e0e50",
    "indexed_xrefs": 8
  },
  {
    "VA": "0x212C4292230",
    "RVA": "0x00F92230",
    "module": "KERNEL32.DLL",
    "name": "QueryPerformanceFrequency",
    "dump_pointer": "0x7ffd9c3e4470",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292238",
    "RVA": "0x00F92238",
    "module": "KERNEL32.DLL",
    "name": "RaiseException",
    "dump_pointer": "0x7ffd9c3e8030",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4292240",
    "RVA": "0x00F92240",
    "module": "KERNEL32.DLL",
    "name": "ReadConsoleA",
    "dump_pointer": "0x7ffd9c3f0c90",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292248",
    "RVA": "0x00F92248",
    "module": "KERNEL32.DLL",
    "name": "ReadConsoleW",
    "dump_pointer": "0x7ffd9c3f0cc0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292250",
    "RVA": "0x00F92250",
    "module": "KERNEL32.DLL",
    "name": "ReadFile",
    "dump_pointer": "0x7ffd9c3f0800",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4292258",
    "RVA": "0x00F92258",
    "module": "ntdll.dll",
    "name": "RtlReleaseSRWLockExclusive",
    "dump_pointer": "0x7ffd9df636d0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292260",
    "RVA": "0x00F92260",
    "module": "ntdll.dll",
    "name": "RtlReleaseSRWLockShared",
    "dump_pointer": "0x7ffd9df56780",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292268",
    "RVA": "0x00F92268",
    "module": "ntdll.dll",
    "name": "RtlRemoveVectoredExceptionHandler",
    "dump_pointer": "0x7ffd9dfb55a0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292270",
    "RVA": "0x00F92270",
    "module": "KERNEL32.DLL",
    "name": "RtlCaptureContext",
    "dump_pointer": "0x7ffd9c3eff90",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4292278",
    "RVA": "0x00F92278",
    "module": "KERNEL32.DLL",
    "name": "RtlLookupFunctionEntry",
    "dump_pointer": "0x7ffd9c3e5540",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292280",
    "RVA": "0x00F92280",
    "module": "KERNEL32.DLL",
    "name": "RtlPcToFileHeader",
    "dump_pointer": "0x7ffd9c3e9090",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292288",
    "RVA": "0x00F92288",
    "module": "KERNEL32.DLL",
    "name": "RtlRestoreContext",
    "dump_pointer": "0x7ffd9c3f6e30",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292290",
    "RVA": "0x00F92290",
    "module": "KERNEL32.DLL",
    "name": "RtlUnwind",
    "dump_pointer": "0x7ffd9c3f6e50",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292298",
    "RVA": "0x00F92298",
    "module": "KERNEL32.DLL",
    "name": "RtlUnwindEx",
    "dump_pointer": "0x7ffd9c3e8150",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C42922A0",
    "RVA": "0x00F922A0",
    "module": "KERNEL32.DLL",
    "name": "RtlVirtualUnwind",
    "dump_pointer": "0x7ffd9c3e3ba0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42922A8",
    "RVA": "0x00F922A8",
    "module": "KERNEL32.DLL",
    "name": "SetConsoleCtrlHandler",
    "dump_pointer": "0x7ffd9c3f0ce0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42922B0",
    "RVA": "0x00F922B0",
    "module": "KERNEL32.DLL",
    "name": "SetConsoleMode",
    "dump_pointer": "0x7ffd9c3f0cf0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42922B8",
    "RVA": "0x00F922B8",
    "module": "KERNEL32.DLL",
    "name": "SetEndOfFile",
    "dump_pointer": "0x7ffd9c3f0850",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42922C0",
    "RVA": "0x00F922C0",
    "module": "KERNEL32.DLL",
    "name": "SetEnvironmentVariableW",
    "dump_pointer": "0x7ffd9c3e7dc0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42922C8",
    "RVA": "0x00F922C8",
    "module": "KERNEL32.DLL",
    "name": "SetEvent",
    "dump_pointer": "0x7ffd9c3f0380",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C42922D0",
    "RVA": "0x00F922D0",
    "module": "KERNEL32.DLL",
    "name": "SetFileInformationByHandle",
    "dump_pointer": "0x7ffd9c3f0880",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C42922D8",
    "RVA": "0x00F922D8",
    "module": "KERNEL32.DLL",
    "name": "SetFilePointerEx",
    "dump_pointer": "0x7ffd9c3f08a0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42922E0",
    "RVA": "0x00F922E0",
    "module": "KERNEL32.DLL",
    "name": "SetLastError",
    "dump_pointer": "0x7ffd9c3e10c0",
    "indexed_xrefs": 35
  },
  {
    "VA": "0x212C42922F0",
    "RVA": "0x00F922F0",
    "module": "KERNEL32.DLL",
    "name": "SetStdHandle",
    "dump_pointer": "0x7ffd9c40ca60",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42922F8",
    "RVA": "0x00F922F8",
    "module": "KERNEL32.DLL",
    "name": "SetUnhandledExceptionFilter",
    "dump_pointer": "0x7ffd9c3e8b30",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292300",
    "RVA": "0x00F92300",
    "module": "KERNEL32.DLL",
    "name": "SetWaitableTimer",
    "dump_pointer": "0x7ffd9c3f0390",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292308",
    "RVA": "0x00F92308",
    "module": "KERNEL32.DLL",
    "name": "Sleep",
    "dump_pointer": "0x7ffd9c3e8650",
    "indexed_xrefs": 12
  },
  {
    "VA": "0x212C4292310",
    "RVA": "0x00F92310",
    "module": "KERNELBASE.dll",
    "name": "SleepConditionVariableSRW",
    "dump_pointer": "0x7ffd9b942e20",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292318",
    "RVA": "0x00F92318",
    "module": "KERNEL32.DLL",
    "name": "SleepEx",
    "dump_pointer": "0x7ffd9c3f03a0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292320",
    "RVA": "0x00F92320",
    "module": "ntdll.dll",
    "name": "TpPostWork",
    "dump_pointer": "0x7ffd9df646b0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292328",
    "RVA": "0x00F92328",
    "module": "KERNEL32.DLL",
    "name": "SwitchToFiber",
    "dump_pointer": "0x7ffd9c3f0b90",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292330",
    "RVA": "0x00F92330",
    "module": "KERNEL32.DLL",
    "name": "SystemTimeToFileTime",
    "dump_pointer": "0x7ffd9c3e8630",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292338",
    "RVA": "0x00F92338",
    "module": "KERNEL32.DLL",
    "name": "SystemTimeToTzSpecificLocalTime",
    "dump_pointer": "0x7ffd9c3d2430",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292340",
    "RVA": "0x00F92340",
    "module": "KERNEL32.DLL",
    "name": "TerminateProcess",
    "dump_pointer": "0x7ffd9c3e97c0",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4292348",
    "RVA": "0x00F92348",
    "module": "KERNEL32.DLL",
    "name": "TlsAlloc",
    "dump_pointer": "0x7ffd9c3e6940",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292350",
    "RVA": "0x00F92350",
    "module": "KERNEL32.DLL",
    "name": "TlsFree",
    "dump_pointer": "0x7ffd9c3e7c30",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292358",
    "RVA": "0x00F92358",
    "module": "KERNEL32.DLL",
    "name": "TlsGetValue",
    "dump_pointer": "0x7ffd9c3e0d30",
    "indexed_xrefs": 214
  },
  {
    "VA": "0x212C4292360",
    "RVA": "0x00F92360",
    "module": "KERNEL32.DLL",
    "name": "TlsSetValue",
    "dump_pointer": "0x7ffd9c3e0e30",
    "indexed_xrefs": 10
  },
  {
    "VA": "0x212C4292368",
    "RVA": "0x00F92368",
    "module": "ntdll.dll",
    "name": "RtlTryAcquireSRWLockExclusive",
    "dump_pointer": "0x7ffd9dfa5c80",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292370",
    "RVA": "0x00F92370",
    "module": "KERNEL32.DLL",
    "name": "UnhandledExceptionFilter",
    "dump_pointer": "0x7ffd9c40cb50",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292378",
    "RVA": "0x00F92378",
    "module": "KERNEL32.DLL",
    "name": "UnmapViewOfFile",
    "dump_pointer": "0x7ffd9c3e7c10",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292380",
    "RVA": "0x00F92380",
    "module": "ntdll.dll",
    "name": "VerSetConditionMask",
    "dump_pointer": "0x7ffd9dfab1d0",
    "indexed_xrefs": 8
  },
  {
    "VA": "0x212C4292388",
    "RVA": "0x00F92388",
    "module": "KERNEL32.DLL",
    "name": "VerifyVersionInfoW",
    "dump_pointer": "0x7ffd9c3e47f0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292390",
    "RVA": "0x00F92390",
    "module": "KERNEL32.DLL",
    "name": "VirtualAlloc",
    "dump_pointer": "0x7ffd9c3e3bf0",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4292398",
    "RVA": "0x00F92398",
    "module": "KERNEL32.DLL",
    "name": "VirtualFree",
    "dump_pointer": "0x7ffd9c3e47d0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C42923A0",
    "RVA": "0x00F923A0",
    "module": "KERNEL32.DLL",
    "name": "VirtualProtect",
    "dump_pointer": "0x7ffd9c3e5470",
    "indexed_xrefs": 13
  },
  {
    "VA": "0x212C42923A8",
    "RVA": "0x00F923A8",
    "module": "KERNEL32.DLL",
    "name": "VirtualQuery",
    "dump_pointer": "0x7ffd9c3e5490",
    "indexed_xrefs": 30
  },
  {
    "VA": "0x212C42923B0",
    "RVA": "0x00F923B0",
    "module": "KERNEL32.DLL",
    "name": "WaitForMultipleObjects",
    "dump_pointer": "0x7ffd9c3f03b0",
    "indexed_xrefs": 7
  },
  {
    "VA": "0x212C42923B8",
    "RVA": "0x00F923B8",
    "module": "KERNEL32.DLL",
    "name": "WaitForSingleObject",
    "dump_pointer": "0x7ffd9c3f03d0",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C42923C8",
    "RVA": "0x00F923C8",
    "module": "ntdll.dll",
    "name": "RtlWakeAllConditionVariable",
    "dump_pointer": "0x7ffd9df8b180",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C42923D0",
    "RVA": "0x00F923D0",
    "module": "ntdll.dll",
    "name": "RtlWakeConditionVariable",
    "dump_pointer": "0x7ffd9df99530",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42923D8",
    "RVA": "0x00F923D8",
    "module": "KERNEL32.DLL",
    "name": "WideCharToMultiByte",
    "dump_pointer": "0x7ffd9c3e0e10",
    "indexed_xrefs": 22
  },
  {
    "VA": "0x212C42923E0",
    "RVA": "0x00F923E0",
    "module": "KERNEL32.DLL",
    "name": "WriteConsoleW",
    "dump_pointer": "0x7ffd9c3f0d10",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42923E8",
    "RVA": "0x00F923E8",
    "module": "KERNEL32.DLL",
    "name": "WriteFile",
    "dump_pointer": "0x7ffd9c3f08f0",
    "indexed_xrefs": 7
  },
  {
    "VA": "0x212C42923F8",
    "RVA": "0x00F923F8",
    "module": "USER32.dll",
    "name": "AdjustWindowRectEx",
    "dump_pointer": "0x7ffd9c4bc5c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292400",
    "RVA": "0x00F92400",
    "module": "USER32.dll",
    "name": "ClientToScreen",
    "dump_pointer": "0x7ffd9c4bfff0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292408",
    "RVA": "0x00F92408",
    "module": "USER32.dll",
    "name": "CloseClipboard",
    "dump_pointer": "0x7ffd9c4a3f40",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4292410",
    "RVA": "0x00F92410",
    "module": "USER32.dll",
    "name": "EmptyClipboard",
    "dump_pointer": "0x7ffd9c526c50",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292428",
    "RVA": "0x00F92428",
    "module": "USER32.dll",
    "name": "GetClassNameW",
    "dump_pointer": "0x7ffd9c4c69f0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292430",
    "RVA": "0x00F92430",
    "module": "USER32.dll",
    "name": "GetClientRect",
    "dump_pointer": "0x7ffd9c4a64c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292438",
    "RVA": "0x00F92438",
    "module": "USER32.dll",
    "name": "GetClipboardData",
    "dump_pointer": "0x7ffd9c52aab0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292440",
    "RVA": "0x00F92440",
    "module": "USER32.dll",
    "name": "GetCursor",
    "dump_pointer": "0x7ffd9c4cdb70",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292448",
    "RVA": "0x00F92448",
    "module": "USER32.dll",
    "name": "GetPhysicalCursorPos",
    "dump_pointer": "0x7ffd9c4ca440",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292450",
    "RVA": "0x00F92450",
    "module": "USER32.dll",
    "name": "GetDC",
    "dump_pointer": "0x7ffd9c4c70f0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292458",
    "RVA": "0x00F92458",
    "module": "USER32.dll",
    "name": "GetDesktopWindow",
    "dump_pointer": "0x7ffd9c4a7530",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292468",
    "RVA": "0x00F92468",
    "module": "USER32.dll",
    "name": "GetKeyNameTextA",
    "dump_pointer": "0x7ffd9c525700",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292478",
    "RVA": "0x00F92478",
    "module": "USER32.dll",
    "name": "GetMenu",
    "dump_pointer": "0x7ffd9c4a3a20",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292480",
    "RVA": "0x00F92480",
    "module": "USER32.dll",
    "name": "GetProcessWindowStation",
    "dump_pointer": "0x7ffd9c4cde20",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292488",
    "RVA": "0x00F92488",
    "module": "USER32.dll",
    "name": "GetUserObjectInformationW",
    "dump_pointer": "0x7ffd9c4cdf10",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292498",
    "RVA": "0x00F92498",
    "module": "USER32.dll",
    "name": "GetWindowPlacement",
    "dump_pointer": "0x7ffd9c4cdf90",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42924A0",
    "RVA": "0x00F924A0",
    "module": "USER32.dll",
    "name": "GetWindowTextW",
    "dump_pointer": "0x7ffd9c4bbb10",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42924A8",
    "RVA": "0x00F924A8",
    "module": "USER32.dll",
    "name": "LoadCursorW",
    "dump_pointer": "0x7ffd9c4b52d0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42924B0",
    "RVA": "0x00F924B0",
    "module": "USER32.dll",
    "name": "MapVirtualKeyA",
    "dump_pointer": "0x7ffd9c525830",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42924C0",
    "RVA": "0x00F924C0",
    "module": "USER32.dll",
    "name": "MessageBoxA",
    "dump_pointer": "0x7ffd9c519770",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C42924C8",
    "RVA": "0x00F924C8",
    "module": "USER32.dll",
    "name": "MonitorFromWindow",
    "dump_pointer": "0x7ffd9c4c1110",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42924D0",
    "RVA": "0x00F924D0",
    "module": "USER32.dll",
    "name": "OpenClipboard",
    "dump_pointer": "0x7ffd9c4a3900",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C42924E0",
    "RVA": "0x00F924E0",
    "module": "USER32.dll",
    "name": "ReleaseDC",
    "dump_pointer": "0x7ffd9c4b3120",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42924E8",
    "RVA": "0x00F924E8",
    "module": "USER32.dll",
    "name": "ScreenToClient",
    "dump_pointer": "0x7ffd9c4b9eb0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42924F8",
    "RVA": "0x00F924F8",
    "module": "USER32.dll",
    "name": "SetClipboardData",
    "dump_pointer": "0x7ffd9c52b9a0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292500",
    "RVA": "0x00F92500",
    "module": "USER32.dll",
    "name": "SetCursor",
    "dump_pointer": "0x7ffd9c4c99a0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292508",
    "RVA": "0x00F92508",
    "module": "USER32.dll",
    "name": "SetPhysicalCursorPos",
    "dump_pointer": "0x7ffd9c4ce730",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292510",
    "RVA": "0x00F92510",
    "module": "USER32.dll",
    "name": "SystemParametersInfoW",
    "dump_pointer": "0x7ffd9c4c2800",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292540",
    "RVA": "0x00F92540",
    "module": "tier0.dll",
    "name": "??0CUtlBuffer@@QEAA@HHW4BufferFlags_t@0@@Z",
    "dump_pointer": "0x7ffd339d5050",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292548",
    "RVA": "0x00F92548",
    "module": "tier0.dll",
    "name": "?AcquireLock@CAtomicMutex@@AEAAXI@Z",
    "dump_pointer": "0x7ffd339b2960",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292550",
    "RVA": "0x00F92550",
    "module": "tier0.dll",
    "name": "?FindKey@KeyValues@@QEAAPEAV1@PEBD_N@Z",
    "dump_pointer": "0x7ffd338f1d70",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C4292558",
    "RVA": "0x00F92558",
    "module": "tier0.dll",
    "name": "?FreeMemoryBlock@CUtlString@@AEAAXXZ",
    "dump_pointer": "0x7ffd339dc720",
    "indexed_xrefs": 66
  },
  {
    "VA": "0x212C4292560",
    "RVA": "0x00F92560",
    "module": "tier0.dll",
    "name": "?Insert@CBufferString@@QEAAPEBDHPEBDH_N@Z",
    "dump_pointer": "0x7ffd33891cb0",
    "indexed_xrefs": 13
  },
  {
    "VA": "0x212C4292570",
    "RVA": "0x00F92570",
    "module": "tier0.dll",
    "name": "?IsEqual_FastCaseInsensitive@CUtlString@@QEBA_NPEBD@Z",
    "dump_pointer": "0x7ffd339dcee0",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292578",
    "RVA": "0x00F92578",
    "module": "tier0.dll",
    "name": "?LoadKV3@@YA_NPEAVKeyValues3@@PEAVCUtlString@@PEAVCUtlBuffer@@AEBUKV3ID_t@@PEBDI@Z",
    "dump_pointer": "0x7ffd339658d0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292588",
    "RVA": "0x00F92588",
    "module": "tier0.dll",
    "name": "?MakeCopy@KeyValues@@QEBAPEAV1@XZ",
    "dump_pointer": "0x7ffd338f53a0",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C4292590",
    "RVA": "0x00F92590",
    "module": "tier0.dll",
    "name": "?MoveFrom@CBufferString@@QEAAXAEAV1@@Z",
    "dump_pointer": "0x7ffd33893d90",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292598",
    "RVA": "0x00F92598",
    "module": "tier0.dll",
    "name": "?Purge@CBufferString@@QEAAXH@Z",
    "dump_pointer": "0x7ffd33893c80",
    "indexed_xrefs": 70
  },
  {
    "VA": "0x212C42925A0",
    "RVA": "0x00F925A0",
    "module": "tier0.dll",
    "name": "?PutString@CUtlBuffer@@QEAAXPEBD@Z",
    "dump_pointer": "0x7ffd339d84a0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42925C0",
    "RVA": "0x00F925C0",
    "module": "tier0.dll",
    "name": "?RemoveAt@CBufferString@@QEAAPEBDHH@Z",
    "dump_pointer": "0x7ffd33892e40",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42925C8",
    "RVA": "0x00F925C8",
    "module": "tier0.dll",
    "name": "?Set@CUtlString@@QEAAXPEBD@Z",
    "dump_pointer": "0x7ffd339dc8a0",
    "indexed_xrefs": 18
  },
  {
    "VA": "0x212C42925D0",
    "RVA": "0x00F925D0",
    "module": "tier0.dll",
    "name": "?SetDirect@CUtlString@@QEAAXPEBDH@Z",
    "dump_pointer": "0x7ffd339dc7d0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42925D8",
    "RVA": "0x00F925D8",
    "module": "tier0.dll",
    "name": "?SetInt@KeyValues@@QEAAXPEBDH@Z",
    "dump_pointer": "0x7ffd338f46c0",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C42925E0",
    "RVA": "0x00F925E0",
    "module": "tier0.dll",
    "name": "?SetLength@CBufferString@@QEAAPEADH_NPEAH@Z",
    "dump_pointer": "0x7ffd338916c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42925F8",
    "RVA": "0x00F925F8",
    "module": "tier0.dll",
    "name": "?SetString@KeyValues@@QEAAXPEBD0@Z",
    "dump_pointer": "0x7ffd338f4410",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C4292600",
    "RVA": "0x00F92600",
    "module": "tier0.dll",
    "name": "?ToLowerFast@CBufferString@@QEAAXH@Z",
    "dump_pointer": "0x7ffd338943e0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292608",
    "RVA": "0x00F92608",
    "module": "tier0.dll",
    "name": "?UnregisterEventListener_Base@CEventDispatcher_Base@@IEAA_NAEBVCUtlAbstractDelegate@@AEAV?$CUtlVector@UEventListenerInfo_t@CEventDispatcher_Base@@HV?$CUtlVectorMemory_Growable@UEventListenerInfo_t@CEventDispatcher_Base@@H$0A@@@@@@Z",
    "dump_pointer": "0x7ffd338c6530",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292620",
    "RVA": "0x00F92620",
    "module": "tier0.dll",
    "name": "GetQuantizedFloatEncoderByReg",
    "dump_pointer": "0x7ffd3399caf0",
    "indexed_xrefs": 7
  },
  {
    "VA": "0x212C4292628",
    "RVA": "0x00F92628",
    "module": "tier0.dll",
    "name": "GetQuantizedFloatEncoderNameByReg",
    "dump_pointer": "0x7ffd3399cb50",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292638",
    "RVA": "0x00F92638",
    "module": "tier0.dll",
    "name": "LoggingSystem_GetChannelCount",
    "dump_pointer": "0x7ffd3396ada0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292648",
    "RVA": "0x00F92648",
    "module": "tier0.dll",
    "name": "LoggingSystem_SetChannelFlags",
    "dump_pointer": "0x7ffd3396b0d0",
    "indexed_xrefs": 8
  },
  {
    "VA": "0x212C4292650",
    "RVA": "0x00F92650",
    "module": "tier0.dll",
    "name": "Plat_FloatTime",
    "dump_pointer": "0x7ffd33983350",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C4292658",
    "RVA": "0x00F92658",
    "module": "tier0.dll",
    "name": "RandomFloat",
    "dump_pointer": "0x7ffd3399d4b0",
    "indexed_xrefs": 11
  },
  {
    "VA": "0x212C4292660",
    "RVA": "0x00F92660",
    "module": "tier0.dll",
    "name": "RandomInt",
    "dump_pointer": "0x7ffd3399d6c0",
    "indexed_xrefs": 11
  },
  {
    "VA": "0x212C4292668",
    "RVA": "0x00F92668",
    "module": "tier0.dll",
    "name": "RandomSeed",
    "dump_pointer": "0x7ffd3399d3f0",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C4292670",
    "RVA": "0x00F92670",
    "module": "tier0.dll",
    "name": "RegisterStringToken",
    "dump_pointer": "0x7ffd339e0cb0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292678",
    "RVA": "0x00F92678",
    "module": "tier0.dll",
    "name": "ThreadInMainThread",
    "dump_pointer": "0x7ffd339b1430",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292680",
    "RVA": "0x00F92680",
    "module": "tier0.dll",
    "name": "UtlVectorMemory_Alloc",
    "dump_pointer": "0x7ffd339e3ca0",
    "indexed_xrefs": 20
  },
  {
    "VA": "0x212C4292688",
    "RVA": "0x00F92688",
    "module": "tier0.dll",
    "name": "UtlVectorMemory_CalcNewAllocationCount",
    "dump_pointer": "0x7ffd339e3c20",
    "indexed_xrefs": 17
  },
  {
    "VA": "0x212C4292690",
    "RVA": "0x00F92690",
    "module": "tier0.dll",
    "name": "V_CompareNameWithWildcards",
    "dump_pointer": "0x7ffd3384d1e0",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C4292698",
    "RVA": "0x00F92698",
    "module": "tier0.dll",
    "name": "V_GetFileExtension",
    "dump_pointer": "0x7ffd33852b50",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42926A0",
    "RVA": "0x00F926A0",
    "module": "tier0.dll",
    "name": "V_IsAbsolutePath",
    "dump_pointer": "0x7ffd33853c00",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42926A8",
    "RVA": "0x00F926A8",
    "module": "tier0.dll",
    "name": "V_RemoveDotSlashes",
    "dump_pointer": "0x7ffd33852bc0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42926B0",
    "RVA": "0x00F926B0",
    "module": "tier0.dll",
    "name": "V_StringToFloat32",
    "dump_pointer": "0x7ffd33858fd0",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C42926B8",
    "RVA": "0x00F926B8",
    "module": "tier0.dll",
    "name": "V_StringToInt32",
    "dump_pointer": "0x7ffd339a5c80",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C42926C0",
    "RVA": "0x00F926C0",
    "module": "tier0.dll",
    "name": "V_StringToUint32",
    "dump_pointer": "0x7ffd339a5e20",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42926D0",
    "RVA": "0x00F926D0",
    "module": "tier0.dll",
    "name": "V_expf",
    "dump_pointer": "0x7ffd33847f30",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C42926D8",
    "RVA": "0x00F926D8",
    "module": "tier0.dll",
    "name": "V_fmodf",
    "dump_pointer": "0x7ffd33847fb0",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C42926E0",
    "RVA": "0x00F926E0",
    "module": "tier0.dll",
    "name": "V_logf",
    "dump_pointer": "0x7ffd338480b0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42926E8",
    "RVA": "0x00F926E8",
    "module": "tier0.dll",
    "name": "V_modff",
    "dump_pointer": "0x7ffd33848120",
    "indexed_xrefs": 107
  },
  {
    "VA": "0x212C42926F8",
    "RVA": "0x00F926F8",
    "module": "tier0.dll",
    "name": "V_stricmp_fast",
    "dump_pointer": "0x7ffd33848fb0",
    "indexed_xrefs": 8
  },
  {
    "VA": "0x212C42928A0",
    "RVA": "0x00F928A0",
    "module": "SHELL32.dll",
    "name": "ShellExecuteW",
    "dump_pointer": "0x7ffd9c702480",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C42928F0",
    "RVA": "0x00F928F0",
    "module": "advapi32.dll",
    "name": "DeregisterEventSource",
    "dump_pointer": "0x7ffd9c288870",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42928F8",
    "RVA": "0x00F928F8",
    "module": "advapi32.dll",
    "name": "RegCloseKey",
    "dump_pointer": "0x7ffd9c289920",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292900",
    "RVA": "0x00F92900",
    "module": "advapi32.dll",
    "name": "RegEnumValueW",
    "dump_pointer": "0x7ffd9c28ac40",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292908",
    "RVA": "0x00F92908",
    "module": "advapi32.dll",
    "name": "RegOpenKeyExW",
    "dump_pointer": "0x7ffd9c2896b0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292910",
    "RVA": "0x00F92910",
    "module": "advapi32.dll",
    "name": "RegQueryInfoKeyW",
    "dump_pointer": "0x7ffd9c2897f0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292918",
    "RVA": "0x00F92918",
    "module": "advapi32.dll",
    "name": "RegisterEventSourceA",
    "dump_pointer": "0x7ffd9c2d4270",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292920",
    "RVA": "0x00F92920",
    "module": "advapi32.dll",
    "name": "ReportEventA",
    "dump_pointer": "0x7ffd9c2d42f0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42929B8",
    "RVA": "0x00F929B8",
    "module": "WS2_32.dll",
    "name": "WSAAddressToStringW",
    "dump_pointer": "0x7ffd9d0d9d20",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42929C0",
    "RVA": "0x00F929C0",
    "module": "WS2_32.dll",
    "name": "WSACleanup",
    "dump_pointer": "0x7ffd9d0e1390",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C42929C8",
    "RVA": "0x00F929C8",
    "module": "WS2_32.dll",
    "name": "WSACloseEvent",
    "dump_pointer": "0x7ffd9d0e41b0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42929D0",
    "RVA": "0x00F929D0",
    "module": "WS2_32.dll",
    "name": "WSACreateEvent",
    "dump_pointer": "0x7ffd9d0e42c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C42929D8",
    "RVA": "0x00F929D8",
    "module": "WS2_32.dll",
    "name": "WSAEnumNetworkEvents",
    "dump_pointer": "0x7ffd9d0db640",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C42929E0",
    "RVA": "0x00F929E0",
    "module": "WS2_32.dll",
    "name": "WSAEventSelect",
    "dump_pointer": "0x7ffd9d0e3250",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C42929E8",
    "RVA": "0x00F929E8",
    "module": "WS2_32.dll",
    "name": "WSAGetLastError",
    "dump_pointer": "0x7ffd9d0e3ae0",
    "indexed_xrefs": 78
  },
  {
    "VA": "0x212C42929F0",
    "RVA": "0x00F929F0",
    "module": "WS2_32.dll",
    "name": "WSAIoctl",
    "dump_pointer": "0x7ffd9d0dbcd0",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C42929F8",
    "RVA": "0x00F929F8",
    "module": "WS2_32.dll",
    "name": "WSARecv",
    "dump_pointer": "0x7ffd9d0e15c0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292A00",
    "RVA": "0x00F92A00",
    "module": "WS2_32.dll",
    "name": "WSAResetEvent",
    "dump_pointer": "0x7ffd9d0e3ac0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292A08",
    "RVA": "0x00F92A08",
    "module": "WS2_32.dll",
    "name": "WSASend",
    "dump_pointer": "0x7ffd9d0d2690",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292A10",
    "RVA": "0x00F92A10",
    "module": "WS2_32.dll",
    "name": "WSASetLastError",
    "dump_pointer": "0x7ffd9d0d2670",
    "indexed_xrefs": 15
  },
  {
    "VA": "0x212C4292A18",
    "RVA": "0x00F92A18",
    "module": "WS2_32.dll",
    "name": "WSASocketW",
    "dump_pointer": "0x7ffd9d0da170",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292A20",
    "RVA": "0x00F92A20",
    "module": "WS2_32.dll",
    "name": "WSAStartup",
    "dump_pointer": "0x7ffd9d0dee40",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292A28",
    "RVA": "0x00F92A28",
    "module": "WS2_32.dll",
    "name": "WSAWaitForMultipleEvents",
    "dump_pointer": "0x7ffd9d0e4b80",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292A38",
    "RVA": "0x00F92A38",
    "module": "WS2_32.dll",
    "name": "accept",
    "dump_pointer": "0x7ffd9d0e2710",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292A40",
    "RVA": "0x00F92A40",
    "module": "WS2_32.dll",
    "name": "bind",
    "dump_pointer": "0x7ffd9d0e3540",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292A48",
    "RVA": "0x00F92A48",
    "module": "WS2_32.dll",
    "name": "closesocket",
    "dump_pointer": "0x7ffd9d0db740",
    "indexed_xrefs": 19
  },
  {
    "VA": "0x212C4292A50",
    "RVA": "0x00F92A50",
    "module": "WS2_32.dll",
    "name": "connect",
    "dump_pointer": "0x7ffd9d0e3330",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292A58",
    "RVA": "0x00F92A58",
    "module": "WS2_32.dll",
    "name": "freeaddrinfo",
    "dump_pointer": "0x7ffd9d0d2d70",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292A60",
    "RVA": "0x00F92A60",
    "module": "WS2_32.dll",
    "name": "getaddrinfo",
    "dump_pointer": "0x7ffd9d0d3ce0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292A68",
    "RVA": "0x00F92A68",
    "module": "WS2_32.dll",
    "name": "gethostname",
    "dump_pointer": "0x7ffd9d0f96c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292A70",
    "RVA": "0x00F92A70",
    "module": "WS2_32.dll",
    "name": "getnameinfo",
    "dump_pointer": "0x7ffd9d0e58d0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292A78",
    "RVA": "0x00F92A78",
    "module": "WS2_32.dll",
    "name": "getpeername",
    "dump_pointer": "0x7ffd9d0e2e30",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292A80",
    "RVA": "0x00F92A80",
    "module": "WS2_32.dll",
    "name": "getsockname",
    "dump_pointer": "0x7ffd9d0e3170",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4292A88",
    "RVA": "0x00F92A88",
    "module": "WS2_32.dll",
    "name": "getsockopt",
    "dump_pointer": "0x7ffd9d0e2b30",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4292A90",
    "RVA": "0x00F92A90",
    "module": "WS2_32.dll",
    "name": "ntohl",
    "dump_pointer": "0x7ffd9d0e3770",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4292A98",
    "RVA": "0x00F92A98",
    "module": "WS2_32.dll",
    "name": "ntohs",
    "dump_pointer": "0x7ffd9d0e3aa0",
    "indexed_xrefs": 9
  },
  {
    "VA": "0x212C4292AA0",
    "RVA": "0x00F92AA0",
    "module": "WS2_32.dll",
    "name": "inet_pton",
    "dump_pointer": "0x7ffd9d0e7ac0",
    "indexed_xrefs": 19
  },
  {
    "VA": "0x212C4292AA8",
    "RVA": "0x00F92AA8",
    "module": "WS2_32.dll",
    "name": "ioctlsocket",
    "dump_pointer": "0x7ffd9d0dc080",
    "indexed_xrefs": 13
  },
  {
    "VA": "0x212C4292AB0",
    "RVA": "0x00F92AB0",
    "module": "WS2_32.dll",
    "name": "listen",
    "dump_pointer": "0x7ffd9d0e25f0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292AB8",
    "RVA": "0x00F92AB8",
    "module": "WS2_32.dll",
    "name": "ntohl",
    "dump_pointer": "0x7ffd9d0e3770",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292AC0",
    "RVA": "0x00F92AC0",
    "module": "WS2_32.dll",
    "name": "ntohs",
    "dump_pointer": "0x7ffd9d0e3aa0",
    "indexed_xrefs": 11
  },
  {
    "VA": "0x212C4292AC8",
    "RVA": "0x00F92AC8",
    "module": "WS2_32.dll",
    "name": "recv",
    "dump_pointer": "0x7ffd9d0e2280",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4292AD0",
    "RVA": "0x00F92AD0",
    "module": "WS2_32.dll",
    "name": "select",
    "dump_pointer": "0x7ffd9d0e2980",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292AD8",
    "RVA": "0x00F92AD8",
    "module": "WS2_32.dll",
    "name": "send",
    "dump_pointer": "0x7ffd9d0d28c0",
    "indexed_xrefs": 7
  },
  {
    "VA": "0x212C4292AE0",
    "RVA": "0x00F92AE0",
    "module": "WS2_32.dll",
    "name": "setsockopt",
    "dump_pointer": "0x7ffd9d0e2000",
    "indexed_xrefs": 12
  },
  {
    "VA": "0x212C4292AE8",
    "RVA": "0x00F92AE8",
    "module": "WS2_32.dll",
    "name": "shutdown",
    "dump_pointer": "0x7ffd9d0e2f20",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292AF0",
    "RVA": "0x00F92AF0",
    "module": "WS2_32.dll",
    "name": "socket",
    "dump_pointer": "0x7ffd9d0da060",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4292B10",
    "RVA": "0x00F92B10",
    "module": "OLEAUT32.dll",
    "name": "SysAllocString",
    "dump_pointer": "0x7ffd9bddd6c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292B18",
    "RVA": "0x00F92B18",
    "module": "OLEAUT32.dll",
    "name": "SysFreeString",
    "dump_pointer": "0x7ffd9bddd860",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292B38",
    "RVA": "0x00F92B38",
    "module": "combase.dll",
    "name": "CoCreateInstance",
    "dump_pointer": "0x7ffd9db22050",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292B60",
    "RVA": "0x00F92B60",
    "module": "combase.dll",
    "name": "PropVariantClear",
    "dump_pointer": "0x7ffd9dba2ed0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292BA0",
    "RVA": "0x00F92BA0",
    "module": "CRYPT32.dll",
    "name": "CertCloseStore",
    "dump_pointer": "0x7ffd9b65dc10",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292BA8",
    "RVA": "0x00F92BA8",
    "module": "CRYPT32.dll",
    "name": "CertEnumCertificatesInStore",
    "dump_pointer": "0x7ffd9b65b820",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292BB0",
    "RVA": "0x00F92BB0",
    "module": "CRYPT32.dll",
    "name": "CertFreeCertificateContext",
    "dump_pointer": "0x7ffd9b65b5b0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292BB8",
    "RVA": "0x00F92BB8",
    "module": "CRYPT32.dll",
    "name": "CertGetEnhancedKeyUsage",
    "dump_pointer": "0x7ffd9b655df0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4292BC0",
    "RVA": "0x00F92BC0",
    "module": "CRYPT32.dll",
    "name": "CertGetIntendedKeyUsage",
    "dump_pointer": "0x7ffd9b674400",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4292BC8",
    "RVA": "0x00F92BC8",
    "module": "CRYPT32.dll",
    "name": "CertOpenSystemStoreA",
    "dump_pointer": "0x7ffd9b6c5b90",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27000",
    "RVA": "0x01827000",
    "module": "KERNEL32.DLL",
    "name": "GetSystemTimeAsFileTime",
    "dump_pointer": "0x7ffd9c3e1100",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27018",
    "RVA": "0x01827018",
    "module": "KERNEL32.DLL",
    "name": "TerminateProcess",
    "dump_pointer": "0x7ffd9c3e97c0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B27048",
    "RVA": "0x01827048",
    "module": "KERNEL32.DLL",
    "name": "CloseHandle",
    "dump_pointer": "0x7ffd9c3f01e0",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4B27060",
    "RVA": "0x01827060",
    "module": "KERNEL32.DLL",
    "name": "GetCurrentThreadId",
    "dump_pointer": "0x7ffd9c3d2750",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4B27068",
    "RVA": "0x01827068",
    "module": "KERNEL32.DLL",
    "name": "GetCurrentProcessId",
    "dump_pointer": "0x7ffd9c3f0170",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27090",
    "RVA": "0x01827090",
    "module": "KERNEL32.DLL",
    "name": "Sleep",
    "dump_pointer": "0x7ffd9c3e8650",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4B270B0",
    "RVA": "0x018270B0",
    "module": "KERNEL32.DLL",
    "name": "GetTickCount",
    "dump_pointer": "0x7ffd9c3e0d60",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B270D8",
    "RVA": "0x018270D8",
    "module": "KERNEL32.DLL",
    "name": "LoadLibraryA",
    "dump_pointer": "0x7ffd9c3e92c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B270E0",
    "RVA": "0x018270E0",
    "module": "ntdll.dll",
    "name": "RtlAllocateHeap",
    "dump_pointer": "0x7ffd9df6c610",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B270E8",
    "RVA": "0x018270E8",
    "module": "KERNEL32.DLL",
    "name": "HeapFree",
    "dump_pointer": "0x7ffd9c3e0d10",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B270F0",
    "RVA": "0x018270F0",
    "module": "KERNEL32.DLL",
    "name": "GetProcAddress",
    "dump_pointer": "0x7ffd9c3e3c10",
    "indexed_xrefs": 6
  },
  {
    "VA": "0x212C4B270F8",
    "RVA": "0x018270F8",
    "module": "KERNEL32.DLL",
    "name": "ExitProcess",
    "dump_pointer": "0x7ffd9c3e7fa0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B27100",
    "RVA": "0x01827100",
    "module": "KERNEL32.DLL",
    "name": "MultiByteToWideChar",
    "dump_pointer": "0x7ffd9c3e0d40",
    "indexed_xrefs": 12
  },
  {
    "VA": "0x212C4B27108",
    "RVA": "0x01827108",
    "module": "KERNEL32.DLL",
    "name": "WideCharToMultiByte",
    "dump_pointer": "0x7ffd9c3e0e10",
    "indexed_xrefs": 10
  },
  {
    "VA": "0x212C4B27170",
    "RVA": "0x01827170",
    "module": "KERNEL32.DLL",
    "name": "GetLastError",
    "dump_pointer": "0x7ffd9c3e0df0",
    "indexed_xrefs": 16
  },
  {
    "VA": "0x212C4B27188",
    "RVA": "0x01827188",
    "module": "KERNEL32.DLL",
    "name": "FlushFileBuffers",
    "dump_pointer": "0x7ffd9c3f05c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27198",
    "RVA": "0x01827198",
    "module": "KERNEL32.DLL",
    "name": "RtlUnwindEx",
    "dump_pointer": "0x7ffd9c3e8150",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B271A0",
    "RVA": "0x018271A0",
    "module": "KERNEL32.DLL",
    "name": "FlsSetValue",
    "dump_pointer": "0x7ffd9c3e50f0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4B271A8",
    "RVA": "0x018271A8",
    "module": "KERNEL32.DLL",
    "name": "GetCommandLineA",
    "dump_pointer": "0x7ffd9c3e8ad0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B271B0",
    "RVA": "0x018271B0",
    "module": "KERNEL32.DLL",
    "name": "GetCPInfo",
    "dump_pointer": "0x7ffd9c3e7e00",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4B271B8",
    "RVA": "0x018271B8",
    "module": "KERNEL32.DLL",
    "name": "GetACP",
    "dump_pointer": "0x7ffd9c3e8130",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B271C0",
    "RVA": "0x018271C0",
    "module": "KERNEL32.DLL",
    "name": "GetOEMCP",
    "dump_pointer": "0x7ffd9c3e9700",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B271C8",
    "RVA": "0x018271C8",
    "module": "KERNEL32.DLL",
    "name": "IsValidCodePage",
    "dump_pointer": "0x7ffd9c3e89b0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B271E0",
    "RVA": "0x018271E0",
    "module": "KERNEL32.DLL",
    "name": "FlsGetValue",
    "dump_pointer": "0x7ffd9c3e3310",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B271E8",
    "RVA": "0x018271E8",
    "module": "KERNEL32.DLL",
    "name": "FlsFree",
    "dump_pointer": "0x7ffd9c3e9100",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B271F0",
    "RVA": "0x018271F0",
    "module": "KERNEL32.DLL",
    "name": "SetLastError",
    "dump_pointer": "0x7ffd9c3e10c0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B271F8",
    "RVA": "0x018271F8",
    "module": "KERNEL32.DLL",
    "name": "FlsAlloc",
    "dump_pointer": "0x7ffd9c3e8b10",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27200",
    "RVA": "0x01827200",
    "module": "KERNEL32.DLL",
    "name": "GetCurrentProcess",
    "dump_pointer": "0x7ffd9c3f0160",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B27208",
    "RVA": "0x01827208",
    "module": "KERNEL32.DLL",
    "name": "UnhandledExceptionFilter",
    "dump_pointer": "0x7ffd9c40cb50",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4B27210",
    "RVA": "0x01827210",
    "module": "KERNEL32.DLL",
    "name": "SetUnhandledExceptionFilter",
    "dump_pointer": "0x7ffd9c3e8b30",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4B27218",
    "RVA": "0x01827218",
    "module": "KERNEL32.DLL",
    "name": "IsDebuggerPresent",
    "dump_pointer": "0x7ffd9c3e7f20",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B27228",
    "RVA": "0x01827228",
    "module": "KERNEL32.DLL",
    "name": "RtlCaptureContext",
    "dump_pointer": "0x7ffd9c3eff90",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4B27230",
    "RVA": "0x01827230",
    "module": "KERNEL32.DLL",
    "name": "RaiseException",
    "dump_pointer": "0x7ffd9c3e8030",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4B27240",
    "RVA": "0x01827240",
    "module": "KERNEL32.DLL",
    "name": "GetModuleHandleW",
    "dump_pointer": "0x7ffd9c3e66b0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27248",
    "RVA": "0x01827248",
    "module": "KERNEL32.DLL",
    "name": "SetHandleCount",
    "dump_pointer": "0x7ffd9c3ef730",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27250",
    "RVA": "0x01827250",
    "module": "KERNEL32.DLL",
    "name": "GetStdHandle",
    "dump_pointer": "0x7ffd9c3e7d00",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B27258",
    "RVA": "0x01827258",
    "module": "KERNEL32.DLL",
    "name": "GetFileType",
    "dump_pointer": "0x7ffd9c3f06b0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B27260",
    "RVA": "0x01827260",
    "module": "KERNEL32.DLL",
    "name": "GetStartupInfoA",
    "dump_pointer": "0x7ffd9c3eeac0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27268",
    "RVA": "0x01827268",
    "module": "ntdll.dll",
    "name": "RtlDeleteCriticalSection",
    "dump_pointer": "0x7ffd9df8a770",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4B27270",
    "RVA": "0x01827270",
    "module": "KERNEL32.DLL",
    "name": "GetModuleFileNameA",
    "dump_pointer": "0x7ffd9c3e89f0",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B27278",
    "RVA": "0x01827278",
    "module": "KERNEL32.DLL",
    "name": "FreeEnvironmentStringsA",
    "dump_pointer": "0x7ffd9c40b540",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B27280",
    "RVA": "0x01827280",
    "module": "KERNEL32.DLL",
    "name": "GetEnvironmentStrings",
    "dump_pointer": "0x7ffd9c40b6f0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27288",
    "RVA": "0x01827288",
    "module": "KERNEL32.DLL",
    "name": "FreeEnvironmentStringsW",
    "dump_pointer": "0x7ffd9c3e8880",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27290",
    "RVA": "0x01827290",
    "module": "KERNEL32.DLL",
    "name": "GetEnvironmentStringsW",
    "dump_pointer": "0x7ffd9c3e8860",
    "indexed_xrefs": 2
  },
  {
    "VA": "0x212C4B27298",
    "RVA": "0x01827298",
    "module": "KERNEL32.DLL",
    "name": "HeapSetInformation",
    "dump_pointer": "0x7ffd9c3e8f60",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B272A0",
    "RVA": "0x018272A0",
    "module": "KERNEL32.DLL",
    "name": "HeapCreate",
    "dump_pointer": "0x7ffd9c3e8f20",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B272A8",
    "RVA": "0x018272A8",
    "module": "KERNEL32.DLL",
    "name": "HeapDestroy",
    "dump_pointer": "0x7ffd9c3e9a80",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B272B0",
    "RVA": "0x018272B0",
    "module": "KERNEL32.DLL",
    "name": "QueryPerformanceCounter",
    "dump_pointer": "0x7ffd9c3e0e50",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B272B8",
    "RVA": "0x018272B8",
    "module": "KERNEL32.DLL",
    "name": "LCMapStringA",
    "dump_pointer": "0x7ffd9c40bd50",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4B272C0",
    "RVA": "0x018272C0",
    "module": "KERNEL32.DLL",
    "name": "LCMapStringW",
    "dump_pointer": "0x7ffd9c3e3290",
    "indexed_xrefs": 5
  },
  {
    "VA": "0x212C4B272C8",
    "RVA": "0x018272C8",
    "module": "KERNEL32.DLL",
    "name": "GetStringTypeExA",
    "dump_pointer": "0x7ffd9c40b910",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B272D0",
    "RVA": "0x018272D0",
    "module": "KERNEL32.DLL",
    "name": "GetStringTypeW",
    "dump_pointer": "0x7ffd9c3e80a0",
    "indexed_xrefs": 3
  },
  {
    "VA": "0x212C4B272D8",
    "RVA": "0x018272D8",
    "module": "ntdll.dll",
    "name": "RtlLeaveCriticalSection",
    "dump_pointer": "0x7ffd9df56ab0",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4B272E0",
    "RVA": "0x018272E0",
    "module": "ntdll.dll",
    "name": "RtlEnterCriticalSection",
    "dump_pointer": "0x7ffd9df51690",
    "indexed_xrefs": 4
  },
  {
    "VA": "0x212C4B272E8",
    "RVA": "0x018272E8",
    "module": "KERNEL32.DLL",
    "name": "GetLocaleInfoA",
    "dump_pointer": "0x7ffd9c3ee9c0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B272F0",
    "RVA": "0x018272F0",
    "module": "ntdll.dll",
    "name": "RtlSizeHeap",
    "dump_pointer": "0x7ffd9df6a4e0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B272F8",
    "RVA": "0x018272F8",
    "module": "KERNEL32.DLL",
    "name": "WriteFile",
    "dump_pointer": "0x7ffd9c3f08f0",
    "indexed_xrefs": 7
  },
  {
    "VA": "0x212C4B27300",
    "RVA": "0x01827300",
    "module": "KERNEL32.DLL",
    "name": "SetFilePointer",
    "dump_pointer": "0x7ffd9c3f0890",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27308",
    "RVA": "0x01827308",
    "module": "KERNEL32.DLL",
    "name": "GetConsoleCP",
    "dump_pointer": "0x7ffd9c3f0c30",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27310",
    "RVA": "0x01827310",
    "module": "KERNEL32.DLL",
    "name": "GetConsoleMode",
    "dump_pointer": "0x7ffd9c3f0c40",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27318",
    "RVA": "0x01827318",
    "module": "ntdll.dll",
    "name": "RtlReAllocateHeap",
    "dump_pointer": "0x7ffd9df71c50",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27320",
    "RVA": "0x01827320",
    "module": "KERNEL32.DLL",
    "name": "InitializeCriticalSectionAndSpinCount",
    "dump_pointer": "0x7ffd9c3f02e0",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27328",
    "RVA": "0x01827328",
    "module": "KERNEL32.DLL",
    "name": "SetStdHandle",
    "dump_pointer": "0x7ffd9c40ca60",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27330",
    "RVA": "0x01827330",
    "module": "KERNEL32.DLL",
    "name": "WriteConsoleA",
    "dump_pointer": "0x7ffd9c3f0d00",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27338",
    "RVA": "0x01827338",
    "module": "KERNEL32.DLL",
    "name": "GetConsoleOutputCP",
    "dump_pointer": "0x7ffd9c3f0c50",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27340",
    "RVA": "0x01827340",
    "module": "KERNEL32.DLL",
    "name": "WriteConsoleW",
    "dump_pointer": "0x7ffd9c3f0d10",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C4B27348",
    "RVA": "0x01827348",
    "module": "KERNEL32.DLL",
    "name": "CreateFileA",
    "dump_pointer": "0x7ffd9c3f0450",
    "indexed_xrefs": 1
  },
  {
    "VA": "0x212C82F2000",
    "RVA": "0x04FF2000",
    "module": "KERNEL32.DLL",
    "name": "GetModuleHandleA",
    "dump_pointer": "0x7ffd9c3e8940",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C82F2008",
    "RVA": "0x04FF2008",
    "module": "KERNEL32.DLL",
    "name": "GetProcAddress",
    "dump_pointer": "0x7ffd9c3e3c10",
    "indexed_xrefs": 0
  },
  {
    "VA": "0x212C82F2018",
    "RVA": "0x04FF2018",
    "module": "USER32.dll",
    "name": "MessageBoxA",
    "dump_pointer": "0x7ffd9c519770",
    "indexed_xrefs": 0
  }
]
```
