#ifndef FUGIO_TIMEDATAGRAM_H
#define FUGIO_TIMEDATAGRAM_H

namespace fugio
{

typedef struct TimeDatagram
{
	qint64		mServerTimestamp;
	qint64		mClientTimestamp;
} TimeDatagram;

} // namespace fugio

#endif // FUGIO_TIMEDATAGRAM_H
