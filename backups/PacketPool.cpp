/**********************************************************
 *  \file PacketPool.cpp
 *  \brief
 *  \note	注意事项： 
 * 
 * \version 
 * * \author Allen.L
 **********************************************************/

#include "PacketPool.h"

PacketBuffer::PacketBuffer() :
		m_pool(NULL), total_len(0) {
	http_body.first = 0;
	http_body.second = NULL;
	analysis_pos.first = 0;
	analysis_pos.second = NULL;
	end_pos.first = 0;
	end_pos.second = NULL;
	m_state = H_START;
	m_rlen = 0;
}

PacketBuffer::~PacketBuffer() {
	m_pool->handBackBuffer(this);
}

void PacketBuffer::req(char* &buf, int &size, int& len) {
	size = BUFFER_SIZE - (end_pos.second - buflist.back()); //构建时保证已分配一块
	buf = end_pos.second;
	len = total_len;
}

//二次申请使用
bool PacketBuffer::reqNew(char* &buf, int &size) {
	char* freebuf = m_pool->applyFree();
	if (freebuf == NULL) {
		return false;
	}
	buflist.push_back(freebuf);
	end_pos.first = buflist.size() - 1;
	end_pos.second = freebuf;
	buf = end_pos.second;
	size = BUFFER_SIZE;
	return true;
}

void PacketBuffer::accept(char* buf, int len) {
	end_pos.second = buf;
	total_len = len;
}

void PacketBuffer::pprint() {
	for (deque<char *>::iterator iter = buflist.begin(); iter != buflist.end(); iter++) {
		if (*iter != buflist.back()) {
			fwrite(*iter, BUFFER_SIZE, 1, stdout);
		} else {
			fwrite(*iter, end_pos.second - (*iter), 1, stdout);
		}
	}
	printf("\n");
}

void PacketBuffer::getBodySection(FieldPointer& section) {
	deque<char *>::iterator buf_iter = buflist.begin();
	for (uint32 i = http_body.first; i < buflist.size(); i++) {
		char* pS = buflist[i];
		char* pE = pS + BUFFER_SIZE;
		if (i == http_body.first) {
			pS = http_body.second;
		}
		if (i == end_pos.first) {
			pE = end_pos.second;
		}
		section.push_back(pair<char*, char*>(pS, pE));
	}
}

//报文结构分析
int PacketBuffer::anlaysisPacket() {
	if (m_state == H_START) {
		m_state = H_PACKET_LEN;
		m_rlen = 6;
		analysis_pos.first = 0;
		analysis_pos.second = buflist.front();
	}
	char* &pos = analysis_pos.second;
	uint32 si = analysis_pos.first;
	char* &epos = end_pos.second;
	char* edge = buflist[si] + BUFFER_SIZE;
	while (1) {
		if (pos >= edge) {
			si++;
			if (buflist.size() == si) { //end_pos位置错误
				return -1;
			} else {
				pos = buflist[si];
				edge = buflist[si] + BUFFER_SIZE;
			}
		}

		if (m_state == H_PACKET_LEN) {
			rlen_buf[6 - m_rlen] = *pos;
			m_rlen--;
			if (m_rlen == 0) {
				rlen_buf[6] = '\0';
				m_rlen = atoi(rlen_buf);
				m_state = H_PACKET_BODY_START;
			}
		} else if (m_state == H_PACKET_BODY_START) {
			http_body.first = si;
			http_body.second = pos;
			m_state = H_PACKET_BODY;
			m_rlen--;
		} else if (m_state == H_PACKET_BODY) {
			m_rlen--;
		} else { //无效状态
			return -2;
		}

		if (m_state == H_PACKET_BODY && m_rlen <= 0) {
			return 0;
		}

		pos++;
		if (pos == epos) {
			break;
		}
	}
	return 1;
}

//------------  PacketPool  --------
PacketPool::PacketPool(int maxlen) {
	m_max_len = maxlen;
	m_buffer = new char[m_max_len * BUFFER_SIZE + 1];
	for (int i = 0; i < maxlen; i++) {
		m_qfree.push(i);
	}

	memset(m_buffer, 'A', m_max_len * BUFFER_SIZE);
	*(m_buffer + m_max_len * BUFFER_SIZE) = 0;
}

PacketPool::~PacketPool() {
	delete[] m_buffer;
}

//首次申请使用
PacketBuffer* PacketPool::reqBuffer() {
	char* freebuf = applyFree();
	if (freebuf == NULL) {
		return NULL;
	}
	PacketBuffer* buf = new PacketBuffer();
	buf->m_pool = this;
	buf->end_pos.first = 0;
	buf->end_pos.second = freebuf;
	buf->buflist.push_back(buf->end_pos.second);
	return buf;
}

void PacketPool::handBackBuffer(PacketBuffer* buf) {
	boost::mutex::scoped_lock lock(mtx_qfree);
	while (!buf->buflist.empty()) {
		char* p = buf->buflist.front();
		int no = (p - m_buffer) / BUFFER_SIZE;
		m_qfree.push(no);
		buf->buflist.pop_front();
	}
}

char* PacketPool::applyFree() {
	boost::mutex::scoped_lock lock(mtx_qfree);
	if (m_qfree.size() <= 0) {
		return NULL;
	}
	int no = m_qfree.front();
	m_qfree.pop();
	return (m_buffer + no * BUFFER_SIZE);
}

void PacketPool::pprint() {
	fwrite(m_buffer, m_max_len * BUFFER_SIZE, 1, stdout);
	printf("\n");
}
