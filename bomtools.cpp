#include "bomtools.h"

BomTools::BomTools(QObject *parent)
    : FileManager{parent}
{

}

void BomTools::set_pcb_source(QString *path){
    source_path = *path;
}

void BomTools::set_doc_target(QString *path){
    target_path = *path;
}

int BomTools::_get_component_index(QDomElement *root){
    int total_child=0;
    int component_index=0;
    total_child = root->childNodes().length();

    for(int i=0; i< total_child; i++){
        if(root->childNodes().at(i).toElement().tagName() == "components"){
            component_index = i;
            break;
        }
    }

    return component_index;
}

int BomTools::_get_value_index(QDomElement *root){
    int total_child=0;
    int value_index=0;
    int component_index=0;
    total_child = root->childNodes().length();
    component_index = _get_component_index(root);

    for(int i=0; i< total_child; i++){
        if(root->childNodes().at(component_index).toElement().\
                firstChild().toElement().\
                childNodes().at(i).toElement().\
                tagName() == "value"){
            qDebug() << "value found at "<< i;
            value_index = i;
            break;
        }
    }

    return value_index;
}

int BomTools::_get_footprint_index(QDomElement *root){
    int total_child     = 0;
    int footprint_index = 0;
    int component_index=0;
    total_child = root->childNodes().length();
    component_index = _get_component_index(root);

    for(int i=0; i< total_child; i++){
        if(root->childNodes().at(component_index).toElement().\
            firstChild().toElement().\
            childNodes().at(i).toElement().\
            tagName() == "footprint"){
            qDebug() << "value found at "<< i;
            footprint_index = i;
            break;
        }
    }

    return footprint_index;
}

QStringList BomTools::_get_value_by_column(QDomElement *root){
    QStringList column;
    int component_index =0;
    int value_index     =0;
    int total_component =0;

    value_index     = _get_value_index(root);
    component_index = _get_component_index(root);
    total_component = root->childNodes().at(component_index).toElement().childNodes().length();

    column.clear();
    for(int i=0; i< total_component; i++){
        column << root->childNodes().at(component_index).toElement().\
                        childNodes().at(i).toElement().\
                        childNodes().at(value_index).toElement().\
                        text();
    }

    return column;
}

QStringList BomTools::_get_footprint_by_column(QDomElement *root){
    QStringList column;
    QString row;
    int component_index =0;
    int footprint_index =0;
    int total_component =0;

    footprint_index = _get_footprint_index(root);
    component_index = _get_component_index(root);
    total_component = root->childNodes().at(component_index).toElement().childNodes().length();

    column.clear();
    for(int i=0; i< total_component; i++){
        row = root->childNodes().at(component_index).toElement().\
                    childNodes().at(i).toElement().\
                    childNodes().at(footprint_index).toElement().\
                    text();
        row.remove(0, row.indexOf(':')+1);
        column << row;
    }

    return column;
}

QStringList BomTools::_get_component_list_by_column(QDomElement *root){
    QStringList column;
    QString row;
    int component_index =0;
    int footprint_index =0;
    int total_component =0;

    footprint_index = _get_footprint_index(root);
    component_index = _get_component_index(root);
    total_component = root->childNodes().at(component_index).toElement().childNodes().length();

    column.clear();
    // child.childNodes().at(i-1).toElement().attribute("ref")
    for(int i=0; i< total_component; i++){
        row = root->childNodes().at(component_index).toElement().\
              childNodes().at(i).toElement().\
              attribute("ref");
        column << row;
    }

    return column;
}
QString BomTools::_get_schematic_path(QString *source_path){
    QString schematic_path;
    schematic_path = *source_path;
    schematic_path.replace(schematic_path.length()-4, 4,"_sch");

    return schematic_path;
}

QStringList BomTools::_append_scores_to_list(QStringList *list){
    int length = 0;

    bool isSame;

    QStringList tag;
    int tag_sequence=0;

    length = list->length();

    for(int i=0; i<length; i++){
        isSame = false;
        for(int j=0; j<i; j++){
            if(list->at(i)== list->at(j)){
                tag << tag.at(j);
                isSame = true;
                break;
            }
        }
        if(!isSame){
            tag_sequence = tag_sequence +1;
            tag << QString::number(tag_sequence);
        }
    }

    return tag;
}

int BomTools::_get_max_score_value(QStringList *list){
    int max=0;

    for(int i=0; i<list->length(); i++){
        // qDebug() << "i " << i << list->at(i).toInt();
        if(list->at(i).toInt() > max){
            max = list->at(i).toInt();
            // qDebug() << "i " << i << " max " << max;
        }
    }
    return max;
}
QStringList BomTools::_cluster_based_on_scores(QStringList *list1, QStringList *list2){
    int length = 0;

    bool isSame;

    QStringList tag;
    int tag_sequence=0;

    length = list1->length();

    for(int i=0; i<length; i++){
        isSame = false;
        for(int j=0; j<i; j++){
            if((list1->at(j) == list1->at(i)) && (list2->at(j) == list2->at(i))){
                tag << tag.at(j);
                isSame = true;
                break;
            }
        }
        if(!isSame){
            tag_sequence = tag_sequence +1;
            tag << QString::number(tag_sequence);
        }
    }

    return tag;
}

bool BomTools::_set_xml_domelement_root(QString *file_path){
    QDomDocument xmlDom;

    QFile f(*file_path);

    if(!f.open(QIODevice::ReadOnly)){
        return false;
    }

    xmlDom.setContent(&f);
    f.close();

    if(!xmlDom.documentElement().hasChildNodes()){
        return false;
    }

    xml_root = xmlDom.documentElement();
    return true;
}

bool BomTools::generate_kicad_template_file(void){
    QStringList args;
    QString cmd;
    QString schematic_path;
    QString resp;

    qDebug() << "Generating BOM File";

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
    resp.append('/'+BOM_FILE_NAME+".xml");

    args.clear();
    schematic_path = _get_schematic_path(&source_path);
    args << "-c" << "kicad-cli sch export python-bom -o " + resp + " " + \
                        schematic_path;
    qDebug() << args;
    terminal_process->start(cmd, args);
    terminal_process->waitForFinished();
    QString error = terminal_process->readAllStandardError();
    terminal_process->close();

    if(!error.isEmpty()){
        return false;
    }

    return true;
}

QByteArray BomTools::_compose_as_jlc_pcb_format(QStringList components, \
                                                QStringList values,
                                                QStringList footprints){
    QString data;
    QString temp_comment;
    QString temp_designator;
    QString temp_footprint;
    int number_of_component=0;

    /* compose first row */
    data.append("Comment,");
    data.append("Designator,");
    data.append("Footprint,");
    data.append("Part,");
    data.append("Amount\n");
    /*********************/

    auto tag1 = _append_scores_to_list(&values);
    auto tag2 = _append_scores_to_list(&footprints);
    auto tag3 = _cluster_based_on_scores(&tag1, &tag2);
    qDebug() << tag3;
    auto max_score = _get_max_score_value(&tag3);
    for(int i=1; i<=max_score; i++){
        temp_comment.clear();
        temp_designator.clear();
        temp_footprint.clear();
        number_of_component = 0;
        for(int j=0; j<components.length();j++){
            if(tag3.at(j).toInt() == i){
                temp_comment = values.at(j);
                temp_designator.append(components.at(j)+',');
                temp_footprint = footprints.at(j);
                number_of_component = number_of_component + 1;
            }
        }
        /* compose each row */
        data.append(temp_comment+',');
        temp_designator.removeLast(); // this to remove last comma
        data.append('"'+temp_designator+'"'+',');
        data.append(temp_footprint+',');
        data.append("TBD,");
        data.append(QString::number(number_of_component)+'\n');

        /********************/
    }
    return data.toLocal8Bit();
}
bool BomTools::generate_jlc_pcb_file(void){
    QString resp;
    QString target_doc_path;
    resp = get_current_workdir();
    target_doc_path = resp;
    resp.append('/'+BOM_FILE_NAME+".xml");
    target_doc_path.append("/03_bom_jlcpcb.csv");

    if(!_set_xml_domelement_root(&resp)){
        qDebug() << "Dom file failed to refer";
        return false;
    }

    QStringList components = _get_component_list_by_column(&xml_root);
    QStringList values = _get_value_by_column(&xml_root);
    QStringList footprints = _get_footprint_by_column(&xml_root);

    QString data_file = QString::fromLocal8Bit(_compose_as_jlc_pcb_format(components, values, footprints));

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
