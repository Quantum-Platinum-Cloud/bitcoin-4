// Copyright (c) 2011-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_NOTIFICATOR_H
#define BITCOIN_QT_NOTIFICATOR_H

#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

#include <QIcon>
#include <QObject>
#include <QThread>

#include <atomic>

QT_BEGIN_NAMESPACE
class QSystemTrayIcon;

#ifdef USE_DBUS
class QDBusInterface;
#endif
QT_END_NAMESPACE

class Notificator;

#ifdef USE_DBUS
class DBusInitThread : public QThread
{
    Q_OBJECT

    Notificator& m_notificator;

public:
    DBusInitThread(Notificator& notificator) : m_notificator(notificator) {};

protected:
    void run() override;
};
#endif

/** Cross-platform desktop notification client. */
class Notificator: public QObject
{
    Q_OBJECT

public:
    /** Create a new notificator.
       @note Ownership of trayIcon is not transferred to this object.
    */
    Notificator(const QString &programName, QSystemTrayIcon *trayIcon, QWidget *parent);
    ~Notificator();

    // Message class
    enum Class
    {
        Information,    /**< Informational message */
        Warning,        /**< Notify user of potential problem */
        Critical        /**< An error occurred */
    };

public Q_SLOTS:
    /** Show notification message.
       @param[in] cls    general message class
       @param[in] title  title shown with message
       @param[in] text   message content
       @param[in] icon   optional icon to show with message
       @param[in] millisTimeout notification timeout in milliseconds (defaults to 10 seconds)
       @note Platform implementations are free to ignore any of the provided fields except for \a text.
     */
    void notify(Class cls, const QString &title, const QString &text,
                const QIcon &icon = QIcon(), int millisTimeout = 10000);

private:
    QWidget *parent;
    enum Mode {
        None,                       /**< Ignore informational notifications, and show a modal pop-up dialog for Critical notifications. */
        Freedesktop,                /**< Use DBus org.freedesktop.Notifications */
        QSystemTray,                /**< Use QSystemTrayIcon::showMessage() */
        UserNotificationCenter      /**< Use the 10.8+ User Notification Center (Mac only) */
    };
    QString programName;
    std::atomic<Mode> mode;
    QSystemTrayIcon *trayIcon;
#ifdef USE_DBUS
    QThread *m_dbus_init_thread{nullptr};
protected:
    QDBusInterface *interface;
    friend class DBusInitThread;

private:
    void notifyDBus(Class cls, const QString &title, const QString &text, const QIcon &icon, int millisTimeout);
#endif
    void notifySystray(Class cls, const QString &title, const QString &text, int millisTimeout);
#ifdef Q_OS_MAC
    void notifyMacUserNotificationCenter(const QString &title, const QString &text);
#endif
};

#endif // BITCOIN_QT_NOTIFICATOR_H
