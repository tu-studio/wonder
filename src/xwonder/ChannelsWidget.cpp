/*
 * (c) Copyright 2006-7 -- Hans-Joachim Mond
 * sWONDER:
 * Wave field synthesis Of New Dimensions of Electronic music in Realtime
 * http://swonder.sourceforge.net
 *
 * created at the Technische Universitaet Berlin, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

//----------------------------------includes----------------------------------//

#include "ChannelsWidget.h"

#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>
#include <QVBoxLayout>

#include "AddChannelsDialog.h"
#include "Colors.h"
#include "Source.h"
#include "SourceWidget.h"
#include "XwonderConfig.h"
//----------------------------------------------------------------------------//

ChannelsWidget::ChannelsWidget(QWidget* parent) : QWidget(parent) {
    this->projectOnlyMode = true;

    vLayout = new QVBoxLayout();
    vLayout->setSpacing(5);
    vLayout->setMargin(3);

    vLayout->addStretch();
    setLayout(vLayout);
}

void ChannelsWidget::init() {
    for (int id = 0; id < xwConf->maxNoSources; ++id) {
        SourceWidget* temp = new SourceWidget("", id + 1, id);
        sourceWidgets.insert(id + 1, temp);

        connect(temp, SIGNAL(deactivateMeSignal(unsigned int)), this,
                SLOT(deactivateChannel(unsigned int)));
        connect(temp, SIGNAL(nameChanged(unsigned int, const QString&)), this,
                SIGNAL(nameChanged(unsigned int, const QString&)));
        connect(temp, SIGNAL(nameChanged(unsigned int, const QString&)), this,
                SLOT(modified(unsigned int, const QString&)));
        connect(temp, SIGNAL(colorChanged(unsigned int, const QColor&)), this,
                SIGNAL(colorChanged(unsigned int, const QColor&)));
        connect(temp, SIGNAL(colorChanged(unsigned int, const QColor&)), this,
                SLOT(modified(unsigned int, const QColor&)));
        connect(temp, SIGNAL(visibleChanged(unsigned int)), this,
                SIGNAL(visibleChanged(unsigned int)));
        connect(temp, SIGNAL(dopplerEffectChanged(unsigned int, bool)), this,
                SIGNAL(dopplerEffectChanged(unsigned int, bool)));
        connect(temp, SIGNAL(dopplerEffectChanged(unsigned int, bool)), this,
                SLOT(modified(unsigned int, bool)));
        connect(temp, SIGNAL(rotationDirectionChanged(unsigned int, bool)), this,
                SIGNAL(rotationDirectionChanged(unsigned int, bool)));
        connect(temp, SIGNAL(rotationDirectionChanged(unsigned int, bool)), this,
                SLOT(modified(unsigned int, bool)));
        connect(temp, SIGNAL(scalingDirectionChanged(unsigned int, bool)), this,
                SIGNAL(scalingDirectionChanged(unsigned int, bool)));
        connect(temp, SIGNAL(scalingDirectionChanged(unsigned int, bool)), this,
                SLOT(modified(unsigned int, bool)));
        connect(temp, SIGNAL(typeChanged(unsigned int, bool)), this,
                SIGNAL(typeChanged(unsigned int, bool)));
        connect(temp, SIGNAL(typeChanged(unsigned int, bool)), this,
                SLOT(modified(unsigned int, bool)));
        connect(temp, SIGNAL(sourceIDChanged(unsigned int, int, int)), this,
                SIGNAL(sourceIDChanged(unsigned int, int, int)));
        connect(temp, SIGNAL(sourceIDChanged(unsigned int, int, int)), this,
                SLOT(modified(unsigned int, int, int)));
        connect(temp, SIGNAL(recordModeChanged(unsigned int, bool)), this,
                SIGNAL(sourceRecordModeChanged(unsigned int, bool)));
        connect(temp, SIGNAL(readModeChanged(unsigned int, bool)), this,
                SIGNAL(sourceReadModeChanged(unsigned int, bool)));
        connect(temp, SIGNAL(selected(unsigned int)), this,
                SIGNAL(sourceSelected(unsigned int)));

        // delete the spacer and insert it again later
        vLayout->removeItem(vLayout->itemAt(vLayout->count() - 1));
        vLayout->addWidget(temp);
        vLayout->addStretch();
        temp->hide();
    }
}

void ChannelsWidget::reset() {
    foreach (SourceWidget* temp, sourceWidgets) {
        temp->reset();
        temp->hide();
    }
}

void ChannelsWidget::setMode(bool projectOnlyMode) {
    this->projectOnlyMode = projectOnlyMode;

    foreach (SourceWidget* temp, sourceWidgets) { temp->setMode(projectOnlyMode); }
}

QSize ChannelsWidget::sizeHint() const {
    // temporal dummy-widget, in order to give ChannelsWidget its later size
    SourceWidget temp("1234567890", 0, 0);
    return temp.sizeHint();
}

void ChannelsWidget::setChannel(Source& source) {
    // activate a source parsed from cwonder's xmlDump, set properties accordingly and
    // emit an activate signal to the other widgets
    activateChannel(source.getID(), true);
    changeType(source.getID(), source.isPlanewave());
    changeName(source.getID(), source.getName());
    changeRotationDirection(source.getID(), source.hasInvertedRotation());
    changeScalingDirection(source.getID(), source.hasInvertedScaling());
    changeDopplerEffect(source.getID(), source.hasDopplerEffect());

    const GLfloat* color = source.getColor();
    QColor qColor;
    qColor.setRedF(color[0]);
    qColor.setGreenF(color[1]);
    qColor.setBlueF(color[2]);
    qColor.setAlphaF(color[3]);

    changeColor(source.getID(), qColor);
}

int ChannelsWidget::getNoSources() const {
    int noActiveSources = 0;

    for (int i = 1; i <= sourceWidgets.size(); ++i) {
        if (sourceWidgets.value(i)->isActive()) { noActiveSources++; }
    }

    return noActiveSources;
}

int ChannelsWidget::activateChannel(int sourceID,
                                    bool external) {  // default call with( -1, false )
    if (sourceID < -1 || sourceID >= sourceWidgets.size()) { return -1; }

    unsigned int xID = 0;

    foreach (SourceWidget* temp, sourceWidgets) {
        if (sourceID == -1 && !temp->isActive()) {
            sourceID = 0;

            while (sourceIDIsActive(sourceID)) {
                if (!(sourceID < xwConf->maxNoSources)) {
                    return 0;
                } else {
                    ++sourceID;
                }
            }

            temp->activate();
            temp->setID(sourceID);
            xID = temp->getXID();
            temp->show();
            break;
        } else if (temp->getID() == sourceID) {
            temp->activate();
            sourceID = temp->getID();
            xID      = temp->getXID();
            temp->show();
            break;
        }
    }

    // nothing was activated
    if (sourceID == -1) { return 0; }

    // if this is xwonders request for a new source send appropriate OSC message
    if (!external) { emit sourceActivated(sourceID); }

    // notify other parts in xwonder of new channel with xID, sourceID
    // sources3DWidget will send the new source data via OSC
    emit channelActivated(xID, sourceID, external);

    // if this is xwonders request for a new source set new color and name
    // and  send the appropriate OSC-Messages for this source in cwonder
    if (!external) {
        sourceWidgets.value(xID)->setName("Source #" + QString::number(xID));
        sourceWidgets.value(xID)->setColor(colors[xID % noColors]);

        emit nameChanged(xID, sourceWidgets.value(xID)->getName());
        emit colorChanged(xID, sourceWidgets.value(xID)->getColor());
    }

    emit modified();

    return 0;
}

void ChannelsWidget::activateChannels() {
    int noSourcesLeft = xwConf->maxNoSources - getNoSources();

    if (noSourcesLeft == 0) { return; }

    AddChannelsDialog* addChannelsDialog = new AddChannelsDialog(noSourcesLeft, this);
    addChannelsDialog->move(QCursor::pos());

    int ret = addChannelsDialog->exec();

    if (ret == QDialog::Accepted) {
        // noSources is validated in AddChannelsDialog
        for (int i = 0; i < addChannelsDialog->noSources; ++i) { activateChannel(); }
    }
}

void ChannelsWidget::modified(unsigned int, int, int) { emit modified(); }

void ChannelsWidget::modified(unsigned int, bool) { emit modified(); }

void ChannelsWidget::modified(unsigned int, const QColor&) { emit modified(); }

void ChannelsWidget::modified(unsigned int, const QString&) { emit modified(); }

void ChannelsWidget::deactivateChannel(int sourceID) {
    if (!sourceIDIsActive(sourceID)) {
        return;
    } else {
        SourceWidget* temp = nullptr;
        QMap<unsigned int, SourceWidget*>::iterator iter;

        for (iter = sourceWidgets.begin(); iter != sourceWidgets.end(); ++iter) {
            if ((*iter)->getID() == sourceID) {
                temp = *iter;
                temp->deactivate();
                temp->hide();
                emit channelDeactivated(temp->getXID());
                break;
            }
        }
    }

    emit modified();
}

void ChannelsWidget::deactivateChannel(unsigned int xID) {
    if (xID == 0) { return; }

    if (!(sourceWidgets.value(xID)->isActive())) {
        return;
    } else {
        sourceWidgets.value(xID)->hide();

        emit channelDeactivated(xID);
        emit sourceDeactivated(sourceWidgets.value(xID)->getID());

        sourceWidgets.value(xID)->deactivate();
    }

    emit modified();
}

void ChannelsWidget::deactivateAllChannels() {
    int n = sourceWidgets.size();

    for (int i = 1; i <= n; ++i) { deactivateChannel(i); }

    emit modified();
}

void ChannelsWidget::changeType(int id, bool planewave) {
    QMap<unsigned int, SourceWidget*>::iterator iter = sourceWidgets.begin();

    while (iter != sourceWidgets.end()) {
        if (iter.value()->getID() == id) {
            iter.value()->setType(planewave);
            break;
        }

        ++iter;
    }
}

void ChannelsWidget::changeType(unsigned int xID, bool planewave) {
    if (sourceWidgets.contains(xID)) { sourceWidgets.value(xID)->setType(planewave); }
}

void ChannelsWidget::changeName(int id, QString name) {
    QMap<unsigned int, SourceWidget*>::iterator iter = sourceWidgets.begin();

    while (iter != sourceWidgets.end()) {
        if (iter.value()->getID() == id) {
            iter.value()->setName(name);
            break;
        }

        ++iter;
    }
}

void ChannelsWidget::changeSourceRecordMode(int id, bool recordEnabled,
                                            bool externalSet) {
    foreach (SourceWidget* temp, sourceWidgets) {
        if (temp->getID() == id) {
            temp->changeRecordMode(recordEnabled, externalSet);
            break;
        }
    }
}

void ChannelsWidget::changeSourceReadMode(int id, bool readEnabled, bool externalSet) {
    foreach (SourceWidget* temp, sourceWidgets) {
        if (temp->getID() == id) {
            temp->changeReadMode(readEnabled, externalSet);
            break;
        }
    }
}

void ChannelsWidget::changeSourceRecordMode(unsigned int xID, bool recordEnabled) {
    if (sourceWidgets.contains(xID)) {
        sourceWidgets.value(xID)->changeRecordMode(recordEnabled);
    }
}

void ChannelsWidget::changeSourceReadMode(unsigned int xID, bool readEnabled) {
    if (sourceWidgets.contains(xID)) {
        sourceWidgets.value(xID)->changeReadMode(readEnabled);
    }
}

void ChannelsWidget::changeColor(unsigned int xID, const QColor& newColor) {
    if (sourceWidgets.value(xID)->isActive()) {
        sourceWidgets.value(xID)->setColor(newColor);
    }
}

void ChannelsWidget::changeColor(int id, QColor newColor) {
    foreach (SourceWidget* temp, sourceWidgets) {
        if (temp->getID() == id) {
            temp->setColor(newColor);
            break;
        }
    }
}

void ChannelsWidget::changeRotationDirection(unsigned int xID, bool inverted) {
    if (sourceWidgets.value(xID)->isActive()) {
        sourceWidgets.value(xID)->setRotationDirection(inverted);
    }
}

void ChannelsWidget::changeRotationDirection(int id, bool inverted) {
    foreach (SourceWidget* temp, sourceWidgets) {
        if (temp->getID() == id) {
            temp->setRotationDirection(inverted);
            break;
        }
    }
}

void ChannelsWidget::changeDopplerEffect(unsigned int xID, bool dopplerOn) {
    if (sourceWidgets.value(xID)->isActive()) {
        sourceWidgets.value(xID)->setDopplerEffect(dopplerOn);
    }
}

void ChannelsWidget::changeDopplerEffect(int id, bool dopplerOn) {
    foreach (SourceWidget* temp, sourceWidgets) {
        if (temp->getID() == id) {
            temp->setDopplerEffect(dopplerOn);
            break;
        }
    }
}

void ChannelsWidget::changeScalingDirection(unsigned int xID, bool inverted) {
    if (sourceWidgets.value(xID)->isActive()) {
        sourceWidgets.value(xID)->setScalingDirection(inverted);
    }
}

void ChannelsWidget::changeScalingDirection(int id, bool inverted) {
    foreach (SourceWidget* temp, sourceWidgets) {
        if (temp->getID() == id) {
            temp->setScalingDirection(inverted);
            break;
        }
    }
}

void ChannelsWidget::contextMenuEvent(QContextMenuEvent* event) {
    if (getNoSources() < xwConf->maxNoSources) {
        QMenu menu(this);
        QAction* activateChannelAct  = menu.addAction("Add 1 new Channel");
        QAction* activateChannelsAct = menu.addAction("Add new Channels");
        connect(activateChannelAct, SIGNAL(triggered()), this, SLOT(activateChannel()));
        connect(activateChannelsAct, SIGNAL(triggered()), this, SLOT(activateChannels()));
        menu.exec(event->globalPos());
    }
}

void ChannelsWidget::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_W:
    case Qt::Key_R:
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
    case Qt::Key_G:
    case Qt::Key_H:
    case Qt::Key_X:
    case Qt::Key_Y:
    case Qt::Key_Z:
    case Qt::Key_B:
        emit viewRelatedKeyPressed(event);
        break;
    }
}

void ChannelsWidget::resetIDAfterIDChange(unsigned int xID, int oldID) {
    if (!sourceWidgets.value(xID)->isActive()) {
        return;
    } else {
        sourceWidgets.value(xID)->setID(oldID);
    }
}

void ChannelsWidget::swapAfterIDChange(unsigned int xID, int oldID, int newID) {
    if (!sourceWidgets.value(xID)->isActive()) {
        return;
    } else {
        QMap<unsigned int, SourceWidget*>::iterator it;

        for (it = sourceWidgets.begin(); it != sourceWidgets.end(); ++it)
            if ((it.value()->getID() == newID) && it.key() != xID) {
                it.value()->setID(oldID);
            }
    }
}

int ChannelsWidget::sourceIDIsActive(int id) {
    int timesActive = 0;

    foreach (SourceWidget* temp, sourceWidgets) {
        if (temp->getID() == id && temp->isActive()) { timesActive++; }
    }

    return timesActive;
}

void ChannelsWidget::updateWrapped(int notUsed) { update(); }
