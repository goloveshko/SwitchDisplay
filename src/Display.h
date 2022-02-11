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

    static LONG connectedMonitors(std::vector<std::wstring> &names);
    static bool queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_ID id, DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId);
    static LONG getDisplayCurrentTopology(DISPLAYCONFIG_TOPOLOGY_ID& currentTopologyId);
    static QString getDisplayCurrentTopologyString();
    static QString getDisplayTopologyString(DISPLAYCONFIG_TOPOLOGY_ID topologyId);

    static bool SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_ID id, DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId);
    static bool Extend(QString &currentTopology);
    static bool Clone(QString& currentTopology);
    static bool Internal(QString& currentTopology);
    static bool External(QString& currentTopology);
    static void TurnOn();
    static void TurnOff();
};

