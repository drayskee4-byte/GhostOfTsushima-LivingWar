#!/usr/bin/env python3
"""Minimal ECMA-335 (.NET) metadata + IL reader.

Lists types, fields, methods and, per method, the string literals it loads
and the members it calls/references. Read-only static analysis.

usage: python3 -I dotnet_inspect.py <assembly.dll> <out_dir>
"""
import struct, sys, os, json
from collections import defaultdict

# ---------------------------------------------------------------- PE
class PE:
    def __init__(self, data):
        self.d = data
        pe = struct.unpack_from('<I', data, 0x3C)[0]
        assert data[pe:pe+4] == b'PE\0\0'
        nsec = struct.unpack_from('<H', data, pe+6)[0]
        optsz = struct.unpack_from('<H', data, pe+20)[0]
        opt = pe + 24
        magic = struct.unpack_from('<H', data, opt)[0]
        ddir = opt + (112 if magic == 0x20b else 96)
        self.clr_rva, self.clr_size = struct.unpack_from('<II', data, ddir + 14*8)
        self.secs = []
        s = opt + optsz
        for i in range(nsec):
            vsz, va, rsz, rptr = struct.unpack_from('<IIII', data, s + i*40 + 8)
            self.secs.append((va, max(vsz, rsz), rptr))

    def off(self, rva):
        for va, sz, rp in self.secs:
            if va <= rva < va + sz:
                return rva - va + rp
        raise ValueError(hex(rva))

# ---------------------------------------------------------------- schema
U1, U2, U4, STR, GUID, BLOB = 'u1', 'u2', 'u4', 'str', 'guid', 'blob'
def T(n): return ('t', n)
def C(n): return ('c', n)

CODED = {
    'TypeDefOrRef': (2, [2, 1, 27]),
    'HasConstant': (2, [4, 8, 23]),
    'HasCustomAttribute': (5, [6, 4, 1, 2, 8, 9, 10, 0, 14, 23, 20, 17, 26, 27, 32, 35, 38, 39, 40, 42, 44, 43]),
    'HasFieldMarshal': (1, [4, 8]),
    'HasDeclSecurity': (2, [2, 6, 32]),
    'MemberRefParent': (3, [2, 1, 26, 6, 27]),
    'HasSemantics': (1, [20, 23]),
    'MethodDefOrRef': (1, [6, 10]),
    'MemberForwarded': (1, [4, 6]),
    'Implementation': (2, [38, 35, 39]),
    'CustomAttributeType': (3, [None, None, 6, 10, None]),
    'ResolutionScope': (2, [0, 26, 35, 1]),
    'TypeOrMethodDef': (1, [2, 6]),
}

SCHEMA = {
    0: ('Module', [U2, STR, GUID, GUID, GUID]),
    1: ('TypeRef', [C('ResolutionScope'), STR, STR]),
    2: ('TypeDef', [U4, STR, STR, C('TypeDefOrRef'), T(4), T(6)]),
    3: ('FieldPtr', [T(4)]),
    4: ('Field', [U2, STR, BLOB]),
    5: ('MethodPtr', [T(6)]),
    6: ('MethodDef', [U4, U2, U2, STR, BLOB, T(8)]),
    7: ('ParamPtr', [T(8)]),
    8: ('Param', [U2, U2, STR]),
    9: ('InterfaceImpl', [T(2), C('TypeDefOrRef')]),
    10: ('MemberRef', [C('MemberRefParent'), STR, BLOB]),
    11: ('Constant', [U2, C('HasConstant'), BLOB]),
    12: ('CustomAttribute', [C('HasCustomAttribute'), C('CustomAttributeType'), BLOB]),
    13: ('FieldMarshal', [C('HasFieldMarshal'), BLOB]),
    14: ('DeclSecurity', [U2, C('HasDeclSecurity'), BLOB]),
    15: ('ClassLayout', [U2, U4, T(2)]),
    16: ('FieldLayout', [U4, T(4)]),
    17: ('StandAloneSig', [BLOB]),
    18: ('EventMap', [T(2), T(20)]),
    19: ('EventPtr', [T(20)]),
    20: ('Event', [U2, STR, C('TypeDefOrRef')]),
    21: ('PropertyMap', [T(2), T(23)]),
    22: ('PropertyPtr', [T(23)]),
    23: ('Property', [U2, STR, BLOB]),
    24: ('MethodSemantics', [U2, T(6), C('HasSemantics')]),
    25: ('MethodImpl', [T(2), C('MethodDefOrRef'), C('MethodDefOrRef')]),
    26: ('ModuleRef', [STR]),
    27: ('TypeSpec', [BLOB]),
    28: ('ImplMap', [U2, C('MemberForwarded'), STR, T(26)]),
    29: ('FieldRVA', [U4, T(4)]),
    30: ('EncLog', [U4, U4]),
    31: ('EncMap', [U4]),
    32: ('Assembly', [U4, U2, U2, U2, U2, U4, BLOB, STR, STR]),
    33: ('AssemblyProcessor', [U4]),
    34: ('AssemblyOS', [U4, U4, U4]),
    35: ('AssemblyRef', [U2, U2, U2, U2, U4, BLOB, STR, STR, BLOB]),
    36: ('AssemblyRefProcessor', [U4, T(35)]),
    37: ('AssemblyRefOS', [U4, U4, U4, T(35)]),
    38: ('File', [U4, STR, BLOB]),
    39: ('ExportedType', [U4, U4, STR, STR, C('Implementation')]),
    40: ('ManifestResource', [U4, U4, STR, C('Implementation')]),
    41: ('NestedClass', [T(2), T(2)]),
    42: ('GenericParam', [U2, U2, C('TypeOrMethodDef'), STR]),
    43: ('MethodSpec', [C('MethodDefOrRef'), BLOB]),
    44: ('GenericParamConstraint', [T(42), C('TypeDefOrRef')]),
}

# ---------------------------------------------------------------- metadata
class Meta:
    def __init__(self, path):
        self.data = open(path, 'rb').read()
        self.pe = PE(self.data)
        d = self.data
        clr = self.pe.off(self.pe.clr_rva)
        md_rva, md_size = struct.unpack_from('<II', d, clr + 8)
        root = self.pe.off(md_rva)
        assert d[root:root+4] == b'BSJB'
        vlen = struct.unpack_from('<I', d, root + 12)[0]
        p = root + 16 + vlen
        nstreams = struct.unpack_from('<H', d, p + 2)[0]
        p += 4
        self.streams = {}
        for _ in range(nstreams):
            off, size = struct.unpack_from('<II', d, p)
            p += 8
            e = d.index(b'\0', p)
            name = d[p:e].decode()
            p = (e + 4) & ~3
            self.streams[name] = (root + off, size)
        self._tables()

    def _tables(self):
        d = self.data
        base, _ = self.streams.get('#~') or self.streams['#-']
        heaps = d[base + 6]
        self.sw = 4 if heaps & 1 else 2
        self.gw = 4 if heaps & 2 else 2
        self.bw = 4 if heaps & 4 else 2
        valid = struct.unpack_from('<Q', d, base + 8)[0]
        p = base + 24
        self.rows = defaultdict(int)
        present = [i for i in range(64) if valid >> i & 1]
        for i in present:
            self.rows[i] = struct.unpack_from('<I', d, p)[0]
            p += 4
        if heaps & 0x40:  # extra data
            p += 4
        self.tab = {}
        for i in present:
            name, cols = SCHEMA[i]
            sizes = [self._csize(c) for c in cols]
            rsz = sum(sizes)
            rows = []
            for r in range(self.rows[i]):
                q = p + r * rsz
                row = []
                for c, s in zip(cols, sizes):
                    v = d[q] if s == 1 else struct.unpack_from('<H' if s == 2 else '<I', d, q)[0]
                    q += s
                    row.append(v)
                rows.append(row)
            self.tab[i] = rows
            p += rsz * self.rows[i]

    def _csize(self, c):
        if c == U1: return 1
        if c == U2: return 2
        if c == U4: return 4
        if c == STR: return self.sw
        if c == GUID: return self.gw
        if c == BLOB: return self.bw
        kind, arg = c
        if kind == 't':
            return 2 if self.rows[arg] < 0x10000 else 4
        bits, tables = CODED[arg]
        mx = max(self.rows[t] for t in tables if t is not None)
        return 2 if mx < (1 << (16 - bits)) else 4

    # heaps
    def s(self, idx):
        base, _ = self.streams['#Strings']
        e = self.data.index(b'\0', base + idx)
        return self.data[base + idx:e].decode('utf-8', 'replace')

    def us(self, idx):
        base, _ = self.streams['#US']
        p = base + idx
        n, hl = self._blen(p)
        raw = self.data[p + hl:p + hl + n - 1] if n else b''
        return raw.decode('utf-16-le', 'replace')

    def blob(self, idx):
        base, _ = self.streams['#Blob']
        p = base + idx
        n, hl = self._blen(p)
        return self.data[p + hl:p + hl + n]

    def _blen(self, p):
        b = self.data[p]
        if b & 0x80 == 0: return b, 1
        if b & 0xC0 == 0x80: return ((b & 0x3F) << 8) | self.data[p+1], 2
        return ((b & 0x1F) << 24) | (self.data[p+1] << 16) | (self.data[p+2] << 8) | self.data[p+3], 4

    def decode_coded(self, kind, v):
        bits, tables = CODED[kind]
        return tables[v & ((1 << bits) - 1)], v >> bits

    # names
    def typedef_name(self, i):  # 1-based
        r = self.tab[2][i-1]
        ns, n = self.s(r[2]), self.s(r[1])
        enc = self.nest_parent.get(i)
        if enc:
            return self.typedef_name(enc) + '/' + n
        return (ns + '.' if ns else '') + n

    def typeref_name(self, i):
        r = self.tab[1][i-1]
        ns, n = self.s(r[2]), self.s(r[1])
        return (ns + '.' if ns else '') + n

    def type_name(self, table, i):
        if table == 2: return self.typedef_name(i)
        if table == 1: return self.typeref_name(i)
        if table == 27: return 'TypeSpec#%d' % i
        if table == 26: return 'module:' + self.s(self.tab[26][i-1][0])
        if table == 6: return self.method_full(i)
        return '?%d:%d' % (table, i)

    def build_index(self):
        self.nest_parent = {r[0]: r[1] for r in self.tab.get(41, [])}
        # method -> owner, field -> owner
        self.m_owner, self.f_owner = {}, {}
        td = self.tab[2]
        nm, nf = self.rows[6], self.rows[4]
        for i, r in enumerate(td, 1):
            mend = td[i][5] if i < len(td) else nm + 1
            fend = td[i][4] if i < len(td) else nf + 1
            for m in range(r[5], mend): self.m_owner[m] = i
            for f in range(r[4], fend): self.f_owner[f] = i

    def method_full(self, m):
        return self.typedef_name(self.m_owner[m]) + '::' + self.s(self.tab[6][m-1][3])

    def field_full(self, f):
        return self.typedef_name(self.f_owner[f]) + '::' + self.s(self.tab[4][f-1][1])

    def memberref_full(self, i):
        r = self.tab[10][i-1]
        t, idx = self.decode_coded('MemberRefParent', r[0])
        if t == 27:
            parent = self.typespec_name(idx)
        else:
            parent = self.type_name(t, idx)
        return parent + '::' + self.s(r[1])

    def typespec_name(self, i):
        b = self.blob(self.tab[27][i-1][0])
        # GENERICINST (0x15) CLASS/VALUETYPE TypeDefOrRefEncoded ...
        if b and b[0] == 0x15:
            p = 2
            v, n = self._cint(b, p)
            t, idx = [2, 1, 27][v & 3], v >> 2
            return self.type_name(t, idx) + '<..>'
        return 'TypeSpec#%d' % i

    @staticmethod
    def _cint(b, p):
        x = b[p]
        if x & 0x80 == 0: return x, 1
        if x & 0xC0 == 0x80: return ((x & 0x3F) << 8) | b[p+1], 2
        return ((x & 0x1F) << 24) | (b[p+1] << 16) | (b[p+2] << 8) | b[p+3], 4

    def token(self, tok):
        t, i = tok >> 24, tok & 0xFFFFFF
        try:
            if t == 0x06: return self.method_full(i)
            if t == 0x0A: return self.memberref_full(i)
            if t == 0x04: return self.field_full(i)
            if t == 0x02: return self.typedef_name(i)
            if t == 0x01: return self.typeref_name(i)
            if t == 0x1B: return self.typespec_name(i)
            if t == 0x2B:
                ct, ci = self.decode_coded('MethodDefOrRef', self.tab[43][i-1][0])
                return (self.method_full(ci) if ct == 6 else self.memberref_full(ci)) + '<..>'
        except Exception:
            pass
        return 'tok:%08x' % tok

# ---------------------------------------------------------------- IL
OP1 = {}
for o in range(0x0E, 0x14): OP1[o] = 1
OP1.update({0x1F: 1, 0x20: 4, 0x21: 8, 0x22: 4, 0x23: 8, 0x27: 'tok', 0x28: 'tok', 0x29: 'tok',
            0xDD: 4, 0xDE: 1})
for o in range(0x2B, 0x38): OP1[o] = 1
for o in range(0x38, 0x45): OP1[o] = 4
for o in (0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x79, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81,
          0x8C, 0x8D, 0x8F, 0xA3, 0xA4, 0xA5, 0xC2, 0xC6, 0xD0):
    OP1[o] = 'tok'
OP2 = {0x06: 'tok', 0x07: 'tok', 0x09: 2, 0x0A: 2, 0x0B: 2, 0x0C: 2, 0x0D: 2, 0x0E: 2,
       0x12: 1, 0x15: 'tok', 0x16: 'tok', 0x19: 1, 0x1C: 'tok'}
CALLS = {0x28, 0x6F, 0x73}
FIELDS = {0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80}

def method_body(meta, rva):
    d = meta.data
    p = meta.pe.off(rva)
    b = d[p]
    if b & 3 == 2:
        return d[p+1:p+1+(b >> 2)]
    hsz = (d[p+1] >> 4) * 4
    size = struct.unpack_from('<I', d, p+4)[0]
    return d[p+hsz:p+hsz+size]

def scan_il(meta, code):
    strs, calls, fields, i64 = [], [], [], []
    p = 0
    n = len(code)
    while p < n:
        op = code[p]; p += 1
        if op == 0xFE:
            op2 = code[p]; p += 1
            k = OP2.get(op2, 0)
            if k == 'tok':
                tok = struct.unpack_from('<I', code, p)[0]
                if op2 in (0x06, 0x07): calls.append(meta.token(tok))
                p += 4
            else:
                p += k
            continue
        if op == 0x45:
            cnt = struct.unpack_from('<I', code, p)[0]
            p += 4 + 4 * cnt
            continue
        k = OP1.get(op, 0)
        if k == 'tok':
            tok = struct.unpack_from('<I', code, p)[0]
            if op == 0x72: strs.append(meta.us(tok & 0xFFFFFF))
            elif op in CALLS: calls.append(meta.token(tok))
            elif op in FIELDS: fields.append(meta.token(tok))
            p += 4
        else:
            if op == 0x21: i64.append(struct.unpack_from('<Q', code, p)[0])
            p += k
    return strs, calls, fields, i64

# ---------------------------------------------------------------- main
def main(path, out):
    os.makedirs(out, exist_ok=True)
    m = Meta(path)
    m.build_index()
    asm = m.tab.get(32, [[0]*9])[0]
    info = {
        'assembly': m.s(asm[7]) if 32 in m.tab else '?',
        'version': '.'.join(map(str, asm[1:5])) if 32 in m.tab else '?',
        'references': ['%s %d.%d.%d.%d' % (m.s(r[6]), r[0], r[1], r[2], r[3]) for r in m.tab.get(35, [])],
        'resources': [m.s(r[2]) for r in m.tab.get(40, [])],
        'counts': {SCHEMA[k][0]: v for k, v in m.rows.items() if v},
    }
    json.dump(info, open(os.path.join(out, 'assembly.json'), 'w'), indent=2)

    types = []
    allstr = []
    native_hashes = defaultdict(set)
    ext_calls = defaultdict(int)
    for ti, r in enumerate(m.tab[2], 1):
        tname = m.typedef_name(ti)
        fields = [m.s(m.tab[4][f-1][1]) for f, o in m.f_owner.items() if o == ti]
        methods = []
        for mi, o in m.m_owner.items():
            if o != ti: continue
            mr = m.tab[6][mi-1]
            name = m.s(mr[3])
            entry = {'name': name}
            if mr[0]:
                try:
                    code = method_body(m, mr[0])
                    s, c, f, h = scan_il(m, code)
                    entry.update(il_bytes=len(code), strings=s, calls=sorted(set(c)), fields=sorted(set(f)))
                    if h: entry['int64'] = ['0x%016X' % x for x in h]
                    for x in s: allstr.append((tname + '::' + name, x))
                    for x in c:
                        if not x.startswith('NemesisSystem'): ext_calls[x] += 1
                    for x in h: native_hashes[x].add(tname + '::' + name)
                except Exception as e:
                    entry['error'] = str(e)
            methods.append(entry)
        types.append({'type': tname, 'flags': hex(r[0]), 'fields': sorted(fields), 'methods': methods})
    json.dump(types, open(os.path.join(out, 'types.json'), 'w'), indent=1)
    with open(os.path.join(out, 'strings.tsv'), 'w') as fh:
        for a, b in allstr:
            fh.write(a + '\t' + b.replace('\n', '\\n').replace('\t', '\\t') + '\n')
    with open(os.path.join(out, 'external_calls.tsv'), 'w') as fh:
        for k, v in sorted(ext_calls.items(), key=lambda x: -x[1]):
            fh.write('%d\t%s\n' % (v, k))
    with open(os.path.join(out, 'int64_constants.tsv'), 'w') as fh:
        for k, v in sorted(native_hashes.items()):
            fh.write('0x%016X\t%s\n' % (k, ', '.join(sorted(v))))
    # human summary
    with open(os.path.join(out, 'types.txt'), 'w') as fh:
        for t in types:
            fh.write('%s  (%d fields, %d methods)\n' % (t['type'], len(t['fields']), len(t['methods'])))
            if t['fields']: fh.write('   fields: ' + ', '.join(t['fields']) + '\n')
            fh.write('   methods: ' + ', '.join(x['name'] for x in t['methods']) + '\n')
    print(json.dumps(info, indent=1))
    print('types:', len(types), 'methods:', sum(len(t['methods']) for t in types), 'strings:', len(allstr))

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
