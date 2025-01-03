#include "terminal.h"
#include <QDebug>
#include <QFile>
#include <QDomDocument>

Terminal::Terminal(QObject *parent)
    : QObject{parent}
{
    process = new QProcess(parent);
}

QString Terminal::get_current_workdir(void)
{
    QString cmd, resp;
    QStringList args;
    args.clear();
    cmd = "sh";
    args << "-c" << "pwd";
    process->start(cmd, args);
    process->waitForFinished();
    resp = process->readAllStandardOutput();
    qDebug() << "pwd : " << resp;
    resp.removeLast();
    process->close();
    return resp;
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

void Terminal::set_working_directory(QString *path){
    if(path == nullptr){
        return;
    }    
    process->setWorkingDirectory(*path);
    process->waitForFinished();
    process->close();
}

void Terminal::create_folder(QString folder_name){
    QStringList args;
    QString cmd;

    cmd = "sh";
    args.clear();
    args << "-c" << "mkdir "+folder_name;
    qDebug() << "Creates: " << folder_name;
    process->start(cmd, args);
    process->waitForFinished();
    qDebug() << process->readAllStandardError();
    process->close();
}

void Terminal::generate_documents(QString folder_name)
{
    create_folder(folder_name);
    if(bom_is_enabled){
        generate_bom_file(folder_name);
    }
    if(gerber_is_enabled){
        generate_gerber_file(folder_name);
    }
    if(pickplace_is_enabled){
        generate_pickplace_file(folder_name);
    }
}

void Terminal::generate_gerber_file(QString folder_name)
{
    QStringList args;
    QString cmd;
    QString resp;

    qDebug() << "Generating Pick Place";

    set_working_directory(target_path);
    cmd = "sh";
    resp = get_current_workdir();
    resp.append("/"+folder_name);

    set_working_directory(&resp);
    create_folder("GBR");

    resp = get_current_workdir();
    resp.append("/GBR");

    set_working_directory(&resp);

    args.clear();
    args << "-c" << "kicad-cli pcb export gerbers -o " + \
                                                  resp + \
                                                  " -l F.Cu," + \
                                                 "In1.Cu," + \
                                                 "In2.Cu," + \
                                                 "B.Cu," + \
                                                 "F.Paste," + \
                                                 "B.Paste," + \
                                                 "F.Silkscreen," + \
                                                 "B.Silkscreen," + \
                                                 "F.Mask," + \
                                                 "B.Mask," + \
                                                 "Edge.Cuts" + \
                                                 " --ev" + \
                                                 " --no-x2" + \
                                                 " --no-netlist" + \
                                                 " --subtract-soldermask " + \
                                                 *source_path;
    qDebug() << args;
    process->start(cmd, args);
    process->waitForFinished();
    qDebug() << "data"  << process->readAllStandardOutput();
    qDebug() << "error" << process->readAllStandardError();
    process->close();

    set_working_directory(&resp);
    args.clear();
    args << "-c" << "kicad-cli pcb export drill -o " + resp + "/" + " --format excellon" + \
                                                              " --drill-origin absolute" + \
                                                              " --excellon-zeros-format decimal" + \
                                                              " --excellon-oval-format route" + \
                                                              " --excellon-units in" + \
                                                              " --generate-map" + \
                                                              " --excellon-separate-th" + \
                                                              " --map-format gerberx2 " + \
                                                              *source_path;
    qDebug() << args;
    process->start(cmd, args);
    process->waitForFinished();
    qDebug() << "data"  << process->readAllStandardOutput();
    qDebug() << "error" << process->readAllStandardError();
    process->close();

    args.clear();
    args << "-c" << "zip -r ../"+folder_name+"_gerberdrill.zip .";
    qDebug() << args;
    process->start(cmd, args);
    process->waitForFinished();
    qDebug() << "data"  << process->readAllStandardOutput();
    qDebug() << "error" << process->readAllStandardError();
    process->close();
}

void Terminal::generate_bom_file(QString folder_name){
    QStringList args;
    QString sch_path;
    QString cmd;
    QString resp;
    QString temporary_file_path;
    QString final_file_path;
    QDomDocument xmlBom;

    qDebug() << "Generating BOM";

    set_working_directory(target_path);
    cmd = "sh";
    resp = get_current_workdir();
    resp.append("/"+folder_name);

    set_working_directory(&resp);
    create_folder("BOM");

    resp = get_current_workdir();
    resp.append("/BOM");

    set_working_directory(&resp);
    temporary_file_path.append(resp);
    temporary_file_path.append("/bill_of_material.xml");
    final_file_path.append(resp);
    final_file_path.append("/bill_of_material.csv");
    // resp.append("/bill_of_material.xml");
    args.clear();
    sch_path = *source_path;

    sch_path.replace(sch_path.length()-4, 4,"_sch");
    qDebug() << sch_path;
    args << "-c" << "kicad-cli sch export python-bom -o " + temporary_file_path + " " + \
                              sch_path;
    qDebug() << args;
    process->start(cmd, args);
    process->waitForFinished();
    qDebug() << "data"  << process->readAllStandardOutput();
    qDebug() << "error" << process->readAllStandardError();
    process->close();

    /***********************************/
    /******  Manipulate XML Files ******/
    /***********************************/
    QFile f(temporary_file_path);
    QDomElement root, child;
    int total_child, component_index, total_component;
    int index_value, index_footprint;

    if (!f.open(QIODevice::ReadOnly ))
    {
        qDebug() << "Load XML Error";
    }
    xmlBom.setContent(&f);
    f.close();

    root = xmlBom.documentElement();

    if(root.hasChildNodes()){
        total_child = root.childNodes().length();
    } else
    {
        // may require exit
    }

    for(int i=0; i< total_child; i++){
        if(root.childNodes().at(i).toElement().tagName() == "components"){
            component_index = i;
            break;
        }
    }

    child = root.childNodes().at(component_index).toElement();
    total_component = child.childNodes().length();

    for(int i=0; i< child.firstChild().toElement().childNodes().length(); i++){
        if(child.firstChild().toElement().childNodes().at(i).toElement().tagName() == "value"){
            qDebug() << "Value found at " << i;
            index_value = i;
            break;
        }
    }

    for(int i=0; i< child.firstChild().toElement().childNodes().length(); i++){
        if(child.firstChild().toElement().childNodes().at(i).toElement().tagName() == "footprint"){
            qDebug() << "Footprint found at " << i;
            index_footprint = i;
            break;
        }
    }

    QString file_data;
    QString temp, row, value, footprint;
    QStringList tag1, tag2, tag3, temp_list;
    bool isSame;
    int number_tag1 = 0;
    int number_tag2 = 0;
    int number_tag3 = 0;
    int j;
    tag1.clear();
    // tag1 << QString::number(number_tag1);
    for(int i=0; i<=total_component; i++){
        temp.clear();
        if(i==0){
            temp.append("Comment,");
            temp.append("Designator,");
            temp.append("Footprint,");
            temp.append("Part,");
            temp.append("Tag1,");
            temp.append("Tag2,");
            temp.append("Tag3\n");
        }
        else{
            qDebug() << " " << i;
            value = child.childNodes().at(i-1).toElement().childNodes().at(index_value).toElement().text();
            temp.append(value+',');
            temp.append(child.childNodes().at(i-1).toElement().attribute("ref")+',');
            row = child.childNodes().at(i-1).toElement().childNodes().at(index_footprint).toElement().text();
            footprint = row;
            row.remove(0, row.indexOf(':')+1);
            temp.append(row+',');
            temp.append("null,");

            /**************************/
            /**** Scoring of Value ****/
            /**************************/
            isSame = false;
            for(int j=1; j < i; j++){
                // qDebug() << " " << j-1 << child.childNodes().at(j-1).toElement().childNodes().at(index_value).toElement().text();
                // qDebug() << " " << j-1 << value;

                if(child.childNodes().at(j-1).toElement().childNodes().at(index_value).toElement().text() == value){
                    tag1 << tag1.at(j-1);
                    isSame = true;
                    // qDebug() << j-1 << " value same";
                    // qDebug() << j-1 << " " << tag1.at(j-1);
                    break;
                }
            }

            if(!isSame){
                number_tag1 = number_tag1+1;
                tag1 << QString::number(number_tag1);
            }

            // qDebug() << tag1;
            // qDebug() << "tag " << i-1 << tag1.at(i-1);
            temp.append(tag1.at(i-1)+',');


            /******************************/
            /**** Scoring of Footprint ****/
            /******************************/
            isSame = false;
            for(int j=1; j < i; j++){
                // qDebug() << " " << j-1 << child.childNodes().at(j-1).toElement().childNodes().at(index_footprint).toElement().text();
                // qDebug() << " " << j-1 << footprint;

                if(child.childNodes().at(j-1).toElement().childNodes().at(index_footprint).toElement().text() == footprint){
                    tag2 << tag2.at(j-1);
                    isSame = true;
                    // qDebug() << j-1 << " value same";
                    // qDebug() << j-1 << " " << tag2.at(j-1);
                    break;
                }
            }

            if(!isSame){
                number_tag2 = number_tag2+1;
                tag2 << QString::number(number_tag2);
            }

            // qDebug() << tag1;
            // qDebug() << "tag " << i-1 << tag1.at(i-1);
            temp.append(tag2.at(i-1)+',');

            /********************/
            /**** Clustering ****/
            /********************/
            isSame = false;
            for(int j=1; j < i; j++){
                if((tag1.at(j-1) == tag1.at(i-1)) && (tag2.at(j-1) == tag2.at(i-1))){
                    tag3 << tag3.at(j-1);
                    isSame = true;
                    break;
                }
            }

            if(!isSame){
                number_tag3 = number_tag3+1;
                tag3 << QString::number(number_tag3);
            }
            temp.append(tag3.at(i-1)+"\n");
            temp_list << temp;
        }

        file_data.append(temp);
    }

    QString temp_buff, val_buff, csv_data;
    QString temp_value, temp_footprint;
    QStringList data_list;
    QStringList temp_slice;
    qDebug() << "Start Clustering";
    for (int i=1; i < number_tag3; i++)
    {
        temp_buff.clear();
        val_buff.clear();
        for(int j=0; j < temp_list.length(); j++){
            temp_slice.clear();
            temp_slice = temp_list.at(j).split(',');
            qDebug() << temp_slice;
            if(temp_slice.at(6).toInt() == i){
                val_buff.append(child.childNodes().at(j).toElement().attribute("ref")+',');
                temp_value = child.childNodes().at(j).toElement().childNodes().at(index_value).toElement().text();
                temp_footprint = child.childNodes().at(j).toElement().childNodes().at(index_footprint).toElement().text();
                temp_footprint.remove(0, temp_footprint.indexOf(':')+1);
            }
        }
        val_buff.removeLast();
        temp_buff.append(temp_value+',');
        temp_buff.append('"'+val_buff+'"'+',');
        temp_buff.append(temp_footprint+',');
        temp_buff.append("TBD\n");
        data_list << temp_buff;
    }
    csv_data.append("Comment,");
    csv_data.append("Designator,");
    csv_data.append("Footprint,");
    csv_data.append("Part\n");
    qDebug() << "**********************";
    for(int i=0; i< data_list.length(); i++){
        qDebug() << data_list.at(i);
        csv_data.append(data_list.at(i));
    }
    qDebug() << "**********************";


    f.setFileName(final_file_path);
    if(f.open(QIODevice::ReadWrite)){
        qDebug() << "File Opened";
        f.remove();
        f.close();
    }

    f.setFileName(final_file_path);
    if(f.open(QIODevice::ReadWrite)){
        qDebug() << "File Opened";
        f.write(csv_data.toLocal8Bit());
        f.close();
    }
    // f.setFileName(temporary_file_path);
    // if(f.remove()){
    //     qDebug() << "File removed";
    // };
}

void Terminal::generate_pickplace_file(QString folder_name){
    QStringList args;
    QStringList row;
    int number_of_length;
    QStringList column;
    QString temp;
    QString file_data;

    QString cmd;
    QString resp;
    QString temporary_file_path;
    QString final_file_path;
    QString data;

    qDebug() << "Generating Pick Place";

    set_working_directory(target_path);
    cmd = "sh";
    resp = get_current_workdir();
    resp.append("/"+folder_name);

    set_working_directory(&resp);
    create_folder("POS");

    resp = get_current_workdir();
    resp.append("/POS");

    set_working_directory(&resp);
    temporary_file_path.append(resp);
    temporary_file_path.append("/data.csv");
    final_file_path.append(resp);
    final_file_path.append("/pos_file.csv");
    // resp.append("/data.csv");
    args.clear();
    args << "-c" << "kicad-cli pcb export pos -o " + temporary_file_path + \
                                            " --side front" + \
                                            " --format csv" + \
                                            " --units mm" + \
                                            " --use-drill-file-origin " + \
                                            *source_path;
    qDebug() << args;
    process->start(cmd, args);
    process->waitForFinished();
    qDebug() << "data"  << process->readAllStandardOutput();
    qDebug() << "error" << process->readAllStandardError();
    process->close();

    QFile f(temporary_file_path);

    if(f.open(QIODevice::ReadOnly)){
        qDebug() << "File Opened";
        data = f.readAll();
        f.close();
    }

    row = data.split('\n');
    number_of_length = row.length()-1;
    for(int i =0; i< number_of_length; i++){
        column = row.at(i).split(',');
        temp.clear();
        if(i==0){
            temp.append("Designator,");
            temp.append("Mid X,");
            temp.append("Mid Y,");
            temp.append("Layer,");
            temp.append("Rotation\n");
        }
        else{
            temp.append(column.at(0)+',');
            temp.append(column.at(3)+"mm,");
            temp.append(column.at(4)+"mm,");
            temp.append(column.at(6)+',');
            temp.append(column.at(5)+'\n');
        }

        file_data.append(temp);
    }


    f.setFileName(final_file_path);

    if(f.open(QIODevice::ReadWrite)){
        qDebug() << "File Opened";
        f.write(file_data.toLocal8Bit());
        f.close();
    }

    f.setFileName(temporary_file_path);
    if(f.remove()){
        qDebug() << "File removed";
    };
}


