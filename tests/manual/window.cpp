/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/
#include "window.h"

#include <QTimeLine>
#include <QtGui>

#include <math.h>

//! [0]
Window::Window()
{
    iconAnimationTimeLine = new QTimeLine(1000, this);
    iconAnimationTimeLine->setLoopCount(0);
    iconAnimationTimeLine->setUpdateInterval(200);
    iconAnimationTimeLine->setCurveShape(QTimeLine::LinearCurve);
    connect(iconAnimationTimeLine, SIGNAL(valueChanged(qreal)), SLOT(updateAnimatedIcon()));

    createIconGroupBox();
    createMessageGroupBox();

    iconLabel->setMinimumWidth(durationLabel->sizeHint().width());

    createActions();
    createTrayIcon();

    connect(showMessageButton, SIGNAL(clicked()), this, SLOT(showMessage()));
    connect(showIconCheckBox, SIGNAL(toggled(bool)),
            trayIcon, SLOT(setVisible(bool)));
    connect(iconComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setIconFromComboBox(int)));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(iconGroupBox);
    mainLayout->addWidget(messageGroupBox);
    setLayout(mainLayout);

    iconComboBox->setCurrentIndex(1);
    trayIcon->show();

    setWindowTitle(tr("Systray"));
    resize(400, 300);
}
//! [0]

//! [1]
void Window::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}
//! [1]

//! [3]
void Window::setIconFromComboBox(int index)
{
    QIcon icon = iconComboBox->itemIcon(index);
    if (icon.isNull()) {
        iconAnimationTimeLine->start();
    } else {
        iconAnimationTimeLine->stop();
        setIcon(icon);
    }

    trayIcon->setToolTip(iconComboBox->itemText(index));
}

void Window::setIcon(const QIcon& icon)
{
    trayIcon->setIcon(icon);
}

void Window::updateAnimatedIcon()
{
    QPixmap pixmap(22, 22);
    pixmap.fill(Qt::transparent);
    {
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::white);
        qreal radius = pixmap.width() / 4. - 1;
        qreal angle = iconAnimationTimeLine->currentValue() * 2 * M_PI;
        qreal cx = pixmap.width() / 2 + radius * cos(angle);
        qreal cy = pixmap.height() / 2 + radius * sin(angle);
        painter.drawEllipse(QPointF(cx, cy), radius, radius);
    }
    setIcon(pixmap);
}

//! [4]
void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1)
                                      % iconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        QMessageBox::information(this, QString(), tr("Middle clicked"));
        break;
    default:
        ;
    }
}
//! [4]

//! [5]
void Window::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
            typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
    trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon,
                          durationSpinBox->value() * 1000);
}
//! [5]

void Window::createIconGroupBox()
{
    iconGroupBox = new QGroupBox(tr("Tray Icon"));

    iconLabel = new QLabel("Icon:");

    iconComboBox = new QComboBox;
    iconComboBox->addItem(QIcon(":/images/bad.svg"), tr("Bad"));
    iconComboBox->addItem(QIcon(":/images/heart.svg"), tr("Heart"));
    iconComboBox->addItem(QIcon(":/images/trash.svg"), tr("Trash"));
    iconComboBox->addItem(QIcon::fromTheme("system-file-manager"), tr("File Manager"));
    iconComboBox->addItem(tr("Generated"));

    showIconCheckBox = new QCheckBox(tr("Show icon"));
    showIconCheckBox->setChecked(true);

#if defined(Q_WS_X11)
    jitToolTipCheckBox = new QCheckBox(tr("Just In Time Tooltip"));
#endif

    QHBoxLayout *iconLayout = new QHBoxLayout;
    iconLayout->addWidget(iconLabel);
    iconLayout->addWidget(iconComboBox);
    iconLayout->addStretch();
    iconLayout->addWidget(showIconCheckBox);
#if defined(Q_WS_X11)
    iconLayout->addWidget(jitToolTipCheckBox);
#endif
    iconGroupBox->setLayout(iconLayout);
}

void Window::createMessageGroupBox()
{
    messageGroupBox = new QGroupBox(tr("Balloon Message"));

    typeLabel = new QLabel(tr("Type:"));

    typeComboBox = new QComboBox;
    typeComboBox->addItem(tr("None"), QSystemTrayIcon::NoIcon);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxInformation), tr("Information"),
            QSystemTrayIcon::Information);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxWarning), tr("Warning"),
            QSystemTrayIcon::Warning);
    typeComboBox->addItem(style()->standardIcon(
            QStyle::SP_MessageBoxCritical), tr("Critical"),
            QSystemTrayIcon::Critical);
    typeComboBox->setCurrentIndex(1);

    durationLabel = new QLabel(tr("Duration:"));

    durationSpinBox = new QSpinBox;
    durationSpinBox->setRange(5, 60);
    durationSpinBox->setSuffix(" s");
    durationSpinBox->setValue(15);

    durationWarningLabel = new QLabel(tr("(some systems might ignore this "
                                         "hint)"));
    durationWarningLabel->setIndent(10);

    titleLabel = new QLabel(tr("Title:"));

    titleEdit = new QLineEdit(tr("Incoming message"));

    bodyLabel = new QLabel(tr("Body:"));

    bodyEdit = new QTextEdit;
    bodyEdit->setPlainText(tr("John Doe is connected"));

    showMessageButton = new QPushButton(tr("Show Message"));
    showMessageButton->setDefault(true);

    QGridLayout *messageLayout = new QGridLayout;
    messageLayout->addWidget(typeLabel, 0, 0);
    messageLayout->addWidget(typeComboBox, 0, 1, 1, 2);
    messageLayout->addWidget(durationLabel, 1, 0);
    messageLayout->addWidget(durationSpinBox, 1, 1);
    messageLayout->addWidget(durationWarningLabel, 1, 2, 1, 3);
    messageLayout->addWidget(titleLabel, 2, 0);
    messageLayout->addWidget(titleEdit, 2, 1, 1, 4);
    messageLayout->addWidget(bodyLabel, 3, 0);
    messageLayout->addWidget(bodyEdit, 3, 1, 2, 4);
    messageLayout->addWidget(showMessageButton, 5, 4);
    messageLayout->setColumnStretch(3, 1);
    messageLayout->setRowStretch(4, 1);
    messageGroupBox->setLayout(messageLayout);
}

void Window::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    quitAction->setIcon(QIcon::fromTheme("application-exit"));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Window::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    QByteArray category = qgetenv("SNI_CATEGORY");
    if (!category.isEmpty()) {
        trayIcon->setProperty("_sni_qt_category", QString::fromLocal8Bit(category));
    }
    trayIcon->setContextMenu(trayIconMenu);

#if defined(Q_WS_X11)
    trayIcon->installEventFilter(this);
#endif
}

#if defined(Q_WS_X11)
bool Window::eventFilter(QObject *, QEvent *event)
{
    switch(event->type()) {
    case QEvent::ToolTip:
        if (jitToolTipCheckBox->isChecked()) {
            QString timeString = QTime::currentTime().toString();
            trayIcon->setToolTip(tr("Current Time: %1").arg(timeString));
        }
        break;
    case QEvent::Wheel: {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int delta = wheelEvent->delta() > 0 ? 1 : -1;
        int index = qBound(0, iconComboBox->currentIndex() + delta, iconComboBox->count() - 1);
        iconComboBox->setCurrentIndex(index);
        break;
    }
    default:
        break;
    }
    return false;
}
#endif

#include <window.moc>
