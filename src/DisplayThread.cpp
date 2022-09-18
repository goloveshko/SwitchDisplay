#include "DisplayThread.h"
#include <QDebug>


DisplayThread::DisplayThread(QObject *parent)
	: QThread(parent) {

}

DisplayThread::~DisplayThread() {

}

bool DisplayThread::Extend(QString &previousTopology) {
	long result = SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_EXTERNAL, previousTopology);
	return result == ERROR_SUCCESS;
}

bool DisplayThread::Clone(QString &previousTopology) {
	long result = SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_CLONE, previousTopology);
	return result == ERROR_SUCCESS;
}

bool DisplayThread::Internal(QString &previousTopology) {
	long result = SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_INTERNAL, previousTopology);
	return result == ERROR_SUCCESS;
}

bool DisplayThread::External(QString &previousTopology) {
	long result = SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_EXTERNAL, previousTopology);
	return result == ERROR_SUCCESS;
}

long DisplayThread::SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_ID id, QString &previousTopology) {
	long result = ERROR_SUCCESS;
	DISPLAYCONFIG_TOPOLOGY_ID previousTopologyId;
	if(!queryDisplayConfig(id, previousTopologyId)) {
		result = SetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY | id);//SDC_TOPOLOGY_EXTERNAL
	}

	previousTopology = getDisplayTopologyString(previousTopologyId);

	return result;
}

void DisplayThread::TurnOn() {
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, -1);
}

void DisplayThread::TurnOff() {
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
}

long DisplayThread::connectedMonitors(std::vector<std::wstring> &names) {
	const UINT32 MAX_COUNT = 0xFF;
	UINT32 pathCount = MAX_COUNT;
	UINT32 modeCount = MAX_COUNT;

	DISPLAYCONFIG_PATH_INFO pathes[MAX_COUNT];
	DISPLAYCONFIG_MODE_INFO modes[MAX_COUNT];

	LONG ret = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, pathes, &modeCount, modes, NULL);
	if(ret != ERROR_SUCCESS) {
		return ret;
	}

	for(UINT32 i = 0; i < modeCount; i++) {
		if(modes[i].infoType != DISPLAYCONFIG_MODE_INFO_TYPE_TARGET) {
			continue;
		}

		DISPLAYCONFIG_TARGET_DEVICE_NAME deviceName;
		deviceName.header.size = sizeof deviceName;
		deviceName.header.adapterId = modes[i].adapterId;
		deviceName.header.id = modes[i].id;
		deviceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
		ret = DisplayConfigGetDeviceInfo(&deviceName.header);
		if(ret != ERROR_SUCCESS) {
			return ret;
		}
		names.push_back(deviceName.monitorFriendlyDeviceName);
	}
	return ERROR_SUCCESS;
}

bool DisplayThread::queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_ID id, DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId) {
	// 	bool connected = false;
	// 	const UINT32 MAX_COUNT = 0xFF;
	// 
	// 	DISPLAYCONFIG_PATH_INFO pathes[MAX_COUNT];
	// 	DISPLAYCONFIG_MODE_INFO modes[MAX_COUNT];
	// 	UINT32 Pathcount = sizeof(pathes) / sizeof(pathes[0]);
	// 	UINT32 modeCount = sizeof(modes) / sizeof(modes[0]);
	// 
	// 	DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId = DISPLAYCONFIG_TOPOLOGY_INTERNAL;
	// 	LONG ret = QueryDisplayConfig(QDC_DATABASE_CURRENT, &Pathcount, pathes, &modeCount, modes, &currentTopologyId);
	// 	if (ret == ERROR_SUCCESS) {
	// 		connected = id == currentTopologyId;
	// 	}
	// 	return connected;

	bool connected = false;
	//DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId;
	LONG ret = getDisplayCurrentTopology(currentTopologyId);
	if(ret == ERROR_SUCCESS) {
		connected = id == currentTopologyId;
	}
	return connected;
}

long DisplayThread::getDisplayCurrentTopology(DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId) {
	const UINT32 MAX_COUNT = 0xFF;

	DISPLAYCONFIG_PATH_INFO pathes[MAX_COUNT];
	DISPLAYCONFIG_MODE_INFO modes[MAX_COUNT];
	UINT32 Pathcount = sizeof(pathes) / sizeof(pathes[0]);
	UINT32 modeCount = sizeof(modes) / sizeof(modes[0]);

	LONG ret = QueryDisplayConfig(QDC_DATABASE_CURRENT, &Pathcount, pathes, &modeCount, modes, &currentTopologyId);

	return ret;
}

QString DisplayThread::getDisplayCurrentTopologyString() {
	QString result = "Unknown";
	DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId;
	LONG ret = getDisplayCurrentTopology(currentTopologyId);
	if(ret == ERROR_SUCCESS) {
		result = getDisplayTopologyString(currentTopologyId);
	}
	return result;
}

QString DisplayThread::getDisplayTopologyString(DISPLAYCONFIG_TOPOLOGY_ID topologyId) {
	QString result = "Unknown";

	if(topologyId == DISPLAYCONFIG_TOPOLOGY_INTERNAL)
		result = "internal";
	else if(topologyId == DISPLAYCONFIG_TOPOLOGY_CLONE)
		result = "clone";
	else if(topologyId == DISPLAYCONFIG_TOPOLOGY_EXTEND)
		result = "extend";
	else if(topologyId == DISPLAYCONFIG_TOPOLOGY_EXTERNAL)
		result = "external";
	else if(topologyId == DISPLAYCONFIG_TOPOLOGY_FORCE_UINT32)
		result = "force_uint32";
	return result;
}

void DisplayThread::setDisplayMode(DISPLAYCONFIG_TOPOLOGY_ID topologyId) {
	newTopologyId = topologyId;
}

void DisplayThread::run() {
	int count = 1000;
	//count = 200000;
	long result = -1;
	QString previousTopology;
	QString newTopology = getDisplayTopologyString(newTopologyId);
	
// 	bool isInterruptionRequested_ = isInterruptionRequested();
// 	qDebug() << "DisplayThread::run()" << isInterruptionRequested_;

	while(!isInterruptionRequested() && result != ERROR_SUCCESS && previousTopology != newTopology && count-- > 0) {
		result = SetDisplayMode(newTopologyId, previousTopology);
		QThread::msleep(100);
	}

	emit signalModeChanged(result, previousTopology, newTopology);
}
