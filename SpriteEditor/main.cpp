#include "spriteeditorwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SpriteModel *m = new SpriteModel();
    SpriteEditorWindow w(nullptr, m);
    w.show();

    return a.exec();
}
