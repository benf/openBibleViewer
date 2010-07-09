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
#include "settings.h"
#include "dbghelper.h"
#include "QUrl"
Settings::Settings()
{
    m_moduleID = QMap<int, int>();

}
void Settings::setModuleIDinMap(const int &moduleID, const int &pos )
{
    m_moduleID[moduleID] = pos;
}
ModuleSettings Settings::getModuleSettings(const int &moduleID)
{
    int id = m_moduleID.value(moduleID);
    if (m_moduleSettings.size() < id || m_moduleSettings.size() == 0) {
        myDebug() << "No Settings aviable";
        return ModuleSettings();
    } else {
        return m_moduleSettings.at(id);
    }
}
void Settings::replaceModuleSettings(const int &bibleID, ModuleSettings m)
{
    int id = m_moduleID[bibleID];
    m_moduleSettings.replace(id, m);
}
void Settings::setBookCount(QString path,QMap<int, int>count)
{
    ModuleCache c = m_moduleCache[path];
    c.bookCount = count;
    m_moduleCache[path] = c;
}
void Settings::setBookNames(QString path, QStringList names)
{
    myDebug() << path << names.size();
    ModuleCache c = m_moduleCache[path];
    c.bookNames = names;
    m_moduleCache[path] = c;
}
void Settings::setTitle(QString path, QString title)
{
    ModuleCache c = m_moduleCache[path];
    c.title = title;
    m_moduleCache[path] = c;
}
ModuleCache Settings::getModuleCache(const QString &path)
{
    return m_moduleCache.value(path);
}
QString Settings::savableUrl(QString url)
{
    return url;
}
QString Settings::recoverUrl(QString url)
{
    return url;
}
