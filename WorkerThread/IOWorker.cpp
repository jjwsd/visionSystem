#include "IOWorker.h"

#include <QDebug>
#include <QThread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <unistd.h>

IOWorker::IOWorker(QObject *parent) : QObject(parent)
{

}

void IOWorker::pollingCallback()
{
    unsigned int value = low;
    jetsonTX2GPIONumber trigger = gpio486;
    gpioExport(trigger);
    gpioSetDirection(trigger,inputPin);
    while(true)
    {
        gpioGetValue(trigger, &value);
        if(value == high)
        {
            emit send_event_by_polling();
            break;
        }
    }
}

