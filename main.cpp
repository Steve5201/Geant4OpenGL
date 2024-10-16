#include "mainwindow.h"
#include <QThread>
#include <QSettings>
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    HKEY hKey;
    std::string key = "GEANT4_DATA_DIR";
    std::string value = argv[0];
    int idx = value.find_last_of('\\');
    if(idx > 0)
    {
        value = value.substr(0, idx);
    }
    value += "\\data";
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, key.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), value.size() + 1);
        RegCloseKey(hKey);
    }
    Geant4Manager *g4 = new Geant4Manager;
    int threadCount = 1;
    int idealThreadCount = QThread::idealThreadCount();
    idealThreadCount = idealThreadCount * 0.8;
    if(idealThreadCount < 1)
    {
        idealThreadCount = 1;
    }
    QSettings settings("g4.ini", QSettings::IniFormat);
    int userThreadCount = settings.value("General/ThreadCount").toInt();
    bool highEM = settings.value("General/HighEM").toBool();
    if(userThreadCount > idealThreadCount)
    {
        userThreadCount = idealThreadCount;
    }
    if(userThreadCount > 0)
    {
        threadCount = userThreadCount;
    }
    else
    {
        threadCount = idealThreadCount;
    }
    g4->initialize(threadCount, highEM);
    QApplication a(argc, argv);
    MainWindow w;
    w.setG4Manager(g4);
    w.showMaximized();
    return a.exec();
}
