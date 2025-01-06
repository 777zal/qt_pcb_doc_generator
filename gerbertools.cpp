#include "gerbertools.h"

GerberTools::GerberTools(QObject *parent)
    : FileManager{parent}
{

}

void GerberTools::set_pcb_source(QString *path){
    source_path = *path;
}

void GerberTools::set_doc_target(QString *path){
    target_path = *path;
}

bool GerberTools::generate_kicad_template_file(void){
    QStringList args;

    QString cmd;
    QString resp;

    qDebug() << "Generating Gerber File";

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
    create_folder(GERBER_DRILL_FOLDER);
    resp.append('/'+GERBER_DRILL_FOLDER);

    set_working_directory(&resp);
    // qDebug() << resp;
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
                        source_path;

    terminal_process->start(cmd, args);
    terminal_process->waitForFinished();
    QString error = terminal_process->readAllStandardError();
    terminal_process->close();

    if(!error.isEmpty()){
        return false;
    }

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
                        source_path;

    terminal_process->start(cmd, args);
    terminal_process->waitForFinished();
    error = terminal_process->readAllStandardError();
    terminal_process->close();

    if(!error.isEmpty()){
        return false;
    }
    return true;
}
