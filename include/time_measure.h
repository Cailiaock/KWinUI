/**
 * mesure elapsed time for code execute
 * Sep 24,2008
 */

#include "windows.h"

class KCodeExecuteTimeMeasure{
	public:
		KCodeExecuteTimeMeasure(){
			//before start measuring,flush cpu cache is necessary! use INVD or WBINVD instruction.
			Start();	
		}

		void Start(){	//get start time
			::GetThreadTimes(::GetCurrentThread(),&m_ftDummy,&m_ftDummy,&m_ftKeTimeStart,&m_ftUserTimeStart);
		}

		__int64 End(){	//get end time
			::GetThreadTimes(::GetCurrentThread(),&m_ftDummy,&m_ftDummy,&m_ftKeTimeEnd,&m_ftUserTimeEnd);

			//get the interval
			m_i64KeTimeElapsed 	=FileTime2int64(m_ftKeTimeEnd)  - FileTime2int64(m_ftKeTimeStart);
			m_i64UserTimeElapsed=FileTime2int64(m_ftUserTimeEnd)- FileTime2int64(m_ftUserTimeStart);

			//the unit of result is 100 nanoseconds
			return m_i64TotalTimeElapsed = m_i64KeTimeElapsed + m_i64UserTimeElapsed;
		}

	private:
		__int64 FileTime2int64(FILETIME& ft){
			__int64 temp;
			memcpy(&temp, &ft, sizeof(FILETIME));
			return temp;
		}

	private:
		FILETIME m_ftKeTimeStart;
		FILETIME m_ftKeTimeEnd;
		FILETIME m_ftUserTimeStart;
		FILETIME m_ftUserTimeEnd;

		FILETIME m_ftDummy;

		__int64  m_i64KeTimeElapsed;
		__int64  m_i64UserTimeElapsed;
		__int64  m_i64TotalTimeElapsed;
};
