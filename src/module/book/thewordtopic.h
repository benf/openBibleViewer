#ifndef THEWORDTOPIC_H
#define THEWORDTOPIC_H
#include "treebookmodule.h"
#include <QtSql/QSqlDatabase>
class TheWordTopic : public TreeBookModule
{
public:
    TheWordTopic();

    MetaInfo readInfo(const QString &name);

    void search(const SearchQuery &query, SearchResult *res) const;
    bool hasIndex() const;
    void buildIndex();

    int moduleID() const;
    QString modulePath() const;
    QString moduleName(bool preferShortName = false) const;

    int loadModuleData(const int moduleID, const QString &path);


    Response* readChapter(const int chapterID);
    BookTree * bookTree();
private:
    QSqlDatabase m_db;
    QString toValidRTF(QString data);

    bool m_loaded;
};

#endif // THEWORDTOPIC_H
