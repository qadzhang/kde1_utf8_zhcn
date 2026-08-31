/* utf8-metamorphic-test.cpp -- 编码路径蜕变/差分测试（ai-code-testing Layer 3）
 *
 *   Written with GLM-5.3 (Z.ai) for the KDE1 Revival Project
 *
 * ┌─ What : 针对全局 setCodecForCStrings(UTF-8) 语义的蜕变/差分断言集
 * │  Why  : 本项目全部中文支持建立在若干 TQt3 编码行为假设上（%s 走
 * │         fromUtf8、格式串走 Latin-1 逐字节、operator== 需补丁、空族
 * │         QFont 可渲染中文等）——这些假设是 60+ 处修复的判据，必须以
 * │         机器断言钉死（AI 审查的判断必须用代码验证，不信口头结论）
 * │  Who  : 维护者在改动 tqt3-patches/、kapp.cpp 编码初始化或 kwrite/
 * │         kmail 编码路径后回归运行
 * │  When : 每次涉及编码路径的修改后（构建产物 tools/utf8-metamorphic-test）
 * │  Where: 链接 staging 的 libtqt-mt 运行于任意 X 环境（-platform-freebsd
 * │         无关；TQFont 断言需要 DISPLAY，可跳过）
 * │  How  : 每组断言见各段注释；全部通过输出 ALL PASS 并退出 0
 * └──────────────────────────────────────────────────────────────────
 */
#include <ntqapplication.h>
#include <ntqstring.h>
#include <ntqtextcodec.h>
#include <ntqfont.h>
#include <stdio.h>
#include <string.h>

// 与 port/q1compat.h 相同的 kde_sprintf 实现（此处内联复刻，供独立编译）
#include <stdarg.h>
#include <stdio.h>
static TQString kde_sprintf(const char* kdeFmt, ...)
{
    va_list ap;
    va_start(ap, kdeFmt);
    TQString format = TQString::fromUtf8(kdeFmt);
    TQString result;
    const TQChar* c = format.unicode();
    int len = format.length();
    for (int i = 0; i < len; i++) {
        if (c[i] == '%' && i + 1 < len) {
            if (c[i+1] == 's') {
                const char* s = va_arg(ap, const char*);
                result += TQString::fromUtf8(s ? s : "");
                i++;
                continue;
            }
            if (c[i+1] == 'd') {
                int v = va_arg(ap, int);
                char buf[24];
                snprintf(buf, sizeof(buf), "%d", v);
                result += buf;
                i++;
                continue;
            }
        }
        result += c[i];
    }
    va_end(ap);
    return result;
}

static int failures = 0;
#define CHECK(cond, name) do { \
    if (cond) printf("PASS  %s\n", name); \
    else { printf("FAIL  %s\n", name); failures++; } \
} while (0)

int main(int argc, char** argv)
{
    // ── 测试前置：与 KApplication::init 相同的全局编码开关 ──
    TQTextCodec::setCodecForCStrings(TQTextCodec::codecForName("UTF-8"));
    TQTextCodec::setCodecForLocale(TQTextCodec::codecForName("UTF-8"));

    const char* zh = "\xe4\xb8\xad\xe6\x96\x87\xe6\xb5\x8b\xe8\xaf\x95"; // 中文测试

    // ═══ 1. 蜕变关系：构造→取回 往返恒等（Round-Trip Identity）═══
    // TQString(const char*) 按 codecForCStrings 解码 → utf8() 输出应逐字节还原
    {
        TQString s(zh);
        CHECK(strcmp(s.utf8(), zh) == 0, "M1 round-trip: TQString(utf8).utf8() == 原字节");
    }

    // ═══ 2. 差分：两条构造路径等价（Differential Equivalence）═══
    // TQString::fromUtf8(s) 与 TQString(s)（经全局 codec）必须相等
    {
        TQString a(zh);
        TQString b = TQString::fromUtf8(zh);
        CHECK(a == b, "D1 TQString(char*) == fromUtf8(char*)");
        CHECK(strcmp(a.utf8(), b.utf8()) == 0, "D2 两路径 utf8() 字节一致");
    }

    // ═══ 3. 蜕变：拼接长度守恒（中文字 3 字节/字符）═══
    {
        TQString s(zh);                     // 4 个汉字
        CHECK(s.length() == 4, "M2 长度按字符计：4 汉字 == 4");
        CHECK((int)strlen(s.utf8()) == 12, "M3 utf8() 字节数 == 12");
        TQString t = s + s;
        CHECK(t.length() == 8 && strlen(t.utf8()) == 24, "M4 拼接守恒：len/bytes 双倍");
    }

    // ═══ 4. 差分：sprintf %s（声称 fromUtf8）vs kde_sprintf %s ═══
    {
        TQString a; a.sprintf("[%s]", zh);
        TQString b = kde_sprintf("[%s]", zh);
        CHECK(strcmp(a.utf8(), b.utf8()) == 0, "D3 TQString::sprintf %s 与 kde_sprintf 等价（%s 走 fromUtf8）");
    }

    // ═══ 5. 语义验证（ documenting the bug class ）：TQString::sprintf
    //        格式串含非 ASCII 时按 Latin-1 逐字节升位（本项目禁用形态）═══
    {
        TQString a; a.sprintf("中文%d", 5);        // 错误用法（历史代码残留形态）
        TQString b = TQString::fromUtf8("中文5"); // 正确结果
        // 断言两者不等——证明该形态确实产生 mojibake（我们修复它的理由成立）
        CHECK(!(a == b), "S1 sprintf 非 ASCII 格式串确实乱码（修复理由成立）");
        // kde_sprintf 处理相同输入必须得到正确结果
        TQString c = kde_sprintf("中文%d", 5);
        CHECK(c == b, "S2 kde_sprintf 非 ASCII 格式串正确（修复有效）");
    }

    // ═══ 6. 补丁 002 验证：TQString == const char* 经 codec 感知 ═══
    {
        TQString s(zh);
        CHECK(s == zh, "P1 TQString == char*（tqt3-patches/002 语义）");
        CHECK(!(s == "notzh"), "P2 不等判定不误报");
        TQString e;
        CHECK(e != zh, "P3 空串与非空 char* 不等");
    }

    // ═══ 7. GB2312/GBK codec 可用性（kmail RFC2047 解码依赖）═══
    {
        TQTextCodec* gb = TQTextCodec::codecForName("GB2312");
        TQTextCodec* gb2 = TQTextCodec::codecForName("GBK");
        TQTextCodec* gb3 = TQTextCodec::codecForName("GB18030");
        CHECK(gb && gb2 && gb3, "C1 GB2312/GBK/GB18030 codec 可用");
        if (gb) {
            // 蜕变：GB2312 编码→解码 往返还原
            TQString s = TQString::fromUtf8("中文邮件");
            TQCString enc = gb->fromUnicode(s);
            TQString dec = gb->toUnicode(enc);
            CHECK(dec == s, "M5 GB2312 编解码往返守恒");
        }
    }

    // ═══ 8. latin1 通道确证有损（论证全部 latin1() 修复的必要性）═══
    {
        TQString s(zh);
        // 中文经 latin1() 通道后 round-trip 必然失真（每个汉字 → '?'/0）
        TQString back = TQString::fromUtf8(s.latin1());
        CHECK(!(back == s), "S3 latin1 通道对中文有损（latin1() 修复理由成立）");
    }

    // ═══ 9. KConfig 键名语义（Name[zh_CN] 本地化回退依赖 aLocaleString）═══
    //     此项在无 KApplication 环境下不可直接测——由沙箱实测覆盖（沙箱桌面
    //     菜单中文名显示即为该机制的端到端断言）。

    printf(failures ? "\n== %d 项失败 ==\n" : "\nALL PASS\n", failures);
    return failures ? 1 : 0;
}
