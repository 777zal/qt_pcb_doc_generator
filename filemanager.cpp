#include "filemanager.h"
#include <QFile>

FileManager::FileManager(QObject *parent)
    : QObject{parent}
{
    if(terminal_process == nullptr){
        qDebug() << "Intialize Terminal Service";
        terminal_process = new QProcess(parent);
    }
}

QString FileManager::get_current_workdir(void){
    QString cmd, resp;
    QStringList args;
    args.clear();
    cmd = "sh";
    args << "-c" << "pwd";
    terminal_process->start(cmd, args);
    terminal_process->waitForFinished();
    resp = terminal_process->readAllStandardOutput();
    // qDebug() << "pwd : " << resp;
    resp.removeLast();
    terminal_process->close();
    return resp;
}

bool FileManager::set_working_directory(QString *path){
    if(terminal_process == nullptr){
        return false;
    }
    terminal_process->setWorkingDirectory(*path);
    terminal_process->waitForFinished();
    terminal_process->close();

    return true;
}

bool FileManager::create_folder(QString folder_name){
    QStringList args;
    QString cmd;

    if(terminal_process == nullptr){
        return false;
    }
    cmd = "sh";
    args.clear();
    args << "-c" << "mkdir "+folder_name;
    qDebug() << "Creates: " << folder_name;
    terminal_process->start(cmd, args);
    terminal_process->waitForFinished();
    terminal_process->close();

    return true;
}

QByteArray FileManager::read_file(QString &file_name){
    QFile f(file_name);
    QByteArray data;
    if(f.open(QIODevice::ReadOnly)){
        qDebug() << "File Opened";
        data = f.readAll();
        f.close();
    }

    return data;
}

bool FileManager::write_file(QString &file_name, QString &data){
    QFile f(file_name);

    if(f.open(QIODevice::ReadWrite)){
        qDebug() << "File Opened";
        f.write(data.toLocal8Bit());
        f.close();
    } else {
        return false;
    }

    return true;
}

bool FileManager::remove_file(QString &file_name){
    QFile f(file_name);

    if(!f.remove()){
        return false;
    };

    return true;
}
