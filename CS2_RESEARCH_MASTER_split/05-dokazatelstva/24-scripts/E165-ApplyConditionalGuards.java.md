<!-- split-part | CS2_RESEARCH_MASTER.md lines 94459-94501 | body-sha256 f94f6415032e4a7b3cf9601c30de315afc578d854c6aa51c61812bbd82bb79f5 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-165"></a>

## E165. `analysis/scripts/ApplyConditionalGuards.java`

Bytes: 1719. SHA-256: `1b6f11c938a256ae99c6b207d96325bf24e61cb8300cf4659eee2ec2e5ac91a7`.

```java
import ghidra.app.script.GhidraScript;
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import java.nio.file.Files;
import java.nio.file.Path;

public class ApplyConditionalGuards extends GhidraScript {
    public void run() throws Exception {
        if (!currentProgram.getName().equals("CONDITIONAL_STATIC_VIEW_NOT_EXECUTABLE.bin")) {
            throw new IllegalStateException("Only the explicitly conditional analysis project may be modified");
        }
        Path table = Path.of(getScriptArgs()[0]);
        int changed = 0;
        for (String row : Files.readAllLines(table)) {
            if (row.isBlank()) continue;
            String[] fields = row.split("\t");
            Address address = toAddr(0x212c3300000L + Long.decode(fields[0]));
            byte[] replacement = java.util.HexFormat.of().parseHex(fields[1]);
            byte[] previous = getBytes(address, replacement.length);
            if (java.util.Arrays.equals(previous, replacement)) continue;
            if ((previous[0] & 0xff) != 0x74 && !((previous[0] & 0xff) == 0x0f && (previous[1] & 0xff) == 0x84)) {
                throw new IllegalStateException("Expected conditional equality branch at " + address);
            }
            Address last = address.add(replacement.length - 1);
            clearListing(address, last);
            setBytes(address, replacement);
            new DisassembleCommand(address, new AddressSet(address, last), false).applyTo(currentProgram, monitor);
            changed++;
        }
        println("CONDITIONAL VIEW ONLY: applied " + changed + " additional assumed-success branches");
    }
}
```
