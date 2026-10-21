#!/usr/bin/env python3
"""Disassemble selected methods of a .NET assembly to readable IL.
usage: python3 -I il_disasm.py <dll> <Type::Method or substring> [...]
"""
import sys, struct, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dotnet_inspect import Meta, method_body, OP1, OP2

N1 = {0x00:'nop',0x01:'break',0x02:'ldarg.0',0x03:'ldarg.1',0x04:'ldarg.2',0x05:'ldarg.3',0x06:'ldloc.0',0x07:'ldloc.1',0x08:'ldloc.2',0x09:'ldloc.3',0x0A:'stloc.0',0x0B:'stloc.1',0x0C:'stloc.2',0x0D:'stloc.3',0x0E:'ldarg.s',0x0F:'ldarga.s',0x10:'starg.s',0x11:'ldloc.s',0x12:'ldloca.s',0x13:'stloc.s',0x14:'ldnull',0x15:'ldc.i4.m1',0x16:'ldc.i4.0',0x17:'ldc.i4.1',0x18:'ldc.i4.2',0x19:'ldc.i4.3',0x1A:'ldc.i4.4',0x1B:'ldc.i4.5',0x1C:'ldc.i4.6',0x1D:'ldc.i4.7',0x1E:'ldc.i4.8',0x1F:'ldc.i4.s',0x20:'ldc.i4',0x21:'ldc.i8',0x22:'ldc.r4',0x23:'ldc.r8',0x25:'dup',0x26:'pop',0x27:'jmp',0x28:'call',0x29:'calli',0x2A:'ret',0x2B:'br.s',0x2C:'brfalse.s',0x2D:'brtrue.s',0x2E:'beq.s',0x2F:'bge.s',0x30:'bgt.s',0x31:'ble.s',0x32:'blt.s',0x33:'bne.un.s',0x34:'bge.un.s',0x35:'bgt.un.s',0x36:'ble.un.s',0x37:'blt.un.s',0x38:'br',0x39:'brfalse',0x3A:'brtrue',0x3B:'beq',0x3C:'bge',0x3D:'bgt',0x3E:'ble',0x3F:'blt',0x40:'bne.un',0x41:'bge.un',0x42:'bgt.un',0x43:'ble.un',0x44:'blt.un',0x45:'switch',0x58:'add',0x59:'sub',0x5A:'mul',0x5B:'div',0x5C:'div.un',0x5D:'rem',0x5F:'and',0x60:'or',0x61:'xor',0x62:'shl',0x63:'shr',0x65:'neg',0x66:'not',0x69:'conv.i4',0x6A:'conv.i8',0x6B:'conv.r4',0x6C:'conv.r8',0x6F:'callvirt',0x72:'ldstr',0x73:'newobj',0x74:'castclass',0x75:'isinst',0x7B:'ldfld',0x7C:'ldflda',0x7D:'stfld',0x7E:'ldsfld',0x7F:'ldsflda',0x80:'stsfld',0x8C:'box',0x8D:'newarr',0x8E:'ldlen',0xA5:'unbox.any',0xD0:'ldtoken',0xDD:'leave',0xDE:'leave.s',0xDC:'endfinally'}
N2 = {0x01:'ceq',0x02:'cgt',0x03:'cgt.un',0x04:'clt',0x05:'clt.un',0x06:'ldftn',0x07:'ldvirtftn',0x15:'initobj',0x16:'constrained.'}

def dis(m, code):
    out = []; p = 0
    while p < len(code):
        at = p; op = code[p]; p += 1
        if op == 0xFE:
            o2 = code[p]; p += 1; k = OP2.get(o2, 0); name = N2.get(o2, 'fe%02x' % o2); arg = ''
            if k == 'tok': arg = m.token(struct.unpack_from('<I', code, p)[0]); p += 4
            elif k: arg = str(int.from_bytes(code[p:p+k], 'little')); p += k
            out.append('IL_%04x %s %s' % (at, name, arg)); continue
        name = N1.get(op, 'op%02x' % op); arg = ''
        if op == 0x45:
            n = struct.unpack_from('<I', code, p)[0]; p += 4
            base = p + 4*n
            arg = ','.join('IL_%04x' % (base + struct.unpack_from('<i', code, p+4*i)[0]) for i in range(n)); p += 4*n
        else:
            k = OP1.get(op, 0)
            if k == 'tok':
                tok = struct.unpack_from('<I', code, p)[0]; p += 4
                arg = repr(m.us(tok & 0xFFFFFF)) if op == 0x72 else m.token(tok)
            elif k:
                raw = code[p:p+k]; p += k
                if op == 0x22: arg = '%g' % struct.unpack('<f', raw)[0]
                elif op == 0x23: arg = '%g' % struct.unpack('<d', raw)[0]
                elif op == 0x21: arg = '0x%016X' % struct.unpack('<Q', raw)[0]
                elif 0x2B <= op <= 0x37 or op == 0xDE: arg = 'IL_%04x' % (p + struct.unpack('<b', raw)[0])
                elif 0x38 <= op <= 0x44 or op == 0xDD: arg = 'IL_%04x' % (p + struct.unpack('<i', raw)[0])
                elif op == 0x1F: arg = str(struct.unpack('<b', raw)[0])
                elif op == 0x20: arg = str(struct.unpack('<i', raw)[0])
                else: arg = str(int.from_bytes(raw, 'little'))
        out.append('IL_%04x %s %s' % (at, name, arg))
    return out

def main():
    m = Meta(sys.argv[1]); m.build_index()
    for q in sys.argv[2:]:
        exact = '::' in q
        for mi in range(1, m.rows[6] + 1):
            full = m.method_full(mi)
            if (full.endswith(q) if exact else q in full):
                r = m.tab[6][mi-1]
                if not r[0]: continue
                print('=' * 8, full)
                print('\n'.join(dis(m, method_body(m, r[0]))))

if __name__ == '__main__':
    main()
