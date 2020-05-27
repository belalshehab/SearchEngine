#include "search_engine.h"

#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QElapsedTimer>

#include <omp.h>

SearchEngine::SearchEngine(QObject *parent) : QObject(parent),
    m_dir("", "", QDir::Name, QDir::Files), m_stop(false), m_running(false)
{
    m_fsw.addPath(m_dir.currentPath());
    connect(&m_fsw, &QFileSystemWatcher::directoryChanged, this, &SearchEngine::directoryChanged);
}

void SearchEngine::makeIndex(const QString &dirPath)
{
    if(m_running)
    {
        return;
    }
    m_running = true;

    emit indexingStarted();

    m_indexTable.clear();

    m_fsw.removePath(m_dir.currentPath());
    m_fsw.addPath(dirPath);

    m_dir.setPath(dirPath);

    QElapsedTimer timer;
    timer.start();

    QStringList listOfFilesNames = m_dir.entryList();
    listOfFilesNames.sort();

    int count = listOfFilesNames.count();


#pragma omp parallel for schedule(dynamic) //num_threads(1)
    for(int i = 0; i < count; ++i)
    {

        if(m_stop)
        {
            break;
        }
        QString fileName;
#pragma omp critical(sec1)
        {
            fileName = listOfFilesNames[i];
        }
        QString absoluteFileName{m_dir.absoluteFilePath(fileName)};

        if(i %100 == 0)
        {
            QApplication::processEvents();
            emit progressChanged((float(i) / count) *100);
        }

        QFile file(absoluteFileName);
        file.open(QFile::ReadOnly);

        QTextStream stream(&file);

        QString word;

        while (stream.status() == QTextStream::Ok)
        {
            stream >> word;
            if(word.isEmpty())
            {
                continue;
            }
#pragma omp critical(sec2)
            {
                m_indexTable.insert(word.toLower()).insert(fileName);
            }
        }
    }


    if(m_stop)
    {
        m_stop = false;
    }
    else
    {
        emit indexingFinished(timer.restart());
    }
    m_running = false;
}


QVector<std::pair<QString, int> > SearchEngine::search(const QString &word) const
{
    if(word.isEmpty())
    {
        return {};
    }
    return  m_indexTable[word.toLower()].getData();
}

void SearchEngine::abortIndexing()
{
    m_stop = true;
}

void SearchEngine::directoryChanged(const QString &)
{
    QSet<QString> oldList = m_dir.entryList().toSet();
    m_dir.refresh();
    QSet<QString> newList = m_dir.entryList().toSet();

    QSet<QString> added = newList - oldList;

    if(added.isEmpty())
    {
        emit filesRemoved();
    }

    else
    {
        for(const QString &fileName: added)
        {
            QFile file(m_dir.absoluteFilePath(fileName));
            file.open(QFile::ReadOnly);

            QTextStream stream(&file);

            QString word;


            while (stream.status() == QTextStream::Ok)
            {
                stream >> word;
                if(word.isEmpty())
                {
                    continue;
                }
                m_indexTable.insert(word.toLower()).insert(fileName);
            }
        }
        emit filesAdded();
    }
}

bool SearchEngine::running() const
{
    return m_running;
}

