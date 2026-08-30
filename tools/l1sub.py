#!/usr/bin/env python3
# Latin-1 历史源码安全替换工具：绝不截断原文件（先写临时文件、编码成功后原子替换）
# 用法: l1sub.py <文件> <旧串> <新串>   （串从 stdin 读 JSON {"old":..,"new":..} 亦支持单对参数）
import sys, os, json
path = sys.argv[1]
if len(sys.argv) >= 4:
    old, new = sys.argv[2], sys.argv[3]
else:
    d = json.load(sys.stdin); old, new = d['old'], d['new']
raw = open(path, 'rb').read()
text = raw.decode('latin-1')
cnt = text.count(old)
assert cnt >= 1, f"锚点未找到: {old[:40]}"
data = text.replace(old, new).encode('latin-1')  # 编码成功才继续
tmp = path + '.l1new'
open(tmp, 'wb').write(data)
os.replace(tmp, path)
print(f"OK 替换 {cnt} 处, {len(data)} 字节")
