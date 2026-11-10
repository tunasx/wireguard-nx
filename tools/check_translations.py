"""Ensure translated printf strings preserve argument types and order."""
import json
import re
from pathlib import Path

source = Path("common/include/wgnx/i18n.hpp").read_text()
entries = re.findall(r'\{("(?:[^"\\]|\\.)*"), ("(?:[^"\\]|\\.)*")\}', source)
formats = re.compile(r'%(?:[-+ #0]*\d*(?:\.\d+)?[hljztL]*[diuoxXfFeEgGaAcsp]|%)')
seen = set()
for left, right in entries:
    english, chinese = json.loads(left), json.loads(right)
    assert english not in seen, f"Duplicate key: {english}"
    seen.add(english)
    assert formats.findall(english) == formats.findall(chinese), english
    assert chinese, english
assert len(entries) > 80
print(f"Validated {len(entries)} translations and printf placeholders")
