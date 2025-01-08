#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QDomDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fileDialog      = new FileDialog(this);
    terminal        = new Terminal(this);
    place_handler   = new PlaceTools();
    gerber_handler  = new GerberTools();
    bom_handler     = new BomTools();

    ui->pushButton_bom->setEnabled(false);
    ui->pushButton_gbr->setEnabled(false);
    ui->pushButton_pos->setEnabled(false);
    ui->pushButton_gen->setEnabled(false);

    connect(ui->pushButton_open,&QPushButton::clicked, this, &MainWindow::on_opendialog);
    connect(ui->pushButton_bom, &QPushButton::clicked, this, &MainWindow::on_bomEnabled);
    connect(ui->pushButton_pos, &QPushButton::clicked, this, &MainWindow::on_pickplaceEnabled);
    connect(ui->pushButton_gbr, &QPushButton::clicked, this, &MainWindow::on_gerberEnabled);
    connect(ui->pushButton_clr, &QPushButton::clicked, this, &MainWindow::on_clearSelect);
    connect(ui->pushButton_gen, &QPushButton::clicked, this, &MainWindow::on_generateFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_opendialog(void){

    if(fileDialog->get_root_path(&root_path))
    {
        ui->line_open->setText(root_path);
    }

    // target_path = terminal->get_current_workdir();
    // terminal->set_source_path(&root_path);
    // terminal->set_target_path(&target_path);

    place_handler->set_pcb_source(&root_path);
    if(!place_handler->generate_kicad_template_file()){
        qDebug() << "Generate failed!";
    }
    if(!place_handler->generate_jlc_pcb_file()){
        qDebug() << "Generate failed!";
    }

    gerber_handler->set_pcb_source(&root_path);
    if(!gerber_handler->generate_kicad_template_file()){
        qDebug() << "Generate failed";
    }

    bom_handler->set_pcb_source(&root_path);
    if(!bom_handler->generate_kicad_template_file()){
        qDebug() << "Generate Failed";
    }
    if(!bom_handler->generate_jlc_pcb_file()){
        qDebug() << "Generate Failed";
    }
    // ui->pushButton_bom->setEnabled(true);
    // ui->pushButton_gbr->setEnabled(true);
    // ui->pushButton_pos->setEnabled(true);
}

void MainWindow::on_bomEnabled(void){
    ui->label_report->setText("BOM is set to generate");
    terminal->enable_BOM();
    ui->line_bom->setText(target_path+'/'+fileDialog->get_root_file_name(&root_path)+'/'+fileDialog->get_root_file_name(&root_path)+"_bom.csv");
    ui->pushButton_gen->setEnabled(true);
}

void MainWindow::on_pickplaceEnabled(void){
    ui->label_report->setText("pickplace is set to generate");
    terminal->enable_pickplace();
    ui->line_pos->setText(target_path+'/'+fileDialog->get_root_file_name(&root_path)+'/'+fileDialog->get_root_file_name(&root_path)+"_pos.csv");
    ui->pushButton_gen->setEnabled(true);
}

void MainWindow::on_gerberEnabled(void){
    ui->label_report->setText("gerber is set to generate");
    terminal->enable_gerber();
    ui->line_gerber->setText(target_path+'/'+fileDialog->get_root_file_name(&root_path)+'/'+"gbr");
    ui->pushButton_gen->setEnabled(true);
}

void MainWindow::on_generateFile(void){
    terminal->generate_documents(fileDialog->get_root_file_name(&root_path));
}

void MainWindow::on_clearSelect(void){
    ui->label_report->setText("clear selection");
    terminal->disable_BOM();
    terminal->disable_pickplace();
    terminal->disable_gerber();

    ui->pushButton_bom->setEnabled(false);
    ui->pushButton_gbr->setEnabled(false);
    ui->pushButton_pos->setEnabled(false);
    ui->pushButton_gen->setEnabled(false);

    root_path.clear();
    target_path.clear();
    ui->line_open->setText("");
    ui->line_bom->setText("");
    ui->line_pos->setText("");
    ui->line_gerber->setText("");

}
