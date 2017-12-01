/* Simple Browser Sample
 * Put this file (browser.h) and browser.cpp, browser.pro in one folder.
 * Use Qt Creator with Qt 5 to compile the project.
 *
 * Content of browser.cpp
 *
#include "browser.h"
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    MainWindow window;
    window.loadUrl(QUrl("http://lib.ustc.edu.cn/"));
    window.showFullScreen();
    return app.exec();
}
 * Content of browser.pro
 *
QT += widgets webkitwidgets
SOURCES = browser.cpp
HEADERS += \
    browser.h
*/

#ifndef BROWSER_H
#define BROWSER_H

#include <QtGui>
#include <QtWebKitWidgets>
#include <QBoxLayout>
#include <QPushButton>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <time.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <map>
#include <string.h>
#include <vector>
#include <unordered_set>
#include <fstream>

//PERF COUNT

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>



#include "mythread.h"

using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    long sysTime;
    long sysTimeend;
    MyThread bg_load;
    string directory = "/home/aastha/Desktop/kernel/project/mem/";
    unordered_set<std::string> saved_pages;
    map<string,vector<string> > preload_graph;
    struct perf_event_attr read_miss;
    long long read_miss_count = -1;
    int fd;
    bool perf_start;
    std::ofstream statistics;

public:
    MainWindow(void) {
        content = new QWebView;
        toolbar = new QToolBar;
        location = new QLineEdit;
        homeButton = new QPushButton(QString("Home"));
        topBarLayout = new QBoxLayout(QBoxLayout::LeftToRight);
        layout = new QBoxLayout(QBoxLayout::TopToBottom);
        window = new QWidget;
        progressBar = new QProgressBar;
        sysTime = 0;
        sysTimeend = 0;
        statistics.open ("/home/aastha/Desktop/kernel/project/stat.txt", std::ofstream::out);

        //perf_event
        perf_start = false;
        memset(&read_miss, 0, sizeof(struct perf_event_attr));
        read_miss.type = PERF_TYPE_HW_CACHE  ;
        read_miss.size = sizeof(struct perf_event_attr);
        read_miss.disabled = 1;
        read_miss.exclude_kernel = 1;
        read_miss.exclude_hv = 1;
        read_miss.config = (PERF_COUNT_HW_CACHE_LL) | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
        fd = -1;
        //

        content->setHtml(QString("<h1>Loading...</h1>"));

        toolbar->addAction(content->pageAction(QWebPage::Back));
        toolbar->addAction(content->pageAction(QWebPage::Forward));
        toolbar->addAction(content->pageAction(QWebPage::Reload));
        toolbar->addAction(content->pageAction(QWebPage::Stop));

        topBarLayout->addWidget(toolbar);
        topBarLayout->addWidget(location);
        topBarLayout->addWidget(homeButton);
        layout->addLayout(topBarLayout);
        layout->addWidget(content);
        layout->addWidget(progressBar);
        window->setLayout(layout);

        setCentralWidget(window);

        progressBar->setMinimum(0);
        progressBar->setMaximum(100);

        QObject::connect(content, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinish()));
        QObject::connect(content, SIGNAL(loadFinished(bool)), progressBar, SLOT(reset()));
        QObject::connect(content, SIGNAL(loadProgress(int)), this, SLOT(onLoadProgress()));
        QObject::connect(content, SIGNAL(loadProgress(int)), progressBar, SLOT(setValue(int)));
        QObject::connect(location, SIGNAL(returnPressed()), this, SLOT(gotoUrl()));
        QObject::connect(homeButton, SIGNAL(clicked()), this, SLOT(goHome()));
    }

    void loadUrl(const QUrl url) {
        homeUrl = url;
        content->load(url);
        content->setFocus();
    }

//    static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags)
//    {
//        int ret;
//        ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
//        return ret;
//    }

protected slots:


    void gotoUrl() {
        QUrl url = QUrl::fromUserInput(location->text());
        vector<string> load_pages;
        string x = url.toString().toUtf8().constData();
        string loaded = x;
        cout << "URL entered " << loaded << endl;
        x = x.substr(7,x.length());
        cout << "URL changed " << x << endl;
        statistics << "URL " << x << "\n";
        perf_start = false;
        if(saved_pages.find(x) != saved_pages.end())
        {
            perf_start = true;
            cout << "load from file" << endl;
            statistics << "from file \n";
            loaded = "file://"+directory+x+".html";
        }
        const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        const boost::posix_time::time_duration td = now.time_of_day();
        //fd = perf_event_open(&read_miss_count, 0, -1, -1, 0);
        sysTime = td.total_milliseconds();
        if(perf_start)
        {
            fd = syscall(__NR_perf_event_open, &read_miss, 0,-1,-1,0);
            ioctl(fd, PERF_EVENT_IOC_RESET, 0);
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
            cout << "ioctl enable" << endl;
        }
        content->load(QUrl(QString::fromStdString(loaded)));
        content->setFocus();
        if(preload_graph.find(x) != preload_graph.end())
        {
            cout << "preload_pages"<< endl;
            load_pages = preload_graph.find(x)->second;
            bg_load.startSystem(load_pages);
            for(int i = 0 ; i <load_pages.size() ; i++)
            {
                saved_pages.insert(load_pages[i]);
            }
        }

    }

    void goHome() {
        content->load(homeUrl);
        content->setFocus();
    }

    void onLoadFinish() {
        cout << "load finish" << endl;
        setWindowTitle(QString("%1").arg(content->title()));
        const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        const boost::posix_time::time_duration td = now.time_of_day();
        cout << perf_start << " " << fd << endl;
        sysTimeend = td.total_milliseconds();
        if(perf_start && fd != -1)
        {
            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
            read(fd, &read_miss_count, sizeof(long long));
            cout << "Read Misses" << read_miss_count << "\n";
            perf_start = false;
        }
        //close(fd);

        //cout <<" thread status " << bg_load.isRunning << "\n";
        const long elapsed = sysTimeend - sysTime;
        statistics << "Read Misses " << read_miss_count << "\n";
        statistics << sysTime << " " << sysTimeend << " " << elapsed << "\n";
        statistics << "\n\n";
        read_miss_count = -1;
        //QTimer::singleShot(2000, content, SLOT(close()));
    }

    void onLoadProgress() {
        location->setText(content->url().toString());
    }

private:
    QWebView *content;
    QLineEdit *location;
    QPushButton *homeButton;
    QBoxLayout *layout;
    QToolBar *toolbar;
    QBoxLayout *topBarLayout;
    QProgressBar *progressBar;
    QWidget *window;
    QUrl homeUrl;
};

#endif // BROWSER_H
