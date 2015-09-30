
#include <stddef.h>

#include "snapshot.h"
#include "snapshotview.h"

#include <QtDebug>

SnapShot::SnapShot(QMainWindow* parent, QString name) :
    QObject(parent),
    _showAction(name, this),
    _deleteAction("Delete", this)
{
    _name = name;

    view = NULL;
    mainWindow = parent;
    connect(&_showAction, &QAction::triggered, this, &SnapShot::show);

    _deleteAction.setToolTip(QString("Delete ") + _name);
    connect(&_deleteAction, &QAction::triggered, this, &SnapShot::onDeleteTriggered);
}

SnapShot::~SnapShot()
{
    if (view != NULL)
    {
        delete view;
    }
}

QAction* SnapShot::showAction()
{
    return &_showAction;
}

QAction* SnapShot::deleteAction()
{
    return &_deleteAction;
}

void SnapShot::show()
{
    if (view == NULL)
    {
        view = new SnapShotView(mainWindow, this);
        connect(view, &SnapShotView::closed, this, &SnapShot::viewClosed);
    }
    view->show();
    view->activateWindow();
    view->raise();
}

void SnapShot::viewClosed()
{
    view->deleteLater();
    view = NULL;
}

void SnapShot::onDeleteTriggered()
{
    emit deleteRequested(this);
}

QString SnapShot::name()
{
    return _name;
}

void SnapShot::setName(QString name)
{
    _name = name;
    _showAction.setText(_name);
    emit nameChanged(this);
}
