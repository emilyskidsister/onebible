/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#include "YVApp.h"

#include <bb/cascades/Application>

Q_DECL_EXPORT int main(int argc, char **argv) {
    bb::cascades::Application app(argc, argv);
    new YVApp(&app);
    return app.exec();
}
