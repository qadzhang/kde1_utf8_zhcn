#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <ntqpopupmenu.h>
#include <ntqmainwindow.h>
#include <ntqintdict.h>
#include <ntqcanvas.h>

class BouncyLogo : public TQCanvasSprite {
    void initPos();
    void initSpeed();
public:
    BouncyLogo(TQCanvas*);
    ~BouncyLogo();
    void advance(int);
    int rtti() const;
};


class FigureEditor : public TQCanvasView {
    TQ_OBJECT

public:
    FigureEditor(TQCanvas&, TQWidget* parent=0, const char* name=0, WFlags f=0);
    void clear();

protected:
    void contentsMousePressEvent(TQMouseEvent*);
    void contentsMouseMoveEvent(TQMouseEvent*);

signals:
    void status(const TQString&);

private:
    TQCanvasItem* moving;
    TQPoint moving_start;
};

class Main : public TQMainWindow {
    TQ_OBJECT

public:
    Main(TQCanvas&, TQWidget* parent=0, const char* name=0, WFlags f=0);
    ~Main();

public slots:
    void help();

private slots:
    void aboutTQt();
    void newView();
    void clear();
    void init();

    void addSprite();
    void addCircle();
    void addHexagon();
    void addPolygon();
    void addSpline();
    void addText();
    void addLine();
    void addRectangle();
    void addMesh();
    void addLogo();
    void addButterfly();

    void enlarge();
    void shrink();
    void rotateClockwise();
    void rotateCounterClockwise();
    void zoomIn();
    void zoomOut();
    void mirror();
    void moveL();
    void moveR();
    void moveU();
    void moveD();

    void print();

    void toggleDoubleBuffer();

private:
    TQCanvas& canvas;
    FigureEditor *editor;

    TQPopupMenu* options;
    TQPrinter* printer;
    int dbf_id;
};

#endif
