#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QScopedPointer>

//#include "../console/clientconsole.h"
#include "../lib/timesync.h"

int main( int argc, char *argv[] )
{
	bool gui = true;

	for( int i = 1 ; i < argc ; i++ )
	{
		if( !strcmp( argv[ i ], "--console" ) )
		{
			gui = false;
		}
	}

	QScopedPointer<QCoreApplication>	a( gui ? new QApplication( argc, argv ) : new QCoreApplication( argc, argv ) );

	if( !a )
	{
		return( -1 );
	}

	QCoreApplication::setApplicationName( "FugioTime" );
	QCoreApplication::setApplicationVersion( "1.0.0" );

	QCommandLineParser		Parser;

	Parser.addHelpOption();

	// A boolean option with a single name (--console)
	QCommandLineOption OptionConsole( "console", QCoreApplication::translate( "main", "Console only" ) );

	QCommandLineOption OptionServer( QStringList() << "s" << "server", "Server <address>.", "address" );

	Parser.addOption( OptionConsole );
	Parser.addOption( OptionServer );

	Parser.process( *QCoreApplication::instance() );

	if( !Parser.isSet( OptionConsole ) )
	{
		MainWindow w;

		w.show();

		return( a->exec() );
	}

//	ClientConsole	c;

//	if( Parser.isSet( OptionServer ) )
//	{
//		fugio::TimeSync		*Client = new fugio::TimeSync();

//		Client->setTimeServer( Parser.value( OptionServer ), 45456 );
//	}

	return( a->exec() );
}
