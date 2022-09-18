#pragma once

#include <QObject>
#include <QThread>

#include <Windows.h>
#include <vector>
#include <string>

class Display:
    public QThread
{
    Q_OBJECT
public:
    Display();
    ~Display();

	virtual void run() override;

	LONG connectedMonitors(std::vector<std::wstring> &names);
	bool queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_ID id, DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId);
	LONG getDisplayCurrentTopology(DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId);
	QString getDisplayCurrentTopologyString();
	QString getDisplayTopologyString(DISPLAYCONFIG_TOPOLOGY_ID topologyId);

	bool SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_ID id, DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId);
	bool Extend(QString &currentTopology);
	bool Clone(QString &currentTopology);
	bool Internal(QString &currentTopology);
	bool External(QString &currentTopology);
	void TurnOn();
	void TurnOff();
};

