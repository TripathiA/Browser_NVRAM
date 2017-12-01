#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <string>
#include <QThread>
#include <QMutex>
#include <vector>

using namespace std;

class MyThread : public QThread
{
   Q_OBJECT

public:

    bool isRunning;
    string directory;
    vector<string> pages_to_load;
    MyThread(void);
    void startSystem(vector<string> pages);
    void stopSystem();

protected:
   virtual void run();

// signals:
//    void signalValueUpdated(QString);

// private:
//     bool isRunning;

};

#endif // MYTHREAD_H
