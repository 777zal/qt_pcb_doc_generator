#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QObject>
#include <QFileDialog>

class FileDialog : public QObject
{
    Q_OBJECT
public:
    explicit FileDialog(QWidget *widget_source, QObject *parent = nullptr);
    int get_root_path(QString *path);
    QString get_root_file_name(QString *path);
private:
    QWidget *source_parent;
    QString root_path;

signals:
};

#endif // FILEDIALOG_H
