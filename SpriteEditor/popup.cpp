#include "popup.h"
#include "ui_popup.h"

Popup::Popup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Popup)
{
    ui->setupUi(this);
    frameIndex = 0;
    popupOpen = false;
    fpsTimer = new QTimer(this);

    QObject::connect(fpsTimer, &QTimer::timeout,
                    this, &Popup::updateImage);
}

void Popup::start()
{
    fpsTimer->start(1000/fps);
}

void Popup::setFrames(QList<Frame*> frameList)
{
    frames = frameList;
}

void Popup::setFps(int newFps)
{
    fps = newFps;

    if(fpsTimer->isActive())
    {
        fpsTimer->stop();
        fpsTimer->start(1000/fps);
    }
}

void Popup::updateImage()
{
    if (popupOpen == true)
    {
        QPixmap current = QPixmap::fromImage(frames[frameIndex]->getImage());
        ui->imageLabel->setPixmap(current);
        ui->imageLabel->show();
        incrementFrameIndex();
    }
}


void Popup::incrementFrameIndex()
{
    if (frameIndex < frames.size() - 1) // if this is not the last image in the sequence
    {
        frameIndex++;
    }
    else
    {
        frameIndex = 0; //otherwise, go back to the first image in the sequence
    }
}

void Popup::closeEvent(QCloseEvent *event)
{
    popupOpen = false;
    fpsTimer->stop();
}

Popup::~Popup()
{
    delete ui;
}
