<!-- split-part | CS2_RESEARCH_MASTER.md lines 13600-13697 | body-sha256 12b748f845e906322847c6efa7f18dd956d1f8906a27053da163eddd82e9bba9 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-038"></a>

## E038. `analysis/phase2/logs/phase2-trace.log`

Bytes: 7637. SHA-256: `8035aefb9285776a4296ccfa7b796e3b90f8fdf7cc632837aa7e0aa12c3cc28f`.

```text
openjdk version "21.0.12.1" 2026-08-18
OpenJDK Runtime Environment (build 21.0.12.1+1-1-deb13u1-Debian)
OpenJDK 64-Bit Server VM (build 21.0.12.1+1-1-deb13u1-Debian, mixed mode)
INFO  Using log config file: jar:file:/home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Framework/Generic/lib/Generic.jar!/generic.log4j.xml (LoggingInitialization)  
INFO  Using log file: /home/daytona/.config/ghidra/ghidra_12.1.4_PUBLIC/application.log (LoggingInitialization)  
INFO  Loading user preferences: /home/daytona/.config/ghidra/ghidra_12.1.4_PUBLIC/preferences (Preferences)  
INFO  Searching for classes... (ClassSearcher)  
INFO  Class search complete (969 ms) (ClassSearcher)  
INFO  Initializing SSL Context (DefaultSSLContextInitializer)  
INFO  Initializing Random Number Generator... (SecureRandomFactory)  
INFO  Random Number Generator initialization complete: NativePRNGNonBlocking (SecureRandomFactory)  
INFO  Trust manager disabled, cacerts have not been set (DefaultTrustManagerFactory)  
INFO  Headless startup complete (2953 ms) (AnalyzeHeadless)  
INFO  Class searcher loaded 59 extension points (20 false positives) (ClassSearcher)  
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
INFO  Opening existing project: /home/daytona/albigg/analysis/phase2/ghidra_conditional/CONDITIONAL_PATH_MODEL (HeadlessAnalyzer)  
INFO  Opening project: /home/daytona/albigg/analysis/phase2/ghidra_conditional/CONDITIONAL_PATH_MODEL (HeadlessProject)  
INFO  REPORT: Processing project file: /CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin (HeadlessAnalyzer)  
INFO  REPORT: Execute script: ApplyConditionalGuards.java '/home/daytona/albigg/analysis/phase2/main/conditional_guards.tsv'  (HeadlessAnalyzer)  
INFO  SCRIPT: /home/daytona/albigg/analysis/scripts/ApplyConditionalGuards.java (HeadlessAnalyzer)  
INFO  ApplyConditionalGuards.java> CONDITIONAL VIEW ONLY: applied 1496 additional assumed-success branches (GhidraScript)  
INFO  REPORT: Execute script: RecoverDump.java '/home/daytona/albigg/analysis' 'phase2/main/trace_targets.tsv' 'phase2/conditional_trace'  (HeadlessAnalyzer)  
INFO  SCRIPT: /home/daytona/albigg/analysis/scripts/RecoverDump.java (HeadlessAnalyzer)  
INFO  RecoverDump.java> DECOMPILED 0x51dac0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x2f19c0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x2f2110 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x2f0dd0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x51cf40 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x51c9c0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x51d320 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x526290 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x475e90 true (GhidraScript)  
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
INFO  REPORT: Execute script: RecoverDump.java '/home/daytona/albigg/analysis' 'phase2/main/bullet_targets.tsv' 'phase2/conditional_bullet'  (HeadlessAnalyzer)  
INFO  SCRIPT: /home/daytona/albigg/analysis/scripts/RecoverDump.java (HeadlessAnalyzer)  
INFO  RecoverDump.java> DECOMPILED 0x51d910 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x5248e0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x525830 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x51c5f0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x515ca0 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x512710 true (GhidraScript)  
INFO  RecoverDump.java> DECOMPILED 0x4beb50 false (GhidraScript)  
INFO  REPORT: Execute script: ExportEvidence.java '/home/daytona/albigg/analysis/phase2/conditional_assembly'  (HeadlessAnalyzer)  
INFO  SCRIPT: /home/daytona/albigg/analysis/scripts/ExportEvidence.java (HeadlessAnalyzer)  
INFO  REPORT: Save succeeded for processed file: /CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin (HeadlessAnalyzer)  
```
