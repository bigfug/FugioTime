#include "timeserver.h"

#include <QtEndian>
#include <QTimer>

#include "timedatagram.h"

#define TIME_SERVER_MESSAGES

TimeServer::TimeServer( QObject *pParent )
	: QObject( pParent ), mPlayheadStartTime( 0 )
{
	mUniverseStartTime = QDateTime::currentMSecsSinceEpoch();

	mUniverseTimer.start();

	mSocket = new QUdpSocket( this );

	connect( mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)) );

	if( mSocket->bind( 45456 ) )
	{
		connect( mSocket, SIGNAL(readyRead()), this, SLOT(responseReady()) );
	}
	else
	{
		qWarning() << logtime() << "Couldn't bind socket";
	}

#if defined( TIME_SERVER_MESSAGES )
	qInfo() << logtime() << "TimeServer:" << mSocket->localAddress() << mSocket->localPort();
#endif

	QTimer *ClientTimeoutTimer = new QTimer( this );

	connect( ClientTimeoutTimer, &QTimer::timeout, this, &TimeServer::clientTimeout );

	ClientTimeoutTimer->start( 15000 );
}

void TimeServer::socketError( QAbstractSocket::SocketError pError )
{
	qWarning() << logtime() << "sendError" << pError << mSocket->errorString();
}

void TimeServer::responseReady( void )
{
	fugio::TimeDatagram		TDG;
	QByteArray				DatagramBuffer;
	QHostAddress			ServerAddress;
	quint16					ServerPort;

	while( mSocket->hasPendingDatagrams() )
	{
		DatagramBuffer.resize( mSocket->pendingDatagramSize() );

		if( DatagramBuffer.size() != mSocket->pendingDatagramSize() )
		{
			break;
		}

		mSocket->readDatagram( DatagramBuffer.data(), DatagramBuffer.size(), &ServerAddress, &ServerPort );

		if( DatagramBuffer.size() != sizeof( TDG ) )
		{
			continue;
		}

		memcpy( &TDG, DatagramBuffer.constData(), sizeof( TDG ) );

		//---------------------------------------------------------------------
		// See if we have a record of this client (add one if not)

		bool		ClientFound = false;

		for( ClientInfo &CI : mClientInfo )
		{
			if( CI.mAddress == ServerAddress && CI.mPort == ServerPort )
			{
				CI.mLastSeen = timestamp();

				ClientFound = true;

				break;
			}
		}

		if( !ClientFound )
		{
			ClientInfo		CI;

			CI.mAddress  = ServerAddress;
			CI.mPort     = ServerPort;
			CI.mLastSeen = timestamp();

			mClientInfo << CI;

#if defined( TIME_SERVER_MESSAGES )
		qInfo() << logtime() << "TimeServer: Adding client" << CI.mAddress << CI.mPort;
#endif
		}

		//---------------------------------------------------------------------
		// Is this a command to set the playhead?  Forward it to all other clients

		qint64		DGServer = qToBigEndian<qint64>( TDG.mServerTimestamp );
		qint64		DGClient = qToBigEndian<qint64>( TDG.mClientTimestamp );

		if( DGServer == fugio::TIME_SET_PLAYHEAD )
		{
			if( DGClient != mPlayheadStartTime )
			{
#if defined( TIME_SERVER_MESSAGES )
				qInfo() << logtime() << "TimeServer: Setting playhead start time:" << DGClient;
#endif

				mPlayheadStartTime = DGClient;

				for( ClientInfo &CI : mClientInfo )
				{
					if( ServerAddress == CI.mAddress && ServerPort == CI.mPort )
					{
						continue;
					}

					if( mSocket->writeDatagram( (const char *)&TDG, sizeof( TDG ), CI.mAddress, CI.mPort ) != sizeof( fugio::TimeDatagram ) )
					{

					}
				}
			}

			continue;
		}

		//---------------------------------------------------------------------
		// A normal timeserver ping - send the packet back to the client

		qint64		ServerTimestamp = timestamp();
		qint64		RTT             = ServerTimestamp - DGServer;

//		qDebug() << logtime() << "Received PING from" << DG.senderAddress().toString() << "RC:" << qFromBigEndian<qint64>( TDG.mClientTimestamp ) << "ST:" << ServerTimestamp;

		// Send the response packet

//		qDebug() << logtime() << "PONG" << DG.senderAddress() << DG.senderPort();

		TDG.mServerTimestamp = qToBigEndian( ServerTimestamp );

		if( mSocket->writeDatagram( (const char *)&TDG, sizeof( TDG ), ServerAddress, ServerPort ) != sizeof( fugio::TimeDatagram ) )
		{
			qWarning() << logtime() << "Couldn't write packet";
		}

		emit clientResponse( ServerAddress, ServerPort, ServerTimestamp, RTT );
	}
}

void TimeServer::clientTimeout()
{
	qint64		TimeStamp = timestamp();

	for( int i = 0 ; i < mClientInfo.size() ; )
	{
		ClientInfo	CI = mClientInfo[ i ];

		if( TimeStamp - CI.mLastSeen < 120 * 1000 )
		{
			i++;

			continue;
		}

#if defined( TIME_SERVER_MESSAGES )
		qInfo() << logtime() << "TimeServer: Removing client" << CI.mAddress << CI.mPort;
#endif

		mClientInfo.removeAt( i );
	}
}
