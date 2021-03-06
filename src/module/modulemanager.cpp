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
#include "modulemanager.h"
#include "src/core/verse/reftext.h"

ModuleManager::ModuleManager()
{
    m_moduleMap = QSharedPointer<ModuleMap>(new ModuleMap());
    m_rootModule = NULL;
}
ModuleManager::~ModuleManager()
{
    //DEBUG_FUNC_NAME
    m_moduleMap.clear();

    if(m_moduleModel != NULL) {
        delete m_moduleModel;
        m_moduleModel = NULL;
    }
    if(m_rootModule != NULL) {
        delete m_rootModule;
        m_rootModule = NULL;
    }
    if(m_moduleDisplaySettings != NULL) {
        delete m_moduleDisplaySettings;
        m_moduleDisplaySettings = NULL;
    }
}

void ModuleManager::setSettings(Settings *settings)
{
    m_settings = settings;
}

void ModuleManager::setNotes(Notes *notes)
{
    m_notes = notes;
}
void ModuleManager::newDisplaySettings()
{
    m_moduleDisplaySettings = new ModuleDisplaySettings();
}
ModuleDisplaySettings * ModuleManager::moduleDisplaySetings()
{
    return m_moduleDisplaySettings;
}

/**
  * Load all Modules, and generate a QStandardItemModel.
  */
int ModuleManager::loadAllModules()
{
    DEBUG_FUNC_NAME;
    //update module settings and display settings
    m_moduleMap->data.clear();
    ModuleSettings *root = new ModuleSettings();
    root->moduleID = -1;
    root->parentID = -2;

    foreach(ModuleSettings * set, m_settings->m_moduleSettings) {
        set->clearChildren();
    }
    //set parents
    {
        QMapIterator<int, ModuleSettings*> it2(m_settings->m_moduleSettings);
        while(it2.hasNext())  {
            it2.next();
            ModuleSettings *child = it2.value();
            const int parentID = child->parentID;
            if(parentID == -1) {
                root->appendChild(child);
                child->setParent(root);
            } else if(m_settings->m_moduleSettings.contains(parentID)) {
                ModuleSettings *r = m_settings->m_moduleSettings.value(parentID);
                r->appendChild(child);
                child->setParent(r);
            }
        }
        QSharedPointer<ModuleDisplaySettings> displaySettings = QSharedPointer<ModuleDisplaySettings>(new ModuleDisplaySettings());

        //todo: what is that?
        displaySettings->setShowStudyNotes(true);
        displaySettings->setShowStrong(true);
        displaySettings->setShowRefLinks(false);
        displaySettings->setShowNotes(true);
        displaySettings->setShowMarks(true);
        displaySettings->setShowBottomToolBar(true);
        displaySettings->setShowRMac(true);
        displaySettings->setShowCaptions(false);
        displaySettings->setShowProlog(false);

        root->setDisplaySettings(displaySettings);

        m_settings->m_moduleSettings.insert(-1, root);
    }
    //use parent settings display
    {
        QMapIterator<int, ModuleSettings*> it2(m_settings->m_moduleSettings);
        while(it2.hasNext())  {
            it2.next();
            ModuleSettings *child = it2.value();
            if(child->useParentSettings) {
                const int parentID = child->parentID;
                if(m_settings->m_moduleSettings.contains(parentID)) {
                    child->removeDisplaySettings();
                    ModuleSettings *r = m_settings->m_moduleSettings.value(parentID);
                    makeSureItHasLoaded(r);
                    child->setDisplaySettings(r->displaySettings());
                }
            }
        }
    }


    //The invisible root Module
    m_rootModule = new Module();
    m_rootModule->setSettings(m_settings);
    m_rootModule->setModuleID(-1);
    m_rootModule->setModuleClass(OBVCore::FolderClass);
    m_rootModule->setModuleType(OBVCore::NoneType);

    ModuleSettings *rootModuleSettings = m_settings->getModuleSettings(-1);//it the invisble root item
    if(rootModuleSettings != NULL) {
        foreach(ModuleSettings * s, rootModuleSettings->children()) {
            loadModule(m_rootModule, s);
        }
        ModuleModel model(0);
        model.setSettings(m_settings);
        model.setShowAll(true);
        model.generate();
        m_moduleModel = model.itemModel();
    } else {
        m_moduleModel = new QStandardItemModel;
    }


    return 0;
}
void ModuleManager::makeSureItHasLoaded(ModuleSettings *settings)
{
    if(settings->useParentSettings) {
        const int parentID = settings->parentID;
        if(m_settings->m_moduleSettings.contains(parentID)) {
            settings->removeDisplaySettings();
            ModuleSettings *r = m_settings->m_moduleSettings.value(parentID);
            makeSureItHasLoaded(r);
            settings->setDisplaySettings(r->displaySettings());
        }
    }
}
void ModuleManager::loadModule(Module *parentModule, ModuleSettings *settings)
{
    if(settings == NULL)
        return;
    Module *module = new Module(parentModule);
    module->setSettings(m_settings);
    module->setPath(settings->modulePath);
    module->setModuleType(settings->moduleType);

    module->setModuleID(settings->moduleID);

    //m_map deletes them
    m_moduleMap->data.insert(settings->moduleID, module);

    if(settings->moduleType == OBVCore::BibleQuoteModule
            || settings->moduleType == OBVCore::ZefaniaBibleModule
            || settings->moduleType == OBVCore::TheWordBibleModule
            || settings->moduleType == OBVCore::SwordBibleModule) {

        module->setModuleClass(OBVCore::BibleModuleClass);

    } else if(settings->moduleType == OBVCore::ZefaniaLexModule || settings->moduleType == OBVCore::BibleQuoteDictModule || settings->moduleType == OBVCore::WebDictionaryModule) {

        module->setModuleClass(OBVCore::DictionaryModuleClass);

    } else if(settings->moduleType == OBVCore::WebPageModule) {

        module->setModuleClass(OBVCore::WebPageClass);

    }
    myDebug() << module->moduleID() << module->moduleType() << module->moduleClass();

    //recursive
    foreach(ModuleSettings * s, settings->children()) {
        loadModule(module, s);
    }

}
void ModuleManager::initVerseModule(VerseModule *m) const
{
    if(m != NULL) {
        m->setSettings(m_settings);
        m->setNotes(m_notes);
        m->setModuleMap(m_moduleMap);
        m->setModuleDisplaySettings(m_moduleDisplaySettings);
    }
}
void ModuleManager::initSimpleModule(SimpleModuleClass *m) const
{
    if(m != NULL) {
        m->setSettings(m_settings);
        m->setNotes(m_notes);
        m->setModuleMap(m_moduleMap);
    }
}

bool ModuleManager::dictionaryLoaded(const Dictionary *dict)
{
    if(dict && m_moduleMap->data.contains(dict->moduleID()) && dict->moduleID() >= 0)
        return true;
    return false;
}
bool ModuleManager::metaModuleLoaded(const SimpleModuleClass *m) const
{
    return (m && m_moduleMap->data.contains(m->moduleID()) && m->moduleID() >= 0);
}
bool ModuleManager::verseTableLoaded(const VerseTable *table) const
{
    return (table && table->verseModule() && m_moduleMap->data.contains(table->verseModule()->moduleID()) && table->verseModule()->moduleID() >= 0);
}
bool ModuleManager::contains(const int moduleID)
{
    return m_moduleMap->data.contains(moduleID);
}

Module * ModuleManager::getModule(const int moduleID)
{
    return m_moduleMap->module(moduleID);
}

/**
  * Converts a persistent url to a link.
  */
QString ModuleManager::notePos2Link(const QString &pos)
{
    VerseUrl url;
    url.fromString(pos);
    UrlConverter urlConverter(UrlConverter::PersistentUrl, UrlConverter::InterfaceUrl, url);
    urlConverter.setSettings(m_settings);
    urlConverter.setModuleMap(m_moduleMap.data());
    VerseUrl newUrl = urlConverter.convert();
    RefText refText;
    refText.setSettings(m_settings);
    refText.setShowModuleName(true);
    const QString link = newUrl.toString();
    return "<a href=\"" + link + "\">" + refText.toString(newUrl) + "</a>";
}
QString ModuleManager::notePos2Text(const QString &pos)
{
    VerseUrl url;
    url.fromString(pos);
    UrlConverter urlConverter(UrlConverter::PersistentUrl, UrlConverter::InterfaceUrl, url);
    urlConverter.setSettings(m_settings);
    urlConverter.setModuleMap(m_moduleMap.data());
    VerseUrl newUrl = urlConverter.convert();
    RefText refText;
    refText.setSettings(m_settings);
    refText.setShowModuleName(true);

    const QString string = refText.toString(newUrl);
    return string;
}

QStringList ModuleManager::getBiblePaths()
{
    QStringList paths;
    QMapIterator<int, Module *> i(m_moduleMap->data);
    while(i.hasNext()) {
        i.next();
        if(i.value()->moduleClass() == OBVCore::BibleModuleClass)
            paths.append(i.value()->path());
    }
    return paths;
}
QList<int> ModuleManager::getBibleIDs()
{
    QList<int> ids;
    QMapIterator<int, Module *> i(m_moduleMap->data);
    while(i.hasNext()) {
        i.next();
        if(i.value()->moduleClass() == OBVCore::BibleModuleClass)
            ids.append(i.value()->moduleID());
    }
    return ids;
}
void ModuleManager::checkCache(const int moduleID)
{
    /*Module *m = m_moduleMap->m_map.value(moduleID);
    if(m->moduleClass() == OBVCore::BibleModuleClass && !m_settings->m_moduleCache.keys().contains(m->path())) {
        Bible *b = new Bible();
        initVerseModule(b);
        b->setModuleType(m->moduleType());
        b->loadModuleData(moduleID);//set cache
    }*/
}
VerseModule * ModuleManager::newVerseModule(const int moduleID, QPoint p, VerseTable *table)
{
    if(!contains(moduleID)) {
        myWarning() << "invalid moduleID = " << moduleID;
        return NULL;
    }

    VerseModule *m = newVerseModule(moduleID);
    table->addModule(m, p);
    return m;
}
VerseModule * ModuleManager::newVerseModule(const int moduleID)
{
    if(!contains(moduleID)) {
        myWarning() << "invalid moduleID = " << moduleID;
        return NULL;
    }
    VerseModule *m = NULL;
    if(getModule(moduleID)->moduleClass() == OBVCore::BibleModuleClass) {
        m = new Bible();
        initVerseModule(m);
    } else {
        myWarning() << "not a verse module " << getModule(moduleID)->moduleClass();
        return NULL;
    }
    OBVCore::ModuleType type = getModule(moduleID)->moduleType();
    m->setModuleType(type);
    m->setModuleID(moduleID);
    return m;
}

OBVCore::ModuleType ModuleManager::recognizeModuleType(const QString &fileName)
{
    //myDebug() << fileName;
    if(fileName.endsWith("bibleqt.ini", Qt::CaseInsensitive)) {
        return OBVCore::BibleQuoteModule;
    } else if(fileName.endsWith(".webdict.xml", Qt::CaseInsensitive)) {
        return OBVCore::WebDictionaryModule;
    } else if(fileName.endsWith(".webpage.xml", Qt::CaseInsensitive)) {
        return OBVCore::WebPageModule;
    } else if(fileName.endsWith(".xml", Qt::CaseInsensitive)) {
        QFile data(fileName);
        if(data.open(QFile::ReadOnly)) {
            QString fileData = "";
            QTextStream in(&data);
            for(int i = 0; i < 100; i++)
                fileData += in.readLine();
            //myDebug() << "fileData = " << fileData;
            if(fileData.contains("XMLBIBLE", Qt::CaseInsensitive) && !(fileData.contains("x-quran", Qt::CaseInsensitive) || // i cannot allow this
                    fileData.contains("x-cult", Qt::CaseInsensitive) ||
                    fileData.contains("x-mormon", Qt::CaseInsensitive))) {
                return OBVCore::ZefaniaBibleModule;
            } else if(fileData.contains("<dictionary", Qt::CaseInsensitive)) {
                return OBVCore::ZefaniaLexModule;
            }

        } else {
            myWarning() << "could not open file " << fileName;
        }
    } else if(fileName.endsWith(".idx", Qt::CaseInsensitive)) {
        return OBVCore::BibleQuoteDictModule;
    } else if(fileName.endsWith(".nt", Qt::CaseInsensitive) || fileName.endsWith(".ot", Qt::CaseInsensitive) || fileName.endsWith(".ont", Qt::CaseInsensitive)) {
        myDebug() << "the word module";
        return OBVCore::TheWordBibleModule;
    }
    return OBVCore::NoneType;
}

OBVCore::DefaultModule ModuleManager::toDefaultModule(const OBVCore::ContentType t)
{
    switch (t) {
        case OBVCore::BibleContent:
        case OBVCore::BibleOTContent:
        case OBVCore::BibleNTContent:
            return OBVCore::DefaultBibleModule;
        case OBVCore::StrongsContent:
        case OBVCore::StrongsGreekContent:
        case OBVCore::StrongsHebrewContent:
            return OBVCore::DefaultStrongDictModule;
        case OBVCore::RMacContent:
            return OBVCore::DefaultRMACDictModule;
        case OBVCore::WordDictionaryContent:
            return OBVCore::DefaultDictModule;
        case OBVCore::GramContent:
            return OBVCore::DefaultGramDictModule;
        default:
            return OBVCore::NotADefaultModule;//todo: could this cause some bugs?
    }
}
/**
 * The t2 is the content we want.
 */
bool ModuleManager::alsoOk(const OBVCore::ContentType t1, const OBVCore::ContentType t2)
{
    if(t1 == t2)
        return true;

    if(t2 == OBVCore::BibleOTContent) {
        if(t1 == OBVCore::BibleContent)
            return true;
    }
    if(t2 == OBVCore::BibleNTContent) {
        if(t1 == OBVCore::BibleContent)
            return true;
    }

    if(t2 == OBVCore::StrongsGreekContent) {
        if(t1 == OBVCore::StrongsContent)
            return true;
    }
    if(t2 == OBVCore::StrongsHebrewContent) {
        if(t1 == OBVCore::StrongsContent)
            return true;
    }
    return false;

}

VerseTable * ModuleManager::newVerseTable()
{
    VerseTable *verseTable = new VerseTable();
    Bible *b = new Bible();

    initVerseModule(b);
    verseTable->addModule(b, QPoint(0, 0));

    return verseTable;
}
