/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009-2011 Paul Walger
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
#ifndef MODULE_H
#define MODULE_H
#include <QtCore/QString>
#include "src/core/settings/settings.h"
#include "src/module/dictionary/biblequote-dict.h"
#include "src/module/dictionary/zefania-lex.h"
#include "src/module/bible/biblemodule.h"
class Module
{
public:
    Module(Module *parent = 0);
    virtual ~Module();
    enum ModuleClass {
        NoneClass = 0,
        FolderClass = 3,
        BibleModuleClass = 1,
        DictionaryModuleClass = 2
    };
    enum ModuleType {
        NoneType = 0,
        BibleQuoteModule = 1,
        ZefaniaBibleModule = 2,
        ZefaniaLexModule = 3,
        BibleQuoteDictModule = 4,
        TheWordBibleModule = 5
    };

    QString path() const;
    QString title() const;
    int moduleID() const;
    ModuleClass moduleClass() const;
    ModuleType moduleType() const;

    void setPath(const QString &path);
    void setTitle(const QString &title);
    void setModuleID(const int &moduleID);
    void setModuleClass(const ModuleClass &c);
    void setModuleType(const ModuleType &t);

    BibleModule *m_bibleModule;
    ZefaniaLex *m_zefaniaLex;
    BibleQuoteDict *m_bibleQuoteDict;

    void setSettings(Settings *settings);

    void append(Module *child);
    Module *parent() const;
    static QStringList moduleTypeNames();
private:
    Module* m_parent;
    QList<Module *> m_children;
    Settings *m_settings;

    QString m_path;
    QString m_title;
    int m_id;
    ModuleClass m_moduleClass;
    ModuleType m_moduleType;
};

#endif // MODULE_H
