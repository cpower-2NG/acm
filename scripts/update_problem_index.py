#!/usr/bin/env python3
import csv
import html
import json
import re
from dataclasses import dataclass, asdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from urllib.request import Request, urlopen


ROOT = Path(__file__).resolve().parents[1]
PROBLEMS_DIR = ROOT / "problems"
DATA_DIR = ROOT / "data"
OUTPUT_JSON = DATA_DIR / "problems_index.json"
OUTPUT_MD = ROOT / "PROBLEMS.md"
OUTPUT_TAG_MD = ROOT / "PROBLEMS_BY_TAG.md"
OUTPUT_UNTAGGED_MD = ROOT / "PROBLEMS_UNTAGGED.md"
ANNOTATION_TSV = DATA_DIR / "problem_annotations.tsv"
TAG_ALIASES_JSON = DATA_DIR / "tag_aliases.json"


TSV_COLUMNS = ["path", "tags", "status", "title"]


DEFAULT_TAG_ALIASES = {
    "version": 1,
    "tags": [
        {"id": "图论", "zh": "图论", "en": "graph", "aliases": ["graphs"]},
        {"id": "最短路", "zh": "最短路", "en": "shortest-path", "aliases": ["shortest path", "dijkstra", "floyd"]},
        {"id": "数据结构", "zh": "数据结构", "en": "data-structure", "aliases": ["data structure", "ds"]},
        {"id": "动态规划", "zh": "动态规划", "en": "dp", "aliases": ["dynamic programming"]},
        {"id": "深度优先搜索", "zh": "深度优先搜索", "en": "dfs", "aliases": ["depth-first search"]},
        {"id": "广度优先搜索", "zh": "广度优先搜索", "en": "bfs", "aliases": ["breadth-first search"]},
        {"id": "贪心", "zh": "贪心", "en": "greedy", "aliases": []},
        {"id": "模拟", "zh": "模拟", "en": "simulation", "aliases": ["实现", "implementation"]},
        {"id": "数学", "zh": "数学", "en": "math", "aliases": []},
        {"id": "字符串", "zh": "字符串", "en": "string", "aliases": []},
        {"id": "滑动窗口", "zh": "滑动窗口", "en": "sliding-window", "aliases": ["sliding window"]},
        {"id": "差分", "zh": "差分", "en": "difference-array", "aliases": ["difference array"]},
        {"id": "优先队列", "zh": "优先队列", "en": "priority-queue", "aliases": ["priority queue", "heap"]},
        {"id": "树状数组", "zh": "树状数组", "en": "fenwick", "aliases": ["fenwick", "bit"]},
        {"id": "最小生成树", "zh": "最小生成树", "en": "mst", "aliases": ["minimum spanning tree", "prim", "kruskal"]},
        {"id": "负环", "zh": "负环", "en": "negative-cycle", "aliases": ["negative cycle"]},
        {"id": "贝尔曼福德", "zh": "贝尔曼福德", "en": "bellman-ford", "aliases": ["bellman ford", "bellman-ford", "spfa"]},
        {"id": "构造", "zh": "构造", "en": "constructive", "aliases": ["construction"]},
        {"id": "二分", "zh": "二分", "en": "binary-search", "aliases": ["binary search"]},
    ],
}


def fetch_json(url: str, timeout: int = 15):
    req = Request(url, headers={"User-Agent": "acm-indexer/2.0"})
    with urlopen(req, timeout=timeout) as resp:
        return json.load(resp)


def fetch_json_any(urls: List[str], timeout: int = 15):
    last_error: Optional[Exception] = None
    for url in urls:
        try:
            return fetch_json(url, timeout=timeout)
        except Exception as e:
            last_error = e
    if last_error is not None:
        raise last_error
    raise RuntimeError("no url provided")


def fetch_html(url: str, timeout: int = 15) -> str:
    req = Request(url, headers={"User-Agent": "acm-indexer/2.0"})
    with urlopen(req, timeout=timeout) as resp:
        return resp.read().decode("utf-8", errors="ignore")


def fetch_atcoder_title_from_page(task_url: str) -> Optional[str]:
    try:
        page = fetch_html(task_url, timeout=15)
    except Exception:
        return None

    m = re.search(r"<title>(.*?)</title>", page, flags=re.IGNORECASE | re.DOTALL)
    if not m:
        return None

    title = m.group(1).strip()
    title = html.unescape(title)
    title = re.sub(r"\s*-\s*AtCoder\s*$", "", title, flags=re.IGNORECASE)
    return title if title else None


@dataclass
class ProblemRecord:
    path: str
    platform: str
    contest: str
    problem: str
    task_id: str
    source_url: str
    title: str
    tags: List[str]
    status: str


@dataclass
class AnnotationRow:
    path: str
    tags: List[str]
    status: str
    title: str
    is_new: bool


def normalize_tag_token(token: str) -> str:
    t = token.strip().lower()
    # Normalize common full-width punctuation and separators.
    t = (
        t.replace("（", "(")
        .replace("）", ")")
        .replace("，", ",")
        .replace("：", ":")
    )
    # Make english aliases robust to style differences: bellman ford / bellman-ford / bellman_ford.
    for ch in [" ", "-", "_", "/", "\\", "(", ")", ",", ":", "."]:
        t = t.replace(ch, "")
    return t


def load_tag_aliases() -> Tuple[Dict[str, str], Dict[str, Dict[str, str]]]:
    if not TAG_ALIASES_JSON.exists():
        TAG_ALIASES_JSON.write_text(
            json.dumps(DEFAULT_TAG_ALIASES, ensure_ascii=False, indent=2) + "\n",
            encoding="utf-8",
        )

    try:
        payload = json.loads(TAG_ALIASES_JSON.read_text(encoding="utf-8"))
    except Exception:
        payload = DEFAULT_TAG_ALIASES

    tags = payload.get("tags", []) if isinstance(payload, dict) else []

    alias_to_id: Dict[str, str] = {}
    id_to_meta: Dict[str, Dict[str, str]] = {}

    for item in tags:
        if not isinstance(item, dict):
            continue
        tag_id = str(item.get("id", "")).strip()
        if not tag_id:
            continue
        zh = str(item.get("zh", "")).strip() or tag_id
        en = str(item.get("en", "")).strip()

        id_to_meta[tag_id] = {"zh": zh, "en": en}

        keys = [tag_id, zh, en]
        aliases = item.get("aliases", [])
        if isinstance(aliases, list):
            keys.extend([str(x) for x in aliases])

        for k in keys:
            kk = normalize_tag_token(k)
            if kk:
                alias_to_id[kk] = tag_id

    return alias_to_id, id_to_meta


def resolve_tags(raw_tags: List[str], alias_to_id: Dict[str, str]) -> List[str]:
    out: List[str] = []
    seen = set()
    for t in raw_tags:
        raw = str(t).strip()
        if not raw:
            continue
        key = normalize_tag_token(raw)
        canonical = alias_to_id.get(key, raw)
        if canonical in seen:
            continue
        seen.add(canonical)
        out.append(canonical)
    return out


def render_tag(tag: str, id_to_meta: Dict[str, Dict[str, str]]) -> str:
    meta = id_to_meta.get(tag)
    if not meta:
        return tag
    zh = meta.get("zh", "")
    en = meta.get("en", "")
    if zh and en and zh != en:
        return f"{zh}({en})"
    return zh or en or tag


def parse_tags_text(text: str) -> List[str]:
    if not text:
        return []
    normalized = text.replace("，", ",")
    tags = [x.strip() for x in re.split(r"[,;；|/]", normalized)]
    return tags


def parse_problem_file(p: Path) -> Optional[Tuple[str, str, str, str]]:
    rel = p.relative_to(PROBLEMS_DIR)
    parts = rel.parts
    if len(parts) < 2:
        return None

    platform = parts[0].lower()
    stem = p.stem
    if platform == "practice":
        # Practice filenames may include underscores, e.g. luogu_P1027.cpp.
        if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_]*", stem):
            return None
    else:
        if not re.fullmatch(r"[A-Za-z][A-Za-z0-9]*", stem):
            return None

    if len(parts) >= 3:
        contest = parts[1].lower()
        problem = stem.upper()
        task_id = f"{contest}_{problem.lower()}"
    else:
        contest = "misc"
        problem = stem.upper()
        task_id = ""

    return platform, contest, problem, task_id


def fetch_codeforces_map() -> Dict[Tuple[str, str], Dict]:
    data = fetch_json("https://codeforces.com/api/problemset.problems")
    if data.get("status") != "OK":
        return {}

    out: Dict[Tuple[str, str], Dict] = {}
    for item in data.get("result", {}).get("problems", []):
        contest_id = item.get("contestId")
        index = item.get("index")
        if contest_id is None or index is None:
            continue
        key = (str(contest_id), str(index).upper())
        out[key] = {
            "title": item.get("name", ""),
            "tags": item.get("tags", []),
        }
    return out


def fetch_atcoder_map() -> Dict[str, Dict]:
    problems = fetch_json_any(
        [
            "https://raw.githubusercontent.com/kenkoooo/AtCoderProblems/master/resources/problems.json",
            "https://kenkoooo.com/atcoder/resources/problems.json",
            "https://atcoder-problems.s3.ap-northeast-1.amazonaws.com/resources/problems.json",
        ]
    )

    pmap: Dict[str, Dict] = {}
    for item in problems:
        pid = item.get("id")
        if not pid:
            continue
        pmap[pid] = {
            "title": item.get("title", ""),
        }

    return pmap


def load_annotations(alias_to_id: Dict[str, str]) -> Dict[str, AnnotationRow]:
    if not ANNOTATION_TSV.exists():
        return {}

    rows: Dict[str, AnnotationRow] = {}
    with ANNOTATION_TSV.open("r", encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for raw in reader:
            path = (raw.get("path") or "").strip()
            if not path:
                continue
            row = AnnotationRow(
                path=path,
                tags=resolve_tags(parse_tags_text((raw.get("tags") or "").strip()), alias_to_id),
                status=((raw.get("status") or "ac").strip().lower() or "ac"),
                title=(raw.get("title") or "").strip(),
                is_new=False,
            )
            rows[path] = row
    return rows


def write_annotations(rows: List[AnnotationRow]):
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    with ANNOTATION_TSV.open("w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=TSV_COLUMNS, delimiter="\t")
        writer.writeheader()
        for row in rows:
            writer.writerow(
                {
                    "path": row.path,
                    "tags": ", ".join(row.tags),
                    "status": row.status,
                    "title": row.title,
                }
            )


def merge_annotations(
    records: List[ProblemRecord],
    existing: Dict[str, AnnotationRow],
    alias_to_id: Dict[str, str],
) -> List[AnnotationRow]:
    merged: List[AnnotationRow] = []

    for rec in records:
        prev = existing.get(rec.path)

        if prev is None:
            row = AnnotationRow(
                path=rec.path,
                tags=rec.tags[:],
                status=rec.status,
                title=rec.title,
                is_new=True,
            )
        else:
            row = AnnotationRow(
                path=rec.path,
                tags=prev.tags if prev.tags else rec.tags,
                status=prev.status if prev.status else rec.status,
                title=prev.title if prev.title else rec.title,
                is_new=False,
            )

        rec.tags = resolve_tags(row.tags, alias_to_id)
        rec.status = row.status
        if row.title:
            rec.title = row.title

        merged.append(row)

    # Priority order for manual editing convenience:
    # 1) rows with empty tags first
    # 2) among them, newly discovered rows first
    # 3) then by path for stable output
    def sort_key(r: AnnotationRow):
        needs_tag = 0 if len(r.tags) == 0 else 1
        new_first = 0 if r.is_new else 1
        return (needs_tag, new_first, r.path)

    merged.sort(key=sort_key)
    return merged


def build_records() -> Dict:
    # Collect common C/C++ source file extensions under problems/
    exts = ["*.cpp", "*.cc", "*.cxx", "*.c"]
    files_set = set()
    for pat in exts:
        for p in PROBLEMS_DIR.rglob(pat):
            try:
                rel = p.relative_to(PROBLEMS_DIR)
            except Exception:
                continue
            if "/." in str(rel):
                continue
            files_set.add(p)

    files = sorted(files_set)

    cf_map: Dict[Tuple[str, str], Dict] = {}
    atcoder_map: Dict[str, Dict] = {}
    warnings: List[str] = []
    atcoder_title_cache: Dict[str, str] = {}
    alias_to_id, id_to_meta = load_tag_aliases()

    try:
        cf_map = fetch_codeforces_map()
    except Exception as e:
        warnings.append(f"codeforces metadata unavailable: {e}")

    try:
        atcoder_map = fetch_atcoder_map()
    except Exception as e:
        warnings.append(f"atcoder metadata API unavailable, using page-title fallback: {e}")

    records: List[ProblemRecord] = []

    for p in files:
        parsed = parse_problem_file(p)
        if parsed is None:
            continue

        platform, contest, problem, task_id = parsed
        title = ""
        tags: List[str] = []
        source_url = ""

        if platform == "codeforces":
            m = re.search(r"(\d+)", contest)
            if m:
                cid = m.group(1)
                key = (cid, problem)
                source_url = f"https://codeforces.com/problemset/problem/{cid}/{problem}"
                meta = cf_map.get(key)
                if meta:
                    title = meta.get("title", "")
                    tags = resolve_tags(meta.get("tags", []), alias_to_id)

        elif platform == "atcoder":
            if task_id:
                source_url = f"https://atcoder.jp/contests/{contest}/tasks/{task_id}"
                meta = atcoder_map.get(task_id)
                if meta:
                    title = meta.get("title", "")
                if not title:
                    cached = atcoder_title_cache.get(source_url)
                    if cached is not None:
                        title = cached
                    else:
                        fb = fetch_atcoder_title_from_page(source_url)
                        title = fb or ""
                        atcoder_title_cache[source_url] = title

        records.append(
            ProblemRecord(
                path=str(p.relative_to(ROOT)).replace("\\", "/"),
                platform=platform,
                contest=contest,
                problem=problem,
                task_id=task_id,
                source_url=source_url,
                title=title,
                tags=tags,
                status="ac",
            )
        )

    existing = load_annotations(alias_to_id)
    merged_rows = merge_annotations(records, existing, alias_to_id)
    write_annotations(merged_rows)

    generated_at = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    return {
        "generated_at": generated_at,
        "count": len(records),
        "warnings": warnings,
        "tag_meta": id_to_meta,
        "records": [asdict(r) for r in records],
    }


def write_problems_md(payload: Dict):
    records = payload["records"]
    id_to_meta = payload.get("tag_meta", {})
    lines: List[str] = []
    lines.append("# Problems Index")
    lines.append("")
    lines.append(f"Generated at (UTC): {payload['generated_at']}")
    lines.append("")
    lines.append(f"Total: {payload['count']}")
    lines.append("")

    warnings = payload.get("warnings", [])
    if warnings:
        lines.append("## Warnings")
        lines.append("")
        for w in warnings:
            lines.append(f"- {w}")
        lines.append("")

    lines.append("## All Problems")
    lines.append("")
    lines.append("| Code Path | Platform | Contest | Problem | Title | Tags | Status | Link |")
    lines.append("| --- | --- | --- | --- | --- | --- | --- | --- |")
    for item in records:
        title = item["title"].replace("|", "\\|") if item["title"] else "-"
        tags = ",".join([render_tag(t, id_to_meta) for t in item.get("tags", [])]) if item.get("tags") else "-"
        link = item["source_url"] if item["source_url"] else "-"
        code_path = item["path"]
        code_path_cell = f"[{code_path}]({code_path})"
        title_cell = f"[{title}]({link})" if (title != "-" and link != "-") else title
        problem_cell = f"[{item['problem']}]({link})" if link != "-" else item["problem"]
        status = item.get("status", "ac")
        lines.append(
            f"| {code_path_cell} | {item['platform']} | {item['contest']} | {problem_cell} | {title_cell} | {tags} | {status} | {link} |"
        )

    OUTPUT_MD.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_by_tag_md(payload: Dict):
    records = payload["records"]
    id_to_meta = payload.get("tag_meta", {})
    tag_map: Dict[str, List[Dict]] = {}
    untagged: List[Dict] = []

    for item in records:
        tags = item.get("tags", [])
        if not tags:
            untagged.append(item)
            continue
        for t in tags:
            tag_map.setdefault(t, []).append(item)

    lines: List[str] = []
    lines.append("# Problems By Tag")
    lines.append("")
    lines.append(f"Generated at (UTC): {payload['generated_at']}")
    lines.append("")

    for tag in sorted(tag_map.keys()):
        meta = id_to_meta.get(tag, {})
        zh = meta.get("zh", "")
        en = meta.get("en", "")
        if zh and en:
            lines.append(f"## tag:{zh} | {en}")
        else:
            lines.append(f"## tag:{tag}")
        lines.append("")
        for item in sorted(tag_map[tag], key=lambda x: (x["platform"], x["contest"], x["problem"])):
            title = item["title"] or "(no title)"
            tags_text = ",".join([render_tag(t, id_to_meta) for t in item.get("tags", [])])
            source_url = item.get("source_url") or ""
            pid = f"{item['platform']}/{item['contest']}/{item['problem']}"
            pid_cell = f"[{pid}]({source_url})" if source_url else pid
            code_path = item["path"]
            code_cell = f"[{code_path}]({code_path})"
            lines.append(
                f"- {pid_cell} {title} | tags={tags_text} | status={item.get('status', 'ac')} | code={code_cell} | path={code_path}"
            )
        lines.append("")

    lines.append("## tag:untagged")
    lines.append("")
    if not untagged:
        lines.append("- none")
    else:
        for item in sorted(untagged, key=lambda x: (x["platform"], x["contest"], x["problem"])):
            title = item["title"] or "(no title)"
            source_url = item.get("source_url") or ""
            pid = f"{item['platform']}/{item['contest']}/{item['problem']}"
            pid_cell = f"[{pid}]({source_url})" if source_url else pid
            code_path = item["path"]
            code_cell = f"[{code_path}]({code_path})"
            lines.append(
                f"- {pid_cell} {title} | status={item.get('status', 'ac')} | code={code_cell} | path={code_path}"
            )

    OUTPUT_TAG_MD.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_untagged_md(payload: Dict):
    records = payload["records"]
    untagged = [x for x in records if not x.get("tags")]

    lines: List[str] = []
    lines.append("# Problems Untagged")
    lines.append("")
    lines.append(f"Generated at (UTC): {payload['generated_at']}")
    lines.append("")
    lines.append(f"Total: {len(untagged)}")
    lines.append("")

    for item in sorted(untagged, key=lambda x: (x["platform"], x["contest"], x["problem"])):
        title = item["title"] or "(no title)"
        source_url = item.get("source_url") or ""
        pid = f"{item['platform']}/{item['contest']}/{item['problem']}"
        pid_cell = f"[{pid}]({source_url})" if source_url else pid
        code_path = item["path"]
        code_cell = f"[{code_path}]({code_path})"
        lines.append(f"- {pid_cell}: {title} | code={code_cell} | path={code_path}")

    OUTPUT_UNTAGGED_MD.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main():
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    payload = build_records()

    OUTPUT_JSON.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    write_problems_md(payload)
    write_by_tag_md(payload)
    write_untagged_md(payload)

    print(
        "Updated "
        f"{OUTPUT_JSON.relative_to(ROOT)}, "
        f"{OUTPUT_MD.relative_to(ROOT)}, "
        f"{OUTPUT_TAG_MD.relative_to(ROOT)}, "
        f"{OUTPUT_UNTAGGED_MD.relative_to(ROOT)}, "
        f"{ANNOTATION_TSV.relative_to(ROOT)}"
    )


if __name__ == "__main__":
    main()
