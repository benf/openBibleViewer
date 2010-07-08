#include "biblelistitemdelegate.h"
#include <QtGui/QComboBox>
#include "src/core/dbghelper.h"
#include <QtCore/QTimer>
BibleListItemDelegate::BibleListItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    m_parent = parent;
}
void BibleListItemDelegate::init()
{
    m_names << tr("None") << m_moduleManager->getBibleTitles();
    m_id << -1 << m_moduleManager->getBibleIDs();
}

QWidget * BibleListItemDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem & option ,
     const QModelIndex & index ) const
{
    QComboBox *combo = new QComboBox( parent );
    combo->insertItems(0,m_names);
    if(index.data(Qt::UserRole+3).toInt() > 0) {
        combo->setCurrentIndex(index.data(Qt::UserRole+3).toInt());
    } else {
        combo->setCurrentIndex(0);
    }

    return combo;
}

void BibleListItemDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    DEBUG_FUNC_NAME
    QComboBox *combo = qobject_cast<QComboBox *>( editor );
    if(!combo){
        QItemDelegate::setEditorData( editor, index );
        return;
    }
    myDebug() << "set index = "<< index.model()->data(index,Qt::UserRole+3).toInt();
    combo->setCurrentIndex(index.model()->data(index,Qt::UserRole+3).toInt());
}
void BibleListItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    DEBUG_FUNC_NAME
    QComboBox *combo = qobject_cast<QComboBox *>( editor );
     if ( !combo ) {
       QItemDelegate::setModelData( editor, model, index );
       return;
     }
     myDebug() << "current Text = " << combo->currentText() << " index = " << combo->currentIndex() << " id = " << m_id.at(combo->currentIndex());
     model->setData( index, combo->currentIndex(),Qt::UserRole+3 );
     if(combo->currentIndex() > 0)
         model->setData( index, combo->currentText() );
     else
         model->setData( index, "" );

     if(combo->currentIndex() > 0)
         model->setData(index,m_id.at(combo->currentIndex()),Qt::UserRole+2);
     else
         model->setData(index,-1,Qt::UserRole+2);
 }
void BibleListItemDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
