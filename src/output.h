/*
    This file is part of inputmangler, a programm which intercepts and
    transforms linux input events, depending on the active window.
    Copyright (C) 2014  Arkadiusz Guzinski <kermit@ag.de1.cc>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "inputevent.h"
#include "definitions.h"
#include <linux/input.h> //__u16...
#include <QString>
#include <QVector>
#include <QHash>


/*!
 * @brief An output event, e.g: 
 * "S" for press shift
 * "g" for press g
 * "d+C" for press Ctrl-D
 * more Complex Types will be supported in the future (maybe via plugins)
 */
class OutEvent 
{
	/*!
	 * @brief OutType is the type of the OutEvent.
	 */
	enum OutType {
		Simple,	//!< A single event. e.g. "key a" - the value is determined by source
		Combo,	//!< Combo event. e.g. "ctrl+c"
		Macro,	//!< Complex event sequence. Not yet implemented.vv
		Wait,	//!< Wait a while
		Custom	//!< Handle via plugin. Not yet implemented.
	};
	/*!
	* @brief Data that wil be sent to inputdummy, aka low level input event
	* See linux/input.h for Details on Variables.
	*/
	struct VEvent
	{
		__s32 type; // these two are __s16 in input.h, but input_event(), called in
		__s32 code; // inputdummy expects int
		__s32 value;
	};
	friend class WindowSettings;
	friend class TransformationStructure;
public:
	OutEvent() {};
	OutEvent(int c) {eventcode = c;};
	OutEvent(InputEvent& e);
	OutEvent(QString s);
	OutEvent(const OutEvent &other);
	OutEvent(OutEvent &other);
	~OutEvent();
// 	OutEvent(__s32 code, bool shift, bool alt = false, bool ctrl = false);
	QString toString() const;
	QString print() const{return toString();};
	QVector<__u16> modifiers;
	__u16 eventtype;
	__u16 eventcode;
	__u16 code() const {return eventcode;};
	OutType outType;
	ValueType valueType;
	bool hasCustomValue = false; //!<used only for Macros
	int customValue = 0;  		//!< on Macro: custom value, on Wait: time in microseconds
	void send();
	void send(int value);
	void send(int value, __u16 sourceType);
	// protected?
	static void sendMouseEvent(VEvent *e, int num = 1);
	static void sendKbdEvent(VEvent *e, int num = 1);
	static void sendEvent(VEvent *e, int num = 1);
	
	static void sendRaw(__s32 type, __s32 code, __s32 value, DType dtype = Auto);
	
	static void generalSetup();
	static void closeVirtualDevices();
	static int fds[5]; //!< file descriptors for /dev/virtual_*: None, Keyboard, Mouse, Tablet, Joystick
#ifdef DEBUGME
	QString initString;
#endif
	//OutEvent& operator=(OutEvent &other);
	OutEvent& operator=(const OutEvent &other);
protected:
	void sendMacro();
	void sendSimple(int value);
	void sendCombo(int value);
	void fromInputEvent(InputEvent& e);
	static void openVDevice(char * path, int num);
	void *next = nullptr;  //!< Next event in macro sequence or pointer to custom event
	void proceed();
	void parseCombo(QStringList l);
	void parseMacro(QStringList l);
	OutEvent(QStringList macroParts);
	
};



