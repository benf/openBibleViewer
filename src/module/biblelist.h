#ifndef BIBLELIST_H
#define BIBLELIST_H
#include "src/module/bible.h"
#include <QPoint>
class BibleList
{
public:
    BibleList();
    QHash<int,Bible *> m_bibles;
    QMap<int,QPoint> m_biblePoints;
    Bible * bible();
    void addBible(Bible* b, QPoint p);

    int readBook(int id);
    QString readChapter(int chapterID, int verseID);
    QString readVerse(int chapterID, int startVerse, int endVerse, int markVerseID, bool saveRawDatas);

    int bookID();
    int chapterID();
    int verseID();

private:
    int m_currentBible;
    int m_bookID;
    int m_chapterID;
    int m_verseID;

};

#endif // BIBLELIST_H
