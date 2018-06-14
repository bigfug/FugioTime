#ifndef CLIENTCONSOLE_H
#define CLIENTCONSOLE_H

#include <QObject>
#include <QTimer>

#include "../lib/timesync.h"

class ClientConsole : public QObject
{
	Q_OBJECT

public:
	explicit ClientConsole(QObject *parent = nullptr);

private slots:
	void timeout( void );

private:
	fugio::TimeSync			*TS;
};

#endif // CLIENTCONSOLE_H
