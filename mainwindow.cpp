#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addFile(const QString &name)
{
    QStandardItem *item = new QStandardItem(name);
    searchOutputModel.invisibleRootItem()->appendRow(item);
}

void MainWindow::init()
{
    //setup the list
    ui->filesListView->setModel(&filesModel);

    filesModel.setFilter(QDir::Filter::Files);
    ui->filesListView->setRootIndex(filesModel.setRootPath(QDir::homePath()));

    ui->searchResultTreeView->setModel(&searchOutputModel);

    ui->searchResultTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(&searchEngine, &SearchEngine::indexingFinished, this, &MainWindow::indexingFinished);
}


void MainWindow::on_browsButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);

    if(dialog.exec())
    {
        ui->filesListView->setRootIndex(filesModel.setRootPath(dialog.directory().path()));
    }
}

void MainWindow::on_filesListView_activated(const QModelIndex &index)
{
    QString path = filesModel.filePath(index);

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    ui->fileText->setText(file.readAll());
}

void MainWindow::on_searchResultTreeView_activated(const QModelIndex &index)
{
    QString path =  QDir(filesModel.rootPath()).absoluteFilePath(searchOutputModel.data(index).toString());

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    ui->fileText->setText(file.readAll());
}

void MainWindow::indexingFinished(qint64 time)
{
    QString string = "indexing finished in %1 Sec";

    QMessageBox message(QMessageBox::Icon::Information, "info", string.arg(time /1000.0));
    message.exec();
}

void MainWindow::on_searchInput_textChanged(const QString &arg1)
{
    searchOutputModel.clear();
    const auto &resultList = searchEngine.search(arg1);

    for(const auto &result: resultList)
    {
        addFile(result);
    }
}

void MainWindow::on_searchInput_returnPressed()
{
    on_searchInput_textChanged(ui->searchInput->text());
}

void MainWindow::on_indexingButton_clicked()
{
    if(searchEngine.running())
    {
        return;
    }
    QProgressDialog progress("Making index", "Abort indexing", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    connect(&searchEngine, &SearchEngine::progressChanged, &progress, &QProgressDialog::setValue);
    connect(&progress, &QProgressDialog::canceled, &searchEngine, &SearchEngine::abortIndexing);
    searchEngine.makeIndex(filesModel.rootPath());
}


