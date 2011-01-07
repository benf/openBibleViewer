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
#include "urlconverter.h"
#include "dbghelper.h"
#include "src/core/bible/bibleurl.h"
UrlConverter::UrlConverter(const UrlType &from, const UrlType &to, const BibleUrl &url)
{
    m_from = from;
    m_to = to;
    m_bibleUrl = url;
    m_setBookNames = false;
}
UrlConverter::UrlConverter()
{
    m_setBookNames = false;
}
void UrlConverter::setModuleMap(ModuleMap *moduleMap)
{
    m_moduleMap = moduleMap;
}
void UrlConverter::setSettings(Settings *settings)
{
    m_settings = settings;
}
void UrlConverter::setV11n(Versification *v11n)
{
    m_v11n = v11n;
    m_setBookNames = true;
}

BibleUrl UrlConverter::convert()
{
    BibleUrl url = m_bibleUrl;
    if(m_to == InterfaceUrl) {
        myDebug() << "to interface url";
        myDebug() << m_bibleUrl.toString();
        url.clearRanges();
        foreach(BibleUrlRange range, m_bibleUrl.ranges()) {
            if(range.bible() == BibleUrlRange::LoadBibleByUID) {
                foreach(Module * module, m_moduleMap->m_map) {
                    myDebug() << m_settings->savableUrl(module->path()) << " vs " << range.bibleUID();
                    if(m_settings->savableUrl(module->path()) == range.bibleUID())  {
                        range.setBible(module->moduleID());
                    }
                }
            }
            url.addRange(range);
        }
        for(int i = 0; i < m_bibleUrl.ranges().size(); i++) {
            url.unsetParam("b" + QString::number(i));
        }
    } else if(m_to == PersistentUrl) {
        myDebug() << "to persisent url";
        myDebug() << m_bibleUrl.toString();
//todo: catch errors
        url.clearRanges();
        QList<int> bookIDs;

        foreach(BibleUrlRange range, m_bibleUrl.ranges()) {
            if(range.bible() == BibleUrlRange::LoadBibleByID) {
                range.setBible(m_settings->savableUrl(m_moduleMap->m_map.value(range.bibleID())->path()));
            }
            url.addRange(range);
            bookIDs.append(range.bookID());
        }
        if(m_setBookNames) {
            for(int i = 0; i < bookIDs.size(); i++) {
                url.setParam("b" + QString::number(i), m_v11n->bookName(bookIDs.at(i)));
            }
        }
    }
    return url;
}
void UrlConverter::setFrom(const UrlType &urlType)
{
    m_from = urlType;
}
void UrlConverter::setTo(const UrlType &urlType)
{
    m_to = urlType;
}
void UrlConverter::setUrl(const BibleUrl &url)
{
    m_bibleUrl = url;
}
