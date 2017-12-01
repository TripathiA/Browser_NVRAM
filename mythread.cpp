#include "mythread.h"
#include <unistd.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>

MyThread::MyThread()
{
    isRunning = false;
    directory = "/home/aastha/Desktop/kernel/project/mem/";
}

void MyThread::startSystem(vector<string> pages)
{
        isRunning = true;
        pages_to_load = pages;
        start();
}

void MyThread::stopSystem()
{
    isRunning = false;
    exit();
}

void MyThread::run()
{
    while(isRunning)
    {
        //qDebug("Thread id inside run %d",(int)QThread::currentThreadId());

        for(int i = 0 ; i <pages_to_load.size() ; i++)
        {
            string cmd = "wget "+pages_to_load[i]+" -O"+directory+pages_to_load[i]+".html";
            system(cmd.c_str());
        }
        isRunning = false;
    }
    stopSystem();
}
