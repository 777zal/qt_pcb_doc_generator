#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QProcess>


class Terminal : public QObject
{
    Q_OBJECT
public:
    explicit Terminal(QObject *parent = nullptr);
    QString get_current_workdir(void);
    void set_source_path(QString *path);
    void set_target_path(QString *path);
    void enable_BOM(void);
    void enable_pickplace(void);
    void enable_gerber(void);
    void disable_BOM(void);
    void disable_pickplace(void);
    void disable_gerber(void);
    void generate_documents(QString folder_name);

private:
    QProcess *process;

    QString *source_path;
    QString *target_path;

    bool bom_is_enabled = false;
    bool pickplace_is_enabled = false;
    bool gerber_is_enabled = false;

    void generate_gerber_file(QString folder_name);
    void generate_bom_file(QString folder_name);
    void generate_pickplace_file(QString folder_name);

    void set_working_directory(QString *path);
    void create_folder(QString folder_name);


signals:
};

#endif // TERMINAL_H
