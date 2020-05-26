#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), searchEngine(this)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    //setup the list
    ui->filesListView->setModel(&filesModel);

    filesModel.setFilter(QDir::Filter::Files);

    ui->filesListView->setRootIndex(filesModel.setRootPath(QDir::homePath()));

    ui->searchResultTreeView->setModel(&searchOutputModel);

    ui->searchResultTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->indexingButton, &QPushButton::clicked, this, &MainWindow::makeIndex);
    connect(&searchEngine, &SearchEngine::indexingFinished, this, &MainWindow::indexingFinished);
    connect(&searchEngine, &SearchEngine::filesAdded, this, &MainWindow::filesAdded);
    connect(&searchEngine, &SearchEngine::filesRemoved, this, &MainWindow::filesRemoved);
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
    QStringList headers;
    headers << "File Name";
    searchOutputModel.setHorizontalHeaderLabels(headers);

    QVector<std::pair<QString, int> > resultList = searchEngine.search(arg1);


    ui->searchSize->setText(QString::number(resultList.count()));

    for(const auto &result: resultList)
    {
        QStandardItem *item = new QStandardItem(result.first);
        searchOutputModel.invisibleRootItem()->appendRow(item);
    }
}

void MainWindow::on_searchInput_returnPressed()
{
    on_searchInput_textChanged(ui->searchInput->text());
}

void MainWindow::makeIndex()
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

void MainWindow::filesAdded()
{
    QString string = "files has been added to the directory and the index has been updated";

    QMessageBox message(QMessageBox::Icon::Information, "info", string);
    message.exec();
}

void MainWindow::filesRemoved()
{
    QString string = "files has been removed from the directory need to remake the index";

    QMessageBox message(QMessageBox::Icon::Information, "warn", string);
    message.exec();
    makeIndex();
}


