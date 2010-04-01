#ifndef SIMPLENOTES_H
#define SIMPLENOTES_H

#include <QObject>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeView>
#include <QtGui/QSortFilterProxyModel>
#include <QItemSelectionModel>
#include "src/core/settings.h"
#include "src/core/notes.h"
#include "src/module/modulemanager.h"
#include "src/core/verseselection.h"
class SimpleNotes : public QObject
{
Q_OBJECT

public:
    explicit SimpleNotes();
    void setTitleWidget(QLineEdit *title);
    void setDataWidget(QTextEdit *data);
    void setViewWidget(QTreeView *treeView);
    void setSettings(Settings *settings);
    void setNotes(Notes *notes);
    void setModuleManager(ModuleManager *moduleManager);
    void init();

signals:

public slots:
     void newNote();
     void showNote(const QString &noteID);
     void saveNote();
private slots:
    void fastSave();
    void copyNote();
    void changeData(QString id, QString data);
    void changeTitle(QString id, QString title);
    void changeRef(QString id, QMap<QString, QString> ref);
    void showNote(QModelIndex index);
    void notesContextMenu();
    void removeNote();
private:
    QLineEdit *m_lineEdit_title;
    QTextEdit *m_textEdit_note;

    Settings *m_settings;
    Notes *m_notes;
    ModuleManager *m_moduleManager;

    QStandardItemModel *m_itemModel;
    QTreeView *m_treeView;
    QSortFilterProxyModel *m_proxyModel;
    QItemSelectionModel *m_selectionModel;

    QString m_noteID;
    QMap<QString, QString> currentNoteRef;
    void setTitle(QString title);
    void setData(QString data);
    void setRef(QMap<QString, QString> ref);
    void aktNote();
    void select(QString noteID);

};

#endif // SIMPLENOTES_H