#include "filedialog.h"

FileDialog::FileDialog(QWidget *widget_source, QObject *parent)
    : QObject{parent}
{
    source_parent = widget_source;
}

int FileDialog::get_root_path(QString *path)
{
    if(source_parent == nullptr){
        return 0;
    }
    *path = QFileDialog::getOpenFileName(source_parent, tr("Open File"),
                                         "/home",
                                         tr("PCB File (*.kicad_pcb)"));
    return 1;
}

QString FileDialog::get_root_file_name(QString *path)
{
    QString parse = "";
    int end_idx, start_idx;
    // /home/jal/Documents/qt_proj/pcb_production_file_generator/build/Desktop_Qt_6_7_3-Debug\n"
    end_idx = path->indexOf(".kicad_pcb", Qt::CaseSensitivity::CaseSensitive);
    start_idx = 0;
    for(int i=end_idx; i>0; i--){
        if(path->at(i) == '/')
        {
            start_idx = i+1;
            break;
        }
    }
    parse = path->sliced(start_idx, end_idx-start_idx);

    return parse;
}

