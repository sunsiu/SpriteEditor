#include "frame.h"


using namespace std;
Frame::Frame(QWidget *parent, bool isDrawingMirroredChecked)
    : QWidget(parent)
{
    isDrawingMirrored = isDrawingMirroredChecked;
    image= QImage(GRID_RESOLUTION,GRID_RESOLUTION,QImage::Format_RGB32);

    image.fill(qRgba(160 , 160, 160, 10));
    currentPixelSize= 25;
    currentColor = Qt::gray;
    isPixelSelected = false;
}

Frame::Frame(const Frame& other, bool isDrawingMirroredChecked)
{
    image = other.image.copy();
    currentPixelSize = other.currentPixelSize;
    currentXCoord = other.currentXCoord;
    currentYCoord = other.currentYCoord;
    currentColor = other.currentColor;
    isDrawingMirrored = isDrawingMirroredChecked;

}

Frame& Frame::operator= (Frame other)
{
    swap(image, other.image);
    swap(currentPixelSize, other.currentPixelSize);
    swap(currentXCoord, other.currentXCoord);
    swap(currentYCoord, other.currentYCoord);
    swap(currentColor, other.currentColor);
    return *this;
}

Frame::~Frame()
{

}

void Frame::setImage(QImage* image)
{
    this->image = *image;
}
QImage& Frame::getImage()
{
    return this->image;
}

void Frame::setCurrentPixelSize(int newSize)
{
      this->currentPixelSize = newSize;
}

int Frame::getCurrentPixelSize()
{
    return this->currentPixelSize;
}

Frame::PixelCoordinates Frame::getPixelAtCoordinates(int x, int y)
{
    int xStarting = (x/currentPixelSize)*currentPixelSize;
    int xEnding = xStarting + currentPixelSize -1;
    int yStarting = (y/currentPixelSize)*currentPixelSize;
    int yEnding = yStarting + currentPixelSize -1;

    PixelCoordinates result(xStarting, xEnding, yStarting, yEnding);
    return result;
}

void Frame::setIsPixelSelected(bool input)
{
    this->isPixelSelected = input;
}

void Frame::setCurrentSelectedX(int input)
{
    this->currentSelectedX = input;
}

void Frame::setCurrentSelectedY(int input)
{
    this->currentSelectedY = input;
}

void Frame::setSelectedColor(QColor input)
{
    this->selectedColor = input;
}

bool Frame::getIsPixelSelected()
{
    return this->isPixelSelected;
}

int Frame::getCurrentSelectedX()
{
    return this->currentSelectedX;
}

int Frame::getCurrentSelectedY()
{
    return this->currentSelectedY;
}

QColor Frame::getSelectedColor()
{
    return this->selectedColor;
}

void Frame::saveColor(int x, int y, QColor color)
{
    int xOffset = 12;
    int yOffset = 26;
    int xIndex = (x - xOffset)/currentPixelSize;
    int yIndex = (y - yOffset)/currentPixelSize;
}
void Frame::paintEvent(QPaintEvent *)
{
    // Account for offset of our draw area within the window
    PixelCoordinates points = getPixelAtCoordinates(currentXCoord-10,currentYCoord-26);

    QPainter painter(this);
    QPainter imagePainter(&image);
    QBrush brush(currentColor);

    imagePainter.setBrush(brush);
    QRect rectangle(points.xStarting, points.yStarting, currentPixelSize, currentPixelSize);
    imagePainter.fillRect(rectangle,brush);

    QPen pen(Qt::white);
    painter.setPen(pen);

    if (isDrawingMirrored)
    {
        //819 is the middle of the frame
        currentXCoord = 819 - currentXCoord;

        PixelCoordinates points = getPixelAtCoordinates(currentXCoord-10,currentYCoord-26);

	//draws rectangle on reflected vertical half
        rectangle = QRect(points.xStarting, points.yStarting, this->currentPixelSize,this->currentPixelSize);
        imagePainter.setBrush(brush);

        imagePainter.fillRect(rectangle,brush);
    }

    rectangle= QRect(points.xStarting, points.yStarting, this->currentPixelSize,this->currentPixelSize);
    imagePainter.setBrush(brush);
    imagePainter.fillRect(rectangle,brush);
    imagePainter.drawRect(rectangle);


    painter.drawImage(QPoint(),image);

    //display grid lines
    for(int row = 0; row<=(image.height()/this->getCurrentPixelSize()); row++)
    {
        painter.drawLine(0, row*this->getCurrentPixelSize(), 800, row*this->getCurrentPixelSize());
    }
    for(int column = 0; column<=(image.width()/this->getCurrentPixelSize()); column++)
    {
        painter.drawLine(column*this->getCurrentPixelSize(), 0, column*this->getCurrentPixelSize(), image.height());
    }

}

void Frame::drawPixel(int x, int y, QColor color) {
    currentXCoord = x;
    currentYCoord = y - 25;
    currentColor = color;
    update();
}

void Frame::changeResolution(int newPixelSize)
{

    if (newPixelSize > currentPixelSize)
    {
        image.fill(qRgba(160 , 160, 160, 10));
        currentColor = Qt::transparent;
    }

    this->currentPixelSize = newPixelSize;

    update();

}

void Frame::setDrawMirrored(bool checked)
{

    isDrawingMirrored = checked;
}

Frame::PixelCoordinates::PixelCoordinates(int xStart, int xEnd, int yStart, int yEnd)
{
    xStarting = xStart;
    xEnding = xEnd;
    yStarting = yStart;
    yEnding = yEnd;
}
void Frame::shiftPixel(int x, int y, QColor color)
{
    if(whichArrow == 0)
    {
        currentXCoord = x;
        currentYCoord = y - currentPixelSize;
       
    }
    if(whichArrow == 1)
    {
        currentXCoord = x;
        currentYCoord = y + currentPixelSize;
       
    }
    if(whichArrow == 2)
    {
        currentXCoord = x - currentPixelSize;
        currentYCoord = y ;
       
    }
    if(whichArrow == 3)
    {
        currentXCoord = x+ currentPixelSize;
        currentYCoord = y ;
       
    }

     currentColor = color;
     update();


}

void Frame::updateForLoad()
{
    // Account for offset of our draw area within the window
    PixelCoordinates points = getPixelAtCoordinates(currentXCoord,currentYCoord);


    QPainter painter(this);
    QPainter imagePainter(&image);
    QBrush brush(currentColor);

    imagePainter.setBrush(brush);
    QRect rectangle(points.xStarting, points.yStarting, currentPixelSize, currentPixelSize);
    imagePainter.fillRect(rectangle,brush);

    QPen pen(Qt::white);
    painter.setPen(pen);

    if (isDrawingMirrored)
    {
        //819 is the middle of the frame
        currentXCoord = 819 - currentXCoord;

        //441 is the center coord
        //int mirroredXCoord = 0;
        //int mirroredYCoord = 0;

        //if (currentXCoord <= 410)
        //    currentXCoord = 819 - currentXCoord;
        //else if (currentXCoord > 410)
         //   currentXCoord = 819 - currentXCoord;

        // 10 is the x offset and 26 is the y offset
        PixelCoordinates points = getPixelAtCoordinates(currentXCoord-10,currentYCoord-26);

        rectangle = QRect(points.xStarting, points.yStarting, this->currentPixelSize,this->currentPixelSize);
        imagePainter.setBrush(brush);

        imagePainter.fillRect(rectangle,brush);
    }

    rectangle= QRect(points.xStarting, points.yStarting, this->currentPixelSize,this->currentPixelSize);
    imagePainter.setBrush(brush);
    imagePainter.fillRect(rectangle,brush);
    imagePainter.drawRect(rectangle);
    if(isPixelSelected)
    {
        QRect rectanglePrevious;
        if(whichArrow == 0)
        {
            rectanglePrevious = QRect(points.xStarting, points.yStarting+currentPixelSize, this->currentPixelSize,this->currentPixelSize);
        }
        if(whichArrow == 1)
        {
            rectanglePrevious = QRect(points.xStarting, points.yStarting-currentPixelSize, this->currentPixelSize,this->currentPixelSize);

        }
        if(whichArrow == 2)
        {
            rectanglePrevious = QRect(points.xStarting+currentPixelSize, points.yStarting, this->currentPixelSize,this->currentPixelSize);
        }
        if(whichArrow == 3)
        {
            rectanglePrevious = QRect(points.xStarting-currentPixelSize, points.yStarting, this->currentPixelSize,this->currentPixelSize);
        }
        QBrush grayBrush(Qt::gray);
        imagePainter.setBrush(grayBrush);
        imagePainter.fillRect(rectanglePrevious,grayBrush);
    }
    painter.drawImage(QPoint(),image);

    //display grid lines
    for(int row = 0; row<=(image.height()/this->getCurrentPixelSize()); row++)
    {
        painter.drawLine(0, row*this->getCurrentPixelSize(), 800, row*this->getCurrentPixelSize());
    }
    for(int column = 0; column<=(image.width()/this->getCurrentPixelSize()); column++)
    {
        painter.drawLine(column*this->getCurrentPixelSize(), 0, column*this->getCurrentPixelSize(), image.height());
    }
}
