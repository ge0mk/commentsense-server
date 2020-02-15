#include "sha.hpp"

namespace crypt{
	sha256::sha256(){
		datalen = 0;
		bitlen[0] = 0;
		bitlen[1] = 0;
		state[0] = 0x6a09e667;
		state[1] = 0xbb67ae85;
		state[2] = 0x3c6ef372;
		state[3] = 0xa54ff53a;
		state[4] = 0x510e527f;
		state[5] = 0x9b05688c;
		state[6] = 0x1f83d9ab;
		state[7] = 0x5be0cd19;
	}
	void sha256::update(std::string data){
		for (uint i = 0; i < data.size(); ++i) {
			this->data[datalen] = data[i];
			datalen++;
			if (datalen == 64) {
				transform();
				DBL_INT_ADD(bitlen[0], bitlen[1], 512);
				datalen = 0;
			}
		}
	}
	std::string sha256::final(){
		uchar hash[32];

		uint i = datalen;

		if (datalen < 56) {
			data[i++] = 0x80;

			while (i < 56)
				data[i++] = 0x00;
		}
		else {
			data[i++] = 0x80;

			while (i < 64)
				data[i++] = 0x00;

			transform();
			memset(data, 0, 56);
		}

		DBL_INT_ADD(bitlen[0], bitlen[1], datalen * 8);
		data[63] = bitlen[0];
		data[62] = bitlen[0] >> 8;
		data[61] = bitlen[0] >> 16;
		data[60] = bitlen[0] >> 24;
		data[59] = bitlen[1];
		data[58] = bitlen[1] >> 8;
		data[57] = bitlen[1] >> 16;
		data[56] = bitlen[1] >> 24;
		transform();

		for (i = 0; i < 4; ++i) {
			hash[i] = (state[0] >> (24 - i * 8)) & 0x000000ff;
			hash[i + 4] = (state[1] >> (24 - i * 8)) & 0x000000ff;
			hash[i + 8] = (state[2] >> (24 - i * 8)) & 0x000000ff;
			hash[i + 12] = (state[3] >> (24 - i * 8)) & 0x000000ff;
			hash[i + 16] = (state[4] >> (24 - i * 8)) & 0x000000ff;
			hash[i + 20] = (state[5] >> (24 - i * 8)) & 0x000000ff;
			hash[i + 24] = (state[6] >> (24 - i * 8)) & 0x000000ff;
			hash[i + 28] = (state[7] >> (24 - i * 8)) & 0x000000ff;
		}

		std::string hashStr;
		char s[3];
		for (int i = 0; i < 32; i++) {
			sprintf(s, "%02x", hash[i]);
			hashStr += s;
		}
		return hashStr;
	}
	void sha256::transform(){
		uint a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

		for (i = 0, j = 0; i < 16; ++i, j += 4)
			m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
		for (; i < 64; ++i)
			m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

		a = state[0];
		b = state[1];
		c = state[2];
		d = state[3];
		e = state[4];
		f = state[5];
		g = state[6];
		h = state[7];

		for (i = 0; i < 64; ++i) {
			t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
			t2 = EP0(a) + MAJ(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + t1;
			d = c;
			c = b;
			b = a;
			a = t1 + t2;
		}

		state[0] += a;
		state[1] += b;
		state[2] += c;
		state[3] += d;
		state[4] += e;
		state[5] += f;
		state[6] += g;
		state[7] += h;
	}

	std::string sha256::hashFile(std::string fileName){
		sha256 hash;
		utils::file f(fileName);
		f.open("rb");
		size_t size = f.size();
		size_t hashed = 0;
		while(hashed < size){
			size_t missing = size - hashed;
			size_t s = (missing < 1024*1024) ? missing : 1024*1024;
			std::string data = f.read(s);
			hash.update(data);
			hashed += s;
		}
		f.close();
		return hash.final();
	}
	
	std::string sha256::hash(std::string data){
		sha256 hash;
		hash.update(data);
		return hash.final();
	}
}
