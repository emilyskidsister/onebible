/*******************************************************

     Part of the OneBible Project for BlackBerry 10
Copyright (C) Joshua Netterfield <joshua@nettek.ca> 2013

                  All rights reserved.

*******************************************************/

// source: https://github.com/blackberry/Cascades-Community-Samples/blob/master/stumpers/bbjamcarousel/assets/SwipeHandler.qml

import bb.cascades 1.0
import utils 1.0

Container {
    id: swipeHandler
    property double dragOffset: 0
    property double momentumOffset: 0
    property double speed
    property double endSpeed
    property double touchDownX
    property variant startTime
    property double timeConstant
    property bool decelerationStarted
    signal scrollingStart()
    signal momentumScrollingEnd()
    signal dragScrollingEnd()
    onTouch: {
        if (event.isDown()) {
            scrollingStart();
            touchDownX = event.windowX;
            startTime = Date.now()
            dragOffset = 0
            if (decelerationStarted == true) {
                timer.stop();
                decelerationStarted = false
            }
        } else if (event.isMove()) {
            dragOffset = touchDownX - event.windowX
        } else if (event.isUp()) {
            dragOffset = touchDownX - event.windowX
            var diffTime = Date.now() - startTime
            speed = dragOffset / diffTime;
            dragScrollingEnd();
        }
    }
    onTouchExit: {
        timer.stop();
        dragScrollingEnd();
    }
    attachedObjects: [
        QTimer {
            id: timer
            property double t: 0
            property double prevOffset: 0
            interval: 16
            onTimeout: {
                var delta = speed * Math.exp(- t / timeConstant);
                prevOffset = momentumOffset
                if (t < 1000) {
                    momentumOffset = momentumOffset + 2 * delta;
                } else {
                    momentumOffset = momentumOffset - delta;
                }
                t = t + 16;
                if (Math.abs(momentumOffset - prevOffset) < 3) {
                    endSpeed = delta / 16
                    speed = 0;
                    timer.stop();
                    decelerationStarted = false
                    momentumScrollingEnd()
                }
            }
        }
    ]
    opacity: 1.0
}
