#include <gtest/gtest.h>

#include <sourcepp/FS.h>
#include <vicepp/vicepp.h>

using namespace sourcepp;
using namespace vicepp;

TEST(vicepp, decrypt) {
	auto encrypted = fs::readFileBuffer(ASSET_ROOT "vicepp/weapon_pistol.enc");
	auto decrypted = fs::readFileBuffer(ASSET_ROOT "vicepp/weapon_pistol.dec");

	auto test = decrypt(encrypted);
	ASSERT_EQ(test.size(), decrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], decrypted[i]);
	}
}

TEST(vicepp, encrypt) {
	auto encrypted = fs::readFileBuffer(ASSET_ROOT "vicepp/weapon_pistol.enc");
	auto decrypted = fs::readFileBuffer(ASSET_ROOT "vicepp/weapon_pistol.dec");

	auto test = encrypt(decrypted);
	ASSERT_EQ(test.size(), encrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], encrypted[i]);
	}
}
