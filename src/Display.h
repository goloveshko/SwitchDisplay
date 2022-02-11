#pragma once

#include <QObject>

#include <Windows.h>
#include <vector>
#include <string>

class Display :
    public QObject
{
public:
    Display();
    ~Display();

    static LONG connectedMonitors(std::vector<std::wstring>& names);
    static bool queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_ID id);
    static LONG getDisplayCurrentTopology(DISPLAYCONFIG_TOPOLOGY_ID& currentTopologyId);
    static QString getDisplayCurrentTopologyString();

    static bool Extend();
    static bool Clone();
    static bool Internal();
    static bool External();
    static void TurnOn();
    static void TurnOff();
};

