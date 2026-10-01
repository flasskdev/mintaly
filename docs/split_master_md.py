#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Разбивка CS2_RESEARCH_MASTER.md на мелкие файлы (темы, псевдокод, архивные блоки).

Результат — папка docs/CS2_RESEARCH_MASTER_split:

  01-obzor/           вводная часть (как читать + разделы 1..14)
  02-cpp-model/       документация порта CS2_RECONSTRUCTION.cpp/.h
  03-katalogi/        разделы 15..17 (навигация, каталог C-псевдокодов, каталог ASM)
    pseudocode-c/     карточка на каждый C-псевдокод (63)
    asm/              карточка на каждый ASM/report/архивный блок (218)
  04-otchety/         приложения D01..D13 — дословные исходные отчёты
  05-dokazatelstva/   приложения E: manifest + E001..E184 по категориям
  06-kod/             вырезанные code-блоки обзора/порта/отчётов (копии)
  07-prilozhenie-f.md приложение F (крупные индексы вне текста)

Каждая основная часть пишется так:

  <!-- split-part | ... -->
  [навигационная строка](...)
  <!-- split-body-start -->
  <дословные строки исходника>

Склейка тел частей в порядке разделов даёт исходный файл байт-в-байт; проверка
пишется в _verify.md, карта строк — в _source-map.tsv.

Запуск:  python docs/split_master_md.py
"""

from __future__ import annotations

import hashlib
import re
import shutil
import sys
from collections import OrderedDict
from pathlib import Path

HERE = Path(__file__).resolve().parent  # .../project/docs
SRC = HERE.parent / "CS2_RESEARCH_MASTER.md"
OUT = HERE / "CS2_RESEARCH_MASTER_split"

BODY_MARKER = "<!-- split-body-start -->"
FENCE_RE = re.compile(r"^(`{3,})(.*)$")
HEAD_RE = re.compile(r"^(#{1,4}) (.*)$")

INTRO_SLUGS = {
    1: "01-kratkiy-rezultat",
    2: "02-vhodnye-dannye-i-adresaciya",
    3: "03-metod-i-urovni-dostovernosti",
    4: "04-aim-fire-primenenie-resheniya",
    5: "05-hitchance-i-generatory-napravleniy",
    6: "06-health-budget-minimum-damage-tochka",
    7: "07-damage-penetration-i-vneshniy-kod",
    8: "08-istoriya-i-lag-logika",
    9: "09-melee-knifebot",
    10: "10-resolver-i-lozhnye-sovpadeniya",
    11: "11-ustroystvo-cpp-i-h",
    12: "12-kontrol-polnoty-i-vosproizvodimosti",
    13: "13-chego-ne-poyavilos-ot-obedineniya",
    14: "14-avtomaticheskiy-pasport-obedineniya",
}

CPP_SLUGS = {
    "C++17 mathematical reconstruction port": "00-port-i-delivery-and-scope",
    "API and preserved semantics": "01-api-and-preserved-semantics",
    "Validation results": "02-validation-results",
    "Reproduction commands": "03-reproduction-commands",
    "Main assembly contract": "04-main-assembly-contract",
    "Port limitations": "05-port-limitations",
    "Проверка именно итогового большого CPP/H": "06-proverka-itogovogo-cpp-h",
}

CATALOG_SLUGS = {
    15: "15-navigaciya-po-otchetam",
    16: "16-katalog-c-psevdokodov",
    17: "17-katalog-asm-i-arhivnyh-blokov",
}

TOPIC_TABLE = [
    ("Как читать свод", "01-obzor/00-kak-chitat.md"),
    ("Краткий актуальный результат", "01-obzor/01-kratkiy-rezultat.md"),
    ("Входные данные и адресация", "01-obzor/02-vhodnye-dannye-i-adresaciya.md"),
    ("Метод и уровни достоверности", "01-obzor/03-metod-i-urovni-dostovernosti.md"),
    ("Aim/fire — применение решения", "01-obzor/04-aim-fire-primenenie-resheniya.md"),
    ("Hitchance и генераторы направлений", "01-obzor/05-hitchance-i-generatory-napravleniy.md"),
    ("Health budget, minimum damage, выбор точки", "01-obzor/06-health-budget-minimum-damage-tochka.md"),
    ("Damage/penetration и отсутствующий внешний код", "01-obzor/07-damage-penetration-i-vneshniy-kod.md"),
    ("История и lag-логика", "01-obzor/08-istoriya-i-lag-logika.md"),
    ("Melee/knifebot", "01-obzor/09-melee-knifebot.md"),
    ("Resolver и ложные совпадения", "01-obzor/10-resolver-i-lozhnye-sovpadeniya.md"),
    ("Устройство CPP/H и `#if 0`", "01-obzor/11-ustroystvo-cpp-i-h.md"),
    ("Контроль полноты и воспроизводимости", "01-obzor/12-kontrol-polnoty-i-vosproizvodimosti.md"),
    ("Что не появилось от объединения", "01-obzor/13-chego-ne-poyavilos-ot-obedineniya.md"),
    ("Автопаспорт объединения", "01-obzor/14-avtomaticheskiy-pasport-obedineniya.md"),
    ("Порт C++17-математики", "02-cpp-model/00-port-i-delivery-and-scope.md"),
    ("Навигация по отчётам D01–D13", "03-katalogi/15-navigaciya-po-otchetam.md"),
    ("Каталог 63 C-псевдокодов", "03-katalogi/16-katalog-c-psevdokodov.md"),
    ("Каталог ASM/архивных блоков № 64–218", "03-katalogi/17-katalog-asm-i-arhivnyh-blokov.md"),
    ("Отчёты D01–D13 (дословно)", "04-otchety/00-index.md"),
    ("Машинные свидетельства E001–E184", "05-dokazatelstva/00-index.md"),
    ("Code-блоки обзора/порта/отчётов", "06-kod/00-index.md"),
    ("Приложение F", "07-prilozhenie-f.md"),
]


def sha256_text(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def safe_name(name: str) -> str:
    name = re.sub(r"[^0-9A-Za-z._-]+", "-", name).strip("-._ ")
    name = re.sub(r"-{2,}", "-", name)
    return name or "item"


def slug(text: str) -> str:
    return safe_name(text.replace("/", "-").replace("_", "-")).lower()


# --------------------------------------------------------------------------
# разбор структуры
# --------------------------------------------------------------------------
def iter_headings(lines):
    fence = 0
    for i, raw in enumerate(lines):
        s = raw.rstrip("\r\n")
        m = FENCE_RE.match(s)
        if fence:
            if m and len(m.group(1)) >= fence and m.group(2).strip() == "":
                fence = 0
            continue
        if m and "`" not in m.group(2):
            fence = len(m.group(1))
            continue
        h = HEAD_RE.match(s)
        if h:
            yield i, len(h.group(1)), h.group(2).strip()
    if fence:
        raise SystemExit("несбалансированные code-fence в исходнике")


def collect_anchors(headings):
    anchors = []
    for i, lvl, title in headings:
        if lvl == 1 and re.match(r"^Приложени[ея] [DEF]", title):
            anchors.append((i, "appendix", title))
            continue
        if lvl == 2 and re.match(r"^E\d{3}\.\s", title):
            anchors.append((i, "evidence", title))
            continue
        if lvl == 2 and i < 880 and (re.match(r"^\d{1,2}\.\s", title) or title == "Как читать этот документ"):
            anchors.append((i, "intro", title))
            continue
        if lvl == 2 and re.match(r"^1[567]\.\s", title):
            anchors.append((i, "catalog", title))
            continue
        if lvl == 1 and title == "C++17 mathematical reconstruction port":
            anchors.append((i, "cpp", title))
            continue
        if 879 < i < 1484 and title != "Delivery and scope" and (lvl == 2 or title.startswith("Проверка именно")):
            anchors.append((i, "cpp", title))
            continue
    return anchors


def is_junction(line: str) -> bool:
    s = line.strip()
    return not s or bool(re.match(r'^<a id="[^"]*"></a>$', s))


def split_bounds(lines, anchors):
    bounds = []
    for k, (i, _kind, _title) in enumerate(anchors):
        end = anchors[k + 1][0] if k + 1 < len(anchors) else len(lines)
        bounds.append([i, end])
    bounds[0][0] = 0
    for k in range(1, len(bounds)):
        e = bounds[k - 1][1]
        limit = bounds[k - 1][0] + 1
        while e > limit and is_junction(lines[e - 1]):
            e -= 1
        bounds[k - 1][1] = e
        bounds[k][0] = e
    return bounds


EVIDENCE_ORDER: list[str] = []


def evidence_folder(group: str) -> str:
    if group not in EVIDENCE_ORDER:
        EVIDENCE_ORDER.append(group)
    return f"{EVIDENCE_ORDER.index(group) + 1:02d}-{group}"


def part_target(kind: str, title: str) -> str:
    if kind == "intro":
        if title == "Как читать этот документ":
            return "01-obzor/00-kak-chitat.md"
        num = int(re.match(r"^(\d{1,2})\.", title).group(1))
        return f"01-obzor/{INTRO_SLUGS[num]}.md"
    if kind == "cpp":
        return f"02-cpp-model/{CPP_SLUGS[title]}.md"
    if kind == "catalog":
        num = int(re.match(r"^(\d{1,2})\.", title).group(1))
        return f"03-katalogi/{CATALOG_SLUGS[num]}.md"
    if kind == "evidence":
        m = re.match(r"^(E\d{3})\.\s*`([^`]+)`", title)
        eid, src = (m.group(1), m.group(2)) if m else (safe_name(title), safe_name(title))
        src = src.replace("/home/daytona/albigg/", "")
        group = evidence_folder(evidence_group(src))
        return f"05-dokazatelstva/{group}/{eid}-{safe_name(Path(src).name)}.md"
    if kind == "appendix":
        if title.startswith("Приложения E"):
            return "05-dokazatelstva/00-manifest.md"
        if title.startswith("Приложение F"):
            return "07-prilozhenie-f.md"
        m = re.match(r"^Приложение (D\d+)\.\s*`([^`]+)`", title)
        did, src = (m.group(1), m.group(2)) if m else (safe_name(title), safe_name(title))
        return f"04-otchety/{did}-{safe_name(src.replace('/', '-').removesuffix('.md'))}.md"
    raise AssertionError(kind)


def evidence_group(path: str) -> str:
    parent = Path(path).parent.as_posix()
    if parent in (".", "", "analysis"):
        return "analysis-root"
    parts = [p for p in parent.split("/") if p and p != "analysis"]
    return slug("-".join(parts)) if parts else "analysis-root"


# --------------------------------------------------------------------------
# карточки разделов 16 и 17
# --------------------------------------------------------------------------
def table_rows(text: str):
    rows = []
    for line in text.splitlines():
        s = line.strip()
        if s.startswith("|") and s.endswith("|"):
            rows.append([c.strip() for c in s.strip("|").split("|")])
    return [r for r in rows[2:] if r and r[0]]


def clean_cell(cell: str) -> str:
    return cell.replace("`", "").strip()


def write_card(path: Path, header: str, body: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(header + "\n" + BODY_MARKER + "\n" + body, encoding="utf-8")


def make_pseudocode_cards(section16: str, out: Path):
    made = []
    for row in table_rows(section16):
        if len(row) < 7:
            continue
        num, rva, va, role, src, cpp_line, size = [clean_cell(c) for c in row[:7]]
        rel = f"03-katalogi/pseudocode-c/{safe_name(f'{int(num):02d}-{rva}')}.md"
        body = (
            f"# C-псевдокод № {num} — `{rva}`\n\n"
            "[← каталог C-псевдокодов](../16-katalog-c-psevdokodov.md) · "
            "[индекс карточек](00-index.md) · [все части](../../README.md)\n\n"
            "| Поле | Значение |\n|---|---|\n"
            f"| № | {num} |\n"
            f"| RVA | `{rva}` |\n"
            f"| VA | `{va}` |\n"
            f"| Роль | {role} |\n"
            f"| Источник | `{src}` |\n"
            f"| Строка в `CS2_RECONSTRUCTION.cpp` | {cpp_line} |\n"
            f"| Bytes листинга | {size} |\n\n"
            f"Тело листинга — в архивном `CS2_RECONSTRUCTION.cpp` (строка {cpp_line}); "
            "карточка фиксирует адрес, роль и источник, как в разделе 16 мастер-файла.\n"
        )
        write_card(out / rel, f"<!-- split-card | pseudocode #{num} {rva} | section 16 -->", body)
        made.append({"num": int(num), "rva": rva, "role": role, "rel": rel})
    return made


def group_folder(group: str, order: list[str]) -> str:
    return f"03-katalogi/asm/{order.index(group) + 1:02d}-{group}"


def make_asm_cards(section17: str, out: Path):
    order: list[str] = []
    grouped: "OrderedDict[str, list[dict]]" = OrderedDict()
    for row in table_rows(section17):
        if len(row) < 6:
            continue
        num, fmt, src, cpp_line, size, digest = [clean_cell(c) for c in row[:6]]
        src_path = src.split(":L")[0]
        fragment = fmt.startswith("report_fragment")
        group = evidence_group(src_path)
        if fragment:
            group = "report-" + group
        if group not in order:
            order.append(group)
        rec = {
            "num": int(num),
            "fmt": fmt,
            "src": src,
            "name": Path(src_path).name,
            "cpp_line": cpp_line,
            "size": size,
            "digest": digest,
            "group": group,
        }
        grouped.setdefault(group, []).append(rec)

    for group, records in grouped.items():
        folder = group_folder(group, order)
        for rec in records:
            stem = str(rec["num"]).zfill(3) + "-" + rec["name"]
            rec["rel"] = f"{folder}/{safe_name(stem)}.md"
    for group, records in grouped.items():
        idx = order.index(group) + 1
        for rec in records:
            rel = rec["rel"]
            body = (
                f"# Архивный блок № {rec['num']} — `{rec['src']}`\n\n"
                "[← индекс категории](00-index.md) · "
                "[каталог](../../17-katalog-asm-i-arhivnyh-blokov.md) · "
                "[все части](../../../README.md)\n\n"
                "| Поле | Значение |\n|---|---|\n"
                f"| № | {rec['num']} |\n"
                f"| Формат | `{rec['fmt']}` |\n"
                f"| Исходный материал | `{rec['src']}` |\n"
                f"| Строка в `CS2_RECONSTRUCTION.cpp` | {rec['cpp_line']} |\n"
                f"| Bytes | {rec['size']} |\n"
                f"| SHA-256 | `{rec['digest']}` |\n\n"
                f"Тело листинга хранится в `CS2_RECONSTRUCTION.cpp` (строка {rec['cpp_line']}); "
                "карточка фиксирует источник, размер и hash, как в разделе 17 мастер-файла.\n"
            )
            write_card(out / rel, f"<!-- split-card | archive #{rec['num']} {rec['fmt']} | section 17 -->", body)
    return order, grouped


# --------------------------------------------------------------------------
# вырезание code-блоков
# --------------------------------------------------------------------------
def extract_code_blocks(parts, out: Path):
    extracted = []
    for part in parts:
        if part["path"].split("/")[0] not in ("01-obzor", "02-cpp-model", "04-otchety"):
            continue
        fence = 0
        buf: list[str] = []
        lang = ""
        for raw in part["lines"]:
            s = raw.rstrip("\r\n")
            m = FENCE_RE.match(s)
            if fence and m and len(m.group(1)) >= fence and m.group(2).strip() == "":
                text = "".join(buf)
                if text.strip():
                    extracted.append(
                        {
                            "part": part["path"],
                            "stem": Path(part["path"]).stem,
                            "lang": lang or "text",
                            "text": text,
                        }
                    )
                fence, buf, lang = 0, [], ""
                continue
            if fence:
                buf.append(raw)
                continue
            if m and "`" not in m.group(2):
                fence = len(m.group(1))
                lang = m.group(2).strip()

    counters: dict[str, int] = {}
    written = []
    for item in extracted:
        counters[item["stem"]] = counters.get(item["stem"], 0) + 1
        name = safe_name(f"{item['stem']}-{counters[item['stem']]:02d}-{item['lang']}.md")
        rel = f"06-kod/{name}"
        body = (
            f"# Code-блок из `{item['part']}` (язык `{item['lang']}`)\n\n"
            f"[← исходная часть](../{item['part']}) · [индекс кода](00-index.md) · "
            "[все части](../README.md)\n\n"
            f"Копия fenced-блока из части `{item['part']}`; в самой части блок остаётся на месте. "
            f"Символов: {len(item['text'])}.\n\n"
            f"````{item['lang']}\n{item['text']}````\n"
        )
        write_card(out / rel, f"<!-- split-kod | {item['part']} | lang={item['lang']} -->", body)
        written.append({"rel": rel, "part": item["part"], "lang": item["lang"], "chars": len(item["text"])})
    return written


# --------------------------------------------------------------------------
# навигация и индексы
# --------------------------------------------------------------------------
def nav_for(rel: str) -> str:
    depth = rel.count("/")
    up = "../" * depth
    links = [f"[← все части]({up}README.md)"]
    if rel.startswith("03-katalogi/16"):
        links.append("[карточки C-псевдокода](pseudocode-c/00-index.md)")
    elif rel.startswith("03-katalogi/17"):
        links.append("[карточки ASM](asm/00-index.md)")
    elif rel.startswith("03-katalogi/15"):
        links.append("[отчёты D01–D13](../04-otchety/00-index.md)")
        links.append("[свидетельства E](../05-dokazatelstva/00-manifest.md)")
        links.append("[code-блоки](../06-kod/00-index.md)")
    elif rel.startswith("04-otchety/"):
        links.append("[индекс отчётов](00-index.md)")
    elif rel.startswith("05-dokazatelstva/"):
        if rel.endswith("00-manifest.md"):
            links.append("[категории свидетельств](00-index.md)")
        else:
            links.append("[категория](00-index.md)")
            links.append("[manifest E001–E184](../00-manifest.md)")
    elif rel.startswith("01-obzor/"):
        links.append("[индекс обзора](00-index.md)")
    elif rel.startswith("02-cpp-model/"):
        links.append("[индекс порта](00-index.md)")
    return " · ".join(links)


def write_part(root: Path, rel: str, body: str, start: int, end: int) -> str:
    digest = sha256_text(body)
    header = (
        f"<!-- split-part | CS2_RESEARCH_MASTER.md lines {start + 1}-{end} "
        f"| body-sha256 {digest} -->"
    )
    path = root / rel
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(header + "\n" + nav_for(rel) + "\n\n" + BODY_MARKER + "\n" + body, encoding="utf-8")
    return digest


def read_body(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    marker = BODY_MARKER + "\n"
    if marker not in text:
        raise SystemExit(f"нет маркера тела: {path}")
    return text.split(marker, 1)[1]


def write_indexes(root: Path, parts, cards_c, asm_order, asm_grouped, kod) -> None:
    # 01-obzor / 02-cpp-model
    for folder, header in (("01-obzor", "# Индекс: вводный свод"), ("02-cpp-model", "# Индекс: порт C++17-модели")):
        rows = [
            "| Файл | Строки исходника | Заголовок |",
            "|---|---:|---|",
        ]
        for p in parts:
            if Path(p["path"]).parent.as_posix() != folder:
                continue
            rows.append(
                f"| [{Path(p['path']).name}]({Path(p['path']).name}) | "
                f"{p['start'] + 1}–{p['end']} | {p['title']} |"
            )
        (root / folder / "00-index.md").write_text(
            header + "\n\n[← все части](../README.md)\n\n" + "\n".join(rows) + "\n", encoding="utf-8"
        )

    # 03-katalogi/pseudocode-c/00-index.md
    rows = ["| № | RVA | Роль | Файл |", "|---:|---|---|---|"]
    for c in cards_c:
        rows.append(f"| {c['num']} | `{c['rva']}` | {c['role']} | [{Path(c['rel']).name}]({Path(c['rel']).name}) |")
    (root / "03-katalogi/pseudocode-c/00-index.md").write_text(
        "# Индекс: 63 C-псевдокода\n\n"
        "[← каталог](../16-katalog-c-psevdokodov.md) · [все части](../../README.md)\n\n"
        "Карточка на каждый листинг раздела 16: адрес, роль, исходный путь и строка в архивном "
        "`CS2_RECONSTRUCTION.cpp`.\n\n" + "\n".join(rows) + "\n",
        encoding="utf-8",
    )

    # 03-katalogi/asm/00-index.md
    rows = ["| № | Категория | Карточек | Индекс |", "|---:|---|---:|---|"]
    for n, group in enumerate(asm_order, 1):
        folder = f"{n:02d}-{group}"
        rows.append(
            f"| {n} | `{group}` | {len(asm_grouped[group])} | [{folder}/00-index.md]({folder}/00-index.md) |"
        )
    (root / "03-katalogi/asm/00-index.md").write_text(
        "# Индекс: 218 ASM/архивных блоков\n\n"
        "[← каталог](../17-katalog-asm-i-arhivnyh-blokov.md) · [все части](../../README.md)\n\n"
        "Карточки раздела 17, сгруппированные по исходной папке: формат, источник, строка "
        "в `CS2_RECONSTRUCTION.cpp`, размер и SHA-256.\n\n" + "\n".join(rows) + "\n",
        encoding="utf-8",
    )
    for n, group in enumerate(asm_order, 1):
        folder = f"{n:02d}-{group}"
        rows = ["| № | Файл | Источник | CPP строка |", "|---:|---|---|---:|"]
        for rec in asm_grouped[group]:
            rows.append(
                f"| {rec['num']} | [{Path(rec['rel']).name}]({Path(rec['rel']).name}) | "
                f"`{rec['src']}` | {rec['cpp_line']} |"
            )
        (root / "03-katalogi/asm" / folder / "00-index.md").write_text(
            f"# Индекс: ASM/архивные блоки `{group}`\n\n"
            "[← все категории](../00-index.md) · [каталог](../../17-katalog-asm-i-arhivnyh-blokov.md) · "
            "[все части](../../../README.md)\n\n" + "\n".join(rows) + "\n",
            encoding="utf-8",
        )

    # 04-otchety/00-index.md — строим из раздела 15
    sec15 = next(p for p in parts if p["path"].endswith("15-navigaciya-po-otchetam.md"))
    rows = ["| ID | Файл | Источник | Этап / статус | Bytes | SHA-256 |", "|---|---|---|---|---:|---|"]
    d_parts = [p for p in parts if p["path"].startswith("04-otchety/")]
    for p in d_parts:
        src = re.search(r"`([^`]+)`", p["title"]).group(1)
        meta = {}
        for line in "".join(sec15["lines"]).splitlines():
            if src in line and line.strip().startswith("|"):
                cells = [clean_cell(c) for c in line.strip().strip("|").split("|")]
                if len(cells) >= 5 and cells[0].startswith("["):
                    meta = {"status": cells[2], "bytes": cells[3], "sha": cells[4]}
        did = Path(p["path"]).name.split("-")[0]
        rows.append(
            f"| {did} | [{Path(p['path']).name}]({Path(p['path']).name}) | `{src}` | "
            f"{meta.get('status', '')} | {meta.get('bytes', '')} | `{meta.get('sha', '')}` |"
        )
    (root / "04-otchety/00-index.md").write_text(
        "# Индекс: приложения D01–D13\n\n"
        "[← раздел 15 в каталогах](../03-katalogi/15-navigaciya-po-otchetam.md) · [все части](../README.md)\n\n"
        "Это дословные исходные отчёты с собственными датами и статусами того этапа: "
        "фразы «hitchance не найден», «producer неизвестен» относятся к моменту написания. "
        "Актуальный свод — в `01-obzor`.\n\n" + "\n".join(rows) + "\n",
        encoding="utf-8",
    )

    # 05-dokazatelstva/00-index.md + индекс каждой категории
    ev_parts = [p for p in parts if p["path"].startswith("05-dokazatelstva/") and p["path"] != "05-dokazatelstva/00-manifest.md"]
    groups: "OrderedDict[str, list[dict]]" = OrderedDict()
    for p in ev_parts:
        groups.setdefault(Path(p["path"]).parent.name, []).append(p)
    rows = ["| № | Категория | Файлов | Индекс |", "|---:|---|---:|---|"]
    for n, (group, items) in enumerate(groups.items(), 1):
        rows.append(f"| {n} | `{group}` | {len(items)} | [{group}/00-index.md]({group}/00-index.md) |")
    (root / "05-dokazatelstva/00-index.md").write_text(
        "# Индекс: машинные свидетельства E001–E184\n\n"
        "[← manifest](00-manifest.md) · [раздел 15](../03-katalogi/15-navigaciya-po-otchetam.md) · "
        "[все части](../README.md)\n\n"
        "Всё, что было секцией E мастер-файла: тела скриптов, JSON/TSV/CSV-выгрузки, логи и "
        "ASM-проекции — по категориям исходного дерева анализа.\n\n" + "\n".join(rows) + "\n",
        encoding="utf-8",
    )
    for group, items in groups.items():
        items_sorted = sorted(items, key=lambda p: p["path"])
        rows = ["| ID | Файл | Строки исходника | Исходный путь |", "|---|---|---:|---|"]
        for p in items_sorted:
            eid = Path(p["path"]).name.split("-")[0]
            rows.append(
                f"| {eid} | [{Path(p['path']).name}]({Path(p['path']).name}) | "
                f"{p['start'] + 1}–{p['end']} | {p['title'].split('.', 1)[1].strip()} |"
            )
        (root / "05-dokazatelstva" / group / "00-index.md").write_text(
            f"# Индекс свидетельств: `{group}`\n\n"
            "[← все категории](../00-index.md) · [manifest](../00-manifest.md) · "
            "[все части](../../README.md)\n\n" + "\n".join(rows) + "\n",
            encoding="utf-8",
        )

    # 06-kod/00-index.md
    rows = ["| Файл | Язык | Часть-источник | Символов |", "|---|---|---|---:|"]
    for item in kod:
        rows.append(
            f"| [{Path(item['rel']).name}]({Path(item['rel']).name}) | `{item['lang']}` | "
            f"[{item['part']}](../{item['part']}) | {item['chars']} |"
        )
    (root / "06-kod/00-index.md").write_text(
        "# Индекс: code-блоки обзора, порта и отчётов\n\n"
        "[← все части](../README.md)\n\n"
        "Копии fenced-блоков из `01-obzor`, `02-cpp-model`, `04-otchety` (псевдокод, формулы, "
        "shell-команды, ответы валидаторов). В исходных частях блоки остаются на своих местах.\n\n"
        + "\n".join(rows) + "\n",
        encoding="utf-8",
    )


def build_readme(root: Path, parts, cards_c, asm_order, asm_grouped, kod, source, source_sha, ok, joined_sha):
    by_folder: "OrderedDict[str, list[dict]]" = OrderedDict()
    for p in parts:
        by_folder.setdefault(Path(p["path"]).parent.as_posix(), []).append(p)

    lines = [
        "# CS2_RESEARCH_MASTER — разбивка на мелкие части",
        "",
        f"Источник: [CS2_RESEARCH_MASTER.md](../CS2_RESEARCH_MASTER.md) — "
        f"{len(source.splitlines())} строк, SHA-256 `{source_sha}`.",
        "",
        "Скрипт разбивки: [split_master_md.py](../split_master_md.py) · "
        "проверка целостности: [_verify.md](_verify.md) · "
        "карта строк: [_source-map.tsv](_source-map.tsv).",
        "",
        "## Куда смотреть по темам",
        "",
        "| Тема | Часть |",
        "|---|---|",
    ]
    for title, rel in TOPIC_TABLE:
        lines.append(f"| {title} | [{rel}]({rel}) |")

    lines += [
        "",
        "## Как устроена папка",
        "",
        "```text",
        "01-obzor/              вводный свод: как читать + разделы 1–14",
        "02-cpp-model/          документация порта CS2_RECONSTRUCTION.cpp/.h",
        "03-katalogi/           разделы 15–17: навигация, псевдокод, ASM",
        "  pseudocode-c/        карточки 63 C-псевдокодов",
        "  asm/                 карточки ASM/архивных блоков № 64–218 по 13 категориям",
        "04-otchety/            приложения D01–D13 (дословно)",
        "05-dokazatelstva/      приложения E: manifest + E001–E184 по категориям",
        "06-kod/                code-блоки обзора/порта/отчётов (копии)",
        "07-prilozhenie-f.md    приложение F: крупные индексы вне текста",
        "```",
        "",
        "## Состав",
        "",
        f"- Основных частей: **{len(parts)}** (дословные срезы исходника, каждая строка входит ровно в одну).",
        f"- Карточек C-псевдокода: **{len(cards_c)}** — [индекс](03-katalogi/pseudocode-c/00-index.md).",
        f"- Карточек ASM/архивных блоков: **{sum(len(v) for v in asm_grouped.values())}** — "
        "[индекс](03-katalogi/asm/00-index.md).",
        f"- Вырезанных code-блоков: **{len(kod)}** — [индекс](06-kod/00-index.md).",
        f"- Отчётов D01–D13: **{len(by_folder.get('04-otchety', []))}** — [индекс](04-otchety/00-index.md).",
        f"- Свидетельств E001–E184: **{sum(len(v) for k, v in by_folder.items() if k.startswith('05-dokazatelstva/'))}** — "
        "[manifest](05-dokazatelstva/00-manifest.md), [индекс](05-dokazatelstva/00-index.md).",
        "",
        "## Проверка целостности",
        "",
        f"- Склейка тел всех основных частей совпадает с исходником байт-в-байт: "
        f"**{'да' if ok else 'НЕТ'}** (SHA-256 `{joined_sha}`).",
        "- Каждая часть начинается генерируемым заголовком и строкой навигации; "
        "неизменный текст начинается после `<!-- split-body-start -->`.",
        "- Карточки и code-блоки — производные копии, они не участвуют в склейке; "
        "табличные строки и блоки остаются и в исходных частях.",
        "- Исходный `CS2_RESEARCH_MASTER.md` не изменялся и не удалялся.",
        "",
        "## Раздел 16 и 17: псевдокод и ASM",
        "",
        "- [Каталог C-псевдокодов](03-katalogi/16-katalog-c-psevdokodov.md) — 63 листинга из "
        "архивного `CS2_RECONSTRUCTION.cpp`; на каждый сделана карточка с RVA/VA, ролью, "
        "источником и строкой CPP.",
        "- [Каталог ASM и архивных блоков](03-katalogi/17-katalog-asm-i-arhivnyh-blokov.md) — "
        "номера 64–218: 125 ASM-листингов, 29 report_fragment-вставок и python-модель; "
        "карточки сгруппированы по исходным папкам анализа.",
        "",
    ]

    # перечень частей по папкам
    for folder, items in by_folder.items():
        caption = "корень разбивки" if folder == "." else f"`{folder}`"
        lines += [f"## {caption}", "", "| Файл | Строки исходника | Заголовок |", "|---|---:|---|"]
        for p in items:
            name = Path(p["path"]).name
            lines.append(f"| [{name}]({p['path']}) | {p['start'] + 1}–{p['end']} | {p['title']} |")
        lines.append("")

    (root / "README.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    if not SRC.exists():
        raise SystemExit(f"нет исходника: {SRC}")
    if OUT.exists():
        shutil.rmtree(OUT)  # папка полностью генерируется заново
    source = SRC.read_text(encoding="utf-8")
    lines = source.splitlines(keepends=True)

    anchors = collect_anchors(iter_headings(lines))
    bounds = split_bounds(lines, anchors)

    covered, prev = 0, 0
    for start, end in bounds:
        assert start == prev, (start, prev)
        covered += end - start
        prev = end
    assert covered == len(lines), (covered, len(lines))

    parts = []
    for (start, end), (_i, kind, title) in zip(bounds, anchors):
        body = "".join(lines[start:end])
        rel = part_target(kind, title)
        digest = write_part(OUT, rel, body, start, end)
        parts.append({"path": rel, "kind": kind, "title": title, "start": start, "end": end, "sha": digest, "lines": lines[start:end]})

    sec16 = next(p for p in parts if p["path"].endswith("16-katalog-c-psevdokodov.md"))
    sec17 = next(p for p in parts if p["path"].endswith("17-katalog-asm-i-arhivnyh-blokov.md"))
    cards_c = make_pseudocode_cards("".join(sec16["lines"]), OUT)
    asm_order, asm_grouped = make_asm_cards("".join(sec17["lines"]), OUT)
    kod = extract_code_blocks(parts, OUT)

    map_lines = ["part\tstart_line\tend_line\tlines\tbody_sha256"]
    for p in parts:
        map_lines.append(f"{p['path']}\t{p['start'] + 1}\t{p['end']}\t{p['end'] - p['start']}\t{p['sha']}")
    (OUT / "_source-map.tsv").write_text("\n".join(map_lines) + "\n", encoding="utf-8")

    order = [p["path"] for p in parts]
    joined = "".join(read_body(OUT / rel) for rel in order)
    ok = joined == source
    source_sha, joined_sha = sha256_text(source), sha256_text(joined)

    ev_count = sum(1 for p in parts if p["path"].startswith("05-dokazatelstva/") and p["path"] != "05-dokazatelstva/00-manifest.md")
    report = [
        "# Проверка целостности разбивки",
        "",
        "- Исходник: `../CS2_RESEARCH_MASTER.md`",
        f"  - строк: **{len(lines)}**",
        f"  - SHA-256: `{source_sha}`",
        f"- Основных частей: **{len(parts)}**",
        f"- Отчётов D01–D13: **{sum(1 for p in parts if p['path'].startswith('04-otchety/'))}**",
        f"- Свидетельств E: **{ev_count}**",
        f"- Карточек C-псевдокода: **{len(cards_c)}**",
        f"- Карточек ASM/архивных блоков: **{sum(len(v) for v in asm_grouped.values())}**",
        f"- Вырезанных code-блоков: **{len(kod)}**",
        f"- Склейка тел частей совпадает с исходником: **{'да' if ok else 'НЕТ'}**",
        f"- SHA-256 склейки: `{joined_sha}`",
        "",
        "Каждая основная часть — дословный срез исходника: строка `<!-- split-body-start -->` "
        "отделяет сгенерированный заголовок с навигацией от неизменного текста. Порядок частей "
        "в `_source-map.tsv` соответствует порядку в исходном файле; сумма строк равна "
        f"{len(lines)}.",
    ]
    (OUT / "_verify.md").write_text("\n".join(report) + "\n", encoding="utf-8")

    if ok:
        write_indexes(OUT, parts, cards_c, asm_order, asm_grouped, kod)
        build_readme(OUT, parts, cards_c, asm_order, asm_grouped, kod, source, source_sha, ok, joined_sha)

    print(
        f"частей: {len(parts)}, карточек C: {len(cards_c)}, карточек ASM: "
        f"{sum(len(v) for v in asm_grouped.values())}, code-блоков: {len(kod)}"
    )
    print(f"склейка == исходник: {'да' if ok else 'НЕТ'} ({joined_sha[:16]}…)")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
