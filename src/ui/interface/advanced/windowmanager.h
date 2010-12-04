#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QtGui/QMdiArea>
#include "src/core/basicclass.h"
#include "src/core/windowcache.h"
#include "src/api/api.h"
#include "bibleform.h"
#include <QWebView>
class WindowManager : public QObject , public BasicClass
{
    Q_OBJECT
public:
    explicit WindowManager(QObject *parent = 0);
    void setMdiArea(QMdiArea *area);
    void setApi(Api *api);
    void init();
    BibleForm *activeForm();
    QMdiSubWindow *activeMdiChild();

signals:

public slots:
    void myCascade();
    void myTileVertical();
    void myTileHorizontal();
    void myTile();

    void newSubWindow(bool doAutoLayout = true);
    void closeSubWindow();
    int closingWindow();
    int reloadWindow(QMdiSubWindow * window);
    void mdiAreaResized();
    void zoomIn();
    void zoomOut();
    void reloadActive();


private:
    QMdiArea *m_area;//not in our control
    Api *m_api;//not in out control

    bool m_enableReload;
    void autoLayout();
    QList<QMdiSubWindow*> usableWindowList();
    int currentWindowName();
    int m_lastActiveWindow;
    void setEnableReload(bool enable);

};

#endif // WINDOWMANAGER_H
