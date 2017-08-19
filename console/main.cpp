#include <QCommandLineParser>
#include <QScopedPointer>
#include <QTimer>

#include "clientconsole.h"
#include "../lib/timeserver.h"
#include "../lib/timecast.h"

int main( int argc, char *argv[] )
{
	QCoreApplication	a( argc, argv );

	QCoreApplication::setApplicationName( "fugiotimeserver" );
	QCoreApplication::setApplicationVersion( "1.0.0" );

	TimeServer				 *TS = new TimeServer( &a );
	TimeCast				 *TC = new TimeCast();

	QTimer					  CastTimer;

	QObject::connect( &CastTimer, &QTimer::timeout, [=]()
	{
		TC->sendTime( TS->timestamp() );
	} );

	return( a.exec() );
}
