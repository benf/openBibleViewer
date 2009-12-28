/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009 Paul Walger
This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program; if not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
#include "bible.h"
#include "src/core/settings.h"
#include "src/core/modulesettings.h"
#include "src/core/dbghelper.h"
#include "src/core/urlconverter.h"
#include <QtCore/QtDebug>
#include <QtCore/QMapIterator>
#include <QtCore/QDir>
#include <QtGui/QTextDocument>
Bible::Bible()
{
    m_bibleID = -1;
    m_settings = new Settings();
}
void Bible::setBibleType(const int &type)
{
    //todo:check if type is valid
    bibleType = type;
    return;
}
int Bible::loadBibleData(const int &bibleID, const QString &path)
{
    DEBUG_FUNC_NAME
    myDebug() << "bibleID = " << bibleID << " path = " << path << " bibleType =" << bibleType;
    m_bibleID = bibleID;
    switch (bibleType) {
    case BibleQuoteModule: {
        bq.setSettings(m_settings);

        bq.loadBibleData(bibleID, path);
        bibleName = bq.bibleName;
        bookCount = bq.bookCount;
        bookFullName = bq.bookFullName;
        bookPath = bq.bookPath;
        m_chapterAdd = 1;
        m_biblePath = bq.currentBiblePath;
        break;
    }
    case ZefaniaBibleModule: { //zefania bible

        ModuleSettings m = m_settings->getModuleSettings(m_bibleID);
        zef.setSettings(m_settings);

        zef.loadBibleData(bibleID, path);
        bibleName = m.moduleName;
        bookCount = zef.bookCount;
        bookFullName = zef.bookFullName;

        m_chapterAdd = 0;
        m_biblePath = zef.currentBiblePath;
        break;
    }
    }
    return 0;
}
/*!
  Load only the book without pharsing.
*/
int Bible::readBook(int id)
{
    m_bookID = id;
    qDebug() << "bible::readBook() id= " << id << " bibleType =" << bibleType;
    switch (bibleType) {
    case BibleQuoteModule: {
        chapterData.clear();
        chapterNames.clear();
        if (id < bookPath.size()) {
            bq.readBook(id, bookPath.at(id));
        } else {
            qDebug() << "bible::readBook() index out of range bookPath.size() = " << bookPath.size() << " , id = " << id;
            return 1;
        }
        chapterData = bq.chapterData;

        int cc = bookCount[id];
        if (bq.chapterZero == true) {
            for (int i = 0; i < cc; i++) {
                chapterNames << QString::number(i);
            }
        } else {
            for (int i = 1; i <= cc; i++) {
                chapterNames << QString::number(i);
            }
        }
        break;
    }
    case ZefaniaBibleModule: { //zefania
        chapterData.clear();
        chapterNames.clear();
        zef.readBook(id);
        chapterData = zef.chapterData;
        for (int i = 1; i <= zef.bookCount[id]; i++) {
            chapterNames << QString::number(i);
        }
        break;
    }
    }
    qDebug() << "bible::readBook() end";
    return 0;
}
void Bible::setSettings(Settings *set)
{
    DEBUG_FUNC_NAME
    m_settings = set;
    //  myDebug() << " setings.module.size() = " << m_settings->module.size() << " moduleID = " << m_settings->moduleID;
    zef.setSettings(m_settings);
    bq.setSettings(m_settings);
}
QString Bible::readChapter(int chapterID, int verseID = -1)
{
    m_chapterID = chapterID;
    return readVerse(chapterID, 0, -1, verseID, true);

}
/*!
    Pharse the loaded book.
 */
QString Bible::readVerse(int chapterID, int startVerse, int endVerse, int markVerseID = -1, bool saveRawData = false)
{
    DEBUG_FUNC_NAME
    //endVerse == -1 means all verse
    m_chapterID = chapterID;
    if (saveRawData)
        chapterDataList.clear();

    QString out = "";//Return
    QStringList versList;
    switch (bibleType) {
    case BibleQuoteModule: {
        if (chapterID >= chapterData.size()) {
            myDebug() << "index out of range index";
            break;
        }
        Chapter chapter = chapterData.at(chapterID);//get data for this chapter
        //find out whereto read verse
        int end;
        if (endVerse == -1) {
            end = chapter.data.size();
        } else {
            end  = endVerse;
        }
        for (int i = startVerse; i < end; i++) {
            //no title formatting, because it is already formatted
            QString vers = chapter.data.at(i);

            if (i == markVerseID) {
                vers.prepend("<b>");
                vers.append("</b>"); //make the current verse vold
            }
            versList << vers;
        }

        break;
    }
    case ZefaniaBibleModule: { //zefania
        qDebug() << "bible::readVerse() zefania read";

        if (chapterID >= chapterData.size()) {
            myDebug() << "index out of range index";
            break;
        }
        Chapter c = chapterData.at(chapterID);
        if (saveRawData)
            out = "<b><font size=\"+5\">" + c.bookName + " " + c.chapterName + "</font></b><br /><br />";//title
        if (saveRawData)
            chapterDataList << out;

        int end;
        if (endVerse == -1) {
            end = c.data.size();
        } else {
            end = endVerse;
        }
        for (int i = startVerse; i < end; i++) {
            QString vers = c.data.at(i);
            //main formatting
            /*
            if (m_notes != 0) {
                for (int n = 0; n < m_notes->getIDList().size(); ++n) {
                    QString noteID = m_notes->getIDList().at(n);
                    if (m_notes->getType(noteID) == "text") {
                        QString link = m_notes->getRef(noteID, "link");
                        UrlConverter urlConverter(UrlConverter::PersistentUrl, UrlConverter::None, link);
                        urlConverter.m_biblesIniPath = biblesIniPath;
                        urlConverter.pharse();

                        if (urlConverter.m_bibleID.toInt() == m_bibleID && urlConverter.m_bookID == m_bookID && urlConverter.m_chapterID == chapterID && urlConverter.m_verseID == i) {
                            vers.append("<a href=\"note://" + noteID + "\"><img src=\":/icons/16x16/view-pim-notes.png\" title=\"" + m_notes->getRef(noteID, "title") + "\"></a>");
                        }
                    }
                }
            }*/
            if (i == markVerseID) {
                vers.prepend("<a name=\"currentVerse\"><b>");
                vers.append("</b></a>");
            }
            if (m_settings->getModuleSettings(m_bibleID).zefbible_textFormatting == 0) {
                vers.prepend("<span style=\" font-style:italic;\">" + c.verseNumber.at(i) + "</span> ");
                vers.append("<br />");
            } else {
                vers.prepend(c.verseNumber.at(i) + " ");
            }
            //if is current verse



            versList << vers;
        }
        break;
    }
    }
    /*  if (m_notes != 0) {
          for (int n = 0; n <  m_notes->getIDList().size(); ++n) {
              QString noteID = m_notes->getIDList().at(n);
              if (m_notes->getType(noteID) == "mark") {
                  QString link = m_notes->getRef(noteID, "link");
                  UrlConverter urlConverter(UrlConverter::PersistentUrl, UrlConverter::None, link);
                  urlConverter.m_biblesIniPath = biblesIniPath;
                  urlConverter.pharse();

                  if (urlConverter.m_bibleID.toInt() == m_bibleID && urlConverter.m_bookID == m_bookID && urlConverter.m_chapterID == chapterID) {
                      myDebug() << "mark found id = " << noteID;
                      versList = toUniformHtml(versList);
                      if (m_notes->getRef(noteID, "start") == m_notes->getRef(noteID, "end")) {
                          int versID = m_notes->getRef(noteID, "start").toInt();
                          if (endVerse != -1) {
                              //todo: do something
                          }
                          QString vers = versList.at(versID);
                          int startPos = vers.lastIndexOf(m_notes->getRef(noteID, "startString"));
                          int endPos = vers.lastIndexOf(m_notes->getRef(noteID, "endString")) + m_notes->getRef(noteID, "endString").size();
                          myDebug() << "vers = " << vers << " startPos = " << startPos << " endPos = " << endPos << " startString = " << m_notes->getRef(noteID, "startString") << " endString = " << m_notes->getRef(noteID, "endString");

                          vers.insert(endPos, "</span>");
                          vers.insert(startPos, "<span name=\"mark\" style=\"background-color:" + m_notes->getRef(noteID, "color") + "\">");
                          versList.replace(versID, vers);

                      } else {
                          int startVersID = m_notes->getRef(noteID, "start").toInt();
                          int endVersID = m_notes->getRef(noteID, "end").toInt();
                          if (endVerse != -1) {
                              //todo: do something
                          }
                          if (startVersID >= 0 && endVersID >= 0 && startVersID < versList.size() && endVersID < versList.size()) {
                              myDebug() << " start = " << startVersID << " size = " << versList.size();
                              QString startVers = versList.at(startVersID);
                              QString endVers = versList.at(endVersID);
                              int startPos = startVers.lastIndexOf(m_notes->getRef(noteID, "startString"));
                              int endPos = endVers.lastIndexOf(m_notes->getRef(noteID, "endString")) + m_notes->getRef(noteID, "endString").size();
                              myDebug() << "StartVers = " << startVers << " endVers = " << endVers << " startPos = " << startPos << " endPos = " << endPos << " startString = " << m_notes->getRef(noteID, "startString") << " endString = " << m_notes->getRef(noteID, "endString");

                              startVers.insert(startPos, "<span name=\"mark\" style=\"background-color:" + m_notes->getRef(noteID, "color") + "\">");
                              endVers.insert(endPos, "</span>");
                              versList.replace(startVersID, startVers);
                              versList.replace(endVersID, endVers);
                          }
                      }

                  }
              }
          }
      }*/
    for (int i = 0; i < versList.size(); ++i) {
        out += versList.at(i);
        if (saveRawData)
            chapterDataList.append(versList.at(i));
    }
    if (saveRawData)
        lastout = out;
    return out;
}
QString Bible::toUniformHtml(QString string)
{
    QTextDocument t;
    t.setHtml(string);
    QString raw = t.toHtml();
    QString a1 = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
    QString a2 = "</p></body></html>";
    int i1 = raw.indexOf(a1);
    int i2 = raw.indexOf(a2);
    raw.remove(i2, raw.size());
    raw.remove(0, i1 + a1.size());
    return raw;
}
QStringList Bible::toUniformHtml(QStringList string)
{
    QString wholeText = string.join("[VERSEINSERT_FROM_OPENBIBLEVIEWER]");
    QTextDocument t;
    t.setHtml(wholeText);
    QString raw = t.toHtml();
    QString a1 = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
    QString a2 = "</p></body></html>";
    int i1 = raw.indexOf(a1);
    int i2 = raw.indexOf(a2);
    raw.remove(i2, raw.size());
    raw.remove(0, i1 + a1.size());
    return raw.split("[VERSEINSERT_FROM_OPENBIBLEVIEWER]");
}
/*!
  Search in the current module
  */
SearchResult Bible::search(SearchQuery query)
{
    lastSearchQuery = query;
    SearchResult result;
    switch (bibleType) {
    case BibleQuoteModule: {
        result = bq.search(query);
        break;
    }
    case ZefaniaBibleModule: {
        result = zef.search(query);
        break;
    }
    }
    return result;
}
QMap<int, QList<Chapter> > Bible::getZefCache()
{
    return zef.softCache();
}
void Bible::clearZefCache()
{
    zef.clearSoftCache();
}
void Bible::setZefCache(QMap<int, QList<Chapter> > cache)
{
    zef.setSoftCache(cache);
}
QStringList Bible::getSearchPaths()
{
    if (bibleType == ZefaniaBibleModule) {
        return QStringList();
    } else if (bibleType == BibleQuoteModule) {
        QStringList l;
        l.append(QString(m_biblePath + QDir::separator()));
        if (m_bookID < bookPath.size()) {
            QString p = bookPath.at(m_bookID);
            int pos = p.lastIndexOf(QDir::separator());
            if (pos != -1) {
                p = p.remove(pos, p.size());
            }
            if (!p.startsWith(m_biblePath)) {
                p = m_biblePath + QDir::separator() + p + QDir::separator();
            }
            l.append(p);
        }
        return l;
    }
    return QStringList();
}
void Bible::setNotes(Notes *n)
{
    m_notes = n;
}
int Bible::bibleID()
{
    return m_bibleID;
}
void Bible::setBibleID(const int &bible)
{
    m_bibleID = bible;
}

int Bible::bookID()
{
    return m_bookID;
}
int Bible::chapterID()
{
    return m_chapterID;
}
int Bible::chapterAdd()
{
    return m_chapterAdd;
}
