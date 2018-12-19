#ifndef POPUP_H
#define POPUP_H

#include <QWidget>
#include <QTimer>
#include "frame.h"

namespace Ui {
class Popup;
}

class Popup : public QWidget
{
    Q_OBJECT

public:
    QList<Frame*> frames;
    bool popupOpen;
    explicit Popup(QWidget *parent = nullptr);
    ~Popup();
    void setFrames(QList<Frame*> frameList);
    void start();

public slots:
    void updateImage();
    void setFps(int newFps);

private:
    Ui::Popup *ui;
    int fps;
    int frameIndex;
    void incrementFrameIndex();
    QTimer *fpsTimer;


protected:
    void closeEvent(QCloseEvent *event) override;

};

#endif // POPUP_H
