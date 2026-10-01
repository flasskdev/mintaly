<!-- split-part | CS2_RESEARCH_MASTER.md lines 94547-94652 | body-sha256 7aeee221228e00b86db3bcde463b404c83c1d756f684b064e172bf5ce3a5ba01 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-167"></a>

## E167. `analysis/scripts/RecoverDump.java`

Bytes: 5030. SHA-256: `9ad97a8eaf46061b83d429df5ed59b6a54b1770c9fc7132c1c5f4cd46c1977a0`.

```java
import ghidra.app.script.GhidraScript;
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import ghidra.program.model.data.PointerDataType;
import java.nio.file.Files;
import java.nio.file.Path;
import java.io.PrintWriter;
import java.util.List;

public class RecoverDump extends GhidraScript {
    public void run() throws Exception {
        String[] arguments = getScriptArgs();
        Path root = Path.of(arguments[0]);
        Path targetsPath = root.resolve(arguments.length > 1 ? arguments[1] : "results/decompile_targets.tsv");
        Path output = root.resolve(arguments.length > 2 ? arguments[2] : "decompiled");
        Files.createDirectories(output);
        long base = 0x212c3300000L;
        Path imports = root.resolve("imports/user_imports.csv");
        if (Files.exists(imports)) {
            for (String row : Files.readAllLines(imports)) {
                String[] fields = row.split(",", 4);
                if (fields.length < 4 || !fields[0].toLowerCase().startsWith("0x")) continue;
                long location = Long.decode(fields[0]);
                String name = fields[3].replaceAll("[^A-Za-z0-9_]", "_");
                if (name.length() > 140) name = name.substring(0, 140);
                Address address = toAddr(location);
                try {
                    createLabel(address, "IAT_" + name, true, SourceType.USER_DEFINED);
                    if (getDataAt(address) == null) createData(address, new PointerDataType());
                } catch (Exception exception) {
                    println("IMPORT " + fields[0] + " " + exception.getMessage());
                }
            }
        }
        Path anchors = root.resolve("strings/anchors.csv");
        if (Files.exists(anchors)) {
            for (String row : Files.readAllLines(anchors)) {
                String[] fields = row.split(",");
                if (fields.length < 9 || !fields[2].toLowerCase().startsWith("0x") || !fields[7].equals("ascii")) continue;
                Address location = toAddr(Long.decode(fields[2]));
                try {
                    if (getDataAt(location) == null) createAsciiString(location, Integer.parseInt(fields[6]));
                } catch (Exception exception) {
                    println("STRING " + fields[2] + " " + exception.getMessage());
                }
            }
        }
        if (!Files.exists(targetsPath)) return;
        List<String> targets = Files.readAllLines(targetsPath);
        for (String row : targets) {
            if (row.isBlank() || row.startsWith("#")) continue;
            String[] fields = row.split("\t");
            long begin = Long.decode(fields[0]);
            long end = Long.decode(fields[1]);
            String name = fields.length > 2 ? fields[2] : "FUN_" + Long.toHexString(base + begin);
            Address entry = toAddr(base + begin);
            AddressSet body = new AddressSet(entry, toAddr(base + end - 1));
            DisassembleCommand command = new DisassembleCommand(entry, body, true);
            command.applyTo(currentProgram, monitor);
            Function function = getFunctionAt(entry);
            if (function == null) {
                try {
                    currentProgram.getFunctionManager().createFunction(name, entry, body, SourceType.USER_DEFINED);
                } catch (Exception exception) {
                    println("FUNCTION " + fields[0] + " " + exception.getMessage());
                }
            }
        }
        DecompInterface decompiler = new DecompInterface();
        decompiler.openProgram(currentProgram);
        try (PrintWriter index = new PrintWriter(output.resolve("index.tsv").toFile())) {
            for (String row : targets) {
                if (row.isBlank() || row.startsWith("#")) continue;
                String[] fields = row.split("\t");
                long begin = Long.decode(fields[0]);
                Function function = getFunctionAt(toAddr(base + begin));
                if (function == null) continue;
                DecompileResults result = decompiler.decompileFunction(function, 45, monitor);
                String filename = Long.toHexString(base + begin) + ".c";
                if (result.decompileCompleted()) {
                    Files.writeString(output.resolve(filename), result.getDecompiledFunction().getC());
                    index.println(fields[0] + "\t" + function.getName() + "\t" + filename + "\tok");
                } else {
                    index.println(fields[0] + "\t" + function.getName() + "\t" + filename + "\t" + result.getErrorMessage());
                }
                println("DECOMPILED " + fields[0] + " " + result.decompileCompleted());
            }
        }
        decompiler.dispose();
    }
}
```
