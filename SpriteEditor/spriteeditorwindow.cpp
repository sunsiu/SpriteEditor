#include "spriteeditorwindow.h"
#include "ui_spriteeditorwindow.h"
#include <QGridLayout>

SpriteEditorWindow::SpriteEditorWindow(QWidget *parent, SpriteModel *model) :
    QMainWindow(parent),
    ui(new Ui::SpriteEditorWindow)
{
    ui->setupUi(this);
    previewTimer = new QTimer(this);

    //Listen for signals from view
    QObject::connect(previewTimer, SIGNAL(timeout()),this,SLOT(updatePreviewImage()));
    QObject::connect(ui->addFrameButton, &QPushButton::pressed,
                    model, &SpriteModel::addFrame);
    QObject::connect(ui->removeFrameButton, &QPushButton::pressed,
                    this, &SpriteEditorWindow::handleRemovedFrame);
    QObject::connect(this, &SpriteEditorWindow::frameRemoved,
                    model, &SpriteModel::removeFrame);
    QObject::connect(ui->duplicateButton, &QPushButton::pressed,
                    [=]() {model->duplicateFrame(ui->framesList->currentRow());});
    QObject::connect(this, &SpriteEditorWindow::updateCurrentFrameIndex,
                    model, &SpriteModel::setCurrentFrame);
    QObject::connect(ui->framesList, &QListWidget::itemPressed,
                    this, &SpriteEditorWindow::handleItemClicked);
    QObject::connect(this, &SpriteEditorWindow::resolutionSliderMovedSignal,
                    model, &SpriteModel::changeResolutionOfAllFrames);
    QObject::connect(this, &SpriteEditorWindow::drawMirroredBoxChangedSignal,
                    model, &SpriteModel::setDrawMirrored);
    QObject::connect(ui->popOutButton, &QPushButton::pressed,
                    model, &SpriteModel::getFrames);
    QObject::connect(this, &SpriteEditorWindow::updateAnimation,
                    model, &SpriteModel::updateImages);
    QObject::connect(ui->itemUpButton, &QPushButton::pressed,
                    [=]() {swapItem(false);});
    QObject::connect(ui->itemDownButton, &QPushButton::pressed,
                    [=]() {swapItem(true);});
    QObject::connect(this, &SpriteEditorWindow::itemSwapped,
                    model, &SpriteModel::swapItem);
    QObject::connect(this, &SpriteEditorWindow::saveFrame,
                      model, &SpriteModel::save);
    QObject::connect(this, &SpriteEditorWindow::loadFrame,
                      model, &SpriteModel::load);
    QObject::connect(ui->actionExport,&QAction::triggered,
            model, &SpriteModel::exportGif);



    // Listen for signals from model
    QObject::connect(model, &SpriteModel::frameAdded,
                    this, &SpriteEditorWindow::handleAddedFrame);
    QObject::connect(model, &SpriteModel::frameDuplicated,
                    this, &SpriteEditorWindow::handleDuplicatedFrame);
    QObject::connect(model, &SpriteModel::currentFrameUpdated,
                    this, &SpriteEditorWindow::updateFrame);
    QObject::connect(model, &SpriteModel::sendFrames,
                    this, &SpriteEditorWindow::receiveFrames);

    // Setting up the color picker color
    penColor = Qt::black;
    currentFrame = nullptr;

    // We initialize first frame here instead of the model constructor because the constructor
    // executes before the signals are connected.
    model->addFrame();
    currentFrameIndex = 0;
    imageIndex = 0;
    fps = 1;
    ui->selectionButton->setAttribute(Qt::WA_KeyCompression);


    previewTimer->start(1000/fps);

}

SpriteEditorWindow::~SpriteEditorWindow()
{
    // Current frame points to a frame from model, so to avoid double deletion, we just set ours to nullptr
    // and let the parent handle deletion.
    currentFrame = nullptr;
    delete ui;
}

void SpriteEditorWindow::handleAddedFrame(int framesMade)
{
    // The syntax for interpolating an int in a QString
    QString frameName = QString("Frame %1").arg(framesMade);

    // Add new blank frame and switch focus to it
    ui->framesList->addItem(frameName);

    int lastRow = ui->framesList->count() - 1;
    ui->framesList->setCurrentRow(lastRow);

    currentFrameIndex = lastRow;
    emit updateCurrentFrameIndex(lastRow);
    updateButtonsToDisable();
}

void SpriteEditorWindow::handleRemovedFrame()
{
    // Removes the currently selected item
    int removedIndex = ui->framesList->currentRow();
    ui->framesList->takeItem(removedIndex);
    int newIndex = ui->framesList->currentRow();
    currentFrame->hide();
    previewTimer->stop();
    currentFrameIndex = 0;
    emit frameRemoved(removedIndex, newIndex);

    updateButtonsToDisable();
}

void SpriteEditorWindow::handleDuplicatedFrame()
{
     QString originalName = ui->framesList->currentItem()->text();
     QString copyName = QString(originalName + " Copy");
     int copyIndex = ui->framesList->currentRow() + 1;
     
     ui->framesList->insertItem(copyIndex, copyName);
     ui->framesList->setCurrentRow(copyIndex);
     emit updateCurrentFrameIndex(copyIndex);
     updateButtonsToDisable();
}

void SpriteEditorWindow::updateButtonsToDisable()
{
    bool isLastFrame = (ui->framesList->count() == 1);
    bool isFirstRow = (ui->framesList->currentRow() == 0);
    bool isLastRow = (ui->framesList->currentRow() == ui->framesList->count() - 1);

    ui->removeFrameButton->setDisabled(isLastFrame);
    ui->itemUpButton->setDisabled(isFirstRow);
    ui->itemDownButton->setDisabled(isLastRow);
}

void SpriteEditorWindow::updateFrame(Frame* newCurrent)
{
    if(!previewTimer->isActive())
    {
        previewTimer->start();
    }

    ui->frameLayout->removeWidget(currentFrame);
    currentFrame = newCurrent;
    ui->frameLayout->addWidget(newCurrent, 0 , 0);
    newCurrent->show();
}

void SpriteEditorWindow::swapItem(bool isMoveDown)
{
    // Remove the currently selected item and readd it 1 up or 1 down
    int currentIndex = ui->framesList->currentRow();
    QListWidgetItem* currentItem = ui->framesList->takeItem(currentIndex);
    int nextIndex = currentIndex;

    if(isMoveDown)
    {
        nextIndex = currentIndex + 1;
    }
    else
    {
        nextIndex = currentIndex - 1;
    }

    ui->framesList->insertItem(nextIndex, currentItem);
    emit itemSwapped(currentIndex, nextIndex);

    // Focus on the item after we readd it
    ui->framesList->setCurrentRow(nextIndex);
    updateButtonsToDisable();
}

void SpriteEditorWindow::handleItemClicked()
{
    currentFrame->hide();
    updatePreviewImage();
    emit updateCurrentFrameIndex(ui->framesList->currentRow());

    updateButtonsToDisable();

}

void SpriteEditorWindow::on_chooseColorBox_clicked()
{
    // Opening the QColorDialog
    penColor = QColorDialog::getColor(penColor, this);

    if(penColor.isValid())
    {
        // Changing the label background color to the selected color
        QString currentColor = QString("background-color:" + penColor.name());
        ui->colorLabel->setStyleSheet(currentColor);
    }
}


void SpriteEditorWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (ui->penButton->isChecked() && mousePressed)
    {
        currentFrame->drawPixel(event->x(),event->y(),penColor);
    }

    if(ui->eraserButton->isChecked()){
        currentFrame->drawPixel(event->x(),event->y(),qRgba(160 , 160, 160, 10));
    }
}

void SpriteEditorWindow::mousePressEvent(QMouseEvent *event)
{
    // 10 is the x offset of the frame and 26 is the y offset
    bool isMouseInFrame = (event->x() >= 10 && event->x() <= 810 && event->y() >= 26 && event->x() <= 826);
    if(isMouseInFrame)
    {
        mousePressed = true;
        currentFrame->drawPixel(event->x(),event->y(),penColor);
    }
    QImage& image = currentFrame->getImage();
    emit updateAnimation(currentFrameIndex, image);

    bool isCursorInDrawArea = (event->x() >= 12 && event->x() <= 812 && event->y() >=29 && event->y() <=829);
    if(ui->penButton->isChecked())
    {
        mousePressed = true;

        currentFrame->drawPixel(event->x(),event->y(),penColor);
        currentFrame->setIsPixelSelected(false);
    }
    else if(ui->eraserButton->isChecked())
    {
        currentFrame->drawPixel(event->x(),event->y(),qRgba(160 , 160, 160, 10));
        currentFrame->setIsPixelSelected(false);
    }
    else if(ui->selectionButton->isChecked() && isCursorInDrawArea)
    {
        currentFrame->setIsPixelSelected(true);
        currentFrame->setCurrentSelectedX(event->x());
        currentFrame->setCurrentSelectedY(event->y());

        // Draw a pixel at the current index with offset
        currentFrame->setSelectedColor(currentFrame->getImage().pixelColor(event->x()-12, event->y()-29));
    }
}


void SpriteEditorWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressed = false;
    updatePreviewImage();
    QImage& image = currentFrame->getImage();
    emit updateAnimation(currentFrameIndex, image);

}


void SpriteEditorWindow::updatePreviewImage()
{
    if(frames.count() > 0)
    {
        QImage image;
        image = frames[imageIndex]->getImage();

        // Scale our image to 200x200 size so we can display it in our preview window
        QImage previewImage = image.scaled(200, 200, Qt::KeepAspectRatio);
        ui->previewLabel->setPixmap(QPixmap::fromImage(previewImage));
        ui->previewLabel->show();

        incrementImageIndex();
    }
}

void SpriteEditorWindow::incrementImageIndex()
{
    // If this is not the last image in the sequence
    if (imageIndex < frames.size() - 1)
    {
        imageIndex++;
    }
    else
    {
        // Otherwise, go back to the first image in the sequence
        imageIndex = 0;
    }
}

void SpriteEditorWindow::on_resolutionSlider_sliderMoved(int position)
{
    emit resolutionSliderMovedSignal(position);
}

void SpriteEditorWindow::on_drawMirrorCheckBox_toggled(bool checked)
{
    emit drawMirroredBoxChangedSignal(checked);
}


void SpriteEditorWindow::on_popOutButton_clicked()
{
    popup.setFps(fps);
    popup.popupOpen = true;
    popup.show();
    popup.start();

}

void SpriteEditorWindow::receiveFrames(QList<Frame*> frameList)
{
    frames = frameList;
    popup.setFrames(frameList);
}

void SpriteEditorWindow::on_frameRateSlider_sliderMoved(int position)
{
    fps = position * 2;

    //change the timer to the new fps
    previewTimer->stop();
    previewTimer->start(1000/fps);

    // Update the popup to have the same fps
    popup.setFps(fps);

    //emit frameRateSliderMoved(fps);

    if(ui->penButton->isChecked())
    {
        mousePressed = false;
    }

    if(ui->eraserButton->isChecked())
    {
        mousePressed = false;
    }


}

void SpriteEditorWindow::keyPressEvent(QKeyEvent *event)
{
   
}

void SpriteEditorWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Up) // which arrow == 0
    {
        if(currentFrame->getCurrentSelectedY() < 35 + currentFrame->getCurrentPixelSize()/2)
        {
            return;
        }
        if(currentFrame->getIsPixelSelected())
        {
            //create  the setter for this variable
            currentFrame->whichArrow = 0;
            currentFrame->shiftPixel(currentFrame->getCurrentSelectedX(),currentFrame->getCurrentSelectedY(), currentFrame->getSelectedColor());
            currentFrame->update();
            currentFrame->setCurrentSelectedY(currentFrame->getCurrentSelectedY()-currentFrame->getCurrentPixelSize());

        }
    }

    if(event->key() == Qt::Key_Down)
    {
        if(currentFrame->getCurrentSelectedY() > 815 - currentFrame->getCurrentPixelSize()/2)
        {
            return;
        }
        if(currentFrame->getIsPixelSelected())
        {
            currentFrame->whichArrow = 1;
            currentFrame->shiftPixel(currentFrame->getCurrentSelectedX(),currentFrame->getCurrentSelectedY(), currentFrame->getSelectedColor());
            currentFrame->update();
            currentFrame->setCurrentSelectedY(currentFrame->getCurrentSelectedY()+currentFrame->getCurrentPixelSize());

        }
    }

    if(event->key() == Qt::Key_Left)
    {
        if(currentFrame->getCurrentSelectedX() < 25+ currentFrame->getCurrentPixelSize()/2)
        {
            return;
        }
        if(currentFrame->getIsPixelSelected())
        {
            currentFrame->whichArrow = 2;
            currentFrame->shiftPixel(currentFrame->getCurrentSelectedX(),currentFrame->getCurrentSelectedY(), currentFrame->getSelectedColor());
            currentFrame->update();
            currentFrame->setCurrentSelectedX(currentFrame->getCurrentSelectedX()-currentFrame->getCurrentPixelSize());

        }
    }

    if(event->key() == Qt::Key_Right)
    {
        if(currentFrame->getCurrentSelectedX() > 800 - currentFrame->getCurrentPixelSize()/2)
        {
            return;
        }
        if(currentFrame->getIsPixelSelected())
        {
            currentFrame->whichArrow = 3;
            currentFrame->shiftPixel(currentFrame->getCurrentSelectedX(),currentFrame->getCurrentSelectedY(), currentFrame->getSelectedColor());
            currentFrame->update();
            currentFrame->setCurrentSelectedX(currentFrame->getCurrentSelectedX()+currentFrame->getCurrentPixelSize());

        }
    }

    updatePreviewImage();
}

void SpriteEditorWindow::on_actionSave_triggered()
{
    //QFileDialog dialog(this);
     QString fileName = QFileDialog::getSaveFileName(this,tr("Save Sprite Sheet Project"), "", tr("Sprite Sheet Project (*.ssp)"));
     if (!fileName.endsWith(".ssp"))
         fileName += ".ssp";
    emit saveFrame(fileName);
}

void SpriteEditorWindow::on_actionOpen_triggered()
{
    currentFrame->hide();
    ui->framesList->clear();
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open Sprite Sheet Project"), "",
            tr("Sprite Sheet Project (*.ssp)"));
    emit loadFrame(fileName);
}

void SpriteEditorWindow::setFps(int newFps)
{
    fps = newFps;
}
