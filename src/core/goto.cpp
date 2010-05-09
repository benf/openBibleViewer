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
#include "src/core/goto.h"
#include "src/core/dbghelper.h"
#include <QtCore/QStringList>
#include <QtCore/QRegExp>
GoTo::GoTo(int currentBibleID, QStringList bookFullName)
{
    m_currentBibleID = currentBibleID;
    m_bookFullName = bookFullName;
}
QString GoTo::getUrl(const QString& text)
{
    if (text.size() < 1)
        return QString();
    QStringList reg;
    QRegExp foundRegExp;
    reg << "(.*)" << "(.*)(\\s+)(\\d+)"  << "(.*)(\\s+)(\\d+),(\\d+)";
    int found = -1;
    for (int i = reg.size() - 1; i >= 0; --i) {
        QRegExp r(reg.at(i));
        int pos = r.indexIn(text);
        if (pos > -1) {
            found = i;
            foundRegExp = r;
            break;
        }
    }
    if (found == 0) { //example: Hiob
        int bookID = bookNameToBookID(foundRegExp.cap(1));
        return "bible://" + QString::number(m_currentBibleID) + "/" + QString::number(bookID) + "," + QString::number(0) + "," + QString::number(0);

    } else if (found == 1) { //Hiob 4
        int bookID =  bookNameToBookID(foundRegExp.cap(1));
        int chapterID = foundRegExp.cap(3).toInt() - 1;
        return "bible://" + QString::number(m_currentBibleID) + "/" + QString::number(bookID) + "," + QString::number(chapterID) + "," + QString::number(0);
    } else if (found == 2) { //Hiob 4,9
        int bookID =  bookNameToBookID(foundRegExp.cap(1));
        int chapterID = foundRegExp.cap(3).toInt() - 1;
        int verseID = foundRegExp.cap(4).toInt() - 1;
        return "bible://" + QString::number(m_currentBibleID) + "/" + QString::number(bookID) + "," + QString::number(chapterID) + "," + QString::number(verseID);
    }
    return QString();
}
int GoTo::bookNameToBookID(const QString& name)
{
    //todo: use short names
    int min = -1, bookID = -1;
    for (int i = 0; i < m_bookFullName.size(); ++i) {
        if (name == m_bookFullName.at(i)) {
            bookID = i;
            break;
        }
    }
    if (bookID == -1) {
        for (int i = 0; i < m_bookFullName.size(); ++i) {
            if (m_bookFullName.at(i).startsWith(name, Qt::CaseInsensitive)) {
                bookID = i;
                break;
            }
        }
    }
    /*  if (bookID == -1) {
          for (int i = 0; i < bookFullName.size(); ++i) {
              if (bookFullName.at(i).contains(name, Qt::CaseInsensitive)) {
                  bookID = i;
                  break;
              }
          }
      }*/
    if (bookID == -1) {
        for (int i = 0; i < m_bookFullName.size(); ++i) {
            int lev = levenshteinDistance(name, m_bookFullName.at(i));
            if (lev < min || min < 0) {
                bookID = i;
                min = lev;
            }
        }
    }
    return bookID;
}
int GoTo::levenshteinDistance(const QString& s, const QString& t)
{
    QByteArray m_d;
    int m = s.length();
    int n = t.length();
    int dWidth = m + 1 ;

    // make sure the matrix is big enough

    if (m_d.size() < (m + 1) *(n + 1)) {
        m_d.resize((m + 1) *(n + 1));
    }

    int i;

    int j;

    // init 0..m, 0..n as starting values - distance to ""

    for (i = 0; i <= m; i++) {
        m_d[i + 0*dWidth] = i;
    }
    for (j = 0; j <= n; j++) {
        m_d[0 + j*dWidth] = j;
    }
    int cost;
    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            if (s[i-1] == t[j-1]) {
                // if current char is equal, no cost for substitution
                cost = 0;
            } else {
                cost = 1;
            }

            m_d[i + j*dWidth] = qMin(qMin(// min of three possibilities
                                         m_d[i-1 + (j)*dWidth] + 1,// deletion
                                         m_d[i   + (j-1)*dWidth] + 1),// insertion
                                     m_d[i-1 + (j-1)*dWidth] + cost);// substitution
        }
    }

    return m_d[m + n*dWidth];
}
