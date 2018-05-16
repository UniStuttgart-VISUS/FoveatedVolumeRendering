/**
 * \file
 *
 * \author Valentin Bruder
 *
 * \copyright Copyright (C) 2018 Valentin Bruder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mainwindow.h"
#include <QApplication>
#include <QErrorMessage>

 /*
 Console output from:
 https://forum.qt.io/topic/56484/solved-attach-console-to-gui-application-on-windows/4
 */

#define DEBUG 

#ifdef DEBUG
#include <windows.h>
#include <stdio.h>
#endif

int main(int argc, char *argv[])
{
#ifdef DEBUG
	// detach from the current console window
	// if launched from a console window, that will still run waiting for the new console (below) to close
	// it is useful to detach from Qt Creator's <Application output> panel
	FreeConsole();

	// create a separate new console window
	AllocConsole();

	// attach the new console to this application's process
	AttachConsole(GetCurrentProcessId());

	// reopen the std I/O streams to redirect I/O to the new console
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	freopen("CON", "r", stdin);
#endif

    QApplication a(argc, argv);
    QErrorMessage::qtHandler();

    MainWindow w;
    w.show();

    return a.exec();
}
