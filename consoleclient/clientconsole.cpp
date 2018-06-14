#include "clientconsole.h"

#include <QTimer>
#include <QDateTime>

ClientConsole::ClientConsole(QObject *parent)
	: QObject(parent)
{
	TS = new fugio::TimeSync( this );

	QTimer::singleShot( 1000, this, &ClientConsole::timeout );
}

void ClientConsole::timeout()
{
	qint64		UT = TS->universalTimestamp();

	qInfo() << UT;

	QTimer::singleShot( std::max<int>( 500, 1000 - ( UT % 1000 ) ), Qt::PreciseTimer, this, &ClientConsole::timeout );
}
