<!-- split-part | CS2_RESEARCH_MASTER.md lines 13336-13419 | body-sha256 b10872087c17705dcb41e97e1a77b1142199c8f2a7bd5547df6f4ab86246b02f -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-034"></a>

## E034. `analysis/phase2/logs/phase2-core.log`

Bytes: 6682. SHA-256: `ed30d3319ac158df97811ea56eade349d480c8e21d96d58b4c42ccca4aac2aa2`.

```text
openjdk version "21.0.12.1" 2026-08-18
OpenJDK Runtime Environment (build 21.0.12.1+1-1-deb13u1-Debian)
OpenJDK 64-Bit Server VM (build 21.0.12.1+1-1-deb13u1-Debian, mixed mode)
INFO  Using log config file: jar:file:/home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Framework/Generic/lib/Generic.jar!/generic.log4j.xml (LoggingInitialization)  
INFO  Using log file: /home/daytona/.config/ghidra/ghidra_12.1.4_PUBLIC/application.log (LoggingInitialization)  
INFO  Loading user preferences: /home/daytona/.config/ghidra/ghidra_12.1.4_PUBLIC/preferences (Preferences)  
INFO  Searching for classes... (ClassSearcher)  
INFO  Class search complete (1126 ms) (ClassSearcher)  
INFO  Initializing SSL Context (DefaultSSLContextInitializer)  
INFO  Initializing Random Number Generator... (SecureRandomFactory)  
INFO  Random Number Generator initialization complete: NativePRNGNonBlocking (SecureRandomFactory)  
INFO  Trust manager disabled, cacerts have not been set (DefaultTrustManagerFactory)  
INFO  Headless startup complete (6218 ms) (AnalyzeHeadless)  
INFO  Class searcher loaded 93 extension points (46 false positives) (ClassSearcher)  
INFO  HEADLESS Script Paths:
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/Decompiler/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Debug/Debugger/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/BytePatterns/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/SwiftDemangler/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/Base/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/VersionTracking/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/FileFormats/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Processors/8051/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Processors/Atmel/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/WildcardAssembler/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Processors/DATA/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Processors/PIC/ghidra_scripts
    /home/daytona/albigg/analysis/scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/PyGhidra/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/BSim/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Debug/Debugger-rmi-trace/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/MicrosoftCodeAnalyzer/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/FunctionID/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/DecompilerDependent/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Processors/JVM/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/GnuDemangler/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/PDB/ghidra_scripts
    /home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Features/SystemEmulation/ghidra_scripts (HeadlessAnalyzer)  
INFO  HEADLESS: execution starts (HeadlessAnalyzer)  
INFO  Creating project: /home/daytona/albigg/analysis/phase2/ghidra_conditional/CONDITIONAL_PATH_MODEL (HeadlessAnalyzer)  
INFO  Creating project: /home/daytona/albigg/analysis/phase2/ghidra_conditional/CONDITIONAL_PATH_MODEL (DefaultProject)  
INFO  REPORT: Processing input files:  (HeadlessAnalyzer)  
INFO       project: /home/daytona/albigg/analysis/phase2/ghidra_conditional/CONDITIONAL_PATH_MODEL (HeadlessAnalyzer)  
INFO  IMPORTING: file:///home/daytona/albigg/analysis/phase2/model/CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin (HeadlessAnalyzer)  
INFO  Successfully applied "-loader-baseAddr" to "Base Address" (old: "null", new: "0x212C3300000") (ProgramLoader)  
INFO  Using Loader: Raw Binary (ProgramLoader)  
INFO  Using Language/Compiler: x86:LE:64:default:windows (ProgramLoader)  
INFO  Using Library Search Path: [., /bin, /lib, /lib64, /lib/x86_64-linux-gnu, /lib/aarch64-linux-gnu, /usr/bin, /usr/lib, /usr/X11R6/bin, /usr/X11R6/lib, /usr/java/packages/lib, /usr/lib/x86_64-linux-gnu/jni, /usr/lib/x86_64-linux-gnu, /usr/lib/jni] (ProgramLoader)  
INFO  IMPORTING: Loaded 0 additional files (HeadlessAnalyzer)  
INFO  REPORT: Execute script: RecoverDump.java '/home/daytona/albigg/analysis' 'phase2/main/core_targets.tsv' 'phase2/conditional_core'  (HeadlessAnalyzer)  
INFO  SCRIPT: /home/daytona/albigg/analysis/scripts/RecoverDump.java (HeadlessAnalyzer)  
INFO  RecoverDump.java> DECOMPILED 0xf57b0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x1463a0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x52ad40 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x528b90 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x5239d0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x524a30 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x51bea0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x52bb10 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x52dd20 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x51df10 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x511160 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x5114d0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x530cd0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x5302c0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x50ce90 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x30d580 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x52f030 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x52e720 true (GhidraScript)  
INFO  REPORT: Execute script: ExportEvidence.java '/home/daytona/albigg/analysis/phase2/conditional_assembly'  (HeadlessAnalyzer)  
INFO  SCRIPT: /home/daytona/albigg/analysis/scripts/ExportEvidence.java (HeadlessAnalyzer)  
INFO  /CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin: file created (daytona) (LocalFileSystem)  
INFO  REPORT: Save succeeded for: /CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin (CONDITIONAL_PATH_MODEL:/CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin) (HeadlessAnalyzer)  
INFO  REPORT: Import succeeded (HeadlessAnalyzer)  
```
