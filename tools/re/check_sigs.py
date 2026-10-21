#!/usr/bin/env python3
"""Check every signature in GotSignatures.ini against a GhostOfTsushima.exe file, offline.

usage: python3 -I check_sigs.py <GhostOfTsushima.exe> <GotSignatures.ini> [--json out.json]

Maps the PE sections the way Windows loads them, then for each signature reports:
  FOUND  (unique match)      address as exe+RVA, plus the decoded target
  MULTI  (several matches)   the first is what the plugin will use; worth tightening
  NOT FOUND                  different game build or a stale signature
"""
import json, re, struct, sys


def load_image(path):
    d = open(path, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3C)[0]
    assert d[pe:pe + 4] == b'PE\0\0', 'not a PE file'
    nsec, = struct.unpack_from('<H', d, pe + 6)
    ts, = struct.unpack_from('<I', d, pe + 8)
    optsz, = struct.unpack_from('<H', d, pe + 20)
    opt = pe + 24
    size_img, = struct.unpack_from('<I', d, opt + 56)
    img = bytearray(size_img)
    img[:0x1000] = d[:0x1000]
    exec_ranges = []
    for i in range(nsec):
        s = opt + optsz + i * 40
        name = d[s:s + 8].rstrip(b'\0').decode(errors='replace')
        vsz, va, rsz, rptr, = struct.unpack_from('<IIII', d, s + 8)
        chars, = struct.unpack_from('<I', d, s + 36)
        n = min(rsz, vsz) if vsz else rsz
        img[va:va + n] = d[rptr:rptr + n]
        exec_ranges.append((name, va, max(vsz, rsz), chars))
    return bytes(img), ts, exec_ranges


def to_regex(pat):
    out = b''
    for t in pat.split():
        out += b'.' if t in ('?', '??', '*', '**') else re.escape(bytes([int(t, 16)]))
    return re.compile(out, re.DOTALL)


def parse_ini(path):
    sigs, sec = [], ''
    for line in open(path, encoding='utf-8'):
        line = line.strip()
        if not line or line[0] in ';#':
            continue
        if line.startswith('['):
            sec = line.strip('[]').lower()
            continue
        if sec != 'signatures' or '=' not in line:
            continue
        name, v = (x.strip() for x in line.split('=', 1))
        parts = [p.strip() for p in v.split('|')]
        a = int(parts[1], 0) if len(parts) > 1 and parts[1] else 0
        b = int(parts[2], 0) if len(parts) > 2 and parts[2] else 0
        sigs.append((name, parts[0], a, b))
    return sigs


def main():
    exe, ini = sys.argv[1], sys.argv[2]
    img, ts, secs = load_image(exe)
    print(f'{exe}: image size 0x{len(img):x}, build timestamp 0x{ts:08X}')
    res, counts = [], {'FOUND': 0, 'MULTI': 0, 'NOT FOUND': 0, 'UNKNOWN': 0}
    for name, pat, a, b in parse_ini(ini):
        if not pat:
            counts['UNKNOWN'] += 1
            res.append(dict(name=name, status='UNKNOWN'))
            continue
        hits = [m.start() for m in to_regex(pat).finditer(img)]
        if not hits:
            counts['NOT FOUND'] += 1
            res.append(dict(name=name, status='NOT FOUND'))
            print(f'  NOT FOUND  {name}')
            continue
        h = hits[0]
        target = h + a if b == 0 else h + b + struct.unpack_from('<i', img, h + a)[0]
        st = 'FOUND' if len(hits) == 1 else 'MULTI'
        counts[st] += 1
        res.append(dict(name=name, status=st, matches=len(hits), match_rva=h, target_rva=target))
        extra = f' ({len(hits)} matches)' if len(hits) > 1 else ''
        print(f'  {st:9s}  {name:32s} match exe+0x{h:x}  -> exe+0x{target:x}{extra}')
    print('summary:', counts)
    if '--json' in sys.argv:
        json.dump(dict(timestamp=ts, results=res), open(sys.argv[sys.argv.index('--json') + 1], 'w'), indent=1)


if __name__ == '__main__':
    main()
