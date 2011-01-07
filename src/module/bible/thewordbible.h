/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009-2010 Paul Walger
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
#ifndef THEWORDBIBLE_H
#define THEWORDBIBLE_H
#include "src/module/bible/biblemodule.h"

#include <QtCore/QFileInfo>
#include <QtGui/QProgressDialog>
#include <QtCore/QDir>
#include <utility>

#include "src/core/bible/versification/versification_kjv.h"
#include "src/core/dbghelper.h"

#include "CLucene.h"
#include "CLucene/_clucene-config.h"

class TheWordBible: public BibleModule
{
public:
    TheWordBible();
    ~TheWordBible();
    void setSettings(Settings *settings);
    void loadBibleData(const int &id, const QString &path);
    int readBook(const int &id);

    QString readInfo(QFile &file);
    QString readInfo(const QString &fileName);
    void search(const SearchQuery &query, SearchResult *res) const;
    bool hasIndex() const;
    void buildIndex();

    int moduleID() const;
    QString modulePath() const;
    QString moduleName(bool preferShortName = false) const;
    QMap<int, int> bookCount() const;
    BookNames getBookNames();
    Book book() const;
    QString uID() const;

private:
    Book m_book;
    QString indexPath() const;
    int m_moduleID;
    QString m_modulePath;

    bool hasNT() const;
    bool hasOT() const;
    bool hasONT() const;
    QHash<int, Book> m_books;
    QMap<int, int> m_bookCount;
    int m_currentBookID;
    Versification *m_versification;
    BookNames m_bookNames;
    QString m_moduleName;
    QString m_shortModuleName;
    QString m_uID;


};

#endif // THEWORDBIBLE_H
