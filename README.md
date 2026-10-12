# asmcore

Kernbibliothek des [KRON-1-Assemblers](https://github.com/Hirschi247/assembler): eine statische
C-Bibliothek, die Assembler-Quelltext in 36-Bit-Befehlswörter für die KRON-1-CPU übersetzt.

| Modul               | Aufgabe |
|---------------------|---------|
| `scanner/`          | Zerlegt den Quelltext in Tokens (Mnemonics, Register, Zahlen, Labels, Satzzeichen). |
| `encode/`           | Kodiert eine Token-Zeile anhand der ISA-Tabelle in ein Befehlswort. |
| `symtab/`           | Symboltabelle für Labels und ihre Befehlsadressen. |
| `Helpers/Dictionary/` | Einfache Hashtabelle (verkettete Buckets), auf der die Symboltabelle aufbaut. |

Die Befehlsdefinitionen (Opcodes, Bitfelder, `isa_table`) kommen aus der Bibliothek
[kron-1-isa](https://github.com/Hirschi247/kron-1-isa), gegen die `asmcore` gelinkt wird.

Benutzung, Quelltext-Syntax sowie Einrichten und Bauen aller drei Repositories sind in der
[README des Assemblers](https://github.com/Hirschi247/assembler#readme) beschrieben.

## Lizenz

[MIT](LICENSE) © 2026 Simon Hirsch
