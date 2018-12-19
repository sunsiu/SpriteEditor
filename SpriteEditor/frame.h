#ifndef FRAME_H
#define FRAME_H
#include <QMainWindow>
#include <QPainter>
#include <QColor>
#include <QRgba64>
#include <QWidget>
#include <algorithm>

class Frame : public QWidget
{
    Q_OBJECT

    /*
      contains the four coordinates of the pixel
     */
struct PixelCoordinates
{
public:
    int xStarting, xEnding, yStarting, yEnding;
    PixelCoordinates(int xStarting, int xEnding, int yStarting, int yEnding);
};

private:
    QImage image;
    int currentPixelSize;
    bool isDrawingMirrored;
    const int GRID_RESOLUTION = 800;
    QColor colorGrid[32][32];

    void saveColor(int x, int y, QColor color);

public:
    Frame(QWidget *parent = nullptr, bool isDrawingMirroredChecked = false);
    Frame(const Frame &other, bool isDrawingMirroredChecked = false);
    ~Frame() override;
    Frame &operator= (Frame other);
    void updateForLoad();


    // Theses 4 variablies are for moving pixels
    bool isPixelSelected;
    int currentSelectedX;
    int currentSelectedY;
    int currentXCoord;
    int currentYCoord;
    QColor currentColor;
    int whichArrow; //direction of keypress
    QColor selectedColor;


    QImage& getImage();
    void setImage(QImage* image);
    void setCurrentPixelSize(int newSize);
    int getCurrentPixelSize();
    void drawPixel(int x, int y, QColor color);
    /**
     *  Returns an object with the boundaries of whatever pixel x, y are inside of.
     */
    PixelCoordinates getPixelAtCoordinates(int x, int y);


public slots:
  /*
    adjusts the number of pixels shown in the grid where the length and height of the pixel is newPixelSize
   */
    void changeResolution(int newPixelSize);
    void setDrawMirrored(bool checked);

    // These are for moving pixels


    void setIsPixelSelected(bool input);
    void setCurrentSelectedX(int input);
    void setCurrentSelectedY(int input);
    void setSelectedColor(QColor input);


    bool getIsPixelSelected();
    int getCurrentSelectedX();
    int getCurrentSelectedY();
    QColor getSelectedColor();

    /*
      the selected is moved in the direction the arrow key is pressed
     */
    void shiftPixel(int x, int y, QColor color);



protected:
    void paintEvent (QPaintEvent *event) override;
};

#endif // FRAME_H
