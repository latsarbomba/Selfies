#include "Crypto.h"

void Crypto::xtea_encipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4])
{
	unsigned int i;
	uint32_t v0 = v[0], v1 = v[1], sum = 0, delta = 0x9E3779B9;
	for (i = 0; i < num_rounds; i++) {
		v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
		sum += delta;
		v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
	}
	v[0] = v0; v[1] = v1;
}

void Crypto::xtea_decipher(unsigned int num_rounds, uint32_t v[2], uint32_t const key[4])
{
	unsigned int i;
	uint32_t v0 = v[0], v1 = v[1], delta = 0x9E3779B9, sum = delta*num_rounds;
	for (i = 0; i < num_rounds; i++) {
		v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
		sum -= delta;
		v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
	}
	v[0] = v0; v[1] = v1;
}

Crypto::Crypto()
{
	key[0] = 0xFDA5;
	key[1] = 0xD54E;
	key[2] = 0xFC00;
	key[3] = 0xB55A;
}

std::vector<uint32_t> Crypto::EncryptString(std::wstring text)
{
	size_t size;
	BYTE* buffer;
	std::vector<uint32_t> ret;
	size_t bytes = text.size() * sizeof(wchar_t);
	auto rest = bytes % BLOCK_SIZE;
	if (rest != 0)
	{
		size = bytes + BLOCK_SIZE - rest;
		buffer = new BYTE[size];
		buffer[bytes] = buffer[bytes + 1] = '\0';
	}
	else
	{
		size = bytes;
		buffer = new BYTE[size];
	}

	memcpy(buffer, text.c_str(), bytes);

	for (int j = 0; j < size / BLOCK_SIZE; j++)
	{
		uint32_t v[2];
		v[0] = *(uint32_t*)(buffer + j*BLOCK_SIZE);
		v[1] = *(uint32_t*)(buffer + j*BLOCK_SIZE + BLOCK_SIZE / 2);

		xtea_encipher(ROUNDS, v, key);

		ret.push_back(v[0]);
		ret.push_back(v[1]);
	}

	delete[] buffer;
	return ret;
}

std::wstring Crypto::decryptString(std::vector<uint32_t> r)
{
	if (r.size() % 2 != 0)
		return L"Error";

	size_t size = BLOCK_SIZE*r.size() / 2;

	std::wstring w;
	w.resize(size);

	BYTE* buffer = new BYTE[size];
	uint32_t v[2];
	for (int i = 0; i < r.size(); i += 2)
	{
		v[0] = r[i];
		v[1] = r[i + 1];

		xtea_decipher(ROUNDS, v, key);

		auto offset = i / 2 * BLOCK_SIZE;
		memcpy(buffer + offset, v, 8);
	}

	memcpy((void*)w.c_str(), buffer, size);

	auto strlen = wcslen(w.c_str());

	std::wstring toRet;
	toRet.resize(strlen);

	memcpy((void*)toRet.c_str(), (void*)w.c_str(), strlen * sizeof(wchar_t));

	delete[] buffer;
	return toRet;
}

std::string Crypto::toString(std::vector<uint32_t> r)
{
	std::string s;
	for (auto b : r)
		s += "\\x" + std::to_string(b);

	return s;
}

std::vector<uint32_t> Crypto::fromString(std::string r)
{
	std::vector<uint32_t> s;
	std::regex pattern("(\\\\x(\\d+))+?");

	std::sregex_iterator begin(r.begin(), r.end(), pattern);
	std::sregex_iterator end;
	std::smatch match;
	for (auto it = begin; it != end; ++it)
	{
		match = *it;
		auto number = match[2].str();
		s.push_back(std::stoul(number));
	}

	return s;
}