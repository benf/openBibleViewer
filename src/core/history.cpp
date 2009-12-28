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
#include "src/core/history.h"
#include <QtCore/QtDebug>
History::History()
{
}
/*!
    Return the next url in the history.
  */
QString History::forward()
{
    qDebug() << "history::forward() items = " << forwardItems;
    if (!forwardItems.isEmpty()) {
        goF = true;
        return forwardItems.takeLast();
    } else
        return QString();
}
/*!
    Return the previous url in the history.
  */
QString History::backward()
{
    qDebug() << "history::backward() items = " << backwardItems;
    if (backwardItems.size() >= 2) {
        goF = true;
        forwardItems.append(backwardItems.takeLast());
        return backwardItems.takeLast();
    } else
        return QString();
}
/*!
    Check if a next url is available.
  */
bool History::forwardAvailable()
{
    return !forwardItems.isEmpty();
}
/*!
    Check if a prevoius url is available.
  */
bool History::backwardAvailable()
{
    return backwardItems.size() >= 2;
}
/*!
    Add a new url to the history.
    \param url the new url
  */
void History::setCurrent(const QString &url)
{
    if (goF == true) {
        goF = false;
    } else {
        forwardItems.clear();
    }
    backwardItems.append(url);
}
