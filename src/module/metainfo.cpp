/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009-2011 Paul Walger <metaxy@walger.name>
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
#include "metainfo.h"

MetaInfo::MetaInfo() : m_name(), m_shortName()
{
    m_defaultModule = OBVCore::NotADefaultModule;
    m_content = OBVCore::UnkownContent;
    m_name = "";
    m_shortName = "";
    m_uid = "";
}
QString MetaInfo::name() const
{
    return m_name;
}

QString MetaInfo::shortName() const
{
    return m_shortName;
}
QString MetaInfo::uid() const
{
    return m_uid;
}
OBVCore::DefaultModule MetaInfo::defaultModule() const
{
    return m_defaultModule;
}

void MetaInfo::setName(const QString &name)
{
    m_name = name;
}

void MetaInfo::setShortName(const QString &shortName)
{
    m_shortName = shortName;
}
void MetaInfo::setUID(const QString &uid)
{
    m_uid = uid;
}

void MetaInfo::setDefaultModule(const OBVCore::DefaultModule d)
{
    m_defaultModule = d;
}
void MetaInfo::setContent(const OBVCore::ContentType t)
{
    m_content = t;
}
OBVCore::ContentType MetaInfo::content() const
{
    return m_content;
}
