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

#ifndef SOURCEWIDGET_H
#define SOURCEWIDGET_H

#include <GL/glu.h>

#ifdef __gl_glext_h_
#ifndef __glext_h_
#define __glext_h_ 1
#endif
#endif

#include <QApplication>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QWheelEvent>
#include <QColor>


class QLineEdit;
class QToolButton;
class QLabel;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QString;
class QContextMenuEvent;

class QCustomComboBox;
class QCustomCheckBox;


//Doxygen description
/*!
 *      \brief
 *      Visualise and edit sourceproperties (not position)
 *
 *      \details
 *      The name, color, id-number, recordmode (on/off), readmode (on/off) and sourcetype of a single source are shown
 *      and can be edited. According to the current working mode (project with/without score) properties
 *      will vary.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      20.07.2008
 */

class SourceWidget : public QGroupBox {
    Q_OBJECT

public:
    //According to Scene- or Scoremode this Widget offers different options
    SourceWidget(QString name, unsigned int xID, int id, QWidget* parent = 0);
    ~SourceWidget();

    void reset();

    bool           isActive()    const;
    unsigned int   getXID()      const;
    int            getID()       const;
    QColor         getColor()    const;
    bool           isPlanewave() const;
    const QString& getName()     const;

    void setMode(bool projectOnlyMode);
    void setXID(unsigned int xID);
    void setID(int sourceID);
    void setType(bool planewave);
    void setName(QString name);
    void setColor(const GLfloat* const newColor);
    void setColor(const QColor& newColor);
    void setRotationDirection(bool inverted);
    void setScalingDirection(bool inverted);
    void setDopplerEffect(bool dopplerOn);

public slots:
    void activate();
    void deactivate();
    void changeRecordMode(bool recEnabled, bool externalSet = false);
    void changeReadMode(bool readEnabled, bool externalSet = false);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void keyPressEvent(QKeyEvent*         event);
    void mousePressEvent(QMouseEvent*       event);

signals:
    void deactivateMeSignal(unsigned int xID);
    void nameChanged(unsigned int xID, const QString& name);
    void sourceIDChanged(unsigned int xID, int oldId, int id);
    void colorChanged(unsigned int xID, const QColor& newcolor);
    void visibleChanged(unsigned int xID);
    void typeChanged(unsigned int xID, bool planewave);
    void recordModeChanged(unsigned int xID, bool recEnabled);
    void readModeChanged(unsigned int xID, bool readEnabled);
    void selected(unsigned int xID);
    void rotationDirectionChanged(unsigned int xID, bool inverted);
    void scalingDirectionChanged(unsigned int xID, bool inverted);
    void dopplerEffectChanged(unsigned int xID, bool dopplerOn);

    //signals are caught by each individual SourceWidget, tagged with an xID and forwarded to the Channelwidget
private slots:
    void changeName();
    void changeID(int id);
    void changeColor();
    void deactivateMe();
    void changeVisible(const int argNotUsed);
    void changeRotationDirection(bool inverted);
    void changeScalingDirection(bool inverted);
    void changeDopplerEffect(bool dopplerOn);
    void changeType(const QString& type);

private:
    //this is the id which is used in OSC-Messages
    int sourceID;

    bool active;
    bool projectOnlyMode;

    //this is the xwonder-id used, this one is fixed for every SourceWidget
    unsigned int xID;

    QString name;

    QLineEdit*   nameLE;
    QToolButton* colorButton;
    QToolButton* moreButton;
    QToolButton* recordButton;
    QToolButton* readButton;

    QWidget*         extraOptionsWidget;
    QVBoxLayout*     extraOptionsLayout;
    QCustomCheckBox* dopplerButton;
    QCustomCheckBox* invertRotationButton;
    QCustomCheckBox* invertScalingButton;
    QCustomCheckBox* showButton;
    QWidget*         sourcetypeWidget;
    QHBoxLayout*     sourcetypeLayout;
    QLabel*          sourcetypeLabel;
    QCustomComboBox* sourcetypeBox;
    QWidget*         idWidget;
    QHBoxLayout*     idLayout;
    QLabel*          idLabel;
    QCustomComboBox* idBox;

    QGridLayout* gridLayout;
    QColor color;

    static int noActiveSources;
};

// a custom combobox which does not accept wheel events to prevent accidental switching
class QCustomComboBox : public QComboBox {
    Q_OBJECT

protected:
    void wheelEvent(QWheelEvent* event) {
        event->ignore();
    };

};

// a custom checkbox which does not accept arrow keys as input to prevent accidental focusshift
// to other widgets and to keep arrow keys available for movement of sources
class QCustomCheckBox: public QCheckBox {
    Q_OBJECT

public:
    QCustomCheckBox(const QString& text, QWidget* parent = 0) : QCheckBox(text, parent) {
    };

protected:
    void keyPressEvent(QKeyEvent* event) {
        switch(event->key()) {
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
                QApplication::sendEvent(parent(), event);
                break;

            default:
                QCheckBox::keyPressEvent(event);
        }
    };
};
#endif // SOURCEWIDGET_H
