from pathlib import Path
import gzip
p=Path(__file__).resolve().parent
s=(p/"page.h").read_text(encoding="utf-8")
a=s.index("<!doctype html>")
z=s.index("</html>",a)+len("</html>")
data=gzip.compress(s[a:z].encode("utf-8"),compresslevel=9,mtime=0)
lines=["  "+", ".join(str(x) for x in data[i:i+24])+"," for i in range(0,len(data),24)]
(p/"page_gzip.h").write_text("#pragma once\n#include <Arduino.h>\n// Genere depuis page.h par generer_page.py.\nconst uint8_t PAGE_GZIP[] PROGMEM = {\n"+"\n".join(lines)+"\n};\n",encoding="utf-8")
print("Page compressee :",len(data),"octets")
