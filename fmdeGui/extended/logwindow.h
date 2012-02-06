#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QDialog>
#include <QCloseEvent>

#include "qdebugstream.h"

namespace Ui {
    class LogWindow;
}

class LogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = 0);
    ~LogWindow();

public slots:
    void finish();
protected:
    void closeEvent(QCloseEvent *);


private:
    Ui::LogWindow *ui;

    QDebugStream * debugStream;

    bool running;

};

#endif // LOGWINDOW_H
