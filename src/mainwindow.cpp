#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "paintarea.h"

#include <QApplication>
#include <QCloseEvent>
#include <QColorDialog>
#include <QFileDialog>
#include <QImageWriter>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>

MainWindow::MainWindow(QWidget* parent, const QString& appname)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mScrollArea(new QScrollArea(this)),
      mPaintArea(new PaintArea(this)),
      mAppName(appname) {
  ui->setupUi(this);
  ui->verticalLayout_drawingArea->addWidget(mScrollArea);

  mScrollArea->setWidget(mPaintArea);
  mScrollArea->setWidgetResizable(false);

  setWindowTitle(mAppName);
  setWindowIcon(QIcon("./res/empainter.ico"));
  createActions();
  createMenus();
  setupButtons();

  initialize();
}

MainWindow::~MainWindow() {
  delete ui;
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
  if (event->type() == QEvent::MouseButtonPress) {
    if (watched == ui->label_currentBrush) {
      mSetBrushImageAction->trigger();
    } else if (watched == ui->label_currentColor) {
      mSetPenColorAction->trigger();
    }
  }
  return QObject::eventFilter(watched, event);
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if (askToSave()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::open() {
  if (askToSave()) {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    QDir::currentPath());
    if (!fileName.isEmpty()) {
      mPaintArea->openImage(fileName);
    }
  }
}

void MainWindow::setBrushImage(QString path) {
  if (path.isEmpty()) {
    path = QFileDialog::getOpenFileName(this, tr("Open File"),
                                        QDir::currentPath());
  }
  if (!path.isEmpty()) {
    mPaintArea->setBrushImage(path);
    updateBrushImage(mPaintArea->brushImage());
  }
}

void MainWindow::save() {
  QAction* action = qobject_cast<QAction*>(sender());
  QByteArray fileFormat = action->data().toByteArray();
  saveFile(fileFormat);
}

void MainWindow::setPenColor() {
  QColor newColor = QColorDialog::getColor(mPaintArea->penColor());
  if (newColor.isValid()) {
    mPaintArea->setPenColor(newColor);
    updatePenColor(newColor);
  }
}

void MainWindow::penSize() {
  bool ok;
  int newWidth = QInputDialog::getInt(this, mAppName, tr("Select pen width:"),
                                      mPaintArea->penSize(), 1, 50, 1, &ok);
  if (ok) {
    mPaintArea->setPenSize(newWidth);
  }
}

void MainWindow::createActions() {
  mOpenAction = new QAction(tr("&Open..."), this);
  mOpenAction->setShortcuts(QKeySequence::Open);
  connect(mOpenAction, &QAction::triggered, this, &MainWindow::open);
  connect(ui->label_currentBrush, &QLabel::customContextMenuRequested,
          [this]() { setBrushImage(); });

  const QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
  for (const QByteArray& format : imageFormats) {
    QString text = tr("%1...").arg(QString::fromLatin1(format).toUpper());

    QAction* action = new QAction(text, this);
    action->setData(format);
    connect(action, &QAction::triggered, this, &MainWindow::save);
    mSaveAsActions.append(action);
  }
  for (auto s : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
    QString text = tr("%1px").arg(s);

    QAction* action = new QAction(text, this);
    action->setData(s);
    connect(action, &QAction::triggered, [this, action]() {
      mPaintArea->setPenSize(action->data().toInt());
      mPaintArea->changeTool(Tool::Pen);
    });
    mPenSizeActions.append(action);
  }

  mExitAction = new QAction(tr("E&xit"), this);
  mExitAction->setShortcuts(QKeySequence::Quit);
  connect(mExitAction, &QAction::triggered, this, &MainWindow::close);

  mSetBrushImageAction = new QAction(tr("&Set Brush Image"), this);
  connect(mSetBrushImageAction, &QAction::triggered,
          [this]() { setBrushImage(); });

  mSetPenColorAction = new QAction(tr("&Pen Color..."), this);
  connect(mSetPenColorAction, &QAction::triggered, this,
          &MainWindow::setPenColor);

  mSetPenSizeAction = new QAction(tr("Pen &Width..."), this);
  connect(mSetPenSizeAction, &QAction::triggered, this, &MainWindow::penSize);

  mSetToolPenAction = new QAction(tr("Pen Tool"), this);
  mSetToolPenAction->setIcon(QIcon("./svg/pen-solid.svg"));
  connect(mSetToolPenAction, &QAction::triggered,
          [this]() { mPaintArea->changeTool(Tool::Pen); });
  mSetToolBrushAction = new QAction(tr("Brush Tool"), this);
  mSetToolBrushAction->setIcon(QIcon("./svg/brush-solid.svg"));
  connect(mSetToolBrushAction, &QAction::triggered,
          [this]() { mPaintArea->changeTool(Tool::Brush); });

  mClearScreenAction = new QAction(tr("&Clear Screen"), this);
  mClearScreenAction->setShortcut(tr("Ctrl+L"));
  connect(mClearScreenAction, &QAction::triggered, mPaintArea,
          &PaintArea::clearImage);

  mNewImageAction = new QAction(tr("&NewImage"), this);
  connect(mNewImageAction, &QAction::triggered,
          [this]() { mPaintArea->newImage(); });
}

void MainWindow::createMenus() {
  mSaveAsMenu = new QMenu(tr("&Save As"), this);
  for (QAction* action : qAsConst(mSaveAsActions)) {
    mSaveAsMenu->addAction(action);
  }

  mSetPenSizeMenu = new QMenu(tr("&Set Pen Size"), this);
  for (QAction* action : qAsConst(mPenSizeActions)) {
    mSetPenSizeMenu->addAction(action);
  }

  mFileMenu = new QMenu(tr("&File"), this);
  mFileMenu->addAction(mNewImageAction);
  mFileMenu->addAction(mOpenAction);
  mFileMenu->addMenu(mSaveAsMenu);
  mFileMenu->addAction(mClearScreenAction);
  mFileMenu->addSeparator();
  mFileMenu->addAction(mExitAction);

  mOptionMenu = new QMenu(tr("&Options"), this);
  mOptionMenu->addAction(mSetBrushImageAction);
  mOptionMenu->addAction(mSetPenColorAction);
  mOptionMenu->addAction(mSetPenSizeAction);
  mOptionMenu->addSeparator();

  menuBar()->addMenu(mFileMenu);
  menuBar()->addMenu(mOptionMenu);
}

void MainWindow::setupButtons() {
  ui->label_currentBrush->installEventFilter(this);
  ui->label_currentColor->installEventFilter(this);
  mPenColorPixmap = QPixmap(50, 50);
  mBrushPixmap = QPixmap(50, 50);
  mBrushPixmap.fill(Qt::gray);
  ui->label_currentBrush->setPixmap(mBrushPixmap);

  ui->toolButton_clear->setDefaultAction(mClearScreenAction);
  ui->toolButton_clear->setIcon(QIcon("./svg/eraser-solid.svg"));
  ui->toolButton_clear->setIconSize({32, 32});
  ui->toolButton_openFile->setDefaultAction(mOpenAction);
  ui->toolButton_openFile->setIcon(QIcon("./svg/file-solid.svg"));
  ui->toolButton_openFile->setIconSize({32, 32});

  ui->toolButton_saveAs->setMenu(mSaveAsMenu);
  ui->toolButton_saveAs->setIcon(QIcon("./svg/floppy-disk-solid.svg"));
  ui->toolButton_saveAs->setPopupMode(QToolButton::InstantPopup);

  ui->toolButton_toolPen->setMenu(mSetPenSizeMenu);
  ui->toolButton_toolPen->setIcon(QIcon("./svg/pen-solid.svg"));
  ui->toolButton_toolPen->setPopupMode(QToolButton::InstantPopup);
  ui->toolButton_toolBrush->setDefaultAction(mSetToolBrushAction);
}

void MainWindow::initialize() {
  mPaintArea->setPenColor(Qt::black);
  updatePenColor(mPaintArea->penColor());
  mPaintArea->setBrushImage(Qt::green);
  updateBrushImage(mPaintArea->brushImage());
}

bool MainWindow::askToSave() {
  if (mPaintArea->isModified()) {
    using Button = QMessageBox::StandardButton;
    auto buttons = Button::Save | Button::Discard | Button::Cancel;
    QString text(
        tr("The image has been modified.\n Do you want to save your changes?"));

    Button msgBox = QMessageBox::warning(this, mAppName, text, buttons);

    if (msgBox == Button::Save) {
      return saveFile("png");
    } else if (msgBox == Button::Cancel) {
      return false;
    }
  }
  return true;
}

bool MainWindow::saveFile(const QByteArray& fileFormat) {
  QString defaultPath = QDir::currentPath() + "/untitled." + fileFormat;
  QString title = tr("Save As");

  auto filter = tr("%1 Files (*.%2);;All Files (*)")
                    .arg(QString::fromLatin1(fileFormat.toUpper()))
                    .arg(QString::fromLatin1(fileFormat));

  QString fileName =
      QFileDialog::getSaveFileName(this, title, defaultPath, filter);
  if (fileName.isEmpty()) {
    return false;
  }
  return mPaintArea->saveImage(fileName, fileFormat.constData());
}

void MainWindow::updatePenColor(QColor color) {
  mPenColorPixmap.fill(color);
  ui->label_currentColor->setPixmap(mPenColorPixmap);
}

void MainWindow::updateBrushImage(const QImage& image) {
  mBrushPixmap = QPixmap::fromImage(image);
  ui->label_currentBrush->setPixmap(mBrushPixmap);
}
