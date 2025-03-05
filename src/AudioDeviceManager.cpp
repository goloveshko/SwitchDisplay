#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include "AudioDeviceManager.h"

using namespace FredEmmott::Audio;

AudioDeviceManager::AudioDeviceManager(QObject* parent)
	: QObject(parent), m_callbackHandle(nullptr)
{
	initialize();
}

AudioDeviceManager::~AudioDeviceManager() {
}

void AudioDeviceManager::initialize() {
	try {
		m_callbackHandle = std::make_unique<AudioDevicePlugEventCallbackHandle>(AddAudioDevicePlugEventCallback(
			std::bind_front(&AudioDeviceManager::onAudioDevicePlugEvent, this)));
	}
	catch (const std::exception& e) {
		qCritical() << "Ошибка при регистрации обратного вызова:" << e.what();
	}
}

QMap<QString, QVariantMap> AudioDeviceManager::getAudioDevicesInfo()
{
	QMap<QString, QVariantMap> result;
	const auto devices = GetAudioDeviceList(AudioDeviceDirection::OUTPUT);
	for (const auto& [id, device] : devices) {
		QVariantMap deviceInfo;
		deviceInfo["id"] = QString::fromStdString(id);
		deviceInfo["connected"] = device.state == AudioDeviceState::CONNECTED;

		result[QString::fromStdString(device.displayName)] = deviceInfo;
	}

	return result;
}

QString AudioDeviceManager::getAudioDeviceName(const QString& deviceId) {
	QString result = "";

	const auto devices = GetAudioDeviceList(AudioDeviceDirection::OUTPUT);
	for (const auto& [id, device] : devices) {
		if (id == deviceId) {
			result = QString::fromStdString(device.displayName);
		}
	}

	return result;
}

void AudioDeviceManager::onAudioDevicePlugEvent(AudioDevicePlugEvent event, const std::string& deviceId) {
	QString qtDeviceId = QString::fromStdString(deviceId);
	QString deviceName = getAudioDeviceName(QString::fromStdString(deviceId));


	switch (event) {
	case AudioDevicePlugEvent::ADDED:
		qDebug() << "Аудиоустройство добавлено:" << deviceName << "|" << qtDeviceId;
		//SetDefaultAudioDeviceID(AudioDeviceDirection::OUTPUT, AudioDeviceRole::DEFAULT, deviceId);
		emit signalDeviceAdded(deviceName, qtDeviceId);
		break;
	case AudioDevicePlugEvent::REMOVED:
		qDebug() << "Аудиоустройство удалено:" << deviceName << "|" << qtDeviceId;
		emit signalDeviceRemoved(deviceName, qtDeviceId);
		break;
	default:
		qDebug() << "Неизвестное событие для устройства:" << deviceName << "|" << qtDeviceId;
		break;
	}
}

void AudioDeviceManager::setDefaultAudioDevice(const QString& deviceId)
{
	SetDefaultAudioDeviceID(AudioDeviceDirection::OUTPUT, AudioDeviceRole::DEFAULT, deviceId.toStdString());
}