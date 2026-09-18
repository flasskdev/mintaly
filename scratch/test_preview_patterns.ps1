$code = @"
using System;
using System.IO;
using System.Text;

public class CheckCallers {
    public static void Run() {
        string rsPath = @"C:\Program Files (x86)\Steam\steamapps\common\Counter-Strike Global Offensive\game\bin\win64\rendersystemdx11.dll";
        byte[] b = File.ReadAllBytes(rsPath);

        int rva1 = 0x18AA0;
        int rva2 = 0x18FE0;
        int textRaw = 0x400;
        int textVirt = 0x1000;

        Console.WriteLine("=== Callers of Func 1 (RVA 0x18AA0) ===");
        for (int i = textRaw; i < textRaw + 0x1E1800 - 5; i++) {
            if (b[i] == 0xE8) {
                int disp = BitConverter.ToInt32(b, i + 1);
                int instrRva = textVirt + (i - textRaw);
                int targetRva = instrRva + 5 + disp;
                if (targetRva == rva1) {
                    // Print 32 bytes before the call for context
                    int contextStart = Math.Max(textRaw, i - 32);
                    StringBuilder sb = new StringBuilder();
                    for (int j = contextStart; j < i + 5; j++) {
                        sb.Append(b[j].ToString("X2") + " ");
                    }
                    Console.WriteLine("  Call at RVA 0x{0:X}: ...{1}", instrRva, sb.ToString());
                }
            }
        }

        Console.WriteLine("\n=== Callers of Func 2 (RVA 0x18FE0) ===");
        for (int i = textRaw; i < textRaw + 0x1E1800 - 5; i++) {
            if (b[i] == 0xE8) {
                int disp = BitConverter.ToInt32(b, i + 1);
                int instrRva = textVirt + (i - textRaw);
                int targetRva = instrRva + 5 + disp;
                if (targetRva == rva2) {
                    int contextStart = Math.Max(textRaw, i - 32);
                    StringBuilder sb = new StringBuilder();
                    for (int j = contextStart; j < i + 5; j++) {
                        sb.Append(b[j].ToString("X2") + " ");
                    }
                    Console.WriteLine("  Call at RVA 0x{0:X}: ...{1}", instrRva, sb.ToString());
                }
            }
        }
    }
}
"@
Add-Type -TypeDefinition $code
[CheckCallers]::Run()
