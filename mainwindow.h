#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filedialog.h"
#include "terminal.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    FileDialog *fileDialog;
    QString root_path;
    QString target_path;
    Terminal *terminal;

private slots:
    void on_opendialog(void);
    void on_bomEnabled(void);
    void on_pickplaceEnabled(void);
    void on_gerberEnabled(void);
    void on_generateFile(void);
    void on_clearSelect(void);

};
#endif // MAINWINDOW_H
