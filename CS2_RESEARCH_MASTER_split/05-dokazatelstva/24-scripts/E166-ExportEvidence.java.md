<!-- split-part | CS2_RESEARCH_MASTER.md lines 94502-94546 | body-sha256 0fad34dd0a62d6e4d861cc8e3c2b7296487642a128fd2e45a7e6c433be1a87b1 -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-166"></a>

## E166. `analysis/scripts/ExportEvidence.java`

Bytes: 1959. SHA-256: `de64f2d1daaa68a5b7bfb52ddd51e75143b47e752bb92fd172d72f3dc1478afd`.

```java
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import java.nio.file.Files;
import java.nio.file.Path;
import java.io.PrintWriter;

public class ExportEvidence extends GhidraScript {
    public void run() throws Exception {
        Path directory = Path.of(getScriptArgs()[0]);
        Files.createDirectories(directory);
        try (PrintWriter index = new PrintWriter(directory.resolve("functions.tsv").toFile())) {
            index.println("entry_va\tname\tminimum_va\tmaximum_va\tinstruction_count");
            FunctionIterator functions = currentProgram.getFunctionManager().getFunctions(true);
            while (functions.hasNext()) {
                Function function = functions.next();
                long count = 0;
                try (PrintWriter output = new PrintWriter(directory.resolve(function.getEntryPoint().toString() + ".asm").toFile())) {
                    output.println(function.getName());
                    InstructionIterator instructions = currentProgram.getListing().getInstructions(function.getBody(), true);
                    while (instructions.hasNext()) {
                        Instruction instruction = instructions.next();
                        StringBuilder bytes = new StringBuilder();
                        for (byte value : instruction.getBytes()) bytes.append(String.format("%02x", value & 255));
                        output.println(instruction.getAddress() + "\t" + bytes + "\t" + instruction.toString());
                        count++;
                    }
                }
                index.println(function.getEntryPoint() + "\t" + function.getName() + "\t" + function.getBody().getMinAddress() + "\t" + function.getBody().getMaxAddress() + "\t" + count);
            }
        }
    }
}
```
