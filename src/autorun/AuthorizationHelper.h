#pragma once

#include <QObject>

class AuthorizationHelper
{
public:
    static bool hasAdminRights();
    static bool execute(const QString &programs, const QStringList &arguments);

private:
    AuthorizationHelper()
    {

    }
};