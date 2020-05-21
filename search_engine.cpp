#include "search_engine.h"

#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QDataStream>
#include <QApplication>


#include <iostream>
#include <omp.h>
#include <fstream>


SearchEngine::SearchEngine(QObject *parent) : QObject(parent),
    dir("", "", QDir::Name, QDir::Files), stop(false)
{

}

void SearchEngine::makeIndex(const QString &dirPath)
{
    dir.setPath(dirPath);

    if(dir.exists("index.indx"))
    {
        QFile indexFile(dir.absoluteFilePath("index.indx"));
        indexFile.open(QIODevice::ReadOnly);
        QDataStream stream(&indexFile);
        stream.setVersion(QDataStream::Qt_5_12);

        stream >> indexTable;
    }

    else
    {
        QStringList listOfFilesNames = dir.entryList();


        int count = listOfFilesNames.count();

        // num_threads(1)
#pragma omp parallel for schedule(dynamic)
        for(int i = 0; i < count; ++i)
        {

            if(stop)
            {
                break;
            }
            QString fileName;
#pragma omp critical(sec1)
            {
                fileName = listOfFilesNames[i];
            }
            QString absoluteFileName{dir.absoluteFilePath(fileName)};

            if(i %100 == 0)
            {
//                QApplication::processEvents();
                //                            qInfo() << " In file number " << i << " from total " << count << " " << (float(i) / count) *100 << "%";
                emit progressChanged((float(i) / count) *100);
            }

            std::ifstream file(absoluteFileName.toStdString());

            std::string word;

            //        QFile f(dirPath);
            //        f.open(QIODevice::ReadOnly | QIODevice::Text);

            //        QTextStream stream(&f);
            //        QString QWord = QString::fromStdString(stdWord);

            while(file >> word)
            {
#pragma omp critical(sec2)
                {
                    indexTable[QString::fromStdString(word)].insert(fileName);
                }
            }
        }

        //    for(int i = 0; i < count; ++i)
        //    {

        //        QString fileName;
        //        fileName = listOfFilesNames[i];

        //        QString absoluteFileName{dir.absoluteFilePath(fileName)};

        //        if(i %100 == 0)
        //        {
        //            qInfo() << omp_get_thread_num() << " In file number " << i << " from total " << count << " " << (float(i) / count) *100 << "%";
        //        }

        //        std::ifstream file(absoluteFileName.toStdString());

        //        std::string stdWord;


        //        while(file >> stdWord)
        //        {
        //            const QString QWord = QString::fromStdString(stdWord);
        //            indexTable[QWord].insert(fileName);
        //        }
        //    }

        if(!stop)
        {
            qInfo() << "saving the index table";

            QFile output(dir.absoluteFilePath("index.indx"));
            QDataStream dataStream(&output);
            dataStream.setVersion(QDataStream::Qt_5_12);

            output.open(QIODevice::WriteOnly);

            dataStream << indexTable;
        }
    }

    if(stop)
    {
        stop = false;
    }
    else
    {
        emit indexingFinished();
    }
}


QSet<QString> SearchEngine::search(const QString &word) const
{
    return indexTable[word];
}

void SearchEngine::abortIndexing()
{
    stop = true;
}

