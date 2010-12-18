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
#include "bibleapi.h"
#include "src/core/dbghelper.h"
#include <QWebElementCollection>
BibleApi::BibleApi(QObject *parent) :
    QObject(parent)
{
}
void BibleApi::activateBible(const int &bibleListID)
{
    DEBUG_FUNC_NAME
    m_moduleManager->bibleList()->setCurrentBibleListID(bibleListID);

    QWebElementCollection collection = m_frame->documentElement().findAll("td[class~=bibleListTitle]");
    foreach(QWebElement paraElement, collection) {
        paraElement.removeClass("active");
        if(paraElement.attribute("bibleListID") == QString::number(bibleListID)) {
            paraElement.addClass("active");
        }
    }
    m_actions->reloadActive();
}
void BibleApi::setFrame(QWebFrame *frame)
{
    m_frame = frame;
}
