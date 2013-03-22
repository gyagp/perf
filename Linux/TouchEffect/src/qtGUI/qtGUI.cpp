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

#include <QtGui>
#include "imageWidget.h"

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
 
    bool enableAnimation = false;
   
    QStringList paramList = app.arguments();
    for (int i=1; i<paramList.size(); i++) {
        QString param1 = paramList.at(i);
        if (param1 == "-a") {
            enableAnimation = true;
        }
    }

    QMainWindow mainWindow;

    ImageWidget * imageWidget = new ImageWidget(&mainWindow, enableAnimation);
    imageWidget->setImage("../images/demoPic.jpg");
    mainWindow.setCentralWidget(imageWidget);
	
    mainWindow.showFullScreen();

    return app.exec();    
}
