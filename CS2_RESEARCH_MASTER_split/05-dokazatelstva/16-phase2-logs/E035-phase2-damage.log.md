<!-- split-part | CS2_RESEARCH_MASTER.md lines 13420-13477 | body-sha256 3e300c9fbe18e4d6c2b91418b17cfe3e797ef94449fa50459a73837de773fecc -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-035"></a>

## E035. `analysis/phase2/logs/phase2-damage.log`

Bytes: 4593. SHA-256: `d672fc3d9ca1b4c7976814ef3234cd0ed5e92f3b70922d5fa09e9ecd3a2aa77b`.

```text
openjdk version "21.0.12.1" 2026-08-18
OpenJDK Runtime Environment (build 21.0.12.1+1-1-deb13u1-Debian)
OpenJDK 64-Bit Server VM (build 21.0.12.1+1-1-deb13u1-Debian, mixed mode)
INFO  Using log config file: jar:file:/home/daytona/albigg/analysis/tools/ghidra_12.1.4_PUBLIC/Ghidra/Framework/Generic/lib/Generic.jar!/generic.log4j.xml (LoggingInitialization)  
INFO  Using log file: /home/daytona/.config/ghidra/ghidra_12.1.4_PUBLIC/application.log (LoggingInitialization)  
INFO  Loading user preferences: /home/daytona/.config/ghidra/ghidra_12.1.4_PUBLIC/preferences (Preferences)  
INFO  Searching for classes... (ClassSearcher)  
INFO  Class search complete (1159 ms) (ClassSearcher)  
INFO  Initializing SSL Context (DefaultSSLContextInitializer)  
INFO  Initializing Random Number Generator... (SecureRandomFactory)  
INFO  Random Number Generator initialization complete: NativePRNGNonBlocking (SecureRandomFactory)  
INFO  Trust manager disabled, cacerts have not been set (DefaultTrustManagerFactory)  
INFO  Headless startup complete (2737 ms) (AnalyzeHeadless)  
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
INFO  REPORT: Execute script: RecoverDump.java '/home/daytona/albigg/analysis' 'phase2/main/damage_multiplier.tsv' 'phase2/conditional_damage'  (HeadlessAnalyzer)  
INFO  SCRIPT: /home/daytona/albigg/analysis/scripts/RecoverDump.java (HeadlessAnalyzer)  
INFO  RecoverDump.java> DECOMPILED 0x7bdb90 true (GhidraScript)  
INFO  REPORT: Execute script: ExportEvidence.java '/home/daytona/albigg/analysis/phase2/conditional_assembly'  (HeadlessAnalyzer)  
INFO  SCRIPT: /home/daytona/albigg/analysis/scripts/ExportEvidence.java (HeadlessAnalyzer)  
INFO  REPORT: Save succeeded for processed file: /CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin (HeadlessAnalyzer)  
```
