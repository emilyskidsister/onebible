/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

#ifndef YVAPP_H
#define YVAPP_H

#include <QObject>

#include <bb/cascades/Application>

class YVApp : public QObject
{
    Q_OBJECT
public:
    explicit YVApp(bb::cascades::Application *app);
    virtual ~YVApp() {}
};

#endif//YVAPP_H
