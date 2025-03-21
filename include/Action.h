
#ifndef AGILE_ACTION_H
#define AGILE_ACTION_H

#ifdef UNIT_TEST
uint32_t millis() { return mock_millis; }
#else
#include "Arduino.h"
#endif
#include <assert.h>

class State;

class Action
{
public:
	enum Type
	{
		N=0,
		S,
		R,
		L,
		D,
		RE,
		FE,
		TYPE_MAX
	};

	~Action(){	};

	Action(uint8_t type, bool *target, uint32_t time = 0)
		: m_actionType(type), m_actionTarget(target), m_delay(time) {
			assert(target != nullptr);
		}

	uint8_t getType() const { return m_actionType; }
	uint32_t getDelay() const { return m_delay; }
	bool *getTarget() const { return m_actionTarget; }

	void clear()
	{
		switch (m_actionType)
		{
		case Type::N:
		case Type::L:
		case Type::D:
		case Type::RE:
			*m_actionTarget = false;
			m_edge = false;
			m_time = -1;
			break;

		// Falling Edge
		// target variable TRUE on falling edge
		case Type::FE:
			*m_actionTarget = true;
		}
	}

	void execute()
	{
		switch (m_actionType)
		{

		// Set to TRUE the value of target variable
		case Type::S:
			*m_actionTarget = true;
			break;

		// Set to FALSE the value of target variable
		case Type::R:
			*m_actionTarget = false;
			break;

		// Non-stored:
		// target variable TRUE as long as the state is active
		case Type::N:
			*m_actionTarget = true;
			break;

		// Time Limited:
		// target variable TRUE until the end of the set time (FALSE on state exit)
		case Type::L: 
		{
			if (!m_edge)
			{
				*m_actionTarget = true;
				m_time = millis();
				m_edge = true;
			}

			uint32_t current_time = millis();
			uint32_t elapsed = (current_time >= m_time) ? (current_time - m_time) : (UINT32_MAX - m_time + current_time + 1);
			if (elapsed > m_delay && m_edge && m_time > 0)
			{
				*m_actionTarget = false;
			}
			break;
		}

		// Time Delayed:
		// target variable TRUE after the set time has elapsed (FALSE on state exit)
		case Type::D:
    	{
			if (!m_edge)
			{
				m_time = millis();
				m_edge = true;
				*m_actionTarget = false;
			}
			uint32_t current_time = millis();
			uint32_t elapsed = (current_time >= m_time) ? (current_time - m_time) : (UINT32_MAX - m_time + current_time + 1);
			if ( elapsed > m_delay && m_edge && m_time > 0)
			{
				*m_actionTarget = true;
				m_time = -1; // Action executed
			}
			break;
      	}

		// Rising Edge
		// target variable TRUE on rising edge
		case Type::RE:
			*m_actionTarget = false;

			if (!m_edge)
			{
				m_edge = true;
				*m_actionTarget = true;
			}
			break;
		}
	}

protected:
	int32_t m_time = -1; // Last call time of Action (-1 not called)
	bool m_edge = false;

	uint8_t m_actionType; // The type of action  { 'N', 'S', 'R', 'L', 'D'}
	bool *m_actionTarget; // The variable wich is affected by action
	uint32_t m_delay;	  // For L - limited time and D - delayed actions
};

#endif