/*
 * Copyright (C) 2010 Intel Corporation
 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 * Authors:
 *    Chen, Guobing <guobing.chen@intel.com>
 */

#include "imageWidget.h"
#include <QtGui>

ImageWidget::ImageWidget(QWidget *parent, bool enableAnimation): QWidget(parent), enAnimation(enableAnimation), image(), simPressPos(), simDragPos(), simSpeed() 
{
    horizontalOffset = 0.0;
    verticalOffset = 0.0;
    dampCoef = 0.96;
    rotationAngle = 0.0;
    scaleFactor = 1;
    currentScaleFactor = 1;
    simState = PAN_READY;
    QRect screenRect = QApplication::desktop()->screenGeometry();
    screenWidth = screenRect.width();
    screenHeight = screenRect.height();

    setFocusPolicy(Qt::StrongFocus);

    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);

    connect(this, SIGNAL(closeWidget()), qApp, SLOT(quit()));
}

bool ImageWidget::setImage(const QString &fileName)
{
    image.load(fileName);
    return true;
}

bool ImageWidget::event(QEvent *event)
{
    if (QEvent::Gesture == event->type()) {
        return gestureEvent(static_cast<QGestureEvent*>(event));
    } else if (QEvent::TouchBegin == event->type()) {
        event->accept();
        return true;
    } else if (QEvent::MouseButtonPress == event->type() || QEvent::MouseButtonRelease == event->type() || QEvent::MouseMove == event->type()) {
        return simGestureStateMachine(dynamic_cast<QMouseEvent*>(event));
    } else {
        return QWidget::event(event);
    }
}

void ImageWidget::keyPressEvent(QKeyEvent * keyEvent)
{
    int key = keyEvent->key();
    if (Qt::Key_Escape == key) {
        this->close();
        emit this->closeWidget();
    } else {
        QWidget::keyPressEvent(keyEvent);
    }
}

bool ImageWidget::simGestureStateMachine(QMouseEvent *mouseEvent)
{
    if (!mouseEvent || mouseEvent->modifiers() != Qt::NoModifier)
        return false;

    bool consumed = false;
    switch (simState) {
    case PAN_READY:
        if (mouseEvent->type() == QEvent::MouseButtonPress) {
            if (mouseEvent->buttons() == Qt::LeftButton) {
                consumed = true;
                simState = PAN_PRESS;
                simPressPos = mouseEvent->pos();
            }
        }
        break;

    case PAN_PRESS:
        if (mouseEvent->type() == QEvent::MouseButtonRelease) {
            consumed = true;
            simState = PAN_READY;
        }
        if (mouseEvent->type() == QEvent::MouseMove) {
            consumed = true;
            simState = PAN_MOVE;
            simDragPos = QCursor::pos();
            if (!timer.isActive() && enAnimation) {
                timer.start(20, this);
            }
        }
        break;
    case PAN_MOVE:
        if (mouseEvent->type() == QEvent::MouseMove) {
            consumed = true;
            QPoint delta = mouseEvent->pos() - simPressPos;
            simPressPos = mouseEvent->pos();
            horizontalOffset += delta.x();
            verticalOffset += delta.y();
            update(); 
        }
        if (mouseEvent->type() == QEvent::MouseButtonRelease) {
            consumed = true;
            simState = PAN_AUTOMOVE;
        }
        break;

    case PAN_AUTOMOVE:
        if (mouseEvent->type() == QEvent::MouseButtonPress) {
            consumed = true;
            simState = PAN_STOP;
            simPressPos = mouseEvent->pos();
            simSpeed = QPointF(0, 0);
        }
        if (mouseEvent->type() == QEvent::MouseButtonRelease) {
            consumed = true;
            simState = PAN_READY;
            simSpeed = QPointF(0, 0);
        }
        break;

    case PAN_STOP:
        if (mouseEvent->type() == QEvent::MouseButtonRelease) {
            consumed = true;
            simState = PAN_READY;
        }
        if (mouseEvent->type() == QEvent::MouseMove) {
            consumed = true;
            simState = PAN_MOVE;
            simDragPos = QCursor::pos();
            if (!timer.isActive() && enAnimation) {
                timer.start(20, this);
            }
        }
        break;

    default:
        break;
    }

    return consumed;
}

void ImageWidget::timerEvent(QTimerEvent *event)
{
    if (simState == PAN_MOVE) {
        QPoint point = QCursor::pos();
        simSpeed.setX((point.x() - simDragPos.x())*1.5);
        simSpeed.setY((point.y() - simDragPos.y())*1.5);
        simDragPos = point;
        
    } else if (simState == PAN_AUTOMOVE) {
        slowDown(simSpeed, dampCoef);
        horizontalOffset += simSpeed.x();
        verticalOffset += simSpeed.y();
        update();

        if (simSpeed == QPointF(0.0, 0.0)) {
            simState = PAN_READY;
        }
    } else {
        timer.stop();
    }
    
    boundLimit();

    QObject::timerEvent(event);
}

void ImageWidget::slowDown(QPointF &speed, float a)
{
    float x = speed.x();
    float y = speed.y();
    if (x < 0.5 && x > -0.5) {
        x = 0.0;
    } else {
        x = (x > 0.0) ? qMax(float(0.0), x*a) : qMin(float(0.0), x*a);
    }

    if (y < 0.5 && y > -0.5) {
        y = 0.0;
    } else {
        y = (y > 0.0) ? qMax(float(0.0), y*a) : qMin(float(0.0), y*a); 
    }

    speed.setX(x);
    speed.setY(y);
}

void ImageWidget::boundLimit()
{
    if (horizontalOffset > screenWidth || horizontalOffset < -screenWidth
                                       || verticalOffset > screenHeight
                                       || verticalOffset < -screenHeight ) {
        float x = simSpeed.x();
        float y = simSpeed.y();
        float xBoundSpeed = (qAbs(horizontalOffset) - screenWidth)/8;
        float yBoundSpeed = (qAbs(verticalOffset)-screenHeight)/8;

        if ((horizontalOffset>screenWidth && x>0.0) || (horizontalOffset<-screenWidth && x<0.0)) {
            x = -x*0.8;
        }
        if ((verticalOffset>screenHeight && y>0.0) || (verticalOffset < -screenHeight && y<0.0)) {
            y = -y*0.8;
        }
        
        if (simState == PAN_MOVE) {
            x = horizontalOffset > screenWidth ? qMin(-xBoundSpeed, x) : qMax(xBoundSpeed, x);
            y = verticalOffset > screenHeight ? qMin(-yBoundSpeed, y) : qMax(yBoundSpeed, y);
        }
        simSpeed.setX(x);
        simSpeed.setY(y);
    }    
}

void ImageWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    float iw = image.width();
    float ih = image.height();
    float wh = height();
    float ww = width();

    p.translate(ww/2, wh/2);
    p.translate(horizontalOffset, verticalOffset);
    p.rotate(rotationAngle);
    p.scale(currentScaleFactor * scaleFactor, currentScaleFactor * scaleFactor);
    p.translate(-iw/2, -ih/2);
    p.drawPixmap(0, 0, image);
}

bool ImageWidget::gestureEvent(QGestureEvent *event)
{
    qDebug() << "[Info]: Gesture Event";
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinching(static_cast<QPinchGesture *>(pinch));
    else if (QGesture *pan = event->gesture(Qt::PanGesture))
        panning(static_cast<QPanGesture *>(pan));
    return true;
}

void ImageWidget::panning(QPanGesture *gesture)
{
    qDebug() << "[Info]: pan Gesture";
    QPointF delta(gesture->delta());
    horizontalOffset += delta.x();
    verticalOffset += delta.y();
    update();
}

void ImageWidget::pinching(QPinchGesture *gesture)
{
    qDebug() << "[Info]: pinch Gesture";
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::RotationAngleChanged) {
        rotationAngle += gesture->property("rotationAngle").toReal() - gesture->property("lastRotationAngle").toReal();
    }
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        currentScaleFactor = gesture->property("scaleFactor").toReal();
    }
    if (gesture->state() == Qt::GestureFinished) {
        scaleFactor *= currentScaleFactor;
        currentScaleFactor = 1;
    }
    update();
}

void ImageWidget::resizeEvent(QResizeEvent*)
{
    update();
}


