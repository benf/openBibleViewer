#include "swordlexicon.h"

SwordLexicon::SwordLexicon()
{
    m_loaded = false;
    m_library = NULL;
    m_target = NULL;
}
SwordLexicon::~SwordLexicon()
{
    if(m_target != NULL) {
        delete m_target;
        m_target = NULL;
    }
    if( m_library != NULL) {
            delete m_library;
            m_library = NULL;
    }
}
QString SwordLexicon::getEntry(const QString &entry)
{

#ifdef BUILD_WITH_SWORD
    VerseKey mykey = entry.toStdString().c_str();
    m_target->setKey(mykey);
    return QString::fromLocal8Bit(m_target->RenderText());
#else
    return QString();
#endif


}
int SwordLexicon::load(const QString &id)
{
#ifdef BUILD_WITH_SWORD

    if ( id == QString("ZhEnglish")) {
        myWarning() << "Module ZhEnglish is buggy and will not be loaded.";
        return 1;
    }
    if ( id == QString("EReo_en")) {
        myWarning() << "Module EReo_en is buggy and will not be loaded.";
        return 1;
    }

    m_library = new SWMgr(new MarkupFilterMgr(FMT_PLAIN));

    m_target = m_library->getModule(id.toStdString().c_str());


    if(!m_target) {
        myWarning() << "could not load " << id;
        return 1;
    }
    m_loaded = true;
    myDebug() << "return zero";
#endif
    return 0;
}

QStringList SwordLexicon::getAllKeys()
{
    #ifdef BUILD_WITH_SWORD
    if(!m_loaded) {
        load(m_modulePath);
    }
   /* namespace DU = util::directory;

    if (!module()) {
        return 0;
    }

    if (m_entryList) return m_entryList;

    m_entryList = new QStringList();

    //Check for buggy modules! They will not be loaded any more.

    QString dir(DU::getUserCacheDir().absolutePath());
    QFile f1( QString(dir).append("/").append(name()));

    if ( f1.open( QIODevice::ReadOnly ) ) {
        QDataStream s( &f1 );
        QString ModuleVersion, CacheVersion, QDataStreamVersion;
        s >> ModuleVersion;
        s >> CacheVersion;
        s >> QDataStreamVersion;

        qDebug() << "Lexicon cache metadata"
        << "Name" << name()
        << "ModuleVersion" << ModuleVersion
        << "CacheVersion" << CacheVersion
        << "QDataStreamVersion" << QDataStreamVersion;

        // Check if cache is valid
        if (ModuleVersion == config(CSwordModuleInfo::ModuleVersion)
                && CacheVersion == CACHE_FORMAT
                && QDataStreamVersion == QString::number(s.version())) {
            s >> *m_entryList;

            f1.close();
            qDebug() << "Read" << m_entryList->count() << "entries from lexicon cache for module" << name();
            return m_entryList;
        }

        f1.close();
    }


    // Ok, no cache or invalid.

    qDebug() << "Read all entries of lexicon" << name();*/
    m_entryList.clear();

    m_target->setSkipConsecutiveLinks(true);
    (*m_target) = sword::TOP;

    do {
       // if ( isUnicode() ) {
            m_entryList.append(QString::fromUtf8(m_target->KeyText()));
      /*  }
        else {
            //for latin1 modules use fromLatin1 because of speed
            QTextCodec* codec = QTextCodec::codecForName("Windows-1252");
            m_entryList->append(codec->toUnicode(my_module->KeyText()));
        }*/

        (*m_target)++;
    }
    while ( !m_target->Error() );

    (*m_target) = sword::TOP; //back to the first entry

    m_target->setSkipConsecutiveLinks(false);

    if (m_entryList.count()) {
        m_entryList.first().simplified();

        if (m_entryList.first().trimmed().isEmpty()) {
            m_entryList.erase( m_entryList.begin() );
        }
    }


   /* if (m_entryList->count()) {
        //create cache
        QString dir(DU::getUserCacheDir().absolutePath());
        QFile f2( QString(dir).append("/").append(name()) );

        if (f2.open( QIODevice::WriteOnly )) {
            QDataStream s( &f2 );
            s << config(CSwordModuleInfo::ModuleVersion) //store module version
            << QString(CACHE_FORMAT) //store BT version -- format may change
            << QString::number(s.version()) //store QDataStream version -- format may change
            << *m_entryList;
            f2.close();
        }
    }*/

    return m_entryList;
#else
    return QStringList();
#endif
}

void SwordLexicon::search(SearchQuery query, SearchResult *result)
{

}

MetaInfo SwordLexicon::readInfo(const QString &name)
{
    return MetaInfo();
}
