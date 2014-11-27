/*
 * endian detect and convert routines
 * proguru
 * July 21,2008
 */

#ifndef __ENDIAN_H__
#define __ENDIAN_H__
/*
typedef union uEndianDetect{
	struct{
  		bool bLittleEndian;
  		bool bFill[3];
 	};
	long lValue;
}EndianDetect;

inline bool IsLittleEndian(){
	EndianDetect endian={(long)1};
	return endian.bLittleEndian;
}*/
inline bool IsLittleEndian(){
	int n=1;
	bool* pb=(bool*)&n;
	return *pb;
}

inline void ConvertEndian16(LPVOID lpMem){
	BYTE* p = (BYTE*)lpMem;
	p[0] 	= p[0] ^ p[1];
	p[1] 	= p[0] ^ p[1];
	p[0] 	= p[0] ^ p[1];
}

inline void ConvertEndian32(LPVOID lpMem){
	BYTE* p = (BYTE*)lpMem;
	p[0] 	= p[0] ^ p[3];
	p[3] 	= p[0] ^ p[3];
	p[0] 	= p[0] ^ p[3];
	p[1] 	= p[1] ^ p[2];
	p[2] 	= p[1] ^ p[2];
	p[1] 	= p[1] ^ p[2];
}

inline void ConvertEndian64(LPVOID lpMem){
	BYTE* p = (BYTE*)lpMem;
	p[0] 	= p[0] ^ p[7];
	p[7] 	= p[0] ^ p[7];
	p[0] 	= p[0] ^ p[7];

	p[1] 	= p[1] ^ p[6];
	p[6] 	= p[1] ^ p[6];
	p[1] 	= p[1] ^ p[6];

	p[2] 	= p[2] ^ p[5];
	p[5] 	= p[2] ^ p[5];
	p[2] 	= p[2] ^ p[5];

	p[3] 	= p[3] ^ p[4];
	p[4] 	= p[3] ^ p[4];
	p[3] 	= p[3] ^ p[4];
}

#endif //__ENDIAN_H__
