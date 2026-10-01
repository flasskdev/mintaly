<!-- split-part | CS2_RESEARCH_MASTER.md lines 3365-3824 | body-sha256 08dc43574744b337a1a777196e3b81b20a39456f1ec73f1adda0c4f4adccb8ae -->
[← все части](../../README.md) · [категория](00-index.md) · [manifest E001–E184](../00-manifest.md)

<!-- split-body-start -->


<a id="evidence-001"></a>

## E001. `/home/daytona/albigg/analysis/consolidated/assemble_bundle.py`

Bytes: 26652. SHA-256: `cbec30ef828747199ff580c318260ef052f9cb93e37065608c0f6145d8c6e8ad`.

````python
from pathlib import Path
import hashlib
import json
import os
import re

ROOT = Path('analysis')
OUTPUT = ROOT / 'consolidated'
STAGING = OUTPUT / 'staging'
MATH = STAGING / 'math'
BASE = 0x212C3300000
ORIGINAL_HASH = '3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27'
EXCLUDED_DIRECTORIES = {'tools', 'venv', 'ghidra_project', 'ghidra_conditional', 'model', 'consolidated', '__pycache__'}
OMITTED_DATA = {
    'analysis/results/runtime_functions.json': 'Механический полный unwind-index, 45065 записей; не отдельный восстановленный исходник.',
    'analysis/phase2/lagcomp/instructions.json': 'Машинный instruction-index; все сохранённые ASM-листинги включены в CPP.',
    'analysis/phase2/main/environment_guards.json': 'Промежуточные guards дублируются итоговым conditional_model_manifest.json, включённым полностью.',
    'analysis/phase2/main/conditional_model_manifest_v1.json': 'Историческая промежуточная модель v1; итоговая v2 с полным журналом включена.',
}
REPORT_ORDER = [
    'analysis/REPORT_RU.md',
    'analysis/review_rng/summary.md',
    'analysis/review_rng/addendum_vector_sampling.md',
    'analysis/CORE_ADDENDUM_RU.md',
    'analysis/review_rng/shortlist.md',
    'analysis/strings/summary.md',
    'analysis/REPRODUCE.md',
    'analysis/phase2/REPORT_RU.md',
    'analysis/phase2/spread/summary.md',
    'analysis/phase2/main/PENETRATION_AND_MELEE_RU.md',
    'analysis/phase2/lagcomp/summary.md',
    'analysis/phase2/reconstructed/README.md',
    'analysis/phase2/REPRODUCE.md',
]
ROLES = {
    0x4ff1690: 'Init/DllMain-like wrapper; reason==1 calls two helpers',
    0x50fb10: 'Apply/finalize selected aim/fire command',
    0x505f50: 'Caller with 0x30 candidate selection before finalization',
    0x511d50: 'Snapshot deep-copy helper; preserve B6/B7/B8 flags',
    0x3375a0: 'Cached temporal angular correction predictor',
    0x309500: 'Tick/fraction selection from separate ring32',
    0x52b250: 'Select up to two records from ring16',
    0x528b90: 'Melee/knifebot-like damage and mode evaluator',
    0x52ad40: 'Append melee candidate, stride 0x30',
    0x512710: 'Initialize two deterministic Vec2[64] tables',
    0x5239d0: 'Prepare bullet evaluation and scaled sample caches',
    0x525830: 'Full 64-direction four-component aggregation',
    0x526290: 'Fast 8x8 aggregate with extrapolating early-fill',
    0x51cf40: 'Prepare candidate direction/basis/sample frame',
    0x51d320: 'Sample worker and scalar-score dispatch',
    0x51dac0: 'Scalar bullet score evaluation and trace orchestration',
    0x51df10: 'Direction/hit-score gate; incomplete semantic typing',
    0x2f19c0: 'Local prepared hit-volume geometric test',
    0x2f2110: 'External trace workspace orchestration',
    0x2f0dd0: 'Penetration segment traversal with missing external core',
    0x1463a0: 'External ray/hull trace adapter',
    0x51c9c0: 'Sampling-based point coverage/optimization',
    0x51bea0: 'Candidate comparator',
    0x524a30: 'Candidate selection and point optimization',
    0x5248e0: 'Candidate score and fast-aggregate preparation',
    0x51e4f0: 'Health/minimum-score context and candidate generation',
    0x7bdb90: 'Category multiplier with external scalar settings',
    0x4721d0: 'Ring16 capture producer, aging and continuity checks',
    0x473490: 'Runtime history-window scalar source',
    0x6ac690: 'History orchestration, move and worker dispatch',
    0x661010: 'Additional history producer caller',
    0x475830: 'Record-prefix and owned-buffer copy',
    0x474d20: 'Derived scratch record; not ring insertion',
    0x475ef0: 'History producer worker',
    0x471d70: 'History reset after scratch/cache cleanup',
    0x51b480: 'History-based angular correction; not proven resolver',
    0x515ca0: 'Angular processing including random correction; broad role unresolved',
    0xc421c0: 'FreeType SDF property setter, not weapon spread',
    0xc422b0: 'FreeType SDF property getter, not weapon spread',
    0x2e9b50: 'Spatial visual effect endpoints/color/lifetime',
    0x422970: 'UI interval selection and random value',
    0x406c10: 'UI value randomization of decimal suffix',
    0x71ebe0: 'UI transition/reset with small random offsets',
    0x54bc60: 'Shot-result diagnostics and additional state writes',
    0x4fd710: 'String selector; significant known decompiler inaccuracies',
}
HEADER_APPENDIX = r'''
#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cs2_reconstruction::evidence {

inline constexpr std::uint64_t image_base = 0x212C3300000ULL;
inline constexpr std::uint64_t image_size = 0x5001000ULL;
inline constexpr char original_sha256[] = "3224604483481c57a18ccfb20448a5e634a9583421abfac3a2712bba5cc77f27";
inline constexpr char conditional_sha256[] = "bd4b358c86a23027dc72de102790018803d1dbe9aabfcbe789196f826d6f1f39";
inline constexpr std::size_t conditional_branch_assumptions = 3089;
inline constexpr bool binary_behavioral_equivalence_proven = false;
inline constexpr bool full_resolver_recovered = false;
inline constexpr bool external_penetration_core_available = false;

struct ObservedMeleeCandidate30 {
    std::array<float, 3> point;
    std::int32_t score;
    std::uint8_t lethal;
    std::uint8_t attack_mode;
    std::array<std::byte, 6> unknown_12;
    std::uint64_t source_address;
    std::uint64_t target_address;
    std::uint64_t record_address;
};

struct ObservedBulletCandidate58 {
    std::array<float, 3> point;
    float score;
    std::uint64_t target_address;
    std::uint64_t source_address;
    std::uint64_t geometry_address;
    std::uint64_t record_address;
    std::array<std::byte, 4> unknown_30;
    std::array<float, 4> preliminary_aggregates;
    std::array<float, 4> final_aggregates;
    std::array<std::byte, 4> unknown_54;
};

struct HistoryLayout {
    static constexpr std::size_t capacity = 16;
    static constexpr std::size_t slots_offset = 0xf0;
    static constexpr std::size_t slot_stride = 0x500;
    static constexpr std::size_t count_offset = 0x50f0;
    static constexpr std::size_t head_offset = 0x50f4;
    static constexpr std::size_t tick_offset = 0;
    static constexpr std::size_t fraction_offset = 4;
    static constexpr std::size_t position_offset = 0x14;
    static constexpr std::size_t angle_pair_offset = 0x44;
};

struct SnapshotLayout {
    static constexpr std::size_t scalar_prefix_end = 0x5c;
    static constexpr std::size_t first_vector_offset = 0x60;
    static constexpr std::size_t first_element_stride = 0x48;
    static constexpr std::size_t second_vector_offset = 0x78;
    static constexpr std::size_t second_element_stride = 0xac;
    static constexpr std::size_t tail_begin = 0x90;
    static constexpr std::size_t tail_end = 0xba;
    static constexpr std::array<std::size_t, 3> retained_flag_offsets{0xb6, 0xb7, 0xb8};
};

struct ArchivedListing {
    const char* source_path;
    const char* source_sha256;
    const char* format;
    const char* basis;
    const char* analytical_role;
    std::uint64_t rva;
    std::uint64_t va;
    std::uint64_t source_bytes;
    std::uint64_t cpp_body_byte_offset;
    std::uint64_t cpp_first_line;
    std::uint64_t cpp_last_line;
    bool has_entry_address;
};

struct MissingDependency {
    std::uint64_t observed_va;
    const char* role;
};

const ArchivedListing* archived_listings() noexcept;
std::size_t archived_listing_count() noexcept;
const MissingDependency* missing_dependencies() noexcept;
std::size_t missing_dependency_count() noexcept;

static_assert(std::is_standard_layout_v<ObservedMeleeCandidate30>);
static_assert(sizeof(ObservedMeleeCandidate30) == 0x30);
static_assert(offsetof(ObservedMeleeCandidate30, score) == 0x0c);
static_assert(offsetof(ObservedMeleeCandidate30, lethal) == 0x10);
static_assert(offsetof(ObservedMeleeCandidate30, attack_mode) == 0x11);
static_assert(offsetof(ObservedMeleeCandidate30, source_address) == 0x18);
static_assert(std::is_standard_layout_v<ObservedBulletCandidate58>);
static_assert(sizeof(ObservedBulletCandidate58) == 0x58);
static_assert(offsetof(ObservedBulletCandidate58, target_address) == 0x10);
static_assert(offsetof(ObservedBulletCandidate58, source_address) == 0x18);
static_assert(offsetof(ObservedBulletCandidate58, preliminary_aggregates) == 0x34);
static_assert(offsetof(ObservedBulletCandidate58, final_aggregates) == 0x44);

}
'''


def digest(data):
    return hashlib.sha256(data).hexdigest()


def discovered_files():
    paths = []
    for directory, subdirectories, names in os.walk(ROOT):
        subdirectories[:] = sorted(name for name in subdirectories if name not in EXCLUDED_DIRECTORIES)
        paths.extend(Path(directory) / name for name in sorted(names))
    return sorted(paths)


def entry_address(path):
    match = re.search(r'(?:^|_)([0-9a-fA-F]{8,12})(?:\.|$)', path.name)
    if not match:
        return None
    address = int(match.group(1), 16)
    return address - BASE if address >= BASE else address


def basis_for(path):
    if '/conditional_' in str(path):
        return 'conditional success-path model; runtime predicates not verified'
    if '/ghidra_assembly/' in str(path) or '/decompiled_' in str(path):
        return 'original-image Ghidra output; types and CFG may be incomplete'
    return 'sidecar evidence; preserve the source-specific decoding assumptions'


def fenced_blocks(path):
    lines = path.read_text().splitlines(keepends=True)
    blocks = []
    current = None
    for line_number, line in enumerate(lines, 1):
        if current is None:
            match = re.match(r'^(`{3,}|~{3,})([^\r\n]*)\r?\n?$', line)
            if match:
                current = {'marker': match.group(1), 'language': match.group(2).strip(), 'first_line': line_number + 1, 'lines': []}
        elif re.match(r'^' + re.escape(current['marker'][0]) + '{' + str(len(current['marker'])) + r',}\s*$', line):
            current['body'] = ''.join(current.pop('lines')).encode()
            current['source_path'] = str(path) + ':L' + str(current['first_line'])
            blocks.append(current)
            current = None
        else:
            current['lines'].append(line)
    if current is not None:
        raise ValueError('Unterminated Markdown code block in ' + str(path))
    return blocks


class Writer:
    def __init__(self):
        self.chunks = []
        self.byte_count = 0
        self.line = 1

    def add(self, value):
        data = value if isinstance(value, bytes) else value.encode()
        self.chunks.append(data)
        self.byte_count += len(data)
        self.line += data.count(b'\n')

    def value(self):
        return b''.join(self.chunks)


def archive(writer, data, source_path, format_name, basis, address, raw_string=False):
    ordinal = len(listings) + 1
    writer.add('\n#if 0\n')
    writer.add(f'#define CS2_ARCHIVE_SOURCE_{ordinal:04d} {json.dumps(source_path)}\n')
    writer.add(f'#define CS2_ARCHIVE_SHA256_{ordinal:04d} "{digest(data)}"\n')
    if raw_string:
        delimiter = f'CS2A{ordinal:06d}'
        assert (')' + delimiter + '"').encode() not in data
        writer.add(f'static constexpr char archived_text_{ordinal:04d}[] = R"{delimiter}(')
    else:
        writer.add(f'#line 1 {json.dumps(source_path)}\n')
    offset = writer.byte_count
    first_line = writer.line
    writer.add(data)
    last_line = writer.line - int(data.endswith(b'\n'))
    if raw_string:
        writer.add(f'){delimiter}";\n')
    elif not data.endswith(b'\n'):
        writer.add('\n')
    writer.add('#endif\n')
    listings.append({
        'ordinal': ordinal, 'source_path': source_path, 'source_sha256': digest(data),
        'format': format_name, 'basis': basis,
        'analytical_role': ROLES.get(address, 'Not separately attributed; preserve source evidence'),
        'rva': address, 'va': BASE + address if address is not None else None,
        'source_bytes': len(data), 'cpp_body_byte_offset': offset,
        'cpp_first_line': first_line, 'cpp_last_line': max(first_line, last_line),
    })


def cstring(value):
    return json.dumps(value, ensure_ascii=True)


def table_cell(value):
    return str(value).replace('|', '\\|').replace('\n', ' ')


def fenced(data, language):
    text = data.decode()
    longest = max((len(match.group()) for match in re.finditer(r'`+', text)), default=0)
    fence = '`' * max(3, longest + 1)
    return (fence + language + '\n').encode() + data + (b'' if data.endswith(b'\n') else b'\n') + (fence + '\n').encode()


OUTPUT.mkdir(parents=True, exist_ok=True)
source_files = discovered_files()
reports = [Path(name) for name in REPORT_ORDER]
assert set(reports) == {path for path in source_files if path.suffix == '.md'}
c_sources = [path for path in source_files if path.suffix == '.c']
assembly_sources = [path for path in source_files if path.suffix == '.asm']
assert len(reports) == 13 and len(c_sources) == 63 and len(assembly_sources) == 125
assert digest((ROOT / 'input/cs2_212C3300000.bin').read_bytes()) == ORIGINAL_HASH
for name in ['CS2_RECONSTRUCTION.h', 'model_impl.cpp', 'model_checks.cpp', 'check_results.json', 'notes.md']:
    assert (MATH / name).exists(), 'Missing worker file: ' + name
header = (MATH / 'CS2_RECONSTRUCTION.h').read_bytes() + HEADER_APPENDIX.encode()
(OUTPUT / 'CS2_RECONSTRUCTION.h').write_bytes(header)
cpp = Writer()
cpp.add((MATH / 'model_impl.cpp').read_bytes())
cpp.add('\n#ifdef CS2_RECONSTRUCTION_SELF_TEST\n')
cpp.add((MATH / 'model_checks.cpp').read_bytes())
cpp.add('\n#endif\n')
listings = []
for path in c_sources:
    archive(cpp, path.read_bytes(), str(path), 'decompiler_pseudocode_c', basis_for(path), entry_address(path))
for path in assembly_sources:
    archive(cpp, path.read_bytes(), str(path), 'assembly_text', basis_for(path), entry_address(path), True)
for report in reports:
    for fragment in fenced_blocks(report):
        archive(cpp, fragment['body'], fragment['source_path'], 'report_fragment:' + fragment['language'],
                'Verbatim report fragment, not an independently compiled implementation', None, True)
python_reference = ROOT / 'phase2/reconstructed/recovered_math.py'
archive(cpp, python_reference.read_bytes(), str(python_reference), 'python_reference_model',
        'Original independent mathematical model, ported at the start of this CPP', None, True)
cpp.add('\nnamespace cs2_reconstruction::evidence {\n\nnamespace {\n\n')
cpp.add('constexpr ArchivedListing listing_data[] = {\n')
for listing in listings:
    strings = [listing[key] for key in ['source_path', 'source_sha256', 'format', 'basis', 'analytical_role']]
    numbers = [listing['rva'] or 0, listing['va'] or 0, listing['source_bytes'], listing['cpp_body_byte_offset'], listing['cpp_first_line'], listing['cpp_last_line']]
    cpp.add('    {' + ', '.join([cstring(value) for value in strings] + [str(value) + 'ULL' for value in numbers] + ['true' if listing['rva'] is not None else 'false']) + '},\n')
cpp.add('};\n\nconstexpr MissingDependency dependency_data[] = {\n')
for address, role in [(0x7ffcef4a8db0, 'External penetration-segment handler'), (0x7ffcef4a3e50, 'External trace workspace builder'), (0x7ffcef48e030, 'External changed-list processing'), (0x7ffcf0fe45a0, 'External trace object/interface'), (0x7ffcef616040, 'External ray/hull implementation')]:
    cpp.add(f'    {{{hex(address)}ULL, {cstring(role)}}},\n')
cpp.add('};\n\n}\n\n')
cpp.add('const ArchivedListing* archived_listings() noexcept { return listing_data; }\n')
cpp.add('std::size_t archived_listing_count() noexcept { return sizeof(listing_data) / sizeof(listing_data[0]); }\n')
cpp.add('const MissingDependency* missing_dependencies() noexcept { return dependency_data; }\n')
cpp.add('std::size_t missing_dependency_count() noexcept { return sizeof(dependency_data) / sizeof(dependency_data[0]); }\n\n}\n')
cpp_bytes = cpp.value()
for listing in listings:
    begin = listing['cpp_body_byte_offset']
    content = cpp_bytes[begin:begin + listing['source_bytes']]
    assert digest(content) == listing['source_sha256'], listing['source_path']
(OUTPUT / 'CS2_RECONSTRUCTION.cpp').write_bytes(cpp_bytes)
(OUTPUT / 'archive_index.json').write_text(json.dumps(listings, indent=2, ensure_ascii=False) + '\n')

md = Writer()
md.add(STAGING.joinpath('overview.md').read_bytes())
md.add('\n\n## 14. Автоматический паспорт объединения\n\n')
md.add(f'- Исходных полных отчётов: **{len(reports)}**.\n- C-псевдокодов: **{len(c_sources)}**, уникальных start-address имён: **{len(set(path.stem for path in c_sources))}**.\n- ASM-листингов: **{len(assembly_sources)}**.\n- Всего архивных блоков CPP, включая report fragments и исходную Python-модель: **{len(listings)}**.\n')
md.add(f'- `CS2_RECONSTRUCTION.cpp`: {len(cpp_bytes):,} bytes, SHA-256 `{digest(cpp_bytes)}`.\n- `CS2_RECONSTRUCTION.h`: {len(header):,} bytes, SHA-256 `{digest(header)}`.\n'.replace(',', ' '))
md.add('- Каждый CPP archive body повторно вырезан по byte offset и проверен против SHA-256 исходного текста.\n')
md.add('\n### Сборка и тест математической модели\n\n')
md.add('```sh\ng++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -c CS2_RECONSTRUCTION.cpp\ng++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -DCS2_RECONSTRUCTION_SELF_TEST CS2_RECONSTRUCTION.cpp -o reconstruction_checks\n./reconstruction_checks\n```\n\n')
md.add('Ни одна из этих команд не загружает исходный dump. Проверяется новая независимая C++-модель, не весь compiler-disabled псевдокод.\n\n')
md.add(fenced(MATH.joinpath('check_results.json').read_bytes(), 'json'))
md.add('\n')
md.add(MATH.joinpath('notes.md').read_bytes())
final_checks = OUTPUT / 'final_checks.json'
if final_checks.exists():
    md.add('\n\n### Проверка именно итогового большого CPP/H\n\n')
    md.add(fenced(final_checks.read_bytes(), 'json'))

md.add('\n\n## 15. Навигация по исходным отчётам\n\n')
md.add('| ID | Источник | Этап / статус | Bytes | SHA-256 |\n|---|---|---|---:|---|\n')
for number, path in enumerate(reports, 1):
    phase = 'Этап 2' if '/phase2/' in str(path) else 'Исторический этап 1; см. актуальные уточнения'
    md.add(f'| [D{number:02d}](#doc-{number:02d}) | `{path}` | {phase} | {path.stat().st_size} | `{digest(path.read_bytes())}` |\n')
md.add('\n## 16. Каталог всех C-псевдокодов\n\n')
md.add('Указанный номер строки относится к окончательному `CS2_RECONSTRUCTION.cpp`; все адреса VA/RVA сохранены. Листинг conditional не подменяет исходный original.\n\n')
md.add('| № | RVA | VA | Роль | Источник | Строка CPP | Bytes |\n|---:|---|---|---|---|---:|---:|\n')
for listing in listings:
    if listing['format'] != 'decompiler_pseudocode_c':
        continue
    md.add(f'| {listing["ordinal"]} | `{listing["rva"]:#x}` | `{listing["va"]:#x}` | {table_cell(listing["analytical_role"])} | `{listing["source_path"]}` | {listing["cpp_first_line"]} | {listing["source_bytes"]} |\n')
md.add('\n## 17. Каталог ASM и дополнительных архивных блоков\n\n')
md.add('| № | Формат | Исходный материал | Строка CPP | Bytes | SHA-256 |\n|---:|---|---|---:|---:|---|\n')
for listing in listings:
    if listing['format'] == 'decompiler_pseudocode_c':
        continue
    md.add(f'| {listing["ordinal"]} | {table_cell(listing["format"])} | `{listing["source_path"]}` | {listing["cpp_first_line"]} | {listing["source_bytes"]} | `{listing["source_sha256"]}` |\n')

embedded_records = []
for number, path in enumerate(reports, 1):
    data = path.read_bytes()
    md.add(f'\n\n---\n\n<a id="doc-{number:02d}"></a>\n\n# Приложение D{number:02d}. `{path}`\n\n')
    md.add('**Дословный исходный отчёт.** Даты/статусы/неизвестные роли внутри относятся к своему этапу. При расхождении использовать актуальный свод и поздние доказательства. Пути внутри текста — исторические: соответствующие материалы встроены в MD/CPP и находятся через поиск исходного пути.\n\n')
    begin = md.byte_count
    md.add(data)
    embedded_records.append({'source_path': str(path), 'format': 'report', 'sha256': digest(data), 'byte_offset': begin, 'bytes': len(data)})

support_extensions = {'.json', '.csv', '.tsv', '.txt', '.log', '.py', '.java'}
support = [path for path in source_files if path.suffix in support_extensions and str(path) not in OMITTED_DATA]
support += [Path(__file__), MATH / 'model_impl.cpp', MATH / 'CS2_RECONSTRUCTION.h', MATH / 'model_checks.cpp']
support = sorted(set(support))
md.add('\n\n---\n\n# Приложения E. Машинные свидетельства и воспроизводимые скрипты\n\n')
md.add('JSON/CSV/TSV/логи ниже — исходные результаты конкретных проходов, не независимое доказательство всех выводов. Старые success counts и negative searches могут быть уточнены следующим этапом. Скрипты приведены как текст; неизвестный sample ими здесь не исполняется.\n\n')
md.add('| ID | Источник | Bytes | SHA-256 |\n|---|---|---:|---|\n')
for number, path in enumerate(support, 1):
    data = path.read_bytes()
    md.add(f'| [E{number:03d}](#evidence-{number:03d}) | `{path}` | {len(data)} | `{digest(data)}` |\n')
for number, path in enumerate(support, 1):
    data = path.read_bytes()
    language = {'.py': 'python', '.java': 'java', '.cpp': 'cpp', '.h': 'cpp', '.json': 'json', '.csv': 'csv', '.tsv': 'tsv'}.get(path.suffix, 'text')
    md.add(f'\n\n<a id="evidence-{number:03d}"></a>\n\n## E{number:03d}. `{path}`\n\nBytes: {len(data)}. SHA-256: `{digest(data)}`.\n\n')
    block = fenced(data, language)
    prefix_length = block.find(b'\n') + 1
    begin = md.byte_count + prefix_length
    md.add(block)
    embedded_records.append({'source_path': str(path), 'format': 'evidence_or_script', 'sha256': digest(data), 'byte_offset': begin, 'bytes': len(data)})
md.add('\n\n# Приложение F. Крупные индексы и бинарные материалы вне текстового объединения\n\n')
md.add('Следующие файлы не выдаются за восстановленные функции и не раздувают MD повторяющимися индексами. Их существование и hash зафиксированы. Все человекочитаемые отчёты и C/ASM-проекции при этом включены.\n\n')
md.add('| Файл | Bytes | SHA-256 | Причина |\n|---|---:|---|---|\n')
omitted_records = []
for name, reason in OMITTED_DATA.items():
    path = Path(name)
    data = path.read_bytes()
    record = {'source_path': name, 'bytes': len(data), 'sha256': digest(data), 'reason': reason}
    omitted_records.append(record)
    md.add(f'| `{path}` | {len(data)} | `{record["sha256"]}` | {reason} |\n')
for name, reason in [('analysis/input/cs2_212C3300000.bin', 'Оригинальный 80 MiB memory image, не исходный код.'), ('analysis/results/code.sqlite', 'Механический индекс ссылок; SQL-база не заменяет восстановленные функции.')]:
    path = Path(name)
    data = path.read_bytes()
    record = {'source_path': name, 'bytes': len(data), 'sha256': digest(data), 'reason': reason}
    omitted_records.append(record)
    md.add(f'| `{path}` | {len(data)} | `{record["sha256"]}` | {reason} |\n')
md.add('\nGhidra projects и дистрибутивы инструментов остаются отдельными двоичными материалами ранее выданных архивов. Они не превращены в фиктивный C++.\n\n')
md.add('# Итог\n\nОбъединение завершает упаковку всех выполненных исследований и сохранённых восстановлений, но не закрывает неизвестные алгоритмы. Полнота переноса проверена отдельно от корректности математической модели и от недоказанной эквивалентности исходной программе.\n')
md_bytes = md.value()
for record in embedded_records:
    begin = record['byte_offset']
    assert digest(md_bytes[begin:begin + record['bytes']]) == record['sha256'], record['source_path']
(OUTPUT / 'CS2_RESEARCH_MASTER.md').write_bytes(md_bytes)
summary = {
    'reports_included_verbatim': len(reports), 'c_pseudocode_listings_included_verbatim': len(c_sources),
    'unique_c_listing_address_names': len(set(path.stem for path in c_sources)),
    'asm_listings_included_verbatim': len(assembly_sources), 'cpp_archive_blocks': len(listings),
    'support_files_embedded_verbatim': len(support), 'byte_verified_md_bodies': len(embedded_records),
    'original_bin_sha256': ORIGINAL_HASH, 'raw_sample_executed': False,
    'all_historical_recovered_listings_preserved': True,
    'native_model_is_not_complete_game_dll': True,
    'artifacts': [{ 'path': str(OUTPUT / name), 'bytes': len(data), 'lines': data.count(b'\n'), 'sha256': digest(data)}
                  for name, data in [('CS2_RESEARCH_MASTER.md', md_bytes), ('CS2_RECONSTRUCTION.cpp', cpp_bytes), ('CS2_RECONSTRUCTION.h', header)]],
    'omitted_mechanical_or_binary_files': omitted_records,
}
(OUTPUT / 'md_embedding_index.json').write_text(json.dumps(embedded_records, ensure_ascii=False, indent=2) + '\n')
(OUTPUT / 'bundle_manifest.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2) + '\n')
print(json.dumps({key:value for key,value in summary.items() if key != 'omitted_mechanical_or_binary_files'}, ensure_ascii=False, indent=2))
````
