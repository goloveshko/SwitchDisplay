#include "Display.h"


Display::Display() {

}

Display::~Display() {

}

bool Display::Extend(QString &currentTopology) {
	DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId;
	bool result = SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_EXTERNAL, currentTopologyId);
	currentTopology = getDisplayTopologyString(currentTopologyId);
	return result;
}

bool Display::Clone(QString &currentTopology) {
	DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId;
	bool result = SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_CLONE, currentTopologyId);
	currentTopology = getDisplayTopologyString(currentTopologyId);
	return result;
}

bool Display::Internal(QString &currentTopology) {
	DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId;
	bool result = SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_INTERNAL, currentTopologyId);
	currentTopology = getDisplayTopologyString(currentTopologyId);
	return result;
}

bool Display::External(QString &currentTopology) {
	DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId;
	bool result = SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_EXTERNAL, currentTopologyId);
	currentTopology = getDisplayTopologyString(currentTopologyId);
	return result;
}

bool Display::SetDisplayMode(DISPLAYCONFIG_TOPOLOGY_ID id, DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId) {
	bool result = false;
	if(!queryDisplayConfig(id, currentTopologyId)) {
		LONG ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY | id);//SDC_TOPOLOGY_EXTERNAL
		result = ret == ERROR_SUCCESS;
	}

	return result;
}

void Display::TurnOn() {
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, -1);
}

void Display::TurnOff() {
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
}

LONG Display::connectedMonitors(std::vector<std::wstring> &names) {
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

bool Display::queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_ID id, DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId) {
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

LONG Display::getDisplayCurrentTopology(DISPLAYCONFIG_TOPOLOGY_ID &currentTopologyId) {
	const UINT32 MAX_COUNT = 0xFF;

	DISPLAYCONFIG_PATH_INFO pathes[MAX_COUNT];
	DISPLAYCONFIG_MODE_INFO modes[MAX_COUNT];
	UINT32 Pathcount = sizeof(pathes) / sizeof(pathes[0]);
	UINT32 modeCount = sizeof(modes) / sizeof(modes[0]);

	LONG ret = QueryDisplayConfig(QDC_DATABASE_CURRENT, &Pathcount, pathes, &modeCount, modes, &currentTopologyId);

	return ret;
}

QString Display::getDisplayCurrentTopologyString() {
	QString result = "Unknown";
	DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId;
	LONG ret = getDisplayCurrentTopology(currentTopologyId);
	if(ret == ERROR_SUCCESS) {
		result = getDisplayTopologyString(currentTopologyId);
	}
	return result;
}

QString Display::getDisplayTopologyString(DISPLAYCONFIG_TOPOLOGY_ID topologyId) {
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