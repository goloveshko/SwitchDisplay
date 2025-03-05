#include "AuthorizationHelper.h"

#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>

#include <qt_windows.h>


QString qt_create_commandline(const QStringList& arguments)
{
    QString args;
    for (int i = 0; i < arguments.size(); ++i) {
        QString tmp = arguments.at(i);
        // in the case of \" already being in the string the \ must also be escaped
        tmp.replace(QLatin1String("\\\""), QLatin1String("\\\\\""));
        // escape a single " because the arguments will be parsed
        tmp.replace(QLatin1Char('\"'), QLatin1String("\\\""));
        if (tmp.isEmpty() || tmp.contains(QLatin1Char(' ')) || tmp.contains(QLatin1Char('\t'))) {
            // The argument must not end with a \ since this would be interpreted
            // as escaping the quote -- rather put the \ behind the quote: e.g.
            // rather use "foo"\ than "foo\"
            QString endQuote(QLatin1Char('\"'));
            int i = tmp.length();
            while (i > 0 && tmp.at(i - 1) == QLatin1Char('\\')) {
                --i;
                endQuote += QLatin1Char('\\');
            }
            args += QLatin1String(" \"") + tmp.left(i) + endQuote;
        }
        else {
            args += QLatin1Char(' ') + tmp;
        }
    }
    return args;
}

bool AuthorizationHelper::hasAdminRights()
{
    SID_IDENTIFIER_AUTHORITY authority = { SECURITY_NT_AUTHORITY };
    PSID adminGroup;
    // Initialize SID.
    if (!AllocateAndInitializeSid(&authority,
                                  2,
                                  SECURITY_BUILTIN_DOMAIN_RID,
                                  DOMAIN_ALIAS_RID_ADMINS,
                                  0, 0, 0, 0, 0, 0,
                                  &adminGroup))
        return false;

    BOOL isInAdminGroup = FALSE;
    if (!CheckTokenMembership(0, adminGroup, &isInAdminGroup))
    {
        isInAdminGroup = FALSE;
    }

    FreeSid(adminGroup);
    return isInAdminGroup;
}

bool AuthorizationHelper::execute(const QString &program, const QStringList &arguments)
{
    if (!hasAdminRights())
    {
        QLatin1String key("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System");
        QSettings registry(key, QSettings::NativeFormat);
        const QVariant enableLUA = registry.value(QLatin1String("EnableLUA"));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        if ((enableLUA.type() == QMetaType::Int) && (enableLUA.toInt() == 0))
#else
        if ((enableLUA.typeId() == QMetaType::Int) && (enableLUA.toInt() == 0))
#endif
        {
            return false;
        }
    }

    if (!QFileInfo(program).isExecutable())
    {
        QMessageBox::critical(QApplication::activeWindow(), QApplication::applicationName(), QObject::tr("Something went wrong."));
        return false;
    }

    const QString file = QDir::toNativeSeparators(program);
    const QString args = qt_create_commandline(arguments);

    SHELLEXECUTEINFOW shellExecuteInfo = { 0 };
    shellExecuteInfo.nShow = SW_NORMAL;
    shellExecuteInfo.lpVerb = L"runas";
    shellExecuteInfo.lpFile = (wchar_t *)file.utf16();
    shellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
    shellExecuteInfo.lpParameters = (wchar_t *)args.utf16();
    shellExecuteInfo.fMask = SEE_MASK_NOASYNC;

    if (ShellExecuteExW(&shellExecuteInfo))
    {
        return true;
    }
    
    return false;
}