#ifndef TIMESYNC_H
#define TIMESYNC_H

#include <QElapsedTimer>
#include <QHostAddress>

#include <chrono>

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
		std::chrono::high_resolution_clock::time_point	TP = std::chrono::high_resolution_clock::now();
		std::chrono::milliseconds						MS = std::chrono::duration_cast<std::chrono::milliseconds>( TP - mGlobalTimer );

		return( MS.count() );
	}

	qint64 universalTimestamp( void ) const
	{
		std::chrono::high_resolution_clock::time_point	TP = std::chrono::high_resolution_clock::now();
		std::chrono::milliseconds						MS = std::chrono::duration_cast<std::chrono::milliseconds>( TP - mUniversalTimer );

		return( MS.count() + mUniversalOffset );
	}

	qint64 universalToGlobal( qint64 pTimeStamp ) const
	{
		return( ( pTimeStamp - mUniversalOffset ) + mGlobalOffset );
	}

	qint64 globalToUniversal( qint64 pTimeStamp ) const
	{
		return( ( pTimeStamp - mGlobalOffset ) + mUniversalOffset );
	}

	qint64 playhead( void ) const
	{
		return( mPlayheadStartTime > 0 ? universalTimestamp() - mPlayheadStartTime : 0 );
	}

public slots:
	void updateUniversalTimestamp( qint64 pTimeStamp )
	{
		mUniversalTimer = std::chrono::high_resolution_clock::now();

		mUniversalOffset = pTimeStamp;

		mGlobalOffset = timestamp();
	}

	void setTimeServer( const QString &pServer, int pPort = 45456 );

	void resetPlayhead( void );

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
	qint64			 mPlayheadStartTime;
	qint64			 mPlayheadStartSet;
	qint64			 mRTT;
	QHostAddress	 mServerAddress;
	quint16			 mServerPort;
	int				 mServerLookupPort;
	bool			 mLockedOn;

	std::chrono::high_resolution_clock::time_point			 mGlobalTimer;
	qint64													 mGlobalOffset;		// convert from universal to global
	std::chrono::high_resolution_clock::time_point			 mUniversalTimer;
	qint64													 mUniversalOffset;
};

} // namespace fugio

#endif // TIMESYNC_H
