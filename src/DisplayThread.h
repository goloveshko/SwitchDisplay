#pragma once

#include <QThread>

#include <Windows.h>
#include <vector>
#include <string>

class DisplayThread :
    public QThread
{
    Q_OBJECT
public:
    DisplayThread(QObject *parent = nullptr);
    ~DisplayThread();

	void setDisplayMode(DISPLAYCONFIG_TOPOLOGY_ID topologyId);

	virtual void run() override;

	long connectedMonitors(std::vector<std::wstring> &names);
	bool queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_ID id, DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId);
	long getDisplayCurrentTopology(DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId);
	QString getDisplayCurrentTopologyString();
	QString getDisplayTopologyString(DISPLAYCONFIG_TOPOLOGY_ID topologyId);

	long SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_ID id, QString &previousTopology);
	bool Extend(QString &previousTopology);
	bool Clone(QString &previousTopology);
	bool Internal(QString &previousTopology);
	bool External(QString &previousTopology);
	void TurnOn();
	void TurnOff();

signals:
	void signalModeChanged(long result, const QString &previousTopology, const QString &newTopology);

private:
	DISPLAYCONFIG_TOPOLOGY_ID newTopologyId;
};

