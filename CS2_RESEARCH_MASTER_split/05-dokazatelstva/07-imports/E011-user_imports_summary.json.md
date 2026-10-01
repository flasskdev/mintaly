<!-- split-part | CS2_RESEARCH_MASTER.md lines 5417-6700 | body-sha256 56f7b63299636b7f70c6858b1b35ec234d068e0c5753d35dd39a0369bdaab3b2 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-011"></a>

## E011. `analysis/imports/user_imports_summary.json`

Bytes: 30995. SHA-256: `9d94e65c1eb0afa70d8cb3a8cef258483d3500851bfcdaed18c0c78f9b80d615`.

```json
{
  "schema_version": 1,
  "source": {
    "kind": "user_provided_import_table",
    "description": "Complete brace-delimited table from the user message accompanying cs2_212C3300000.rar, including both trailing address regions.",
    "thread_root_message_id": "msg_01m3ry2zwteqps3dd8qgpmz24m",
    "source_text_available_in_context": "complete",
    "source_table_file": "/home/daytona/albigg/analysis/imports/user_imports_source.txt",
    "source_table_sha256": "f521546cc3050a66c399bc3fa4b73758f38f7418cc403dbf987edf0e3d28b708",
    "extraction_method": "Visible source table copied to UTF-8 text; strict full-line parsing; no binary or external import lookup.",
    "completeness_scope": "All entries in the supplied text table, not a claim that it lists every import in the binary.",
    "binary_inspected": false,
    "binary_executed": false,
    "binary_modified": false
  },
  "base_address": "0x212C3300000",
  "user_declared_image_size": "0x5001000",
  "rva_formula": "RVA = VA - base_address",
  "csv": {
    "path": "/home/daytona/albigg/analysis/imports/user_imports.csv",
    "columns": [
      "VA",
      "RVA",
      "module",
      "name"
    ],
    "encoding": "UTF-8",
    "delimiter": ",",
    "line_ending": "LF",
    "address_format": "0x-prefixed uppercase hexadecimal; RVA padded to 8 hex digits",
    "source_order_preserved": true,
    "module_and_name_spelling_preserved": true,
    "duplicates_preserved": true,
    "sha256": "90d29943165d6337030f69ccd5aff7d8fdb2d7725990a5514e7de54d02bde80a",
    "data_rows": 391,
    "lines_including_header": 392
  },
  "counts": {
    "imports": 391,
    "modules": 11,
    "regions": 3,
    "unique_VA": 391,
    "unique_module_name_pairs": 334,
    "duplicate_symbol_groups": 56,
    "rows_in_duplicate_symbol_groups": 113,
    "duplicate_symbol_extra_rows": 57,
    "duplicate_VA_groups": 0,
    "duplicate_exact_row_groups": 0
  },
  "groups": {
    "by_module": [
      {
        "module": "advapi32.dll",
        "count": 7,
        "unique_names": 7,
        "first_VA": "0x212C42928F0",
        "last_VA": "0x212C4292920"
      },
      {
        "module": "combase.dll",
        "count": 2,
        "unique_names": 2,
        "first_VA": "0x212C4292B38",
        "last_VA": "0x212C4292B60"
      },
      {
        "module": "CRYPT32.dll",
        "count": 6,
        "unique_names": 6,
        "first_VA": "0x212C4292BA0",
        "last_VA": "0x212C4292BC8"
      },
      {
        "module": "KERNEL32.DLL",
        "count": 234,
        "unique_names": 184,
        "first_VA": "0x212C4291D50",
        "last_VA": "0x212C82F2008"
      },
      {
        "module": "KERNELBASE.dll",
        "count": 3,
        "unique_names": 3,
        "first_VA": "0x212C4291FE8",
        "last_VA": "0x212C4292310"
      },
      {
        "module": "ntdll.dll",
        "count": 25,
        "unique_names": 21,
        "first_VA": "0x212C4291D40",
        "last_VA": "0x212C4B27318"
      },
      {
        "module": "OLEAUT32.dll",
        "count": 2,
        "unique_names": 2,
        "first_VA": "0x212C4292B10",
        "last_VA": "0x212C4292B18"
      },
      {
        "module": "SHELL32.dll",
        "count": 1,
        "unique_names": 1,
        "first_VA": "0x212C42928A0",
        "last_VA": "0x212C42928A0"
      },
      {
        "module": "tier0.dll",
        "count": 43,
        "unique_names": 43,
        "first_VA": "0x212C4292540",
        "last_VA": "0x212C42926F8"
      },
      {
        "module": "USER32.dll",
        "count": 29,
        "unique_names": 28,
        "first_VA": "0x212C42923F8",
        "last_VA": "0x212C82F2018"
      },
      {
        "module": "WS2_32.dll",
        "count": 39,
        "unique_names": 37,
        "first_VA": "0x212C42929B8",
        "last_VA": "0x212C4292AF0"
      }
    ],
    "by_region": [
      {
        "label": "primary_list",
        "count": 321,
        "first_VA": "0x212C4291D40",
        "last_VA": "0x212C4292BC8",
        "first_RVA": "0x00F91D40",
        "last_RVA": "0x00F92BC8",
        "by_module": {
          "advapi32.dll": 7,
          "combase.dll": 2,
          "CRYPT32.dll": 6,
          "KERNEL32.DLL": 171,
          "KERNELBASE.dll": 3,
          "ntdll.dll": 19,
          "OLEAUT32.dll": 2,
          "SHELL32.dll": 1,
          "tier0.dll": 43,
          "USER32.dll": 28,
          "WS2_32.dll": 39
        }
      },
      {
        "label": "tail_1",
        "count": 67,
        "first_VA": "0x212C4B27000",
        "last_VA": "0x212C4B27348",
        "first_RVA": "0x01827000",
        "last_RVA": "0x01827348",
        "by_module": {
          "KERNEL32.DLL": 61,
          "ntdll.dll": 6
        }
      },
      {
        "label": "tail_2",
        "count": 3,
        "first_VA": "0x212C82F2000",
        "last_VA": "0x212C82F2018",
        "first_RVA": "0x04FF2000",
        "last_RVA": "0x04FF2018",
        "by_module": {
          "KERNEL32.DLL": 2,
          "USER32.dll": 1
        }
      }
    ],
    "region_definition": "Three observed address clusters in the supplied text; not PE section names and not independently verified import descriptors."
  },
  "duplicates": {
    "symbol_identity_rule": "module case-insensitive, import name case-sensitive; original strings retained in CSV",
    "by_module_and_name": [
      {
        "module": "KERNEL32.DLL",
        "name": "CloseHandle",
        "count": 2,
        "occurrences": [
          {
            "data_row": 4,
            "csv_line": 5,
            "VA": "0x212C4291D58",
            "RVA": "0x00F91D58"
          },
          {
            "data_row": 324,
            "csv_line": 325,
            "VA": "0x212C4B27048",
            "RVA": "0x01827048"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "CreateFileA",
        "count": 2,
        "occurrences": [
          {
            "data_row": 14,
            "csv_line": 15,
            "VA": "0x212C4291DB0",
            "RVA": "0x00F91DB0"
          },
          {
            "data_row": 388,
            "csv_line": 389,
            "VA": "0x212C4B27348",
            "RVA": "0x01827348"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "ExitProcess",
        "count": 2,
        "occurrences": [
          {
            "data_row": 34,
            "csv_line": 35,
            "VA": "0x212C4291E70",
            "RVA": "0x00F91E70"
          },
          {
            "data_row": 333,
            "csv_line": 334,
            "VA": "0x212C4B270F8",
            "RVA": "0x018270F8"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "FlsAlloc",
        "count": 2,
        "occurrences": [
          {
            "data_row": 42,
            "csv_line": 43,
            "VA": "0x212C4291EB0",
            "RVA": "0x00F91EB0"
          },
          {
            "data_row": 348,
            "csv_line": 349,
            "VA": "0x212C4B271F8",
            "RVA": "0x018271F8"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "FlsFree",
        "count": 2,
        "occurrences": [
          {
            "data_row": 43,
            "csv_line": 44,
            "VA": "0x212C4291EB8",
            "RVA": "0x00F91EB8"
          },
          {
            "data_row": 346,
            "csv_line": 347,
            "VA": "0x212C4B271E8",
            "RVA": "0x018271E8"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "FlsGetValue",
        "count": 2,
        "occurrences": [
          {
            "data_row": 44,
            "csv_line": 45,
            "VA": "0x212C4291EC0",
            "RVA": "0x00F91EC0"
          },
          {
            "data_row": 345,
            "csv_line": 346,
            "VA": "0x212C4B271E0",
            "RVA": "0x018271E0"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "FlsSetValue",
        "count": 2,
        "occurrences": [
          {
            "data_row": 45,
            "csv_line": 46,
            "VA": "0x212C4291EC8",
            "RVA": "0x00F91EC8"
          },
          {
            "data_row": 339,
            "csv_line": 340,
            "VA": "0x212C4B271A0",
            "RVA": "0x018271A0"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "FlushFileBuffers",
        "count": 2,
        "occurrences": [
          {
            "data_row": 46,
            "csv_line": 47,
            "VA": "0x212C4291ED0",
            "RVA": "0x00F91ED0"
          },
          {
            "data_row": 337,
            "csv_line": 338,
            "VA": "0x212C4B27188",
            "RVA": "0x01827188"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "FreeEnvironmentStringsW",
        "count": 2,
        "occurrences": [
          {
            "data_row": 49,
            "csv_line": 50,
            "VA": "0x212C4291EE8",
            "RVA": "0x00F91EE8"
          },
          {
            "data_row": 364,
            "csv_line": 365,
            "VA": "0x212C4B27288",
            "RVA": "0x01827288"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetACP",
        "count": 2,
        "occurrences": [
          {
            "data_row": 52,
            "csv_line": 53,
            "VA": "0x212C4291F00",
            "RVA": "0x00F91F00"
          },
          {
            "data_row": 342,
            "csv_line": 343,
            "VA": "0x212C4B271B8",
            "RVA": "0x018271B8"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetCPInfo",
        "count": 2,
        "occurrences": [
          {
            "data_row": 53,
            "csv_line": 54,
            "VA": "0x212C4291F08",
            "RVA": "0x00F91F08"
          },
          {
            "data_row": 341,
            "csv_line": 342,
            "VA": "0x212C4B271B0",
            "RVA": "0x018271B0"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetCommandLineA",
        "count": 2,
        "occurrences": [
          {
            "data_row": 54,
            "csv_line": 55,
            "VA": "0x212C4291F10",
            "RVA": "0x00F91F10"
          },
          {
            "data_row": 340,
            "csv_line": 341,
            "VA": "0x212C4B271A8",
            "RVA": "0x018271A8"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetConsoleMode",
        "count": 2,
        "occurrences": [
          {
            "data_row": 56,
            "csv_line": 57,
            "VA": "0x212C4291F20",
            "RVA": "0x00F91F20"
          },
          {
            "data_row": 381,
            "csv_line": 382,
            "VA": "0x212C4B27310",
            "RVA": "0x01827310"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetConsoleOutputCP",
        "count": 2,
        "occurrences": [
          {
            "data_row": 57,
            "csv_line": 58,
            "VA": "0x212C4291F28",
            "RVA": "0x00F91F28"
          },
          {
            "data_row": 386,
            "csv_line": 387,
            "VA": "0x212C4B27338",
            "RVA": "0x01827338"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetCurrentProcess",
        "count": 2,
        "occurrences": [
          {
            "data_row": 59,
            "csv_line": 60,
            "VA": "0x212C4291F38",
            "RVA": "0x00F91F38"
          },
          {
            "data_row": 349,
            "csv_line": 350,
            "VA": "0x212C4B27200",
            "RVA": "0x01827200"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetCurrentProcessId",
        "count": 2,
        "occurrences": [
          {
            "data_row": 60,
            "csv_line": 61,
            "VA": "0x212C4291F40",
            "RVA": "0x00F91F40"
          },
          {
            "data_row": 326,
            "csv_line": 327,
            "VA": "0x212C4B27068",
            "RVA": "0x01827068"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetCurrentThreadId",
        "count": 2,
        "occurrences": [
          {
            "data_row": 62,
            "csv_line": 63,
            "VA": "0x212C4291F50",
            "RVA": "0x00F91F50"
          },
          {
            "data_row": 325,
            "csv_line": 326,
            "VA": "0x212C4B27060",
            "RVA": "0x01827060"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetEnvironmentStringsW",
        "count": 2,
        "occurrences": [
          {
            "data_row": 65,
            "csv_line": 66,
            "VA": "0x212C4291F68",
            "RVA": "0x00F91F68"
          },
          {
            "data_row": 365,
            "csv_line": 366,
            "VA": "0x212C4B27290",
            "RVA": "0x01827290"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetFileType",
        "count": 2,
        "occurrences": [
          {
            "data_row": 74,
            "csv_line": 75,
            "VA": "0x212C4291FB0",
            "RVA": "0x00F91FB0"
          },
          {
            "data_row": 358,
            "csv_line": 359,
            "VA": "0x212C4B27258",
            "RVA": "0x01827258"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetLastError",
        "count": 2,
        "occurrences": [
          {
            "data_row": 77,
            "csv_line": 78,
            "VA": "0x212C4291FC8",
            "RVA": "0x00F91FC8"
          },
          {
            "data_row": 336,
            "csv_line": 337,
            "VA": "0x212C4B27170",
            "RVA": "0x01827170"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetModuleFileNameA",
        "count": 2,
        "occurrences": [
          {
            "data_row": 82,
            "csv_line": 83,
            "VA": "0x212C4291FF0",
            "RVA": "0x00F91FF0"
          },
          {
            "data_row": 361,
            "csv_line": 362,
            "VA": "0x212C4B27270",
            "RVA": "0x01827270"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetModuleHandleA",
        "count": 2,
        "occurrences": [
          {
            "data_row": 84,
            "csv_line": 85,
            "VA": "0x212C4292000",
            "RVA": "0x00F92000"
          },
          {
            "data_row": 389,
            "csv_line": 390,
            "VA": "0x212C82F2000",
            "RVA": "0x04FF2000"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetModuleHandleW",
        "count": 2,
        "occurrences": [
          {
            "data_row": 87,
            "csv_line": 88,
            "VA": "0x212C4292018",
            "RVA": "0x00F92018"
          },
          {
            "data_row": 355,
            "csv_line": 356,
            "VA": "0x212C4B27240",
            "RVA": "0x01827240"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetOEMCP",
        "count": 2,
        "occurrences": [
          {
            "data_row": 88,
            "csv_line": 89,
            "VA": "0x212C4292020",
            "RVA": "0x00F92020"
          },
          {
            "data_row": 343,
            "csv_line": 344,
            "VA": "0x212C4B271C0",
            "RVA": "0x018271C0"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetProcAddress",
        "count": 3,
        "occurrences": [
          {
            "data_row": 89,
            "csv_line": 90,
            "VA": "0x212C4292030",
            "RVA": "0x00F92030"
          },
          {
            "data_row": 332,
            "csv_line": 333,
            "VA": "0x212C4B270F0",
            "RVA": "0x018270F0"
          },
          {
            "data_row": 390,
            "csv_line": 391,
            "VA": "0x212C82F2008",
            "RVA": "0x04FF2008"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetStdHandle",
        "count": 2,
        "occurrences": [
          {
            "data_row": 93,
            "csv_line": 94,
            "VA": "0x212C4292050",
            "RVA": "0x00F92050"
          },
          {
            "data_row": 357,
            "csv_line": 358,
            "VA": "0x212C4B27250",
            "RVA": "0x01827250"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetStringTypeW",
        "count": 2,
        "occurrences": [
          {
            "data_row": 94,
            "csv_line": 95,
            "VA": "0x212C4292058",
            "RVA": "0x00F92058"
          },
          {
            "data_row": 373,
            "csv_line": 374,
            "VA": "0x212C4B272D0",
            "RVA": "0x018272D0"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetSystemTimeAsFileTime",
        "count": 2,
        "occurrences": [
          {
            "data_row": 98,
            "csv_line": 99,
            "VA": "0x212C4292078",
            "RVA": "0x00F92078"
          },
          {
            "data_row": 322,
            "csv_line": 323,
            "VA": "0x212C4B27000",
            "RVA": "0x01827000"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "GetTickCount",
        "count": 2,
        "occurrences": [
          {
            "data_row": 102,
            "csv_line": 103,
            "VA": "0x212C4292098",
            "RVA": "0x00F92098"
          },
          {
            "data_row": 328,
            "csv_line": 329,
            "VA": "0x212C4B270B0",
            "RVA": "0x018270B0"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "HeapFree",
        "count": 2,
        "occurrences": [
          {
            "data_row": 115,
            "csv_line": 116,
            "VA": "0x212C4292118",
            "RVA": "0x00F92118"
          },
          {
            "data_row": 331,
            "csv_line": 332,
            "VA": "0x212C4B270E8",
            "RVA": "0x018270E8"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "InitializeCriticalSectionAndSpinCount",
        "count": 2,
        "occurrences": [
          {
            "data_row": 118,
            "csv_line": 119,
            "VA": "0x212C4292140",
            "RVA": "0x00F92140"
          },
          {
            "data_row": 383,
            "csv_line": 384,
            "VA": "0x212C4B27320",
            "RVA": "0x01827320"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "IsDebuggerPresent",
        "count": 2,
        "occurrences": [
          {
            "data_row": 122,
            "csv_line": 123,
            "VA": "0x212C4292168",
            "RVA": "0x00F92168"
          },
          {
            "data_row": 352,
            "csv_line": 353,
            "VA": "0x212C4B27218",
            "RVA": "0x01827218"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "IsValidCodePage",
        "count": 2,
        "occurrences": [
          {
            "data_row": 124,
            "csv_line": 125,
            "VA": "0x212C4292178",
            "RVA": "0x00F92178"
          },
          {
            "data_row": 344,
            "csv_line": 345,
            "VA": "0x212C4B271C8",
            "RVA": "0x018271C8"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "LCMapStringW",
        "count": 2,
        "occurrences": [
          {
            "data_row": 127,
            "csv_line": 128,
            "VA": "0x212C42921A0",
            "RVA": "0x00F921A0"
          },
          {
            "data_row": 371,
            "csv_line": 372,
            "VA": "0x212C4B272C0",
            "RVA": "0x018272C0"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "LoadLibraryA",
        "count": 2,
        "occurrences": [
          {
            "data_row": 129,
            "csv_line": 130,
            "VA": "0x212C42921B0",
            "RVA": "0x00F921B0"
          },
          {
            "data_row": 329,
            "csv_line": 330,
            "VA": "0x212C4B270D8",
            "RVA": "0x018270D8"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "MultiByteToWideChar",
        "count": 2,
        "occurrences": [
          {
            "data_row": 136,
            "csv_line": 137,
            "VA": "0x212C4292200",
            "RVA": "0x00F92200"
          },
          {
            "data_row": 334,
            "csv_line": 335,
            "VA": "0x212C4B27100",
            "RVA": "0x01827100"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "QueryPerformanceCounter",
        "count": 2,
        "occurrences": [
          {
            "data_row": 139,
            "csv_line": 140,
            "VA": "0x212C4292228",
            "RVA": "0x00F92228"
          },
          {
            "data_row": 369,
            "csv_line": 370,
            "VA": "0x212C4B272B0",
            "RVA": "0x018272B0"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "RaiseException",
        "count": 2,
        "occurrences": [
          {
            "data_row": 141,
            "csv_line": 142,
            "VA": "0x212C4292238",
            "RVA": "0x00F92238"
          },
          {
            "data_row": 354,
            "csv_line": 355,
            "VA": "0x212C4B27230",
            "RVA": "0x01827230"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "RtlCaptureContext",
        "count": 2,
        "occurrences": [
          {
            "data_row": 148,
            "csv_line": 149,
            "VA": "0x212C4292270",
            "RVA": "0x00F92270"
          },
          {
            "data_row": 353,
            "csv_line": 354,
            "VA": "0x212C4B27228",
            "RVA": "0x01827228"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "RtlUnwindEx",
        "count": 2,
        "occurrences": [
          {
            "data_row": 153,
            "csv_line": 154,
            "VA": "0x212C4292298",
            "RVA": "0x00F92298"
          },
          {
            "data_row": 338,
            "csv_line": 339,
            "VA": "0x212C4B27198",
            "RVA": "0x01827198"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "SetLastError",
        "count": 2,
        "occurrences": [
          {
            "data_row": 162,
            "csv_line": 163,
            "VA": "0x212C42922E0",
            "RVA": "0x00F922E0"
          },
          {
            "data_row": 347,
            "csv_line": 348,
            "VA": "0x212C4B271F0",
            "RVA": "0x018271F0"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "SetStdHandle",
        "count": 2,
        "occurrences": [
          {
            "data_row": 163,
            "csv_line": 164,
            "VA": "0x212C42922F0",
            "RVA": "0x00F922F0"
          },
          {
            "data_row": 384,
            "csv_line": 385,
            "VA": "0x212C4B27328",
            "RVA": "0x01827328"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "SetUnhandledExceptionFilter",
        "count": 2,
        "occurrences": [
          {
            "data_row": 164,
            "csv_line": 165,
            "VA": "0x212C42922F8",
            "RVA": "0x00F922F8"
          },
          {
            "data_row": 351,
            "csv_line": 352,
            "VA": "0x212C4B27210",
            "RVA": "0x01827210"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "Sleep",
        "count": 2,
        "occurrences": [
          {
            "data_row": 166,
            "csv_line": 167,
            "VA": "0x212C4292308",
            "RVA": "0x00F92308"
          },
          {
            "data_row": 327,
            "csv_line": 328,
            "VA": "0x212C4B27090",
            "RVA": "0x01827090"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "TerminateProcess",
        "count": 2,
        "occurrences": [
          {
            "data_row": 173,
            "csv_line": 174,
            "VA": "0x212C4292340",
            "RVA": "0x00F92340"
          },
          {
            "data_row": 323,
            "csv_line": 324,
            "VA": "0x212C4B27018",
            "RVA": "0x01827018"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "UnhandledExceptionFilter",
        "count": 2,
        "occurrences": [
          {
            "data_row": 179,
            "csv_line": 180,
            "VA": "0x212C4292370",
            "RVA": "0x00F92370"
          },
          {
            "data_row": 350,
            "csv_line": 351,
            "VA": "0x212C4B27208",
            "RVA": "0x01827208"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "WideCharToMultiByte",
        "count": 2,
        "occurrences": [
          {
            "data_row": 191,
            "csv_line": 192,
            "VA": "0x212C42923D8",
            "RVA": "0x00F923D8"
          },
          {
            "data_row": 335,
            "csv_line": 336,
            "VA": "0x212C4B27108",
            "RVA": "0x01827108"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "WriteConsoleW",
        "count": 2,
        "occurrences": [
          {
            "data_row": 192,
            "csv_line": 193,
            "VA": "0x212C42923E0",
            "RVA": "0x00F923E0"
          },
          {
            "data_row": 387,
            "csv_line": 388,
            "VA": "0x212C4B27340",
            "RVA": "0x01827340"
          }
        ]
      },
      {
        "module": "KERNEL32.DLL",
        "name": "WriteFile",
        "count": 2,
        "occurrences": [
          {
            "data_row": 193,
            "csv_line": 194,
            "VA": "0x212C42923E8",
            "RVA": "0x00F923E8"
          },
          {
            "data_row": 378,
            "csv_line": 379,
            "VA": "0x212C4B272F8",
            "RVA": "0x018272F8"
          }
        ]
      },
      {
        "module": "ntdll.dll",
        "name": "RtlAllocateHeap",
        "count": 2,
        "occurrences": [
          {
            "data_row": 114,
            "csv_line": 115,
            "VA": "0x212C4292110",
            "RVA": "0x00F92110"
          },
          {
            "data_row": 330,
            "csv_line": 331,
            "VA": "0x212C4B270E0",
            "RVA": "0x018270E0"
          }
        ]
      },
      {
        "module": "ntdll.dll",
        "name": "RtlDeleteCriticalSection",
        "count": 2,
        "occurrences": [
          {
            "data_row": 25,
            "csv_line": 26,
            "VA": "0x212C4291E18",
            "RVA": "0x00F91E18"
          },
          {
            "data_row": 360,
            "csv_line": 361,
            "VA": "0x212C4B27268",
            "RVA": "0x01827268"
          }
        ]
      },
      {
        "module": "ntdll.dll",
        "name": "RtlEnterCriticalSection",
        "count": 2,
        "occurrences": [
          {
            "data_row": 32,
            "csv_line": 33,
            "VA": "0x212C4291E60",
            "RVA": "0x00F91E60"
          },
          {
            "data_row": 375,
            "csv_line": 376,
            "VA": "0x212C4B272E0",
            "RVA": "0x018272E0"
          }
        ]
      },
      {
        "module": "ntdll.dll",
        "name": "RtlLeaveCriticalSection",
        "count": 2,
        "occurrences": [
          {
            "data_row": 128,
            "csv_line": 129,
            "VA": "0x212C42921A8",
            "RVA": "0x00F921A8"
          },
          {
            "data_row": 374,
            "csv_line": 375,
            "VA": "0x212C4B272D8",
            "RVA": "0x018272D8"
          }
        ]
      },
      {
        "module": "USER32.dll",
        "name": "MessageBoxA",
        "count": 2,
        "occurrences": [
          {
            "data_row": 213,
            "csv_line": 214,
            "VA": "0x212C42924C0",
            "RVA": "0x00F924C0"
          },
          {
            "data_row": 391,
            "csv_line": 392,
            "VA": "0x212C82F2018",
            "RVA": "0x04FF2018"
          }
        ]
      },
      {
        "module": "WS2_32.dll",
        "name": "ntohl",
        "count": 2,
        "occurrences": [
          {
            "data_row": 299,
            "csv_line": 300,
            "VA": "0x212C4292A90",
            "RVA": "0x00F92A90"
          },
          {
            "data_row": 304,
            "csv_line": 305,
            "VA": "0x212C4292AB8",
            "RVA": "0x00F92AB8"
          }
        ]
      },
      {
        "module": "WS2_32.dll",
        "name": "ntohs",
        "count": 2,
        "occurrences": [
          {
            "data_row": 300,
            "csv_line": 301,
            "VA": "0x212C4292A98",
            "RVA": "0x00F92A98"
          },
          {
            "data_row": 305,
            "csv_line": 306,
            "VA": "0x212C4292AC0",
            "RVA": "0x00F92AC0"
          }
        ]
      }
    ],
    "by_VA": [],
    "exact_rows": []
  },
  "validation": {
    "all_source_rows_parsed": true,
    "unparsed_source_rows": 0,
    "csv_round_trip_matches_parsed_source": true,
    "all_rvas_recomputed_and_verified": true,
    "source_VA_order_ascending": true,
    "all_VA_8_byte_aligned": true,
    "all_8_byte_slots_within_user_declared_image": true,
    "all_rows_assigned_to_exactly_one_region": true,
    "tail_2_three_entries_verified": true,
    "errors": [],
    "notes": [
      "No absent imports inferred from address gaps; duplicates deliberately preserved.",
      "WS2_32.dll!ntohl and WS2_32.dll!ntohs each appear at two distinct VAs in the primary region, exactly as supplied.",
      "Import spellings and module assignments are user-provided facts, not verified against the dump or installed DLL exports.",
      "apply_patch was not available in PATH; managed file tool and managed shell were used. All writes are confined to analysis/imports/."
    ]
  }
}
```
