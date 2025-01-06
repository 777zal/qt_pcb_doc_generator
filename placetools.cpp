#include "placetools.h"
#include <QFile>

PlaceTools::PlaceTools(QObject *parent)
    : FileManager{parent}
{
    // initialize_terminal();
}

void PlaceTools::set_pcb_source(QString *path){
    source_path = *path;
}

void PlaceTools::set_doc_target(QString *path){
    target_path = *path;
}

int PlaceTools::_get_number_of_row(QByteArray *data){
    return QString::fromLocal8Bit(*data).split('\n').length()-1;
}

QStringList PlaceTools::_get_row_component(QByteArray *data){
    return QString::fromLocal8Bit(*data).split('\n');
}

QByteArray PlaceTools::_compose_as_jlc_pcb_format(QByteArray *data){
    QStringList column;
    QString temp_data;
    int num_of_row = _get_number_of_row(data);
    QStringList row = _get_row_component(data);

    for(int i =0; i < num_of_row; i++){
        column = row.at(i).split(',');
        // temp_data.clear();
        if(i==0){
            temp_data.append("Designator,");
            temp_data.append("Mid X,");
            temp_data.append("Mid Y,");
            temp_data.append("Layer,");
            temp_data.append("Rotation\n");
        }
        else{
            temp_data.append(column.at(0)+',');
            temp_data.append(column.at(3)+"mm,");
            temp_data.append(column.at(4)+"mm,");
            temp_data.append(column.at(6)+',');
            temp_data.append(column.at(5)+'\n');
        }
    }

    return temp_data.toLocal8Bit();
}

bool PlaceTools::generate_kicad_template_file(void){
    QStringList args;

    QString cmd;
    QString resp;

    qDebug() << "Generating Pick Place";

    if(!target_path.isEmpty())
    {
        qDebug() << "Target Path is Predefined";
        resp = target_path;
    } else {
        qDebug() << "Target Path is Empty";
        resp = get_current_workdir();
    }

    cmd = "sh";

    set_working_directory(&resp);
    resp.append("/01_pickplace_temp.csv");

    // qDebug() << resp;
    args.clear();
    args << "-c" << "kicad-cli pcb export pos -o " + resp + \
                        " --side front" + \
                        " --format csv" + \
                        " --units mm" + \
                        " --use-drill-file-origin " + \
                              source_path;

    terminal_process->start(cmd, args);
    terminal_process->waitForFinished();
    QString error = terminal_process->readAllStandardError();
    terminal_process->close();

    if(!error.isEmpty()){
        return false;
    }

    return true;
}

bool PlaceTools::generate_jlc_pcb_file(void){
    QString resp;
    QString target_doc_path;
    resp = get_current_workdir();
    target_doc_path = resp;
    resp.append("/01_pickplace_temp.csv");
    target_doc_path.append("/01_pickplace_jlcpcb.csv");
    // qDebug() << resp;

    QByteArray temp_data = read_file(resp);
    if(temp_data.isEmpty()){
        qDebug() << "Data Empty";
        return false;
    }

    QString data_file = QString::fromLocal8Bit(_compose_as_jlc_pcb_format(&temp_data));

    if(!write_file(target_doc_path, data_file)){
        qDebug() << "Write Failed";
        return false;
    }

    if(!remove_file(resp)){
        qDebug() << "Remove Failed";
        return false;
    }
    return true;
}
