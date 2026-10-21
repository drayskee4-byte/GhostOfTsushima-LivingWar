# .NET assembly reader

Small, dependency-free ECMA-335 reader used to study enRAGE's `NemesisMod.dll` (with its author's permission).

    python3 -I dotnet_inspect.py <assembly.dll> <out_dir>   # types, fields, methods, strings, calls
    python3 -I il_disasm.py <assembly.dll> Type::Method       # readable IL for one method

The output is analysis material only. It is not part of Living War, and nothing it produces belongs in this repository or a release.
