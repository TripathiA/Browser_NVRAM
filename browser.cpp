#include "browser.h"
#include <vector>
#include <map>
#include <fstream>

using namespace std;

//void flush_L1()
//{
//    char buffer[128000];
//     for(int i = 0 ;i<128000; i++)
//     {
//       buffer[i] = 'a';
//     }
//}


int main(int argc, char** argv) {
    QApplication app(argc, argv);
    MainWindow window;

    // generate graph
    std::string line;
    string graph_file="/home/aastha/Desktop/kernel/project/graph.txt";
    ifstream myfile(graph_file);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            std::string input = line;
            std::istringstream ss(input);
            std::string token;
            bool first = false;
            vector<string> v;
            string key;
            while(std::getline(ss, token, ',')) {
                if(!first)
                {
                    key = token;
                    first = true;
                }
                else v.push_back((token));
            }
            window.preload_graph[key] = v;
            vector<string>().swap(v);
        }
        myfile.close();
    }
    vector<string> v;
    v.push_back("www.jabong.com");
    v.push_back("www.yahoo.com");
    v.push_back("www.facebook.com");
    window.preload_graph["www.google.com"] = v;

    for(map<string, vector<string> >::const_iterator it = window.preload_graph.begin();
        it != window.preload_graph.end(); ++it)
    {
        std::cout << it->first << "->";
        for(int i = 0 ;i<it->second.size() ; i++) cout << it->second[i] << " ";
        cout << endl;
    }


    //flush_L1();
    //window.loadUrl(QUrl("http://www.amazon.com"));
    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    const boost::posix_time::time_duration td = now.time_of_day();
//    ioctl(window.fd, PERF_EVENT_IOC_RESET, 0);
//    ioctl(window.fd, PERF_EVENT_IOC_ENABLE, 0);
//    window.perf_start = true;
    window.sysTime = td.total_milliseconds();
    //window.loadUrl(QUrl("http://www.google.com"));
    window.loadUrl(QUrl("file:///home/aastha/Desktop/kernel/project/Project/Browser1/Google.html"));
    //window.loadUrl(QUrl("file:///home/aastha/Desktop/kernel/project/mem/google_img.html"));
    window.show();
    int ret = app.exec();
    return 0;
}

