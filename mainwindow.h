#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <search_engine.h>

#include <QMainWindow>
#include <QDebug>
#include <QDir>
#include <QModelIndex>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QFuture>
#include <QProgressDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addFile(const QString &name);
private:
    void init();
private slots:
    void on_browsButton_clicked();

    void on_filesListView_activated(const QModelIndex &index);


    void on_searchResultTreeView_activated(const QModelIndex &index);

    void indexingFinished();

    void on_searchInput_textChanged(const QString &arg1);

    void on_indexingButton_clicked();


    void directoryLoaded(const QString &path);


private:
    Ui::MainWindow *ui;
    QFileSystemModel filesModel;
    QStandardItemModel searchOutputModel;

    SearchEngine searchEngine;
    QFuture<void> future;
};
#endif // MAINWINDOW_H
