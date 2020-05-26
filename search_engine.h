#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QDir>
#include <QString>

#include "trie.h"

class SearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit SearchEngine(QObject *parent = nullptr);


    void makeIndex(const QString &dirPath);

    QVector<std::pair<QString, int> > search(const QString &word) const;


    bool running() const;

private:

signals:
    void progressChanged(float percentage);
    void indexingFinished(qint64 Ms);
    void searchFinished();
public slots:
    void abortIndexing();

private:
    Trie<Trie<int> > m_indexTable;
    QDir m_dir;
    bool m_stop;
    bool m_running;
};

#endif // INVERTEDINDEX_H
