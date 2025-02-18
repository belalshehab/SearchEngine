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

private:
    void init();
private slots:
    void on_browsButton_clicked();

    void on_filesListView_activated(const QModelIndex &index);


    void on_searchResultTreeView_activated(const QModelIndex &index);

    void indexingFinished(qint64 time);

    void on_searchInput_textChanged(const QString &arg1);

    void on_searchInput_returnPressed();

    void onSearchFinished(qint64 nano);

    void makeIndex();

    void filesAdded();
    void filesRemoved();


private:
    Ui::MainWindow *ui;
    QFileSystemModel filesModel;
    QStandardItemModel searchOutputModel;

    SearchEngine searchEngine;
    QFuture<void> future;
};
#endif // MAINWINDOW_H
