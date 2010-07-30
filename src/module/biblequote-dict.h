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
#ifndef BIBLEQUOTEDICT_H
#define BIBLEQUOTEDICT_H
#include <QtCore/QString>
#include <QtCore/QFile>
#include "src/core/settings.h"

class BibleQuoteDict
{
public:
    BibleQuoteDict();
    void setSettings(Settings *settings);
    QString readInfo(QFile &file);
    QString readInfo(QString data);
    void buildIndex();
    bool hasIndex();
    QString getEntry(const QString &id);
    void setID(const int &id, const QString &path);
private:
    Settings *m_settings;
    QString m_modulePath;
    int m_moduleID;
};

#endif // BIBLEQUOTEDICT_H
