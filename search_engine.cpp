#include "search_engine.h"

#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QDataStream>
#include <QApplication>
#include <QElapsedTimer>

#include <iostream>
#include <omp.h>
#include <fstream>


SearchEngine::SearchEngine(QObject *parent) : QObject(parent),
    m_dir("", "", QDir::Name, QDir::Files), m_stop(false), m_running(false)
{

}

void SearchEngine::makeIndex(const QString &dirPath)
{
    if(m_running)
    {
        return;
    }
    m_running = true;
    m_indexTable.clear();
    m_dir.setPath(dirPath);

    QElapsedTimer timer;
    timer.start();


    if(false)
        //    if(dir.exists("index.indx"))
    {
        QFile indexFile(m_dir.absoluteFilePath("index.indx"));

        indexFile.open(QIODevice::ReadOnly);

        QDataStream stream(&indexFile);
        stream.setVersion(QDataStream::Qt_5_12);

        //        stream >> indexTable;

        qInfo() << "Read index took " << timer.elapsed() << "Ms";
    }

    else
    {
        QStringList listOfFilesNames = m_dir.entryList();

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

            std::ifstream file(absoluteFileName.toStdString());

            std::string word;

            while(file >> word)
            {
#pragma omp critical(sec2)
                {
                    m_indexTable[QString::fromStdString(word)].insert(fileName);
                }
            }
        }
        if(!m_stop)
        {
            //            qInfo() << "saving the index table";

            //            QFile output(dir.absoluteFilePath("index.indx"));

            //            QDataStream dataStream(&output);

            //            dataStream.setVersion(QDataStream::Qt_5_12);

            //            output.open(QIODevice::WriteOnly);

            //            dataStream << indexTable;

            //            qInfo() << "saving index took " << timer.restart() << "Ms";
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


QVector<QString> SearchEngine::search(const QString &word) const
{
    QVector<QString> vec;
    auto x = m_indexTable[word].getData();

    for(int i = 0; i < x.size(); ++i)
    {
        vec.push_back(x[i].first);
    }

    return vec;
}

void SearchEngine::abortIndexing()
{
    m_stop = true;
}

bool SearchEngine::running() const
{
    return m_running;
}

