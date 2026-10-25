/*
The stellarium telescope library helps building
telescope server programs, that can communicate with stellarium
by means of the stellarium TCP telescope protocol.
It also contains sample server classes (dummy, Astro Electronic FS2).

Author and Copyright of this file and of the stellarium telescope library:
Johannes Gajdosik, 2006

Astro Electronic FS2 support by Michael Butti, 2026.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
*/

#include "Fs2Command.hpp"
#include "TelescopeClientDirectFs2.hpp"
#include "common/LogFile.hpp"
#include "StelUtils.hpp"

#include <cmath>

#include <QByteArray>

Fs2Command::Fs2Command(Server &server)
             : server(*static_cast<TelescopeClientDirectFs2*>(&server)),
               has_been_written_to_buffer(false)
{
}


bool Fs2CommandToggleFormat::writeCommandToBuffer(char *&p, char *end)
{
	if (end-p < 3) // FS2 has one less character than LX200 here
		return false;
	//*p++ = '#'; // FS2 does not like this prefix
	*p++ = ':';
	*p++ = 'U';
	*p++ = '#';
	has_been_written_to_buffer = true;
	return true;
}

void Fs2CommandToggleFormat::print(QTextStream &o) const
{
	o << "Fs2CommandToggleFormat";
}


bool Fs2CommandStopSlew::writeCommandToBuffer(char *&p, char *end)
{
	if (end-p < 3) // FS2 has one less character than LX200 here
		return false;
	//*p++ = '#'; // FS2 does not like this prefix
	*p++ = ':';
	*p++ = 'Q';
	*p++ = '#';
	has_been_written_to_buffer = true;
	return true;
}

void Fs2CommandStopSlew::print(QTextStream &o) const {
	o << "Fs2CommandStopSlew";
}


bool Fs2CommandSetSelectedRa::writeCommandToBuffer(char *&p, char *end)
{
	if (end-p < 14) return false; // FS2 patch needs this number updated (+1 for added space character)
	  // set object ra:
	*p++ = ':';
	*p++ = 'S';
	*p++ = 'r';
	*p++ = ' ';  // FS2 needs this space
	int x = ra;
	p += 8;
	p[-1] = '0' + (x % 10); x /= 10;
	p[-2] = '0' + (x %  6); x /=  6;
	p[-3] = ':';
	p[-4] = '0' + (x % 10); x /= 10;
	p[-5] = '0' + (x %  6); x /=  6;
	p[-6] = ':';
	p[-7] = '0' + (x % 10); x /= 10;
	p[-8] = '0' + static_cast<char>(x);
	*p++ = '#';
	has_been_written_to_buffer = true;
	return true;
}

int Fs2CommandSetSelectedRa::readAnswerFromBuffer(const char *&buff,
                                                    const char *end)
{
	if (buff < end && *buff=='#')
		buff++; // ignore silly byte

	if (buff >= end)
		return 0;

	switch (buff[0])
	{
		case '0':
			#ifdef DEBUG4
			*log_file << Now()
			          << "Fs2CommandSetSelectedRa::readAnswerFromBuffer:"
			             "ra invalid"
				  << StelUtils::getEndLineChar();
			#endif
			buff++;
			break;

		case '1':
			#ifdef DEBUG4
			*log_file << Now()
			          << "Fs2CommandSetSelectedRa::readAnswerFromBuffer:"
			             "ra valid"
				  << StelUtils::getEndLineChar();
			#endif
			buff++;
			break;

		default:
			#ifdef DEBUG4
			*log_file << Now()
			          << "Fs2CommandSetSelectedRa::readAnswerFromBuffer:"
			             "strange: unexpected char"
				  << StelUtils::getEndLineChar();
			#endif
			break;
	}

	return 1;
}

void Fs2CommandSetSelectedRa::print(QTextStream &o) const
{
	o << "Fs2CommandSetSelectedRa("
	  << (ra/3600) << ':' << ((ra/60)%60) << ':' << (ra%60) << ')';
}


bool Fs2CommandSetSelectedDec::writeCommandToBuffer(char *&p, char *end)
{
	if (end-p < 14) // FS2 patch needs this number updated (+1 for added space character)
		return false;

	  // set object dec:
	*p++ = ':';
	*p++ = 'S';
	*p++ = 'd';
	*p++ = ' ';  // FS2 needs this space
	int x = dec;
	if (x < 0)
	{
		*p++ = '-';
		x = -x;
	}
	else
	{
		*p++ = '+';
	}
	p += 8;
	p[-1] = '0' + (x % 10); x /= 10;
	p[-2] = '0' + (x %  6); x /=  6;
	p[-3] = ':';
	p[-4] = '0' + (x % 10); x /= 10;
	p[-5] = '0' + (x %  6); x /=  6;
	p[-6] = '\xDF'; // = 223, degree symbol. This is always printed as asterisk in the specs, very confusing.
	p[-7] = '0' + (x % 10); x /= 10;
	p[-8] = '0' + static_cast<char>(x);
	*p++ = '#';
	has_been_written_to_buffer = true;
	return true;
}

int Fs2CommandSetSelectedDec::readAnswerFromBuffer(const char *&buff,
                                                     const char *end)
{
	if (buff < end && *buff=='#')
		buff++; // ignore silly byte

	if (buff >= end)
		return 0;

	switch (buff[0])
	{
		case '0':
			#ifdef DEBUG4
			*log_file << Now()
			          << "Fs2CommandSetSelectedDec::readAnswerFromBuffer:"
			             "dec invalid"
				  << StelUtils::getEndLineChar();
			#endif
			buff++;
			break;

		case '1':
			#ifdef DEBUG4
			*log_file << Now()
			          << "Fs2CommandSetSelectedDec::readAnswerFromBuffer:"
			             "dec valid"
				  << StelUtils::getEndLineChar();
			#endif
			buff++;
			break;

		default:
			#ifdef DEBUG4
			*log_file << Now()
			          << "Fs2CommandSetSelectedDec::readAnswerFromBuffer:"
			             "strange: unexpected char"
				  << StelUtils::getEndLineChar();
			#endif
			break;
	}

	return 1;
}

void Fs2CommandSetSelectedDec::print(QTextStream &o) const
{
	const int d = abs(dec);
	o << "Fs2CommandSetSelectedDec("
	  << ((dec<0)?'-':'+')
	  << (d/3600) << ':' << ((d/60)%60) << ':' << (d%60) << ')';
}


bool Fs2CommandGotoSelected::writeCommandToBuffer(char *&p, char *end)
{
	if (end-p < 4)
		return false;

	  // slew to current object coordinates
	*p++ = ':';
	*p++ = 'M';
	*p++ = 'S';
	*p++ = '#';
	has_been_written_to_buffer = true;
	return true;
}

int Fs2CommandGotoSelected::readAnswerFromBuffer(const char *&buff,
                                                   const char *end)
{
	if (buff < end && *buff=='#')
		buff++; // ignore silly byte

	if (buff >= end)
		return 0;

	const char *p = buff;
	if (first_byte == 256)
	{
		first_byte = buff[0];
		p++;
	}

	switch (first_byte)
	{
		case '0':
			#ifdef DEBUG4
			*log_file << Now()
			          << "Fs2CommandGotoSelected::readAnswerFromBuffer: "
			             "slew ok"
				  << StelUtils::getEndLineChar();
			#endif
			buff++;
			return 1;

		case '1':
		case '2':
		{
			if (p == end)
			{
				// the AutoStar 494 returns just '1', nothing else
				#ifdef DEBUG4
				*log_file << Now()
				          << "Fs2CommandGotoSelected::readAnswerFromBuffer: "
				             "slew failed ("
					  << (static_cast<char>(first_byte))
				          << "), "
				             "but no complete answer yet"
					  << StelUtils::getEndLineChar();
				#endif
				buff++;
				return 0;
			}

			for (;;p++)
			{
				if (p >= end)
				{
					return 0;
				}
				if (*p == '#')
					break;
			}
			#ifdef DEBUG4
			*log_file << Now()
			<< "Fs2CommandGotoSelected::readAnswerFromBuffer: "
			   "slew failed ("
			<< (static_cast<char>(first_byte))
			<< "): '"
			<< QByteArray(buff + 1, static_cast<int>(p - buff - 1))
			<< '\''
			<< StelUtils::getEndLineChar();
			#endif
			buff = p+1;
			return 1;
		}

		default:
			#ifdef DEBUG4
			*log_file << Now()
			          << "Fs2CommandGotoSelected::readAnswerFromBuffer: "
			             "slew returns something weird"
				  << StelUtils::getEndLineChar();
			#endif
			break;
	}

	return -1;
}

void Fs2CommandGotoSelected::print(QTextStream &o) const
{
	o << "Fs2CommandGotoSelected";
}

bool Fs2CommandSyncSelected::writeCommandToBuffer(char *&p, char *end)
{
	if (end-p < 4)
		return false;

	  // slew to current object coordinates
	*p++ = ':';
	*p++ = 'C';
	*p++ = 'M';
	*p++ = '#';
	has_been_written_to_buffer = true;
	return true;
}

int Fs2CommandSyncSelected::readAnswerFromBuffer(const char *&buff,
						   const char *end)
{
	if (buff < end && *buff=='#')
		buff++; // ignore silly byte

	if (buff >= end)
		return 0;

	const char *p = buff;
	if (first_byte == 256)
	{
		first_byte = buff[0];
		p++;
	}

	// Unlike ":MS#" (goto), the Astro Electronic ":CM#" (sync) command does not return a
	// numeric status. On success it replies with a '#'-terminated string - the
	// name of the object the telescope was synchronised to (e.g. "M31#"), a
	// message such as "Coordinates matched.#", or simply "OK#" as sent by
	// INDIGO's FS2 emulation. Accept any '#'-terminated response as success
	// and consume it. A bare '0' (used by some controllers) is still accepted
	// as a single-byte answer.
	if (first_byte == '0')
	{
		#ifdef DEBUG4
		*log_file << Now()
			  << "Fs2CommandSyncSelected::readAnswerFromBuffer: sync ok"
			  << StelUtils::getEndLineChar();
		#endif
		buff++;
		return 1;
	}

	for (;;p++)
	{
		if (p >= end)
			return 0; // wait for the rest of the answer
		if (*p == '#')
			break;
	}
	#ifdef DEBUG4
	*log_file << Now()
	<< "Fs2CommandSyncSelected::readAnswerFromBuffer: sync acknowledged: '"
	<< QByteArray(buff, static_cast<int>(p - buff))
	<< '\''
	<< StelUtils::getEndLineChar();
	#endif
	buff = p+1;
	return 1;
}

void Fs2CommandSyncSelected::print(QTextStream &o) const
{
	o << "Fs2CommandSyncSelected";
}


bool Fs2CommandGetRa::writeCommandToBuffer(char *&p, char *end)
{
	if (end-p < 4) // FS2 has one less character than LX200 here
		return false;

	  // get RA:
	//*p++ = '#'; // FS2 does not like this prefix
	*p++ = ':';
	*p++ = 'G';
	*p++ = 'R';
	*p++ = '#';
	has_been_written_to_buffer = true;
	return true;
}

int Fs2CommandGetRa::readAnswerFromBuffer(const char *&buff,
                                            const char *end)
{
	if (buff < end && *buff=='#')
		buff++; // ignore silly byte

	if (end-buff < 8)
		return 0;

	bool long_format = true;
	int ra;
	const char *p = buff;
	ra = ((*p++) - '0');
	ra *= 10;
	ra += ((*p++) - '0');
	if (*p++ != ':')
	{
		#ifdef DEBUG4
		*log_file << Now()
		          << "Fs2CommandGetRa::readAnswerFromBuffer: "
		             "error: ':' expected"
			  << StelUtils::getEndLineChar();
		#endif
		return -1;
	}

	ra *=  6; ra += ((*p++) - '0');
	ra *= 10; ra += ((*p++) - '0');
	switch (*p++)
	{
		case ':':
			ra *=  6; ra += ((*p++) - '0');
			ra *= 10; ra += ((*p++) - '0');
			if (end-buff < 9)
				return 0;
			break;

		case '.':
			ra *= 10; ra += ((*p++) - '0');
			ra *= 6;
			long_format = false;
			break;

		default:
			*log_file << Now()
			          << "Fs2CommandGetRa::readAnswerFromBuffer: "
			             "error: '.' or ':' expected"
				  << StelUtils::getEndLineChar();
			return -1;
	}

	if (*p++ != '#')
	{
		*log_file << Now()
		          << "Fs2CommandGetRa::readAnswerFromBuffer: "
		             "error: '#' expected"
			  << StelUtils::getEndLineChar();
		return -1;
	}

	#ifdef DEBUG4
	*log_file << Now()
	          << "Fs2CommandGetRa::readAnswerFromBuffer: "
	          << "RA = "
	          << qSetPadChar('0')
	          << qSetFieldWidth(2)  << (ra/3600)
	          << qSetFieldWidth(0) << ':'
	          << qSetFieldWidth(2) << ((ra/60)%60)
	          << qSetFieldWidth(0) << ':'
	          << qSetFieldWidth(2) << (ra%60)
	          << qSetFieldWidth(0) << qSetPadChar(' ')
		  << StelUtils::getEndLineChar();
	#endif

	buff = p;
	server.longFormatUsedReceived(long_format);
	server.raReceived(static_cast<unsigned int>(std::floor(ra * (4294967296.0/86400.0))));
	return 1;
}

void Fs2CommandGetRa::print(QTextStream &o) const
{
	o << "Fs2CommandGetRa";
}




bool Fs2CommandGetDec::writeCommandToBuffer(char *&p,char *end)
{
	if (end-p < 4) // FS2 has one less character than LX200 here
		return false;

	  // get Dec:
	//*p++ = '#'; // FS2 does not like this prefix
	*p++ = ':';
	*p++ = 'G';
	*p++ = 'D';
	*p++ = '#';
	has_been_written_to_buffer = true;
	return true;
}

int Fs2CommandGetDec::readAnswerFromBuffer(const char *&buff,
                                             const char *end)
{
	if (buff < end && *buff=='#')
		buff++; // ignore silly byte

	if (end-buff < 7)
		return 0;

	bool long_format = true;
	int dec;
	const char *p = buff;
	bool sign_dec = false;
	switch (*p++)
	{
		case '+':
			break;

		case '-':
			sign_dec = true;
			break;

		default:
			#ifdef DEBUG4
			*log_file << Now()
			          << "Fs2CommandGetDec::readAnswerFromBuffer: "
			             "error: '+' or '-' expected"
				  << StelUtils::getEndLineChar();
			#endif
			return -1;
	}

	dec = ((*p++) - '0');
	dec *= 10; dec += ((*p++) - '0');
	if (*p++ != (static_cast<char>(223)))
	{
		*log_file << Now()
		          << "Fs2CommandGetDec::readAnswerFromBuffer: "
			     "error: degree sign expected" // not a star, really 0xDF
			  << StelUtils::getEndLineChar();
	}

	dec *=  6; dec += ((*p++) - '0');
	dec *= 10; dec += ((*p++) - '0');
	switch (*p++)
	{
		case '#':
			long_format = false;
			dec *= 60;
			break;

		// High-precision declination is "sDD*MM'SS#": the canonical Astro Electronic
		// FS2 separator between arcminutes and arcseconds is an apostrophe
		// (as sent by real mounts and by INDIGO's FS2 emulation). Some
		// implementations use a colon instead, so accept both.
		case ':':
		case '\'':
			if (end-buff < 10)
				return 0;
			dec *=  6; dec += ((*p++) - '0');
			dec *= 10; dec += ((*p++) - '0');
			if (*p++ != '#')
			{
				*log_file << Now()
				          << "Fs2CommandGetDec::readAnswerFromBuffer: "
				             "error: '#' expected"
					  << StelUtils::getEndLineChar();
				return -1;
			}
			break;

		default:
			*log_file << Now()
			          << "Fs2CommandGetDec::readAnswerFromBuffer: "
			             "error: '#', ':' or ''' expected"
				  << StelUtils::getEndLineChar();
			return -1;
	}
	#ifdef DEBUG4
	*log_file << Now()
	          << "Fs2CommandGetDec::readAnswerFromBuffer: "
	          << "Dec = " << (sign_dec?'-':'+')
	          << qSetPadChar('0')
	          << qSetFieldWidth(2) << (dec/3600)
	          << qSetFieldWidth(0) << ':'
	          << qSetFieldWidth(2) << ((dec/60)%60)
	          << qSetFieldWidth(0) << ':'
	          << qSetFieldWidth(2) << (dec%60)
	          << qSetFieldWidth(0) << qSetPadChar(' ')
		  << StelUtils::getEndLineChar();
	#endif

	if (sign_dec)
		dec = -dec;
	buff = p;
	server.longFormatUsedReceived(long_format);
	server.decReceived(static_cast<unsigned int>(std::floor(dec* (4294967296.0/(360*3600.0)))));
	return 1;
}

void Fs2CommandGetDec::print(QTextStream &o) const
{
	o << "Fs2CommandGetDec";
}
