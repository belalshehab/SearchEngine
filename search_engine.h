#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QDir>
#include <QString>

class SearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit SearchEngine(QObject *parent = nullptr);


    void makeIndex(const QString &dirPath);

    QSet<QString> search(const QString &word) const;


private:

signals:
    void progressChanged(float percentage);
    void indexingFinished();
    void searchFinished();
public slots:
    void abortIndexing();

private:
    QMap<QString, QSet<QString> > indexTable;
    QDir dir;
    bool stop;
};

#endif // INVERTEDINDEX_H
