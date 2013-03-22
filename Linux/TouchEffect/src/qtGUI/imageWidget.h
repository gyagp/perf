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

#ifndef __IMAGEWIDGET
#define __IMAGEWIDGET

#include <QWidget>
#include <QGestureEvent>
#include <QPanGesture>
#include <QPinchGesture>
#include <QBasicTimer>

#define PAN_READY 0
#define PAN_PRESS 1 
#define PAN_MOVE  2
#define PAN_AUTOMOVE    3
#define PAN_STOP  4

class ImageWidget : public QWidget 
{
    Q_OBJECT
public:
    ImageWidget(QWidget *parent = 0, bool enableAnimation = false);
    bool setImage(const QString &fileName);

protected:
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *event);

signals:
    void closeWidget();

private:
    bool gestureEvent(QGestureEvent *event);
    void panning(QPanGesture*);
    void pinching(QPinchGesture*);
    bool simGestureStateMachine(QMouseEvent *mouseEvent);
    void slowDown(QPointF &speed, float a = 0.96);
    void boundLimit();   
    void keyPressEvent(QKeyEvent * keyEvent);
 
    bool  enAnimation;
    int   simState;
    int   screenWidth;
    int   screenHeight;
    float horizontalOffset;
    float verticalOffset;
    float dampCoef;
    float rotationAngle;
    float scaleFactor;
    float currentScaleFactor;
    QPixmap image;

    QPoint simPressPos;
    QPoint simDragPos;
    QPointF simSpeed;

    QBasicTimer timer;
};

#endif
