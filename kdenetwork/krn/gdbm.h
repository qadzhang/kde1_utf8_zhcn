/* ┌─ What : gdbm 公共 API 声明(krn 本地兼容头)
 * │  Why  : 系统仅装了 libgdbm6 运行库而无 libgdbm-dev 的官方 gdbm.h
 * │         (本环境 sudo 不可用装包受限);gdbm ABI 自 1.x 起稳定,
 * │         按 GNU gdbm 1.23 的公开接口逐条声明即可直接链 libgdbm.so.6
 * │  Who  : krn 的 newsrc 数据库存取(krn.cpp/NNTP.cpp 等)
 * │  When : krn 编译期(本头仅声明,无实现)
 * │  Where: kdenetwork/krn/gdbm.h(仓库内自足,不依赖系统头)
 * │  How  : datum 结构 + GDBM_* 常量 + 10 个 API 原型,extern "C" 包裹
 * ────────────────────────────────────────────────────────────────────
 * 本声明依据 GNU gdbm 公开文档,不含第三方版权内容。 */
#ifndef KRN_LOCAL_GDBM_H
#define KRN_LOCAL_GDBM_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char *dptr;
    int   dsize;
} datum;

#define GDBM_READER   0
#define GDBM_WRITER   1
#define GDBM_WRCREAT  2
#define GDBM_NEWDB    3

#define GDBM_INSERT   0
#define GDBM_REPLACE  1

typedef struct gdbm_file_info *GDBM_FILE;

GDBM_FILE gdbm_open(const char *, int, int, int, void (*)(void));
void      gdbm_close(GDBM_FILE);
int       gdbm_store(GDBM_FILE, datum, datum, int);
datum     gdbm_fetch(GDBM_FILE, datum);
int       gdbm_delete(GDBM_FILE, datum);
int       gdbm_exists(GDBM_FILE, datum);
datum     gdbm_firstkey(GDBM_FILE);
datum     gdbm_nextkey(GDBM_FILE, datum);
int       gdbm_reorganize(GDBM_FILE);
void      gdbm_sync(GDBM_FILE);

#ifdef __cplusplus
}
#endif

#endif /* KRN_LOCAL_GDBM_H */
