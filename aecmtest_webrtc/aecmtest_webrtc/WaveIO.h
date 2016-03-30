#ifndef CONFLUENCE_WAVEIO_H
#define CONFLUENCE_WAVEIO_H


#if (defined(WIN32)) || (defined(WP8))

#pragma warning (disable : 4786)
#if (!defined(_WIN32_WINNT)) && (!defined(WP8))
#define _WIN32_WINNT 0x0400
#elif (!defined(_WIN32_WINNT)) && (defined(WP8))
#define _WIN32_WINNT 0x0602
#endif // !defined(_WIN32_WINNT)
#if !defined(POINTER_64) && _MSC_VER > 1200
#define POINTER_64 __ptr64
#endif // !defined(POINTER_64) && _MSC_VER > 1200

//MUST include winsock2.h before windows.h to avoid including winsock.h
#include <winsock2.h>
#include <windows.h>

#include <windef.h>
#include <string>
#include <map>
#include <list>
#include <sstream>

#if _MSC_VER >= 1600
#include <stdint.h>
#else
#include "stdint_msc.h"
#endif


#else

#include <stddef.h>
#include <string.h>
#include <stdint.h>

#endif	// !defined(WIN32)

#include <stdio.h>

struct SWavFileHead 
{
	//Resource Interchange File Flag (0-3) "RIFF"
	char RIFF[4];
	//File Length ( not include 8 bytes from the beginning ) (4-7)
	int32_t FileLength;
	//WAVE File Flag (8-15) "WAVEfmt "
	 char WAVEfmt_[8];
	//Transitory Byte ( normally it is 10H 00H 00H 00H ) (16-19)
    //Format chunk size - 8. (16-19)
	uint32_t fmtSize;
	//Format Category ( normally it is 1 means PCM-u Law ) (20-21)
	int16_t FormatCategory;
	//NChannels (22-23)
	int16_t NChannels;
	//Sample Rate (24-27)
	int32_t SampleRate;
    //l=NChannels*SampleRate*NBitsPersample/8 (28-31)
    int32_t byteRate;
    //i=NChannels*NBitsPersample/8 (32-33)
    int16_t BytesPerSample;
	//NBitsPersample (34-35)
	int16_t NBitsPersample;
	//Data Flag  "data"
	char data[4];
	//Raw Data File Length 
	int32_t RawDataFileLength;
	
};



class CWavFileOp 
{
	private:
		SWavFileHead m_head;
		char *m_pData;
		FILE *m_fp;
        fpos_t m_DATApos;

	public:
		int m_FileStatus;
        
		CWavFileOp(char *name,const char *mode = "r");
		int ReadHeader(SWavFileHead *head);
		int WriteHeader(SWavFileHead head);
        // Data is corresponding to FormatCategory; if FormatCategory =1, Data shoul be short; if FormatCategory == 3, Datao should be float;
        // len is samples of one channel
        // renturn is samples readout
        int ReadSample(unsigned char *Data,int len);
        // goto data position
        int LoopHead();
		int ReadSample(short *Data,int len);
        int ReadFloatSample(float *Data,int len);
		int WriteSample(short *Data,int len );
        int WriteFloatSample(float *Data,int len );
		int UpdateHeader(int outCh,int samplerate);
		int Scan2Data();
        int UpdateHeader_new(int outCh,int samplerate);
		~CWavFileOp();
};
#endif