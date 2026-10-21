#!/usr/bin/env python3
"""Disassemble the whole function containing an RVA, using the exe's .pdata unwind table.
usage: python3 -I fn.py <exe> <rva hex> [maxlines]"""
import struct, subprocess, sys
exe, rva = sys.argv[1], int(sys.argv[2], 16)
maxl = int(sys.argv[3]) if len(sys.argv) > 3 else 400
d = open(exe, 'rb').read()
pe = struct.unpack_from('<I', d, 0x3C)[0]
opt = pe + 24
ib = struct.unpack_from('<Q', d, opt + 24)[0]
nsec = struct.unpack_from('<H', d, pe + 6)[0]; optsz = struct.unpack_from('<H', d, pe + 20)[0]
secs = []
for i in range(nsec):
    s = opt + optsz + i * 40
    vsz, va, rsz, rp = struct.unpack_from('<IIII', d, s + 8); secs.append((va, max(vsz, rsz), rp))
def f(r):
    for va, sz, rp in secs:
        if va <= r < va + sz: return r - va + rp
pd_rva, pd_sz = struct.unpack_from('<II', d, opt + 112 + 3 * 8)
o = f(pd_rva)
best = None
for i in range(pd_sz // 12):
    b, e, u = struct.unpack_from('<III', d, o + i * 12)
    if b <= rva < e: best = (b, e); break
if not best: print('no pdata entry'); sys.exit(1)
b, e = best
print(f'; function exe+0x{b:x} .. exe+0x{e:x} ({e-b} bytes), target exe+0x{rva:x}')
out = subprocess.run(['objdump', '-d', '-M', 'intel', '--no-show-raw-insn', f'--start-address={ib+b}', f'--stop-address={ib+e}', exe], capture_output=True, text=True).stdout
lines = [l for l in out.split('\n') if l.strip() and ':\t' in l]
for l in lines[:maxl]:
    addr = int(l.split(':')[0], 16) - ib
    mark = '>>' if addr == rva else '  '
    print(mark, f'{addr:x}', l.split('\t', 1)[1].strip())
