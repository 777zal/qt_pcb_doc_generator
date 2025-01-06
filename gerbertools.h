#ifndef GERBERTOOLS_H
#define GERBERTOOLS_H

#include <QObject>
#include "filemanager.h"

class GerberTools : public FileManager
{
public:
    explicit GerberTools(QObject *parent = nullptr);

private:
    const QString GERBER_DRILL_FOLDER = "02_Gerber_Drill";
    QString source_path;
    QString target_path;

public:
    void set_pcb_source(QString *path);
    void set_doc_target(QString *path);
    bool generate_kicad_template_file(void);

signals:
};

#endif // GERBERTOOLS_H
