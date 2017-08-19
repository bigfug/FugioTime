#ifndef FUGIO_TIMEDATAGRAM_H
#define FUGIO_TIMEDATAGRAM_H

namespace fugio
{

static const qint64		TIME_SET_PLAYHEAD = -1;

typedef struct TimeDatagram
{
	qint64		mServerTimestamp;
	qint64		mClientTimestamp;
} TimeDatagram;

} // namespace fugio

#endif // FUGIO_TIMEDATAGRAM_H
