#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QDir>
#include <QString>
#include <QFileSystemWatcher>

#include "trie.h"

class SearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit SearchEngine(QObject *parent = nullptr);


    void makeIndex(const QString &dirPath);

    QVector<std::pair<QString, QVector<bool> > > search(const QString &word) const;


    bool running() const;

private:

signals:
    void progressChanged(float percentage);
    void indexingStarted();
    void indexingFinished(qint64 Ms);
    void searchFinished();

    void filesAdded();
    void filesRemoved();
public slots:
    void abortIndexing();
    void directoryChanged(const QString &);

private:
    Trie<Trie<QVector<bool> > > m_indexTable;
    QDir m_dir;
    QFileSystemWatcher m_fsw;


    bool m_stop;
    bool m_running;
};

#endif // INVERTEDINDEX_H
