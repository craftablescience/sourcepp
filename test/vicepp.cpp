#include <gtest/gtest.h>

#include <vicepp/vicepp.h>

#include "Helpers.h"

using namespace vicepp;

TEST(vicepp, decrypt) {
	auto encrypted = ::readFile(ASSET_ROOT "vicepp/weapon_pistol.enc");
	auto decrypted = ::readFile(ASSET_ROOT "vicepp/weapon_pistol.dec");

	auto test = decrypt(encrypted, KnownCodes::HL2DM);
	ASSERT_EQ(test.size(), decrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], decrypted[i]);
	}
}

TEST(vicepp, encrypt) {
	auto encrypted = ::readFile(ASSET_ROOT "vicepp/weapon_pistol.enc");
	auto decrypted = ::readFile(ASSET_ROOT "vicepp/weapon_pistol.dec");

	auto test = encrypt(decrypted, KnownCodes::HL2DM);
	ASSERT_EQ(test.size(), encrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], encrypted[i]);
	}
}
