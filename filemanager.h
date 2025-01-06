#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QProcess>
#include <QDebug>

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = nullptr);

protected:
    QProcess *terminal_process=nullptr;
    void initialize_terminal(void);
    QString get_current_workdir(void);
    bool set_working_directory(QString *path);
    bool create_folder(QString folder_name);

    QByteArray read_file(QString &file_name);
    bool write_file(QString &file_name, QString &data);
    bool remove_file(QString &file_name);

signals:

};

#endif // FILEMANAGER_H
