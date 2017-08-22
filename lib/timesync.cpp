#include "timesync.h"

#include <QtEndian>
#include <QUdpSocket>
#include <QDateTime>
#include <QTimer>
#include <QHostInfo>

#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 )
#include <QNetworkDatagram>
#endif

#include "timedatagram.h"

using namespace fugio;

TimeSync::TimeSync( QObject *pParent )
	: QObject( pParent ), mSocket( nullptr ), mResponseSocket( nullptr ), mServerTimestamp( 0 ), mClientTimestamp( 0 ),
	  mPlayheadStartTime( 0 ), mPlayheadStartSet( -1 ),
	  mRTT( 0 ), mGlobalOffset( 0 ), mUniversalOffset( 0 )
{
//	QHostAddress	groupAddress = QHostAddress( "226.0.0.1" );

	mSocket = new QUdpSocket( this );

	if( mSocket->bind( 45454, QUdpSocket::ShareAddress ) )
	{
//		mSocket->joinMulticastGroup( groupAddress );

		connect( mSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()) );
	}
	else
	{
		qWarning() << "Couldn't bind socket";
	}

	mResponseSocket = new QUdpSocket( this );

	if( mResponseSocket->bind() )
	{
		connect( mResponseSocket, SIGNAL(readyRead()), this, SLOT(responseReady()) );
	}
	else
	{
		qWarning() << "Couldn't bind response socket";
	}

//	qDebug() << logtime() << "TimeSync port:" << mResponseSocket->localPort();

	mGlobalTimer.start();

	qDebug() << "Global Timer Monotonic:" << mGlobalTimer.isMonotonic();

	updateUniversalTimestamp( 0 );

	QTimer::singleShot( 1000, this, SLOT(sendPing()) );
}

void TimeSync::setTimeServer( const QString &pString, int pPort )
{
	QHostInfo::lookupHost( pString, this, SLOT(universalServerLookup(QHostInfo)) );

	mServerLookupPort = pPort;
}

void TimeSync::resetPlayhead()
{
	if( !mServerAddress.isNull() )
	{
		mPlayheadStartTime = universalTimestamp() + 500;

		TimeDatagram	 TDG;

		TDG.mServerTimestamp   = qToBigEndian<qint64>( TIME_SET_PLAYHEAD );
		TDG.mClientTimestamp   = qToBigEndian<qint64>( mPlayheadStartTime );

		if( mResponseSocket->writeDatagram( (const char *)&TDG, sizeof( TDG ), mServerAddress, mServerPort ) == sizeof( TDG ) )
		{
		}
	}
}

void TimeSync::universalServerLookup( const QHostInfo &pHost )
{
	if( pHost.error() != QHostInfo::NoError )
	{
		qWarning() << "Time server lookup failed:" << pHost.errorString();

		return;
	}

	qInfo() << "Time server address:" << pHost.hostName() << pHost.addresses().first().toString();

	mServerAddress = pHost.addresses().first().toString();
	mServerPort    = mServerLookupPort;
}

QString TimeSync::logtime()
{
	return( QDateTime::currentDateTimeUtc().toString( "HH:mm:ss.zzz" ) );
}

void TimeSync::processPendingDatagrams()
{
	fugio::TimeDatagram	 TDG;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 8, 0 )
	QNetworkDatagram	 Datagram;

	while( mSocket->hasPendingDatagrams() )
	{
		Datagram = mSocket->receiveDatagram();

		if( !Datagram.isValid() )
		{
			break;
		}

		if( Datagram.data().size() != sizeof( TDG ) )
		{
			continue;
		}

		memcpy( &TDG, Datagram.data(), sizeof( TDG ) );

		mServerAddress = Datagram.senderAddress();
		mServerPort    = 45456;
	}
#else
	QByteArray			 DatagramBuffer;
	QHostAddress		 ServerAddress;

	while( mSocket->hasPendingDatagrams() )
	{
		int		DatagramSize = mResponseSocket->pendingDatagramSize();

		if( DatagramSize <= 0 )
		{
			break;
		}

		DatagramBuffer.resize( DatagramSize );

		if( DatagramBuffer.size() != mResponseSocket->pendingDatagramSize() )
		{
			break;
		}

		mResponseSocket->readDatagram( DatagramBuffer.data(), DatagramBuffer.size(), &ServerAddress );

		if( DatagramBuffer.size() != sizeof( TDG ) )
		{
			continue;
		}

		memcpy( &TDG, DatagramBuffer.data(), sizeof( TDG ) );

		mServerAddress = ServerAddress;
		mServerPort    = 45456;
	}
#endif
}

void TimeSync::responseReady()
{
	TimeDatagram	 TDG;
	QByteArray		 DatagramBuffer;

	while( mResponseSocket->hasPendingDatagrams() )
	{
		DatagramBuffer.resize( mResponseSocket->pendingDatagramSize() );

		if( DatagramBuffer.size() != mResponseSocket->pendingDatagramSize() )
		{
			break;
		}

		mResponseSocket->readDatagram( DatagramBuffer.data(), DatagramBuffer.size() );

		if( DatagramBuffer.size() != sizeof( TDG ) )
		{
			continue;
		}

		memcpy( &TDG, DatagramBuffer.data(), sizeof( TDG ) );

		TDG.mServerTimestamp   = qFromBigEndian<qint64>( TDG.mServerTimestamp );
		TDG.mClientTimestamp   = qFromBigEndian<qint64>( TDG.mClientTimestamp );

		// Detect playhead start time

		if( TDG.mServerTimestamp == TIME_SET_PLAYHEAD )
		{
			mPlayheadStartTime = TDG.mClientTimestamp;

			continue;
		}

//		qDebug() << logtime() << "PONG" << DG.senderAddress() << "RS:" << TDG.mServerTimestamp << "RC:" << TDG.mClientTimestamp << "LC:" << mClientTimestamp;

		if( TDG.mClientTimestamp == mClientTimestamp )
		{
			mRTT = timestamp() - TDG.mClientTimestamp;

			if( mRTTArray.size() >= 7 )
			{
				mRTTArray.removeFirst();
			}

			mRTTArray << mRTT;

			mRTTSortedArray = mRTTArray;

			std::sort( mRTTSortedArray.begin(), mRTTSortedArray.end() );

			updateUniversalTimestamp( TDG.mServerTimestamp + ( mRTT / 2 ) ); //( qMax( mRTT, mRTTArray[ mRTTArray.size() / 2 ] ) / 2 ) );

			//qDebug() << logtime() << "RTT:" << mRTT << mRTTSortedArray << TDG.mServerTimestamp + ( mRTT / 2 ) << universalTimestamp();
		}
	}
}

void TimeSync::sendPing()
{
	if( !mServerAddress.isNull() )
	{
		mClientTimestamp = timestamp();
		mServerTimestamp = universalTimestamp();

//		qDebug() << logtime() << "Sending PING to" << mServerAddress << mServerPort << "T:" << 0 << "LC:" << mClientTimestamp;

		TimeDatagram	 TDG;

		TDG.mServerTimestamp   = qToBigEndian<qint64>( mServerTimestamp );
		TDG.mClientTimestamp   = qToBigEndian<qint64>( mClientTimestamp );

		if( mResponseSocket->writeDatagram( (const char *)&TDG, sizeof( TDG ), mServerAddress, mServerPort ) == sizeof( TDG ) )
		{
			mPlayheadStartSet = -1;
		}
	}

	QTimer::singleShot( qMax( 2500LL, 5000LL - ( QDateTime::currentMSecsSinceEpoch() % 5000LL ) ), this, SLOT(sendPing()) );
}
