#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextCharFormat>

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
    connect(&searchEngine, &SearchEngine::searchFinished, this, &MainWindow::onSearchFinished);

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
    QString path =  QDir(filesModel.rootPath()).absoluteFilePath(searchOutputModel.data(index.siblingAtColumn(0)).toString());

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QString fileText = file.readAll();

    QString query = ui->searchInput->text().trimmed();

    ui->fileText->setText(fileText);

    QTextCharFormat fmt;
    fmt.setBackground(Qt::yellow);
    QTextCursor cursor(ui->fileText->document());

    int startPos = 0;
    bool left, right;
    while((startPos = fileText.indexOf(query, startPos, Qt::CaseInsensitive)) != -1)
    {
        left = startPos == 0 ? true : fileText[startPos -1].isSpace();

        right = (startPos + query.length()) == (fileText.length()) ? true : fileText[startPos + query.length()].isSpace();

        if(left && right)
        {
            cursor.setPosition(startPos, QTextCursor::MoveAnchor);
            cursor.setPosition(startPos + query.length(), QTextCursor::KeepAnchor);
            cursor.setCharFormat(fmt);
        }
        ++startPos;
    }
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

    QVector<std::pair<QString, int> > resultList = searchEngine.search(arg1);

    QStringList headers;
    headers << "File Name" << "Count";
    searchOutputModel.setHorizontalHeaderLabels(headers);

    ui->searchSize->setText("Number of files: " + QString::number(resultList.count()));

    for(const auto &result: resultList)
    {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(result.first));
        items.append(new QStandardItem(QString::number(result.second)));

        searchOutputModel.invisibleRootItem()->appendRow(items);
    }
}

void MainWindow::on_searchInput_returnPressed()
{
    on_searchInput_textChanged(ui->searchInput->text());
}

void MainWindow::onSearchFinished(qint64 nano)
{
    double time = nano /1000.0;



    QString timeStr;
    if(time > 1000)
    {
        timeStr = QString::number(time /1000.0, 'f', 2) + "Ms";
    }
    else
    {
        timeStr = QString::number(time, 'f', 2) + "Us";
    }
    QString message = "Search took " + timeStr;
    ui->statusbar->showMessage(message);
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


