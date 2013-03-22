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

#include <QPixmap>
#include <QGraphicsLinearLayout>
#if QT_VERSION < 0x040500
#error You must use Qt >= 4.5.0!
#endif

#include <MApplication>
#include <MSceneWindow>
#include <MWidget>
#include <MPannableViewport>
#include <MSceneManager>
#include <MImageWidget>
#include "mtwindow.h" 

int main(int argc, char ** argv)
{
    MApplication app(argc, argv);

    QPixmap pixmap("../images/demoPic.jpg");
    MImageWidget * image = new MImageWidget(&pixmap);   // Can not construct with direct file name, may be bug of meegotouch
    image->setAspectRatioMode(Qt::IgnoreAspectRatio);
    QGraphicsLinearLayout * picLayout = new QGraphicsLinearLayout();
    picLayout->setContentsMargins(0, 0, 0, 0);
    picLayout->addItem(image);
    MWidget * container = new MWidget();
    container->setLayout(picLayout);

    MSceneWindow * sceneWindow = new MSceneWindow();
    MPannableViewport * viewport = new MPannableViewport(sceneWindow);
    viewport->setPanDirection(Qt::Horizontal);
    viewport->setWidget(container);

    QGraphicsLinearLayout * windowLayout = new QGraphicsLinearLayout();
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->addItem(viewport);
    sceneWindow->setLayout(windowLayout);

    MTWindow window;
    window.sceneManager()->appearSceneWindowNow(sceneWindow);
	QObject::connect(&window, SIGNAL(closeWindow()), &app, SLOT(quit()));
    window.show();

    return app.exec();
}
