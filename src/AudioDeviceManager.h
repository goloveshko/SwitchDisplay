
#pragma once

#include <QObject>
#include <QVariantMap>
#include <AudioDevices/AudioDevices.h>


class AudioDeviceManager : public QObject {
	Q_OBJECT

public:
	explicit AudioDeviceManager(QObject* parent = nullptr);
	~AudioDeviceManager();

	void initialize();

	void onAudioDevicePlugEvent(FredEmmott::Audio::AudioDevicePlugEvent event, const std::string& deviceId);

	QString getAudioDeviceName(const QString& deviceId);
	QMap<QString, QVariantMap> getAudioDevicesInfo();

	void setDefaultAudioDevice(const QString& deviceId);
signals:
	void signalDeviceAdded(const QString& deviceName, const QString& deviceId);
	void signalDeviceRemoved(const QString& deviceName, const QString& deviceId);

private:
	std::unique_ptr<FredEmmott::Audio::AudioDevicePlugEventCallbackHandle> m_callbackHandle;
	//FredEmmott::Audio::AudioDevicePlugEventCallbackHandle m_callbackHandle;
};