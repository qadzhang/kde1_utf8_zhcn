/* utf8-boundary-fuzz.c -- UTF-8 边界算法的模糊/性质测试（ai-code-testing Layer 4）
 *
 *   Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
 *
 * What: 对本项目三处 UTF-8 边界算法（与源码同逻辑复刻）跑随机模糊输入，
 *       断言三条契约永真：
 *       I1 ksame utf8_trim_boundary（2026-08-31 修订版）：输出以 ASCII 或
 *          完整多字节字符收尾（残缺序列/孤立前导/悬挂续字节均被裁净）
 *       I2 kvt 选区扩张：guard(8) 限内 a 必落非续字节、b 右无悬挂续字节
 *       I3 kfmman 字符切分：段平铺无损（拼接 == 原流）、段长 ≥1（无死循环）
 * Why : 算法吃的是用户可控字节流（文件名/终端屏幕/高分名），畸形 UTF-8
 *       不得引起死循环/越界/截断错位——本 harness 曾抓出首版 trim 误删
 *       完整尾字符的真缺陷（200 万样本复现 50 万次），证明了其价值
 * How : xorshift 生成 200 万条随机字节流（合法 UTF-8/畸形截断/孤立续
 *       字节/全随机混合），逐一跑三个算法并断言契约
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long rng_state = 0x20260831;
static unsigned long xr(void)
{
    rng_state ^= rng_state << 13; rng_state ^= rng_state >> 7; rng_state ^= rng_state << 17;
    return rng_state;
}

static int is_cont(unsigned char c) { return (c & 0xC0) == 0x80; }

/* ── 与 kfmman writeWrapped / ksame 同款前导字节→序列长度 ── */
static int seq_len(unsigned char b)
{
    if ((b & 0xE0) == 0xC0) return 2;
    if ((b & 0xF0) == 0xE0) return 3;
    if ((b & 0xF8) == 0xF0) return 4;
    return 1;
}

/* ── 与 ksame/HighScore.cpp 同逻辑（2026-08-31 修订版）：只裁尾部残缺
   序列，完整尾字符保留；孤立悬挂续字节裁到声明长度 ── */
static void utf8_trim_boundary(char *buf)
{
    for (;;) {
        int L = strlen(buf);
        if (L <= 0) return;
        unsigned char last = (unsigned char)buf[L-1];
        if (is_cont(last)) {
            int p = L - 1;
            while (p >= 0 && is_cont((unsigned char)buf[p])) p--;
            if (p < 0) { buf[0] = 0; return; }
            int need = seq_len((unsigned char)buf[p]);
            int conts = need - 1;
            int have = L - 1 - p;
            if (have < conts) { buf[p] = 0; continue; }
            if (have > conts) { buf[p + need] = 0; continue; }
            return;                                   /* 完整收尾 */
        }
        if (last >= 0xC0 && last < 0xF8) { buf[L-1] = 0; continue; }
        return;                                       /* ASCII / 无效前导单字节 */
    }
}

/* ── 与 kvt/screen.c 同逻辑：选区起止列扩张到字符边界 ── */
static void sel_expand(unsigned const char *row, int width, int *pa, int *pb)
{
    int a = *pa, b = *pb, guard;
    for (guard = 0; guard < 8 && a > 0 && is_cont(row[a]); a--, guard++) ;
    for (guard = 0; guard < 8 && b < width - 1 && is_cont(row[b+1]); b++, guard++) ;
    *pa = a; *pb = b;
}

/* ── 与 kwrite/kwdoc.cpp 同逻辑（2026-08-31 会话新增）：字符边界辅助 ── */
static int kw_charLen(const unsigned char *text, int len, int pos)
{
    if (pos < 0 || pos >= len) return 1;
    unsigned char c = text[pos];
    if ((c & 0xC0) == 0x80) return 1;
    int l = seq_len(c);
    if (pos + l > len) l = len - pos;
    for (int k = 1; k < l; k++)              /* 2026-08-31 修复：按实际续字节收敛 */
        if ((text[pos+k] & 0xC0) != 0x80) { l = k; break; }
    return l;
}
static int kw_charLenBefore(const unsigned char *text, int len, int pos)
{
    if (pos <= 0) return 1;
    if (pos > len) pos = len;
    int p = pos - 1;
    if (!is_cont(text[p])) return kw_charLen(text, len, p);
    int q = p, guard = 0;
    while (q > 0 && guard < 3 && is_cont(text[q])) { q--; guard++; }
    if (is_cont(text[q])) return 1;
    int cl = kw_charLen(text, len, q);
    if (q + cl == pos) return cl;
    return 1;
}

int main(void)
{
    long f4 = 0, f5 = 0;
    unsigned char buf[64], out[80], rebuilt[80];
    long f1 = 0, f2 = 0, f3 = 0;

    for (long iter = 0; iter < 2000000; iter++) {
        int n = (int)(xr() % 40);
        for (int i = 0; i < n; i++) {
            unsigned long r = xr() % 4;
            if (r == 0) buf[i] = (unsigned char)(xr() % 0x80);             // ASCII
            else if (r == 1) buf[i] = (unsigned char)(0xC0 + xr() % 0x40); // 前导
            else if (r == 2) buf[i] = (unsigned char)(0x80 + xr() % 0x40); // 续字节
            else buf[i] = (unsigned char)(xr() & 0xFF);                    // 全随机
        }
        buf[n] = 0;

        /* ── I1：trim 契约（收尾 = ASCII 或完整序列）── */
        memcpy(out, buf, n + 1);
        if (n > 16) { memcpy(out, buf, 16); out[16] = 0; }  /* 模拟 strncpy(…,16)+[16]=0 */
        utf8_trim_boundary((char*)out);
        int L = strlen((char*)out);
        if (L > 16) f1++;                       /* trim 只会变短 */
        if (L > 0) {
            unsigned char last = out[L-1];
            if (is_cont(last)) {
                int p = L - 1;                  /* 续字节收尾：run 须恰好满足前导声明 */
                while (p >= 0 && is_cont(out[p])) p--;
                if (p < 0 || (L - 1 - p) != seq_len(out[p]) - 1) f1++;
            }
            else if (last >= 0xC0 && last < 0xF8) f1++;  /* 仍是缺后续的有效前导
                （0xF8+ 是无效前导，算法按单字节保留显示为替换符——契约允许） */
        }

        /* ── I2：选区扩张契约 ── */
        if (n >= 2) {
            int a = (int)(xr() % n), b = (int)(xr() % n);
            if (a > b) { int t = a; a = b; b = t; }
            int aorig = a, borig = b;
            sel_expand(buf, n, &a, &b);
            int runL = 0, t = aorig;
            while (t > 0 && is_cont(buf[t]) && runL <= 9) { t--; runL++; }
            if (a > 0 && is_cont(buf[a]) && runL <= 8) f2++;
            int runR = 0; t = borig + 1;
            while (t < n && is_cont(buf[t]) && runR <= 9) { t++; runR++; }
            if (b + 1 < n && is_cont(buf[b+1]) && runR <= 8) f2++;
            if (a < 0 || b >= n || a > b) f2++;               /* 不越界、次序保持 */
        }

        /* ── I3：字符段切分契约（与真实 writeWrapped 一致：盲切——孤立续
           字节按 1 字节段处理，平铺天然无损）── */
        {
            int p = 0, o = 0;
            while (p < n) {
                int l = seq_len(buf[p]);      /* 续字节落在段首时按 1（防死循环） */
                if (l < 1) l = 1;
                if (p + l > n) l = n - p;     /* 尾部残缺截到行尾 */
                for (int k = 0; k < l; k++) rebuilt[o++] = buf[p + k];
                p += l;
            }
            if (o != n) f3++;                              /* 平铺全覆盖 */
            else if (memcmp(rebuilt, buf, n) != 0) f3++;   /* 无损 */
        }
        /* ── I4：kwrite charLen 划分契约：0 出发按 charLen 步进恰好收口 len ── */
        {
            int z = 0, steps = 0;
            while (z < n && steps <= n) { z += kw_charLen(buf, n, z); steps++; }
            if (z != n || steps > n) f4++;      /* 不精确收口 = 划分失败 */
        }

        /* ── I5：kwrite 光标移动蜕变契约：左移再右移必回原位 ──
           （退-进往返不变量；孤立续字节按单字符语义往返） */
        {
            /* pos 按字符分区取合法光标态（kwrite 光标只会停在边界）：
               从 0 按 charLen 步进收集边界，随机挑一个 >0 的 */
            int bnd[64], nb = 1; bnd[0] = 0;
            { int z = 0; while (z < n && nb < 64) { z += kw_charLen(buf, n, z); bnd[nb++] = z; } }
            if (nb < 2) continue;                    /* 空行无光标移动 */
            int pos = bnd[1 + (int)(xr() % (nb - 1))];
            int back = kw_charLenBefore(buf, n, pos);
            if (back < 1 || back > 4) f5++;
            int p2 = pos - back;
            if (p2 < 0) f5++;
            else {
                int fwd = kw_charLen(buf, n, p2);
                if (p2 + fwd != pos) {
                    if (f5 < 1) {
                        fprintf(stderr, "反例: n=%d pos=%d back=%d p2=%d fwd=%d bytes=", n, pos, back, p2, fwd);
                        for (int d = 0; d < n; d++) fprintf(stderr, "%02X ", buf[d]);
                        fprintf(stderr, "\n");
                    }
                    f5++;
                }
            }
        }
    }
    long fails = f1 + f2 + f3 + f4 + f5;
    printf(fails ? "FAIL %ld (I1=%ld I2=%ld I3=%ld I4=%ld I5=%ld)\n"
                 : "ALL PASS（200 万模糊样本 × 5 契约）\n",
           fails, f1, f2, f3, f4, f5);
    return fails ? 1 : 0;
}
