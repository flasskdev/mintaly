//Dumps decompiled code of rage-related functions to per-function .txt files.
//Usage: -postScript RageDump <output_dir>
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.*;

import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;

public class RageDump extends GhidraScript {

    // category -> anchor strings (ASCII substrings found in the binary)
    static final String[][] ANCHORS = {
        {"pipeline",
            "create_move: rage begin",
            "create_move: rage end",
            "create_move: rage",
            "rage_targets expects {order={indices}}",
            "rage_scan expects {points={...}}, at most 128 points",
            "rage_select expects {index=N}",
            "rage_fire expects {cancel=boolean}",
            "rage callback must return nil or a single-field result table",
            "rage callbacks exceeded the 16 ms per-command budget",
            "claim_command is only available in pre_rage",
            "change settings in pre_rage, not inside the rage pipeline",
            "change binds before the rage pipeline",
            "RAGEBOT PROBLEM DETECTED:",
            "rage_targets",
            "pre_rage",
            "post_rage",
            "allow_fire",
        },
        {"hitchance",
            "required_hitchance",
            "trigger hitchance",
            "ragebot.hitchance: at most 32 queries per command",
            "at most 32 hitchance queries per command",
            "ragebot.hitchance is only available in rage_select",
            "Hitchance",
        },
        {"nospread",
            "spreadMethod",
            "get_spread",
            "no_spread",
            "weapon_calculate_spread",
            "shadow spread",
            "No Spread",
            "HIDE SHOTS IS DISABLED IN NO SPREAD MODE",
            "SCOPE IS DISABLED IN NO SPREAD MODE",
            "prediction_seed",
        },
        {"autowall_damage",
            "autowall",
            "Autowall",
            "penetrated",
            "penetrate",
            "min_damage",
            "bind min damage",
            "Min Damage##AW",
            "hitgroup",
            "armor",
        },
        {"trace_geometry",
            "trace_ray_entity",
            "trace_hull",
            "trace_ray",
            "trace_filter_set_collision",
            "game_trace_manager",
            "calc_angle",
            "aim_punch",
            "can_shoot",
        },
        {"hitbox_backtrack",
            "multipoints",
            "point must contain hitbox",
            "hitbox is unavailable in this scan",
            "requested_hitbox",
            "get_transforms_for_hitbox_list",
            "max backtrack ticks",
            "player_hurt",
            "weapon_fire",
            "hitboxes",
        },
    };

    // strings marking generic Lua VM internals -> excluded from dump/expansion
    static final String[] LUA_CORE = {
        "stack traceback:",
        "bad array new length",
        "table keys must be strings or array indices",
        "attempt to index",
        "attempt to call a",
        "attempt to perform arithmetic",
        "attempt to concatenate",
        "attempt to compare",
        "cannot use 'goto'",
        "not enough memory",
    };

    static final int MAX_DEPTH = 3;
    static final int MAX_FUNCTIONS = 1500;
    static final int MAX_CALLERS_FOR_EXPANSION = 30;

    static final String[] CATEGORY_ORDER = {
        "pipeline", "hitchance", "nospread", "autowall_damage",
        "trace_geometry", "hitbox_backtrack", "misc"
    };

    DecompInterface decomp;
    Program prog;
    Memory mem;
    ReferenceManager refMgr;
    Path outDir;

    static class Entry {
        Function fn;
        int depth;
        Map<String, Boolean> cats = new LinkedHashMap<>();
        String via = "";
    }

    Map<String, Set<Function>> catSeeds = new LinkedHashMap<>();
    Set<Function> luaCore = new HashSet<>();
    Map<Function, Entry> included = new LinkedHashMap<>();
    Map<Function, Integer> callerCountCache = new HashMap<>();

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        outDir = Paths.get(args.length > 0 ? args[0]
            : "C:\\Users\\stass\\AppData\\Local\\Temp\\rage_dump");
        Files.createDirectories(outDir);

        prog = currentProgram;
        mem = prog.getMemory();
        refMgr = prog.getReferenceManager();

        println("[rage] scanning strings for anchors...");
        markLuaCore();

        for (String[] group : ANCHORS) {
            String cat = group[0];
            Set<Function> set = new LinkedHashSet<>();
            for (int i = 1; i < group.length; i++) {
                for (Function f : findFunctionsReferencingString(group[i])) {
                    if (!luaCore.contains(f)) {
                        set.add(f);
                    }
                }
            }
            catSeeds.put(cat, set);
            println("[rage] seed functions for " + cat + ": " + set.size());
        }

        // BFS expansion from seed functions through direct calls/data refs
        ArrayDeque<Entry> queue = new ArrayDeque<>();
        for (Map.Entry<String, Set<Function>> e : catSeeds.entrySet()) {
            for (Function f : e.getValue()) {
                Entry en = included.get(f);
                if (en == null) {
                    en = new Entry();
                    en.fn = f;
                    en.depth = 0;
                    en.via = "seed:" + e.getKey();
                    included.put(f, en);
                    queue.add(en);
                }
                en.cats.put(e.getKey(), Boolean.TRUE);
            }
        }
        println("[rage] total seeds: " + included.size());

        int processed = 0;
        while (!queue.isEmpty() && included.size() < MAX_FUNCTIONS) {
            Entry cur = queue.poll();
            if (cur.depth >= MAX_DEPTH) {
                continue;
            }
            processed++;
            if (processed % 500 == 0) {
                println("[rage] expanded " + processed + " funcs, included " + included.size());
            }
            Function fn = cur.fn;
            AddressIterator it = fn.getBody().getAddresses(true);
            while (it.hasNext()) {
                if (included.size() >= MAX_FUNCTIONS) {
                    break;
                }
                Address a = it.next();
                Reference[] refs = refMgr.getReferencesFrom(a);
                for (Reference r : refs) {
                    Address tgt = r.getToAddress();
                    if (tgt == null) {
                        continue;
                    }
                    Function g = prog.getFunctionManager().getFunctionAt(tgt);
                    if (g == null) {
                        g = prog.getFunctionManager().getFunctionContaining(tgt);
                    }
                    if (g == null || g == fn || g.isExternal() || g.isThunk()) {
                        continue;
                    }
                    if (luaCore.contains(g)) {
                        continue;
                    }
                    if (!included.containsKey(g)
                            && callerCount(g) > MAX_CALLERS_FOR_EXPANSION) {
                        // shared utility hub -> do not pull in
                        continue;
                    }
                    Entry ex = included.get(g);
                    if (ex == null) {
                        ex = new Entry();
                        ex.fn = g;
                        ex.depth = cur.depth + 1;
                        ex.via = fn.getName() + " @" + a;
                        ex.cats.putAll(cur.cats);
                        included.put(g, ex);
                        queue.add(ex);
                    } else if (ex.depth > cur.depth + 1) {
                        ex.depth = cur.depth + 1;
                    }
                }
            }
        }
        println("[rage] final function set: " + included.size());
        dumpAll();
    }

    void dumpAll() throws Exception {
        decomp = new DecompInterface();
        DecompileOptions opts = new DecompileOptions();
        decomp.setOptions(opts);
        decomp.openProgram(prog);

        Map<String, Integer> counts = new TreeMap<>();
        StringBuilder index = new StringBuilder();
        index.append("ea\tname\tfile\tcategory\tdepth\tvia\tsize_bytes\n");
        List<String> failed = new ArrayList<>();

        int n = 0;
        for (Entry en : included.values()) {
            n++;
            Function fn = en.fn;
            String cat = pickCategory(en);
            counts.merge(cat, 1, Integer::sum);
            String base = String.format("%08X_%s", fn.getEntryPoint().getOffset(),
                sanitize(fn.getName()));
            Path dir = outDir.resolve(cat);
            Files.createDirectories(dir);
            Path file = dir.resolve(base + ".txt");

            StringBuilder sb = new StringBuilder();
            sb.append("// function: ").append(fn.getName()).append("\n");
            sb.append("// ea: ").append(fn.getEntryPoint()).append("\n");
            sb.append("// category: ").append(cat).append("\n");
            sb.append("// depth: ").append(en.depth).append(", via: ").append(en.via).append("\n");
            sb.append("// size: ").append(fn.getBody().getNumAddresses()).append(" bytes\n");
            sb.append("// anchors: ").append(String.join(", ", en.cats.keySet())).append("\n");
            sb.append("// ----------------------------------------------------\n");

            String code = null;
            try {
                DecompileResults res = decomp.decompileFunction(fn, 60, monitor);
                if (res != null && res.decompileCompleted() && res.getDecompiledFunction() != null) {
                    code = res.getDecompiledFunction().getC();
                }
            } catch (Exception e) {
                println("[rage] decompile error at " + fn.getEntryPoint() + ": " + e);
            }
            if (code == null) {
                failed.add(fn.getName() + " @ " + fn.getEntryPoint());
                code = "// DECOMPILE FAILED - disassembly fallback:\n" + disassemble(fn);
            }
            sb.append(code).append("\n");

            Files.write(file, sb.toString().getBytes(StandardCharsets.UTF_8));
            index.append(fn.getEntryPoint()).append('\t')
                 .append(fn.getName()).append('\t')
                 .append(cat).append('/').append(base).append(".txt").append('\t')
                 .append(cat).append('\t')
                 .append(en.depth).append('\t')
                 .append(en.via.replace('\t', ' ')).append('\t')
                 .append(fn.getBody().getNumAddresses()).append('\n');
            if (n % 200 == 0) {
                println("[rage] dumped " + n + "/" + included.size());
            }
        }

        Files.write(outDir.resolve("INDEX.tsv"),
            index.toString().getBytes(StandardCharsets.UTF_8));

        StringBuilder sum = new StringBuilder();
        sum.append("Rage functions dump from skeet payload DLL (embedded in skeet.exe)\n");
        sum.append("program: ").append(prog.getExecutablePath()).append("\n");
        sum.append("total functions: ").append(included.size()).append("\n\nper category:\n");
        for (Map.Entry<String, Integer> e : counts.entrySet()) {
            sum.append("  ").append(e.getKey()).append(": ").append(e.getValue()).append("\n");
        }
        if (!failed.isEmpty()) {
            sum.append("\nfailed decompilations (").append(failed.size()).append("):\n");
            for (String f : failed) {
                sum.append("  ").append(f).append("\n");
            }
        }
        Files.write(outDir.resolve("SUMMARY.txt"),
            sum.toString().getBytes(StandardCharsets.UTF_8));
        println("[rage] done. output: " + outDir);
        decomp.dispose();
    }

    String disassemble(Function fn) {
        StringBuilder sb = new StringBuilder();
        try {
            Listing listing = prog.getListing();
            InstructionIterator it = listing.getInstructions(fn.getBody(), true);
            int n = 0;
            while (it.hasNext() && n < 400) {
                Instruction ins = it.next();
                sb.append(ins.getAddress()).append("  ")
                  .append(ins.toString()).append("\n");
                n++;
            }
        } catch (Exception e) {
            sb.append("// disassembly failed: ").append(e).append("\n");
        }
        return sb.toString();
    }

    int callerCount(Function g) {
        Integer c = callerCountCache.get(g);
        if (c != null) {
            return c;
        }
        int count = 0;
        try {
            for (Reference r : refMgr.getReferencesTo(g.getEntryPoint())) {
                if (r.getReferenceType().isCall() || r.getReferenceType().isJump()) {
                    count++;
                }
            }
        } catch (Exception ignored) {
        }
        callerCountCache.put(g, count);
        return count;
    }

    void markLuaCore() {
        for (String s : LUA_CORE) {
            for (Function f : findFunctionsReferencingString(s)) {
                luaCore.add(f);
            }
        }
        println("[rage] lua-core functions excluded: " + luaCore.size());
    }

    Set<Function> findFunctionsReferencingString(String needle) {
        Set<Function> out = new LinkedHashSet<>();
        byte[] pat = needle.getBytes(java.nio.charset.StandardCharsets.US_ASCII);
        for (MemoryBlock block : mem.getBlocks()) {
            if (!block.isInitialized()) {
                continue;
            }
            byte[] data;
            try {
                data = new byte[(int) block.getSize()];
                block.getBytes(block.getStart(), data);
            } catch (Exception e) {
                continue;
            }
            int from = 0;
            while (true) {
                int idx = indexOf(data, pat, from);
                if (idx < 0) {
                    break;
                }
                Address strAddr = block.getStart().add(idx);
                from = idx + 1;
                collectRefsToString(strAddr, out);
                // registration tables may hold pointers to this string without
                // Ghidra having created explicit references -> raw pointer scan
                pointerScanFor(strAddr, out);
            }
        }
        return out;
    }

    void pointerScanFor(Address strAddr, Set<Function> out) {
        byte[] needle = new byte[8];
        long v = strAddr.getOffset();
        for (int i = 0; i < 8; i++) {
            needle[i] = (byte) ((v >>> (8 * i)) & 0xFF);
        }
        for (MemoryBlock block : mem.getBlocks()) {
            if (!block.isInitialized() || block.getSize() < 8) {
                continue;
            }
            byte[] data;
            try {
                data = new byte[(int) block.getSize()];
                block.getBytes(block.getStart(), data);
            } catch (Exception e) {
                continue;
            }
            int from = 0;
            while (true) {
                int idx = indexOf(data, needle, from);
                if (idx < 0) {
                    break;
                }
                from = idx + 1;
                if ((idx & 7) != 0) {
                    continue; // require 8-byte alignment for table entries
                }
                scanTableAround(block.getStart().add(idx), out);
            }
        }
    }

    void collectRefsToString(Address strAddr, Set<Function> out) {
        // direct code references -> containing function is a seed
        for (Reference r : refMgr.getReferencesTo(strAddr)) {
            Address fr = r.getFromAddress();
            if (fr == null) {
                continue;
            }
            Function f = prog.getFunctionManager().getFunctionContaining(fr);
            if (f != null && !f.isExternal()) {
                out.add(f);
            }
            // data table heuristic: { name_ptr, fn_ptr, ... } near the reference
            if (r.getReferenceType().isData()) {
                scanTableAround(fr, out);
            }
        }
    }

    void scanTableAround(Address refAddr, Set<Function> out) {
        for (int delta = -48; delta <= 48; delta += 8) {
            try {
                Address a = refAddr.add(delta);
                if (!mem.contains(a)) {
                    continue;
                }
                MemoryBlock b = mem.getBlock(a);
                if (b == null || !b.isInitialized()) {
                    continue;
                }
                long v = mem.getLong(a);
                Address va = prog.getAddressFactory().getDefaultAddressSpace().getAddress(v);
                if (va == null) {
                    continue;
                }
                Function f = prog.getFunctionManager().getFunctionAt(va);
                if (f != null && !f.isExternal() && !f.isThunk()) {
                    out.add(f);
                    continue;
                }
                // descriptor blob in .data -> functions that reference it
                for (Reference r : refMgr.getReferencesTo(va)) {
                    Address fr = r.getFromAddress();
                    if (fr == null) {
                        continue;
                    }
                    Function g = prog.getFunctionManager().getFunctionContaining(fr);
                    if (g != null && !g.isExternal()) {
                        out.add(g);
                    }
                }
            } catch (Exception ignored) {
            }
        }
    }

    String pickCategory(Entry en) {
        for (String c : CATEGORY_ORDER) {
            if (en.cats.containsKey(c)) {
                return c;
            }
        }
        return "misc";
    }

    static String sanitize(String name) {
        StringBuilder sb = new StringBuilder();
        for (char c : name.toCharArray()) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                    || (c >= '0' && c <= '9') || c == '_' || c == '-') {
                sb.append(c);
            } else {
                sb.append('_');
            }
        }
        return sb.length() > 0 ? sb.toString() : "func";
    }

    static int indexOf(byte[] haystack, byte[] needle, int from) {
        outer:
        for (int i = Math.max(0, from); i <= haystack.length - needle.length; i++) {
            for (int j = 0; j < needle.length; j++) {
                if (haystack[i + j] != needle[j]) {
                    continue outer;
                }
            }
            return i;
        }
        return -1;
    }
}


