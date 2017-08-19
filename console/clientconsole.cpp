#include "clientconsole.h"

#include <QTimer>
#include <QDateTime>

ClientConsole::ClientConsole(QObject *parent)
	: QObject(parent)
{
	setTimer();
}

void ClientConsole::setTimer()
{
	qint64	t = QDateTime::currentMSecsSinceEpoch();

	mTimer.singleShot( qMax( 500LL, 1000LL - ( t % 1000LL ) ), this, SLOT(timeout()) );
}

void ClientConsole::timeout( void )
{
//	qInfo() << "CLIENT:" << Global->timestamp() << Global->universalTimestamp();

	setTimer();
}
