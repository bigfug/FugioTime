#ifndef TIMESYNC_H
#define TIMESYNC_H

#include <QElapsedTimer>
#include <QHostAddress>

class QHostInfo;
class QUdpSocket;

namespace fugio {

class TimeSync : public QObject
{
	Q_OBJECT

public:
	TimeSync( QObject *pParent = nullptr );

	inline qint64 timestamp( void ) const
	{
		return( mGlobalTimer.elapsed() );
	}

	qint64 universalTimestamp( void ) const
	{
		return( mUniversalTimer.elapsed() + mUniversalOffset );
	}

	qint64 universalToGlobal( qint64 pTimeStamp ) const
	{
		return( ( pTimeStamp - mUniversalOffset ) + mGlobalOffset );
	}

	qint64 globalToUniversal( qint64 pTimeStamp ) const
	{
		return( ( pTimeStamp - mGlobalOffset ) + mUniversalOffset );
	}

public slots:
	void updateUniversalTimestamp( qint64 pTimeStamp )
	{
		mUniversalTimer.restart();

		mUniversalOffset = pTimeStamp;

		mGlobalOffset = mGlobalTimer.elapsed();
	}

	void setTimeServer( const QString &pServer, int pPort = 45456 );

private:
	static QString logtime( void );

private slots:
	void processPendingDatagrams( void );

	void responseReady( void );

	void sendPing( void );

	void universalServerLookup( const QHostInfo &pHost );

private:
	QUdpSocket		*mSocket;
	QUdpSocket		*mResponseSocket;
	qint64			 mServerTimestamp;
	qint64			 mClientTimestamp;
	qint64			 mRTT;
	QVector<qint64>	 mRTTArray;
	QVector<qint64>	 mRTTSortedArray;
	QHostAddress	 mServerAddress;
	quint16			 mServerPort;
	int				 mServerLookupPort;

	QElapsedTimer					 mGlobalTimer;
	qint64							 mGlobalOffset;		// convert from universal to global
	QElapsedTimer					 mUniversalTimer;
	qint64							 mUniversalOffset;
};

} // namespace fugio

#endif // TIMESYNC_H
