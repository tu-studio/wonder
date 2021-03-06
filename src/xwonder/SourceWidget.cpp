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

#include "SourceWidget.h"

#include <QAction>
#include <QColorDialog>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPalette>
#include <QSpacerItem>
#include <QString>
#include <QToolButton>
#include <QVBoxLayout>

#include "Colors.h"
#include "XwonderConfig.h"

int SourceWidget::noActiveSources = 0;

SourceWidget::SourceWidget(QString name, unsigned int xID, int sourceID, QWidget* parent)
    : QGroupBox(parent) {
    active          = false;
    projectOnlyMode = false;

    // some sizeadjustments
    QFont currFont = font();
    currFont.setPointSize(currFont.pointSize() - 1);
    setFont(currFont);
    int margin  = 3;
    int spacing = 5;

    this->name     = name;
    this->xID      = xID;
    this->sourceID = sourceID;

    nameLE = new QLineEdit(name);

    colorButton = new QToolButton();
    colorButton->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    colorButton->setToolTip("Change color");

    // default color is green
    color = QColor((int)(colors[green][0] * 255), (int)(colors[green][1] * 255),
                   (int)(colors[green][2] * 255), (int)(colors[green][3] * 255));
    // unsigned int i = ( xID -1 ) % noColors;

    // color = QColor( ( int ) ( colors[ i ][ 0 ] * 255 ),
    //                ( int ) ( colors[ i ][ 1 ] * 255 ),
    //                ( int ) ( colors[ i ][ 2 ] * 255 ),
    //                ( int ) ( colors[ i ][ 3 ] * 255 ) );

    QPixmap pixmap(24, 24);
    pixmap.fill(color);
    colorButton->setIcon(pixmap);

    showButton = new QCustomCheckBox("visible");
    showButton->setCheckState(Qt::Checked);  // because sources are visible by default
    showButton->setFixedHeight(14);

    moreButton = new QToolButton();
    moreButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    moreButton->setCheckable(true);
    moreButton->setText("+");
    currFont.setPointSize(currFont.pointSize() - 1);
    moreButton->setFont(currFont);
    moreButton->setToolTip("more options");

    recordButton = new QToolButton(this);
    recordButton->setIcon(QIcon(":/record_icon"));
    recordButton->setCheckable(true);
    recordButton->setToolTip("record score");

    readButton = new QToolButton(this);
    readButton->setIcon(QIcon(":/read_icon"));
    readButton->setCheckable(true);
    readButton->setToolTip("read score");
    changeReadMode(true, true);

    // so that buttons have the same size
    colorButton->setMinimumSize(recordButton->sizeHint());

    // extra options-Widget
    extraOptionsWidget = new QWidget();
    extraOptionsLayout = new QVBoxLayout();
    extraOptionsLayout->setMargin(margin);
    extraOptionsLayout->setSpacing(spacing);

    invertRotationButton = new QCustomCheckBox("invert rotation");
    invertScalingButton  = new QCustomCheckBox("invert scaling");
    dopplerButton        = new QCustomCheckBox("Doppler effect");
    dopplerButton->setCheckState(
        Qt::Checked);  // because sources have the doppler effect on by default
    invertRotationButton->setFixedHeight(14);
    invertScalingButton->setFixedHeight(14);
    dopplerButton->setFixedHeight(14);

    // invertRotationButton->setFocusProxy( this );
    // invertScalingButton->setFocusProxy( this );
    // dopplerButton->setFocusProxy( this );
    // showButton->setFocusProxy( this );

    sourcetypeLabel = new QLabel("Type:");
    sourcetypeBox   = new QCustomComboBox();
    sourcetypeBox->addItem("Point");
    sourcetypeBox->addItem("Plane Wave");

    idLabel = new QLabel("ID:");
    idBox   = new QCustomComboBox();

    // IDs start at 0
    for (int i = 0; i < xwConf->maxNoSources; ++i) { idBox->addItem(QString::number(i)); }

    extraOptionsLayout->addWidget(dopplerButton, Qt::AlignLeft);
    extraOptionsLayout->addWidget(invertRotationButton, Qt::AlignLeft);
    extraOptionsLayout->addWidget(invertScalingButton, Qt::AlignLeft);
    extraOptionsLayout->addWidget(showButton, Qt::AlignLeft);
    extraOptionsLayout->addWidget(sourcetypeLabel, Qt::AlignLeft);
    extraOptionsLayout->addWidget(sourcetypeBox, Qt::AlignLeft);
    extraOptionsLayout->addWidget(idLabel, Qt::AlignLeft);
    extraOptionsLayout->addWidget(idBox, Qt::AlignLeft);

    extraOptionsWidget->setLayout(extraOptionsLayout);

    gridLayout = new QGridLayout();
    gridLayout->setMargin(margin);
    gridLayout->setSpacing(spacing);

    gridLayout->addWidget(nameLE, 0, 0, 1, 3);
    gridLayout->addWidget(colorButton, 0, 3, Qt::AlignLeft);
    gridLayout->addWidget(moreButton, 1, 0);
    gridLayout->addItem(new QSpacerItem(10, 0), 1, 1);
    gridLayout->addWidget(readButton, 1, 2, Qt::AlignRight);
    gridLayout->addWidget(recordButton, 1, 3, Qt::AlignLeft);
    gridLayout->addWidget(extraOptionsWidget, 2, 0, 1, 4, Qt::AlignLeft);

    if (projectOnlyMode) {
        recordButton->hide();
        readButton->hide();
    }

    // construct name and set idBox-item and id
    idBox->setCurrentIndex(sourceID);
    setTitle("ID " + idBox->currentText());

    extraOptionsWidget->hide();

    setLayout(gridLayout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // connect signals/slots
    connect(nameLE, SIGNAL(editingFinished()), this, SLOT(changeName()));
    connect(nameLE, SIGNAL(returnPressed()), this, SLOT(setFocus()));
    connect(colorButton, SIGNAL(clicked()), this, SLOT(changeColor()));
    connect(showButton, SIGNAL(stateChanged(int)), this, SLOT(changeVisible(int)));
    connect(moreButton, SIGNAL(toggled(bool)), extraOptionsWidget,
            SLOT(setVisible(bool)));
    connect(dopplerButton, SIGNAL(clicked(bool)), this, SLOT(changeDopplerEffect(bool)));
    connect(invertRotationButton, SIGNAL(clicked(bool)), this,
            SLOT(changeRotationDirection(bool)));
    connect(invertScalingButton, SIGNAL(clicked(bool)), this,
            SLOT(changeScalingDirection(bool)));
    connect(idBox, SIGNAL(activated(int)), this, SLOT(changeID(int)));
    connect(sourcetypeBox, SIGNAL(activated(const QString&)), this,
            SLOT(changeType(const QString&)));
    connect(recordButton, SIGNAL(clicked(bool)), this, SLOT(changeRecordMode(bool)));
    connect(readButton, SIGNAL(clicked(bool)), this, SLOT(changeReadMode(bool)));
}

SourceWidget::~SourceWidget() {}

void SourceWidget::reset() {
    noActiveSources = 0;
    active          = false;
    projectOnlyMode = false;

    sourceID = xID - 1;

    name = "";
    nameLE->setText(name);

    color = QColor((int)(colors[green][0] * 255), (int)(colors[green][1] * 255),
                   (int)(colors[green][2] * 255), (int)(colors[green][3] * 255));

    QPixmap pixmap(24, 24);
    pixmap.fill(color);
    colorButton->setIcon(pixmap);

    showButton->setChecked(true);

    moreButton->setChecked(false);

    changeRecordMode(false, true);
    changeReadMode(true, true);

    dopplerButton->setChecked(true);

    invertRotationButton->setChecked(false);
    invertScalingButton->setChecked(false);

    // construct name and set idBox-item and id
    idBox->setCurrentIndex(sourceID);
    setTitle("ID " + idBox->currentText());
    sourcetypeBox->setCurrentIndex(0);

    extraOptionsWidget->hide();
}

bool SourceWidget::isActive() const { return active; }

void SourceWidget::activate() {
    noActiveSources++;
    active = true;
}

void SourceWidget::deactivate() {
    noActiveSources--;
    reset();
}

void SourceWidget::changeName() {
    name = nameLE->text();
    emit nameChanged(xID, name);
}

void SourceWidget::changeID(int sourceID) {
    int oldID      = this->sourceID;
    this->sourceID = sourceID;
    setTitle("ID " + idBox->currentText());
    emit sourceIDChanged(xID, oldID, this->sourceID);
}

void SourceWidget::changeColor() {
    color = QColorDialog::getColor();

    // prevent pitch black, that is the background color, instead use red
    if (color == Qt::black) { color.setRed(255); }

    if (color.isValid()) {
        QPixmap pixmap(24, 24);
        pixmap.fill(color);
        colorButton->setIcon(pixmap);
        emit colorChanged(xID, color);
    }
}

void SourceWidget::setColor(const QColor& newColor) {
    if (color.isValid()) {
        color = newColor;
        QPixmap pixmap(24, 24);
        pixmap.fill(color);
        colorButton->setIcon(pixmap);
    }
}

void SourceWidget::changeVisible(const int argNotUsed) { emit visibleChanged(xID); }

void SourceWidget::changeRotationDirection(bool inverted) {
    emit rotationDirectionChanged(xID, inverted);
}

void SourceWidget::changeScalingDirection(bool inverted) {
    emit scalingDirectionChanged(xID, inverted);
}

void SourceWidget::changeDopplerEffect(bool dopplerOn) {
    emit dopplerEffectChanged(xID, dopplerOn);
}

void SourceWidget::changeType(const QString& type) {
    bool planewave = false;

    if (type == "Plane Wave") { planewave = true; }

    emit typeChanged(xID, planewave);
}

void SourceWidget::changeRecordMode(bool recEnabled, bool externalSet) {
    // change color of recordbutton
    static QPalette noRecPalette = recordButton->palette();

    // check if recordmode was set externally, if so, do not send OSC-Message
    if (recEnabled) {
        recordButton->setChecked(true);
        recordButton->setPalette(QPalette(Qt::red));

        if (!externalSet) { emit recordModeChanged(xID, true); }
    } else {
        recordButton->setChecked(false);
        recordButton->setPalette(noRecPalette);

        if (!externalSet) { emit recordModeChanged(xID, false); }
    }
}

void SourceWidget::changeReadMode(bool readEnabled, bool externalSet) {
    // change color of readbutton
    static QPalette noReadPalette = readButton->palette();

    // check if readmode was set externally, if so, do not send OSC-Message
    if (readEnabled) {
        if (externalSet || !readButton->isChecked()) {
            readButton->setChecked(true);
        } else {
            emit readModeChanged(xID, true);
        }

        readButton->setPalette(QPalette(Qt::green));
    } else {
        if (externalSet || readButton->isChecked()) {
            readButton->setChecked(false);
        } else {
            emit readModeChanged(xID, false);
        }

        readButton->setPalette(noReadPalette);
    }
}

void SourceWidget::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);

    if (noActiveSources < xwConf->maxNoSources) {
        QAction* activateChannelAct  = menu.addAction("Add 1 new Channel");
        QAction* activateChannelsAct = menu.addAction("Add new Channels");
        connect(activateChannelAct, SIGNAL(triggered()), parentWidget(),
                SLOT(activateChannel()));
        connect(activateChannelsAct, SIGNAL(triggered()), parentWidget(),
                SLOT(activateChannels()));
    }

    QAction* deactivateMeAct = menu.addAction("Delete this channel");
    connect(deactivateMeAct, SIGNAL(triggered()), this, SLOT(deactivateMe()));

    menu.exec(event->globalPos());
}

void SourceWidget::mousePressEvent(QMouseEvent* argNotUsed) { emit selected(xID); }

void SourceWidget::keyPressEvent(QKeyEvent* event) {
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
        QApplication::sendEvent(parent(), event);
        break;
    }
}

void SourceWidget::deactivateMe() { emit deactivateMeSignal(xID); }

int SourceWidget::getID() const { return sourceID; }

unsigned int SourceWidget::getXID() const { return xID; }

QColor SourceWidget::getColor() const { return color; }

bool SourceWidget::isPlanewave() const {
    return (sourcetypeBox->currentText() == "Plane Wave");
}

const QString& SourceWidget::getName() const { return name; }

void SourceWidget::setMode(bool projectOnlyMode) {
    this->projectOnlyMode = projectOnlyMode;

    if (projectOnlyMode) {
        recordButton->setVisible(false);
        readButton->setVisible(false);
    } else {
        recordButton->setVisible(true);
        readButton->setVisible(true);
    }
}

void SourceWidget::setXID(unsigned int xID) { this->xID = xID; }

void SourceWidget::setID(int sourceID) {
    this->sourceID = sourceID;
    idBox->setCurrentIndex(sourceID);
    setTitle("ID " + idBox->currentText());
}

void SourceWidget::setType(bool planewave) {
    sourcetypeBox->setCurrentIndex((int)planewave);
}

void SourceWidget::setName(QString name) {
    nameLE->setText(name);
    this->name = name;
}

void SourceWidget::setColor(const GLfloat newColor[4]) {
    color = QColor((int)(newColor[0] * 255), (int)(newColor[1] * 255),
                   (int)(newColor[2] * 255), (int)(newColor[3] * 255));

    QPixmap pixmap(24, 24);
    pixmap.fill(color);
    colorButton->setIcon(pixmap);
}

void SourceWidget::setDopplerEffect(bool dopplerOn) {
    dopplerButton->setChecked(dopplerOn);
}

void SourceWidget::setRotationDirection(bool inverted) {
    invertRotationButton->setChecked(inverted);
}

void SourceWidget::setScalingDirection(bool inverted) {
    invertScalingButton->setChecked(inverted);
}
