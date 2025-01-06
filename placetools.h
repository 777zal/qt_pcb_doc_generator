#ifndef PLACETOOLS_H
#define PLACETOOLS_H

#include <QObject>
#include "filemanager.h"

class PlaceTools : public FileManager
{
public:
    explicit PlaceTools(QObject *parent = nullptr);

private:
    QString source_path;
    QString target_path;

    int _get_number_of_row(QByteArray *data);
    QStringList _get_row_component(QByteArray *data);
    QByteArray _compose_as_jlc_pcb_format(QByteArray *data);

public:
    void set_pcb_source(QString *path);
    void set_doc_target(QString *path);
    bool generate_kicad_template_file(void);
    bool generate_jlc_pcb_file(void);
signals:
};

#endif // PLACETOOLS_H
