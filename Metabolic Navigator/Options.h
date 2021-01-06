#pragma once
#include <QSettings>
#include <QWidget>
#include <QString>

class Options
{
public:
    Options(void);
    ~Options(void);
    void saveWindowOptions(QWidget *, QString);
    void loadWindowOptions(QWidget *, QString);
    QSettings *settings;
    bool saveSettings;
};
