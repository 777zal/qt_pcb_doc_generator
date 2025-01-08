#ifndef BOMTOOLS_H
#define BOMTOOLS_H

#include <QObject>
#include "filemanager.h"
#include <QFile>
#include <QDomDocument>

class BomTools : public FileManager
{
public:
    explicit BomTools(QObject *parent = nullptr);

private:
    const QString BOM_FILE_NAME = "03_BOM_File";
    QString source_path;
    QString target_path;

    QDomElement xml_root;
    bool _set_xml_domelement_root(QString *file_path);

    int _get_component_index(QDomElement *root);
    int _get_value_index(QDomElement *root);
    int _get_footprint_index(QDomElement *root);

    QStringList _get_component_list_by_column(QDomElement *root);
    QStringList _get_value_by_column(QDomElement *root);
    QStringList _get_footprint_by_column(QDomElement *root);

    QStringList _append_scores_to_list(QStringList *list);
    QStringList _cluster_based_on_scores(QStringList *list1, QStringList *list2);
    int _get_max_score_value(QStringList *list);

    QString _get_schematic_path(QString *source_path);
    int _get_number_of_row(QByteArray *data);
    QStringList _get_row_component(QByteArray *data);
    QByteArray  _compose_as_jlc_pcb_format(QStringList components, \
                                           QStringList values,
                                           QStringList footprints);

public:
    void set_pcb_source(QString *path);
    void set_doc_target(QString *path);
    bool generate_kicad_template_file(void);
    bool generate_jlc_pcb_file(void);
signals:
};

#endif // BOMTOOLS_H
