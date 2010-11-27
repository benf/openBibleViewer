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
#ifndef VERSEREPLACER_H
#define VERSEREPLACER_H
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QStringList>
template<class T> class VerseReplacer
{
public:
    VerseReplacer();
    void setInsert(const int &verseID, const int &pos, const T &insert);
    void setPrepend(const int &verseID, const T &prepend);
    void setAppend(const int &verseID, const T &append);

    void exec(QList<T> *list);
private:
    QMap<int, QMap<int, T> > m_inserts;
    QMap<int, T> m_prepends;
    QMap<int, T> m_appends;
};

#endif // VERSEREPLACER_H
