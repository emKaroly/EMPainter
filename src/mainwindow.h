#pragma once

#include <QMainWindow>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QScrollArea;
class PaintArea;
class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  MainWindow(QWidget* parent = nullptr, const QString& appname = "");
  ~MainWindow();

  // QObject interface
 public:
  virtual bool eventFilter(QObject* watched, QEvent* event) override;

 protected:
  void closeEvent(QCloseEvent* event) override;

 private slots:
  void open();
  void setBrushImage(QString path = "");
  void save();
  void setPenColor();
  void penSize();

 private:
  void createActions();
  void createMenus();
  void setupButtons();
  void initialize();
  bool askToSave();
  bool saveFile(const QByteArray& fileFormat);
  void updatePenColor(QColor color);
  void updateBrushImage(const QImage& image);

 private:
  QString mAppName;
  Ui::MainWindow* ui;
  QPixmap mPenColorPixmap;
  QPixmap mBrushPixmap;
  QScrollArea* mScrollArea;
  PaintArea* mPaintArea;

  QMenu* mFileMenu;
  QMenu* mSaveAsMenu;
  QAction* mNewImageAction;
  QAction* mOpenAction;
  QList<QAction*> mSaveAsActions;
  QAction* mClearScreenAction;
  QAction* mExitAction;

  QMenu* mOptionMenu;
  QAction* mSetToolBrushAction;
  QAction* mSetToolPenAction;
  QMenu* mSetPenSizeMenu;
  QList<QAction*> mPenSizeActions;
  QAction* mSetBrushImageAction;
  QAction* mSetPenColorAction;
  QAction* mSetPenSizeAction;
};
