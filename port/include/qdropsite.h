/*
 * qdropsite.h — Qt1 QDropSite 空 mixin（迁移脚手架，自动生成配套）
 * Qt1 的拖放靠 "class X : public QWidget, public QDropSite" 多继承混入；
 * Qt3 起拖放（dragEnterEvent 等）已是 QWidget 原生虚函数，QDropSite 被删除。
 * 此处仅保留空壳补齐多继承语法位，行为全部落到 TQWidget 原生实现上。
 * 模块显式化（源码去掉 QDropSite 继承）后随 port/ 脚手架拆除。
 */
#ifndef Q1FW_QDROPSITE_H
#define Q1FW_QDROPSITE_H
#include <ntqwidget.h>
class QDropSite {
public:
    QDropSite( TQWidget * ) {}
};
#endif
