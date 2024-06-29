/*
 * C++ implementation of the ICE encryption algorithm.
 *
 * Written by Matthew Kwan - July 1996
 */

#include <IceKey.h>

namespace {

std::uint32_t ice_sbox[4][1024];
constexpr std::int32_t ice_smod[4][4] = {{333, 313, 505, 369}, {379, 375, 319, 391}, {361, 445, 451, 397}, {397, 425, 395, 505}};
constexpr std::int32_t ice_sxor[4][4] = {{0x83, 0x85, 0x9b, 0xcd}, {0xcc, 0xa7, 0xad, 0x41}, {0x4b, 0x2e, 0xd4, 0x33}, {0xea, 0xcb, 0x2e, 0x04}};
constexpr std::uint32_t ice_pbox[32] = {0x00000001, 0x00000080, 0x00000400, 0x00002000, 0x00080000, 0x00200000, 0x01000000, 0x40000000,
										0x00000008, 0x00000020, 0x00000100, 0x00004000, 0x00010000, 0x00800000, 0x04000000, 0x20000000,
										0x00000004, 0x00000010, 0x00000200, 0x00008000, 0x00020000, 0x00400000, 0x08000000, 0x10000000,
										0x00000002, 0x00000040, 0x00000800, 0x00001000, 0x00040000, 0x00100000, 0x02000000, 0x80000000};
constexpr std::int32_t ice_keyrot[16] = {0, 1, 2, 3, 2, 1, 3, 0, 1, 3, 2, 0, 3, 1, 0, 2};

[[nodiscard]] constexpr std::uint32_t gf_mult(std::uint32_t a, std::uint32_t b, std::uint32_t m) {
	std::uint32_t res = 0;
	while (b) {
		if (b & 1) {
			res ^= a;
		}
		a <<= 1;
		b >>= 1;
		if (a >= 256) {
			a ^= m;
		}
	}
	return res;
}

[[nodiscard]] constexpr std::uint32_t gf_exp7(std::uint32_t b, std::uint32_t m) {
	std::uint32_t x;
	if (b == 0) {
		return 0;
	}
	x = ::gf_mult(b, b, m);
	x = ::gf_mult(b, x, m);
	x = ::gf_mult(x, x, m);
	return ::gf_mult(b, x, m);
}

[[nodiscard]] constexpr std::uint32_t ice_perm32(std::uint32_t x) {
	std::uint32_t res = 0;
	const std::uint32_t* pbox = ice_pbox;
	while (x) {
		if (x & 1) {
			res |= *pbox;
		}
		pbox++;
		x >>= 1;
	}
	return res;
}

void ice_sboxes_init() {
	static bool init = false;
	if (init) {
		return;
	}
	init = true;
	for (int i = 0; i < 1024; i++) {
		int col = (i >> 1) & 0xff;
		int row = (i & 0x1) | ((i & 0x200) >> 8);
		std::uint32_t x;
		x = ::gf_exp7(col ^ ice_sxor[0][row], ice_smod[0][row]) << 24;
		ice_sbox[0][i] = ::ice_perm32(x);
		x = ::gf_exp7(col ^ ice_sxor[1][row], ice_smod[1][row]) << 16;
		ice_sbox[1][i] = ::ice_perm32(x);
		x = ::gf_exp7(col ^ ice_sxor[2][row], ice_smod[2][row]) << 8;
		ice_sbox[2][i] = ::ice_perm32(x);
		x = ::gf_exp7(col ^ ice_sxor[3][row], ice_smod[3][row]);
		ice_sbox[3][i] = ::ice_perm32(x);
	}
}

} // namespace

IceKey::IceKey(int n) {
	::ice_sboxes_init();
	if (n < 1) {
		this->size = 1;
		this->rounds = 8;
	} else {
		this->size = n;
		this->rounds = n * 16;
	}
	this->keySchedule = std::unique_ptr<IceSubkey[]>{new IceSubkey[this->rounds]};
}

void IceKey::encrypt(const unsigned char* ptext, unsigned char* ctext) const {
	std::uint32_t l, r;
	l = (((std::uint32_t) ptext[0]) << 24)
	    | (((std::uint32_t) ptext[1]) << 16)
	    | (((std::uint32_t) ptext[2]) << 8) | ptext[3];
	r = (((std::uint32_t) ptext[4]) << 24)
	    | (((std::uint32_t) ptext[5]) << 16)
	    | (((std::uint32_t) ptext[6]) << 8) | ptext[7];
	for (int i = 0; i < this->rounds; i += 2) {
		l ^= ice_f(r, &this->keySchedule[i]);
		r ^= ice_f(l, &this->keySchedule[i + 1]);
	}
	for (int i = 0; i < 4; i++) {
		ctext[3 - i] = r & 0xff;
		ctext[7 - i] = l & 0xff;
		r >>= 8;
		l >>= 8;
	}
}

void IceKey::decrypt(const unsigned char* ctext, unsigned char* ptext) const {
	std::uint32_t l, r;
	l = (((std::uint32_t) ctext[0]) << 24)
	    | (((std::uint32_t) ctext[1]) << 16)
	    | (((std::uint32_t) ctext[2]) << 8) | ctext[3];
	r = (((std::uint32_t) ctext[4]) << 24)
	    | (((std::uint32_t) ctext[5]) << 16)
	    | (((std::uint32_t) ctext[6]) << 8) | ctext[7];
	for (int i = this->rounds - 1; i > 0; i -= 2) {
		l ^= ice_f(r, &this->keySchedule[i]);
		r ^= ice_f(l, &this->keySchedule[i - 1]);
	}
	for (int i = 0; i < 4; i++) {
		ptext[3 - i] = r & 0xff;
		ptext[7 - i] = l & 0xff;
		r >>= 8;
		l >>= 8;
	}
}

std::uint32_t IceKey::ice_f(std::uint32_t p, const IceSubkey* sk) {
	std::uint32_t tl, tr; // Expanded 40-bit values
	std::uint32_t al, ar; // Salted expanded 40-bit values

	// Left half expansion
	tl = ((p >> 16) & 0x3ff) | (((p >> 14) | (p << 18)) & 0xffc00);

	// Right half expansion
	tr = (p & 0x3ff) | ((p << 2) & 0xffc00);

	// Perform the salt permutation
	al = sk->val[2] & (tl ^ tr);
	ar = al ^ tr;
	al ^= tl;

	al ^= sk->val[0]; // XOR with the subkey
	ar ^= sk->val[1];

	// S-box lookup and permutation
	return ice_sbox[0][al >> 10] | ice_sbox[1][al & 0x3ff] | ice_sbox[2][ar >> 10] | ice_sbox[3][ar & 0x3ff];
}

void IceKey::scheduleBuild(std::uint16_t* kb, int n, const std::int32_t* keyRot) {
	for (int i = 0; i < 8; i++) {
		auto kr = keyRot[i];
		IceSubkey* isk = &this->keySchedule[n + i];
		for (auto& k : isk->val) {
			k = 0;
		}
		for (int j = 0; j < 15; j++) {
			auto* curr_sk = &isk->val[j % 3];
			for (int k = 0; k < 4; k++) {
				auto* curr_kb = &kb[(kr + k) & 3];
				int bit = *curr_kb & 1;
				*curr_sk = (*curr_sk << 1) | bit;
				*curr_kb = (*curr_kb >> 1) | ((bit ^ 1) << 15);
			}
		}
	}
}

void IceKey::set(const unsigned char* key) {
	if (this->rounds == 8) {
		std::uint16_t kb[4];
		for (int i = 0; i < 4; i++) {
			kb[3 - i] = (key[i * 2] << 8) | key[i * 2 + 1];
		}
		this->scheduleBuild(kb, 0, ice_keyrot);
		return;
	}
	for (int i = 0; i < this->size; i++) {
		std::uint16_t kb[4];
		for (int j = 0; j < 4; j++) {
			kb[3 - j] = (key[i * 8 + j * 2] << 8) | key[i * 8 + j * 2 + 1];
		}
		this->scheduleBuild(kb, i * 8, ice_keyrot);
		this->scheduleBuild(kb, this->rounds - 8 - i * 8, &ice_keyrot[8]);
	}
}

int IceKey::keySize() const {
	return this->size * 8;
}
