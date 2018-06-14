#include <QCommandLineParser>
#include <QScopedPointer>
#include <QTimer>

#include <chrono>

#include "clientconsole.h"

int main( int argc, char *argv[] )
{
	QCoreApplication	a( argc, argv );

	QCoreApplication::setApplicationName( "fugiotimeserver" );
	QCoreApplication::setApplicationVersion( "1.0.0" );

	QCommandLineParser		Parser;

	Parser.addHelpOption();
	Parser.addVersionOption();

	Parser.process( a );

	ClientConsole		*TR = new ClientConsole( &a );

	return( a.exec() );
}
