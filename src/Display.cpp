#include "Display.h"


Display::Display() {

}

Display::~Display() {

}

bool Display::Extend()
{
	bool result = false;
	if (!queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_EXTERNAL)) {
		LONG ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY | SDC_TOPOLOGY_EXTEND);
		result = ret == ERROR_SUCCESS;
	}

	return result;
}

bool Display::Clone()
{
	bool result = false;
	if (!queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_CLONE)) {
		LONG ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY | SDC_TOPOLOGY_CLONE);
		result = ret == ERROR_SUCCESS;
	}

	return result;
}

bool Display::Internal()
{
	bool result = false;
	if (!queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_INTERNAL)) {
		LONG ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY | SDC_TOPOLOGY_INTERNAL);
		result = ret == ERROR_SUCCESS;
	}

	return result;
}

bool Display::External()
{
	bool result = false;
	if (!queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_EXTERNAL)) {
		LONG ret = SetDisplayConfig(0, NULL, 0, NULL, SDC_APPLY | SDC_TOPOLOGY_EXTERNAL);
		result = ret == ERROR_SUCCESS;
	}

	return result;
}

void Display::TurnOn()
{
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, -1);
}

void Display::TurnOff()
{
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
}

LONG Display::connectedMonitors(std::vector<std::wstring>& names) {
	const UINT32 MAX_COUNT = 0xFF;
	UINT32 pathCount = MAX_COUNT;
	UINT32 modeCount = MAX_COUNT;

	DISPLAYCONFIG_PATH_INFO pathes[MAX_COUNT];
	DISPLAYCONFIG_MODE_INFO modes[MAX_COUNT];

	LONG ret = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, pathes, &modeCount, modes, NULL);
	if (ret != ERROR_SUCCESS) {
		return ret;
	}

	for (UINT32 i = 0; i < modeCount; i++) {
		if (modes[i].infoType != DISPLAYCONFIG_MODE_INFO_TYPE_TARGET) {
			continue;
		}

		DISPLAYCONFIG_TARGET_DEVICE_NAME deviceName;
		deviceName.header.size = sizeof deviceName;
		deviceName.header.adapterId = modes[i].adapterId;
		deviceName.header.id = modes[i].id;
		deviceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
		ret = DisplayConfigGetDeviceInfo(&deviceName.header);
		if (ret != ERROR_SUCCESS) {
			return ret;
		}
		names.push_back(deviceName.monitorFriendlyDeviceName);
	}
	return ERROR_SUCCESS;
}

bool Display::queryDisplayConfig(DISPLAYCONFIG_TOPOLOGY_ID id) {
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
	DISPLAYCONFIG_TOPOLOGY_ID currentTopologyId;
	LONG ret = getDisplayCurrentTopology(currentTopologyId);
	if (ret == ERROR_SUCCESS) {
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
	if (ret == ERROR_SUCCESS) {
		if (currentTopologyId == DISPLAYCONFIG_TOPOLOGY_INTERNAL)
			result = "DISPLAYCONFIG_TOPOLOGY_INTERNAL";
		else if (currentTopologyId == DISPLAYCONFIG_TOPOLOGY_CLONE)
			result = "DISPLAYCONFIG_TOPOLOGY_CLONE";
		else if (currentTopologyId == DISPLAYCONFIG_TOPOLOGY_EXTEND)
			result = "DISPLAYCONFIG_TOPOLOGY_EXTEND";
		else if (currentTopologyId == DISPLAYCONFIG_TOPOLOGY_EXTERNAL)
			result = "DISPLAYCONFIG_TOPOLOGY_EXTERNAL";
		else if (currentTopologyId == DISPLAYCONFIG_TOPOLOGY_FORCE_UINT32)
			result = "DISPLAYCONFIG_TOPOLOGY_FORCE_UINT32";
	}
	return result;
}