#include "paintarea.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QRgb>

PaintArea::PaintArea(QWidget* parent) : QWidget(parent) {
  setAttribute(Qt::WA_StaticContents);
  setMouseTracking(true);
  newImage();
}

QSize PaintArea::sizeHint() const {
  return mCurrentDrawing.size();
}

void PaintArea::newImage(int w, int h) {
  mCurrentDrawing = QImage(w, h, QImage::Format::Format_ARGB32);
  mCurrentDrawing.fill(Qt::white);

  mCurrentImageData.resize(4 * w * h);

  mImageModified = false;
  resize(mCurrentDrawing.size().expandedTo(size()));
}

bool PaintArea::openImage(const QString& fileName) {
  QImage loadedImage;
  if (!loadedImage.load(fileName)) {
    return false;
  }

  QSize newSize = loadedImage.size().expandedTo(size());
  resizeImage(&loadedImage, newSize);
  mCurrentDrawing = loadedImage;
  mImageModified = false;
  update();
  return true;
}

bool PaintArea::saveImage(const QString& fileName, const char* fileFormat) {
  QImage visibleImage = mCurrentDrawing;
  resizeImage(&visibleImage, size());

  if (visibleImage.save(fileName, fileFormat)) {
    mImageModified = false;
    return true;
  }
  return false;
}

void PaintArea::clearImage() {
  mCurrentDrawing.fill(Qt::white);
  mImageModified = true;
  update();
}

void PaintArea::setBrushImage(QColor color) {
  mBrushImage = QImage(50, 50, QImage::Format::Format_ARGB32);
  mBrushImage.fill(color);
}

bool PaintArea::setBrushImage(const QString& fileName) {
  QImage loadedImage;
  if (!loadedImage.load(fileName)) {
    return false;
  }

  mBrushImage = loadedImage.scaled(50, 50, Qt::KeepAspectRatio);
  return true;
}

const QImage& PaintArea::brushImage() const {
  return mBrushImage;
}

void PaintArea::setPenColor(const QColor& newColor) {
  mBrushColor = newColor;
}

void PaintArea::setPenSize(int size) {
  mPenSize = size;
}

void PaintArea::changeTool(Tool t) {
  mCurrentTool = t;
}

void PaintArea::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    mLastPoint = event->pos();
    mButtonPressed = true;
  }
}

void PaintArea::mouseMoveEvent(QMouseEvent* event) {
  emit mousePositionChanged(event->pos());
  if ((event->buttons() & Qt::LeftButton) && mButtonPressed) {
    drawTo(event->pos());
  }
}

void PaintArea::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton && mButtonPressed) {
    drawTo(event->pos());
    mButtonPressed = false;
  }
}

void PaintArea::paintEvent(QPaintEvent* event) {
  QPainter painter(this);
  QRect dirtyRect = event->rect();
  painter.drawImage(dirtyRect, mCurrentDrawing, dirtyRect);
}

void PaintArea::drawTo(const QPoint& currentPoint) {
  QLine line(mLastPoint, currentPoint);
  int y0 = mLastPoint.y();
  int x0 = mLastPoint.x();
  int y1 = currentPoint.y();
  int x1 = currentPoint.x();
  if (mCurrentTool == Brush) {
    drawWithBrush(x1, y1);
  } else {
    drawWithPen(x1, y1);
  }
  if (x0 != x1 && y0 != y1) {
    drawLine(x0, y0, x1, y1);
  }

  mImageModified = true;
  QRect updateArea(mLastPoint.x(), mLastPoint.y(),
                   currentPoint.x() + mBrushImage.width(),
                   currentPoint.y() + mBrushImage.height());
  update(updateArea);
  mLastPoint = currentPoint;
}

void PaintArea::drawWithBrush(int x, int y) {
  for (int by = 0; by < mBrushImage.height(); by++) {
    uchar* s = mCurrentDrawing.scanLine(y + by);
    for (int bx = 0; bx < mBrushImage.width(); bx++) {
      const uchar* sl = mBrushImage.scanLine(by);
      uint* px = (uint*)(sl + bx * 4);
      ((uint*)s)[x + bx] = *px;
    }
  }
}

void PaintArea::drawWithPen(int x, int y) {
  for (int by = 0; by < mPenSize; by++) {
    uchar* s = mCurrentDrawing.scanLine(y + by);
    for (int bx = 0; bx < mPenSize; bx++) {
      ((uint*)s)[x + bx] = mBrushColor.rgba();
    }
  }
}

void PaintArea::drawLine(int x1, int y1, int x2, int y2) {
  int dx, dy, p;
  dx = x2 - x1;
  dy = y2 - y1;
  p = 2 * (dy) - (dx);
  while (x1 <= x2) {
    if (p < 0) {
      x1 = x1 + 1;
      y1 = y1;
      p = p + 2 * (dy);
    } else {
      x1 = x1 + 1;
      y1 = y1 + 1;
      p = p + 2 * (dy - dx);
    }
    if (mCurrentTool == Brush) {
      drawWithBrush(x1, y1);
    } else {
      drawWithPen(x1, y1);
    }
  }
}

void PaintArea::resizeImage(QImage* image, const QSize& newSize) {
  if (image->size() == newSize) {
    return;
  }

  QImage newImage(newSize, QImage::Format_RGB32);
  newImage.fill(qRgb(255, 255, 255));
  QPainter painter(&newImage);
  painter.drawImage(QPoint(0, 0), *image);
  *image = newImage;
}
