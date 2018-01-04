#pragma once
#include <Windows.h>
#include <vector>
#include <stdint.h>
#include <regex>

#define ROUNDS 10

class Crypto
{
private:
	unsigned int key[4];

	const int BLOCK_SIZE = 8;

	void xtea_encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);

	void xtea_decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4]);

	std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
public:
	Crypto();

	std::vector<uint32_t> EncryptString(std::wstring text);

	std::wstring decryptString(std::vector<uint32_t> r);

	std::string toString(std::vector<uint32_t> r);

	std::vector<uint32_t> fromString(std::string r);

	std::string base64_encode(const std::string &in) {

		std::string out;

		int val = 0, valb = -6;
		for (char c : in) {
			val = (val << 8) + c;
			valb += 8;
			while (valb >= 0) {
				out.push_back(base64_chars[(val >> valb) & 0x3F]);
				valb -= 6;
			}
		}
		if (valb>-6) out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
		while (out.size() % 4) out.push_back('=');
		return out;
	}

	std::string base64_decode(const std::string &in) {

		std::string out;

		std::vector<int> T(256, -1);
		for (int i = 0; i<64; i++) T[base64_chars[i]] = i;

		int val = 0, valb = -8;
		for (char c : in) {
			if (T[c] == -1) break;
			val = (val << 6) + T[c];
			valb += 6;
			if (valb >= 0) {
				out.push_back(char((val >> valb) & 0xFF));
				valb -= 8;
			}
		}
		return out;
	}
};

