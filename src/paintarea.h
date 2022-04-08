#pragma once

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>

enum Tool { Pen, Brush };

class PaintArea : public QWidget {
  Q_OBJECT
 public:
  PaintArea(QWidget* parent = nullptr);
  virtual QSize sizeHint() const override;

  void newImage(int w = 1280, int h = 900);
  bool openImage(const QString& fileName);
  bool saveImage(const QString& fileName, const char* fileFormat);
  void clearImage();

  void setBrushImage(QColor color);
  bool setBrushImage(const QString& fileName);
  const QImage& brushImage() const;

  void setPenColor(const QColor& newColor);
  QColor penColor() const { return mBrushColor; }

  void setPenSize(int size);
  int penSize() const { return mPenSize; }

  void changeTool(Tool t);
  bool isModified() const { return mImageModified; }

 protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

 private:
  void drawTo(const QPoint& currentPoint);
  void drawLine(int x1, int y1, int x2, int y2);
  void resizeImage(QImage* image, const QSize& newSize);
  void drawWithBrush(int x, int y);
  void drawWithPen(int x, int y);

  bool mImageModified = false;
  bool mButtonPressed = false;
  int mPenSize = 1;

  Tool mCurrentTool;
  QImage mCurrentDrawing;
  QImage mBrushImage;
  QColor mBrushColor;
  QPoint mLastPoint;
  std::vector<float> mCurrentImageData;
};
