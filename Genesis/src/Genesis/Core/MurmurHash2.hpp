// MurmurHash2A, by Austin Appleby

// This is a variant of MurmurHash2 modified to use the Merkle-Damgard 
// construction. Bulk speed should be identical to Murmur2, small-key speed 
// will be 10%-20% slower due to the added overhead at the end of the hash.

// This variant fixes a minor issue where null keys were more likely to
// collide with each other than expected, and also makes the function
// more amenable to incremental implementations.

#pragma once

#include <stdint.h>

#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

namespace Genesis
{
	class MurmurHash2
	{
	public:

		void begin(uint32_t seed = 0)
		{
			m_hash = seed;
			m_tail = 0;
			m_count = 0;
			m_size = 0;
		}

		void addData(const unsigned char* data, int len)
		{
			m_size += len;

			mixTail(data, len);

			while (len >= 4)
			{
				uint32_t k = *(uint32_t*)data;

				mmix(m_hash, k);

				data += 4;
				len -= 4;
			}

			mixTail(data, len);
		}

		template<typename T>
		void add(T value)
		{
			this->addData((const unsigned char*)&value, sizeof(T));
		};

		uint32_t end(void)
		{
			mmix(m_hash, m_tail);
			mmix(m_hash, m_size);

			m_hash ^= m_hash >> 13;
			m_hash *= m;
			m_hash ^= m_hash >> 15;

			return m_hash;
		}

	private:

		static const uint32_t m = 0x5bd1e995;
		static const int r = 24;

		void mixTail(const unsigned char * & data, int & len)
		{
			while (len && ((len < 4) || m_count))
			{
				m_tail |= (*data++) << (m_count * 8);

				m_count++;
				len--;

				if (m_count == 4)
				{
					mmix(m_hash, m_tail);
					m_tail = 0;
					m_count = 0;
				}
			}
		}

		uint32_t m_hash;
		uint32_t m_tail;
		uint32_t m_count;
		uint32_t m_size;
	};
}