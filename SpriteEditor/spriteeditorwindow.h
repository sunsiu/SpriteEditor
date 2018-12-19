#ifndef SPRITEEDITORWINDOW_H
#define SPRITEEDITORWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include <QMouseEvent>
#include <QListWidget>
#include <QSignalMapper>
#include <QKeyEvent>
#include <QFileDialog>
#include "frame.h"
#include "spritemodel.h"
#include "popup.h"

namespace Ui {
class SpriteEditorWindow;
}

class SpriteEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SpriteEditorWindow(QWidget *parent = nullptr, SpriteModel *model = new SpriteModel());
    ~SpriteEditorWindow() override;
    QList<Frame*> frames;

signals:
    void updateCurrentFrameIndex(int index);
    void frameRemoved(int removedIndex, int newIndex);
    void resolutionSliderMovedSignal(int value);
    void drawMirroredBoxChangedSignal(bool checked);
    void updateAnimation(int index, QImage& image);
    void frameRateSliderMoved(int newFps);
    void saveFrame(QString fileName);
    void loadFrame(QString fileName);
    void itemSwapped(int index, bool isDown);


public slots:
    void on_chooseColorBox_clicked();
    void handleAddedFrame(int framesMade);
    void updatePreviewImage();
    void receiveFrames(QList<Frame*> frames);
    void setFps(int newFps);
    void handleDuplicatedFrame();
    void updateFrame(Frame* current);
    void on_resolutionSlider_sliderMoved(int position);
    void on_drawMirrorCheckBox_toggled(bool checked);

private:
    Ui::SpriteEditorWindow *ui;
    QColor penColor;
    Frame* currentFrame;
    int lastXPosition;
    int lastYPostion;
    int currentFrameIndex;
    int imageIndex;
    bool mousePressed;
    Popup popup;
    int fps;
    QTimer *previewTimer;

    void updateRemoveButton();
    void incrementImageIndex();
    void updateButtonsToDisable();


protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void handleRemovedFrame();
    void handleItemClicked();
    void on_popOutButton_clicked();
    void on_frameRateSlider_sliderMoved(int position);
    void swapItem(bool isDown);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void on_actionSave_triggered();
    void on_actionOpen_triggered();
};

#endif // SPRITEEDITORWINDOW_H
