// ********************************************************************
// "We can verify this by running the following code on machines of 
// different architectures (a 16 bit short is assumed):"

short *ptr;
char array[2];
ptr = (short *)array;

*ptr = 0x1234;

printf("First value: %02X\n", (unsigned)array[0]);
printf("2nd   value: %02X\n", (unsigned)array[1]);


// ********************************************************************
// "The hex value 123416 if stored to a file would be written as the 
// bytes 34 12 by a little endian machine. This may be confirmed by using 
// the following code snippet to create the file:"

int fd = open("test", O_RDWR, 0);
short value = 0x1234;

write(fd, &value, 2);
close(fd);


// ********************************************************************
// "to read the value back into memory and display the results:"

int fd = open("test", O_RDWR, 0);
unsigned char value;

read(fd, &value, 1);
printf("value1=%02X\n", (unsigned short)value);

read(fd, &value, 1);
printf("value2=%02X\n", (unsigned short)value);
close(fd);

// ********************************************************************
// "That same file if read back on a big endian machine would result 
// in the 16 bit value 341216 being read as demonstrated by the following 
// code:

int fd = open("test", O_RDWR, 0);
unsigned short value;

read(fd, &value, 2);
printf("value=%04X\n", value);
close(fd);


// ********************************************************************

bool MachineLittleEndian()
{
	int value = 1;
	char *p= (char *)&value;
	return *p;
}  

// ********************************************************************

void writeBigEndian(unsigned short value, FILE file)
{
	putc(value >> 8,   file); // write high order byte first
	putc(value & 0xFF, file); // write low order byte next
}

// ********************************************************************

// HelloBits description
class HelloBits {
	unsigned int(8) Bits;
};

class sndSoundStruct {
	char(8) 		magic[4] = ".snd";
	signed int(32)	dataLocation;
	signed int(32)	dataSize;
	signed int(32)	dataFormat;
	signed int(32)	samplingRate;
	signed int(32)	channelCount;
	char(8) 		info[dataLocation-24]; // min size = 4
}

class Chunk {
	signed int(32)	ckID;
	signed int(32)	ckSize;
	char(8)		ckData[ckSize];
}

const signed int FORM_ID=0x464F524D;// "FORM" in big-endian form
const signed int COMM_ID=0x434F4D4D;// "COMM" in big-endian form
const signed int SSND_ID=0x53534E44;// "SSND" in big-endian form

class FormAIFFChunk {
	signed int(32)	ckID = FORM_ID;
	signed int(32)	ckSize;
	signed int(32)	formType;

	do {
		signed int(32)*  chunk;

		switch (chunk) {
		case COMM_ID: 
			CommonChunk cChunk;
			break;
		case SSND_ID:   
			SSNDChunk sChunk;
               	}
		
	} while (chunk == CommonChunk || chunk == SSNDChunk);
}


/* IEEE 80-bit floating point */
class ExtendedFloat {
	bit(1)              sign;
	unsigned int(15)    exponent;
	double(64)          mantissa;
}

class CommonChunk {
	signed int(32)		ckID = COMM_ID;
	signed int(32)		ckSize = 18;
	unsigned int(16)	numChannels;
	unsigned int(32)	numSampleFrames;
	signed int(16)		sampleSize;
	ExtendedFloat		sampleRate;
}

class SSNDChunk {
	signed int(32)	ckID = SSND_ID;
	signed int(32)	ckSize;
	char(8)		data[ckSize];
}

const signed int RIFF_ID	= 0x52494646;// "RIFF" in big-endian form
const signed int FORMAT_ID	= 0x666d7420;// "fmt " in big-endian form
const signed int DATA_ID	= 0x64617461;// "data" in big-endian form
const signed int WAVE_ID	= 0x57415645;// "WAVE" in big-endian form

// Base Chunk
class BaseChunk {
	signed int(32) ckID;
	little signed int(32) ckSize;
}

class RIFFChunk extends BaseChunk {
	// we only handle WAVE files
	signed int(32) fccType = WAVE_ID;
}

class FormatChunk extends BaseChunk {
	little   signed int(16)	formatTag;
	little unsigned int(16) channels;
	little unsigned int(32) samplesPerSec;
	little unsigned int(32) avgBytesPerSec;
	little unsigned int(16) blockAlign;
	little unsigned int(16) bitsPerSample;
}

class DataChunk extends BaseChunk {
	char(8) data[ckSize];
}

