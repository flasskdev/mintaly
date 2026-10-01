<!-- split-part | CS2_RESEARCH_MASTER.md lines 13272-13335 | body-sha256 e384cd5e1e7c0de127e50baddf34024fbab1ce01f57b45f548ed43b8acd7de09 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-033"></a>

## E033. `analysis/phase2/logs/phase2-bullet.log`

Bytes: 4987. SHA-256: `6fe962a01e6d1e221882c7c2f38f0da2352c4ea9080f51bd01b0894c4ea8f781`.

```text
openjdk version "21.0.12.1" 2026-08-18
OpenJDK Runtime Environment (build 21.0.12.1+1-1-deb13u1-Debian)
OpenJDK 64-Bit Server VM (build 21.0.12.1+1-1-deb13u1-Debian, mixed mode)
INFO  Using log config file: jar:file:/home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Framework/Generic/lib/Generic.jar!/generic.log4j.xml (LoggingInitialization)  
INFO  Using log file: /home/daytona/.config/ghidra/ghidra_12.1.4_PUBLIC/application.log (LoggingInitialization)  
INFO  Loading user preferences: /home/daytona/.config/ghidra/ghidra_12.1.4_PUBLIC/preferences (Preferences)  
INFO  Searching for classes... (ClassSearcher)  
INFO  Class search complete (1550 ms) (ClassSearcher)  
INFO  Initializing SSL Context (DefaultSSLContextInitializer)  
INFO  Initializing Random Number Generator... (SecureRandomFactory)  
INFO  Random Number Generator initialization complete: NativePRNGNonBlocking (SecureRandomFactory)  
INFO  Trust manager disabled, cacerts have not been set (DefaultTrustManagerFactory)  
INFO  Headless startup complete (4282 ms) (AnalyzeHeadless)  
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
