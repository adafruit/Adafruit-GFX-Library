#include <stdio.h>
#include <new>
#include <string.h>

//#define TESTING
#define PREVIEW_SIZE 200
class cArray {
public:
	cArray() {
		m_refCnt = 0;
		m_parent = NULL;
		m_begin = m_pos = m_end = NULL;
		m_bitPos = 7;
		m_ones = 0;
		m_manageData = true;
	}
	const cArray(const long c_size) {
		new (this) cArray();
		if(c_size >= 0) {
			m_data = (unsigned char(*)[PREVIEW_SIZE])new unsigned char[c_size];
			AddRef();
			m_begin = (unsigned char*)&m_data[0];
			m_end = m_begin + c_size - 1;
			m_pos = m_begin;
			m_bitPos = 7;
			m_ones = 0;
//			for(int i=0;i<c_size;i++) (*m_data)[i] = i;
		}
	}
	const cArray(const cArray& parent, const long c_size) {
		new (this) cArray();
		m_parent = const_cast<cArray*>(&parent);
		m_pos = parent.m_pos;
		m_bitPos = parent.m_bitPos;
		m_ones = parent.m_ones;
		m_begin = parent.m_pos;
		if(parent.m_end > parent.m_pos + c_size - 1) {
			m_end = parent.m_pos + c_size - 1;
		} else {
			m_end = parent.m_end;
		}
		m_data = parent.m_data;
		AddRef();
	}
	const cArray(const unsigned char *parent, const long c_size, const bool manageData = false) {
		new (this) cArray();
		m_manageData = manageData;
		m_parent = NULL;
		move2cArray(parent, c_size);
		AddRef();
	}
	const int AddRef() {
		return m_refCnt++;
	}
	const int RelRef ()
	{ 
		if (--m_refCnt == 0) {
			if(!m_parent) {
#ifdef TESTING
				printf("Garbage-collecting cArray\n");
#endif
				delete this;
			} else {
#ifdef TESTING
				printf("cArray has parrent !\n");
#endif
				m_refCnt++;
//				throw "cArray has parrent !\n";
			}
			return 0;
		}
		return m_refCnt;
	}
	void set2(const unsigned char val, const bool rest = false) {
		if(rest) {
			if(m_end >= m_pos)
				memset(m_pos, val, m_end - m_pos + 1);
		} else {
			if(m_end >= m_begin)
				memset(m_begin, val, m_end - m_begin + 1);
		}
	}
	const bool addChar(const unsigned char c) {
		if(m_pos <= m_end) {
			*m_pos++ = c;
			return true;
		}
		return false;
	}
	const char countOnes() {
		unsigned char map = *m_pos, ones = 0;
		while(map) { // count bytes used
			if(map & 1) ones++;
			map >>= 1;
		}
		return ones;
	}
	const bool addBitMappedByte(const unsigned char b) {
		if(!b) {
			addBit(0);
			if(m_bitPos == 7) {
				m_pos += 1 + m_ones;
				m_ones = 0;
			}
//			printf("+0 %i %i %i\n", *m_pos, m_pos - m_begin, b);
			return m_pos <= m_end;
		} // else part - b != 0 next
		addBit(1);
		if(m_pos+m_ones > m_end) { // out of buffer - put zero to prevent reading past end
			m_bitPos++;
			addBit(0);
			m_ones--;
			return false;
		}
//		printf("+1 %i %i %i\n", *m_pos, m_pos - m_begin, b);
		*(m_pos+m_ones) = b;
		if(m_bitPos == 7) {
			m_pos += m_ones + 1;
			m_ones = 0;
		}
		return m_pos <= m_end;
	}
	const long getBytesUsed() {
		if(m_bitPos == 7) {
			return m_pos - m_begin;
		} else {
			return m_pos - m_begin + 1 + m_ones;
		}
	}
	const bool shiftPos(const long number) {
		if(m_pos + number > m_end) {
#ifdef TESTING
			printf("cArray move past end !\n");
#endif
			m_pos = m_end + 1;
			return false;
		} else if(m_pos + number < m_begin) {
#ifdef TESTING
			printf("cArray move past end !\n");
#endif
			m_pos = m_begin - 1;
			return false;
		} else {
			m_pos += number;
			return true;
		}
	}
	const bool setPos(const unsigned char *pos) {
		int bug;
		if(!(bug = checkPos(pos))) {
			m_pos = const_cast<unsigned char *>(pos);
			m_bitPos = 7;
			return true;
		}
#ifdef TESTING
			printf("cArray setPos %i !\n", bug);
#endif
		return false;
	}
	const bool setBitPos(const unsigned char pos) {
		int bug;
		if(pos <= 7 && !(bug = checkPos(m_pos))) {
			m_bitPos = pos;
			m_ones = countOnes();
			return true;
		}
#ifdef TESTING
			printf("cArray setPos %i !\n", bug);
#endif
		return false;
	}
	const bool setPos(const long pos) {
		int bug;
		if(!(bug = checkPos(m_begin + pos))) {
			m_pos = m_begin + pos;
			m_bitPos = 7;
			return true;
		}
#ifdef TESTING
			printf("cArray setPos %i !\n", bug);
#endif
		return false;
	}
	const int checkPos(const unsigned char *pos) const {
		if(pos < m_begin) {
			const_cast<unsigned char *>(m_pos) = m_begin - 1; // ev. fix, should report error when it was less
			return -1;
		} else if(pos > m_end) {
			const_cast<unsigned char *>(m_pos) = m_end + 1; // ev. fix, should report error when it was more
			return 1;
		}
		return 0;
	}
	const unsigned char operator * () const {
		int bug;
		if(!(bug = checkPos(m_pos))) {
			return *m_pos;
		} else {
#ifdef TESTING
			printf("cArray read out of array %i !\n", bug);
#endif
			throw "cArray read out of array !\n";
		}
	}
	const bool copyData(const long shiftDest, const long shiftSrc, const unsigned int size) {
		int bug;
		unsigned char *src = m_pos + shiftSrc, *dest = m_pos + shiftDest;
		if(!(bug = checkPos(src)))
			if(!(bug = checkPos(dest)))
				if(!(bug = checkPos(src + size - 1)))
					if(!(bug = checkPos(dest + size - 1))) {
						memcpy(dest, src, size);
						return true;
					}
#ifdef TESTING
		printf("cArray copyData out of array !\n");
#endif
		throw "cArray copyData out of array !\n";
		return false;
	}
	const unsigned char *getPos() const {
		return m_pos;
	}
	const unsigned long Capacity() const {
		return m_end - m_begin;
	}
	const unsigned char getBitPos() const {
		return m_bitPos;
	}
	const bool moveWindow(const long shift) {
		int bug;
		unsigned char *begin = m_begin + shift, *end = m_end + shift;
		if(!(bug = m_parent->checkPos(begin)))
			if(!(bug = m_parent->checkPos(end))) {
				m_begin = begin;
				m_end = end;
				m_pos = begin;
				return true;
			}
#ifdef TESTING
		printf("cArray moveWindow out of parent %i !\n", bug);
#endif
		throw "cArray moveWindow out of parent !\n";
		return false;
	}
	const bool setData(const unsigned char char2Bset) {
		int bug;
		if(!(bug = checkPos(m_pos))) {
			*m_pos = char2Bset;
			return true;
		}
#ifdef TESTING
		printf("cArray setData out of array %i !\n", bug);
#endif
		throw "cArray setData out of array !\n";
	}
	const int isInvalid() const {
		return checkPos(m_pos);
	}
	const unsigned char *end() const {
		return m_end;
	}
	const long getRelPos() const {
		return (long)(m_pos - m_begin);
	}

protected:
	const bool addBit(const bool b) { // m_pos not adjusted => should not be used outside
		if(m_pos <= m_end) {
			if(m_bitPos < 8) {
				unsigned char bitMask = 1 << m_bitPos;
				if(b) {
					*m_pos |= bitMask;
					m_ones++;
				} else {
					*m_pos &= (255-bitMask);
				}
				if(!m_bitPos) {
					m_bitPos = 8;
				}
				m_bitPos--;
			}
			return true;
		}
		return false;
	}
	void move2cArray(const unsigned char *parent, const long c_size) {
		m_pos = const_cast<unsigned char *>(parent);
		m_begin = const_cast<unsigned char *>(parent);
		m_end = m_pos + c_size - 1;
		m_data = (unsigned char(*)[PREVIEW_SIZE])parent;
	}

	cArray *m_parent;
	unsigned char *m_pos;
	unsigned char m_bitPos;
	unsigned char m_ones;
	unsigned char *m_begin;
	unsigned char *m_end;
	unsigned char (*m_data)[PREVIEW_SIZE];
	~cArray() {
		if((m_data && !m_parent) && m_manageData) {
			delete[] m_data;
			m_data = NULL;
		}
	};
private:
	int m_refCnt;
	bool m_manageData;
};

class cArrHolder {
public:
	cArrHolder() {
		m_ptr = NULL;
	}
	cArrHolder(const long size) {
		new (this) cArrHolder();
		m_ptr = new cArray(size);
	}
	cArrHolder(const cArray& parent, const long c_size) {
		m_ptr = new cArray(parent, c_size);
	}
	cArrHolder(const unsigned char* binArray, const long size, const bool manageData = false) {
		m_ptr = new cArray(binArray, size, manageData);
	}
	~cArrHolder() {
		m_ptr->RelRef();
	}
	cArray& operator * () const
	{
#ifdef TESTING
		if (!m_ptr)
			printf("cArray attempt to use NULL pointer. !\n");
#endif
		return *m_ptr;
	}
	const bool addChar(const unsigned char c) { return m_ptr->addChar(c); }
	const cArrHolder& operator += (const long number) const {
		m_ptr->shiftPos(number);
		return *this;
	}
	const cArray& operator ++() const { // boundary check should be there too - max. 1 past end
		m_ptr->setPos(m_ptr->getPos() + 1);
		return *m_ptr;
	}
	const cArray& operator --() const { // boundary check should be there too - min. 1 before begin
		m_ptr->setPos(m_ptr->getPos() - 1);
		return *m_ptr;
	}

protected:
	cArray *m_ptr;
};
#undef TESTING
