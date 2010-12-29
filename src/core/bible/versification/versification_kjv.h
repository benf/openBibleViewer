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
#ifndef VERSIFICATION_KJV_H
#define VERSIFICATION_KJV_H
#include "src/core/bible/versification.h"
#include <QtCore/QFlags>
#include <QtCore/QObject>
class Versification_KJV : public Versification
{
    Q_OBJECT
public:
    Versification_KJV();
    ~Versification_KJV();
    QStringList getBookNames(VersificationFilterFlags filter) const;
    QList<QStringList> multipleBookShortNames(VersificationFilterFlags filter) const;
    QStringList bookShortNames(VersificationFilterFlags filter) const;
    QList<int> maxChapter(VersificationFilterFlags filter) const;
    QList<QList<int> > maxVerse(VersificationFilterFlags filter) const;
};

#endif // VERSIFICATION_KJV_H
