#include "terminal.h"
#include <QDebug>

Terminal::Terminal(QObject *parent)
    : QObject{parent}
{
    process = new QProcess(parent);
}

QString Terminal::get_current_workdir(void)
{
    process->start("pwd");
    process->waitForFinished();
    return process->readAllStandardOutput();
}

void Terminal::set_source_path(QString *path){
    qDebug() << "source path: " << *path;
    Terminal::source_path = path;
}

void Terminal::set_target_path(QString *path){
    qDebug() << "target path: " << *path;
    Terminal::target_path = path;
}

void Terminal::enable_BOM(void){
    bom_is_enabled = true;
}
void Terminal::enable_pickplace(void){
    pickplace_is_enabled = true;
}
void Terminal::enable_gerber(void){
    gerber_is_enabled = true;
}
void Terminal::disable_BOM(void){
    bom_is_enabled = false;
}
void Terminal::disable_pickplace(void){
    pickplace_is_enabled = false;
}
void Terminal::disable_gerber(void){
    gerber_is_enabled = false;
}

void Terminal::change_directory_to_target_directory(void){
    QStringList args;

    if(target_path== nullptr){
        return;
    }

    args << *target_path;

    process->setWorkingDirectory(*target_path);
    process->waitForFinished();
    qDebug() << process->readAllStandardOutput();
    process->close();
}

void Terminal::create_target_folder_name(QString folder_name){
    QStringList args;
    QString cmd;

    cmd = "mkdir";
    args << folder_name;
    qDebug() << "target folder name :" << folder_name;
    process->start(cmd, args);
    process->waitForFinished();
    qDebug() << process->readAllStandardError();
    process->close();
}

void Terminal::enter_target_folder(QString folder_name){
    QStringList args;
    QString cmd;
    QString resp;
    cmd = "cd";
    args << "-c" << "mkdir seat_golden";
    qDebug() << "Enters: " << folder_name;
    process->start("sh", args);
    process->waitForFinished();
    process->close();

    args.clear();
    args << "-c" << "pwd";
    process->start("sh", args);
    process->waitForFinished();
    resp = process->readAllStandardOutput();
    resp.removeLast();
    resp.append("/seat_golden");
    process->close();

    args.clear();
    args << resp;
    process->setWorkingDirectory(resp);
    process->waitForFinished();
    process->close();

    args.clear();
    args << "-c" << "kicad-cli pcb export gerbers -o "+ resp + " -l F.Cu,In1.Cu,In2.Cu,B.Cu,F.Paste,B.Paste,F.Silkscreen,B.Silkscreen,F.Mask,B.Mask,Edge.Cuts --ev --no-x2 --no-netlist --subtract-soldermask "+*source_path;
    qDebug() << args;
    process->start("sh", args);
    process->waitForFinished();
    qDebug() << "data"  << process->readAllStandardOutput();
    qDebug() << "error" << process->readAllStandardError();
    process->close();
}

void Terminal::generate_documents(QString folder_name)
{
    change_directory_to_target_directory();
    create_target_folder_name(folder_name);
    enter_target_folder(folder_name);
    // generate_gerber_file(folder_name);
}

void Terminal::generate_gerber_file(QString folder_name)
{
    QStringList args;
    QString cmd;
    // args << folder_name+"_GBR";
    // process->start("mkdir", args);
    // process->waitForFinished();

    // process->start("cd", args);
    // process->waitForFinished();


    cmd = "touch";
    args << "files.txt";
    // cmd = "kicad-cli";
    // args << "pcb" << "export" << "gerbers" << "-o" << folder_name+"_GBR" \
    //      << "-l" << "F.Cu,In1.Cu,In2.Cu,B.Cu,F.Paste,B.Paste,F.Silkscreen,B.Silkscreen,F.Mask,B.Mask,Edge.Cuts" \
    //      << "--ev" << "--no-x2" << "--no-netlist" << "--substract-soldermas" << source_path;

    process->start(cmd, args);
    process->waitForFinished();
    qDebug() << process->readAllStandardError();
}

void Terminal::generate_bom_file(void){

}

void Terminal::generate_pickplace_file(void){

}


