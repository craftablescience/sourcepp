#include <gtest/gtest.h>

#include <sourcepp/FS.h>
#include <vcryptpp/VICE.h>

using namespace sourcepp;
using namespace vcryptpp;

TEST(vcryptpp, decrypt) {
	auto encrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/weapon_pistol.enc");
	auto decrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/weapon_pistol.dec");

	auto test = VICE::decrypt(encrypted);
	ASSERT_EQ(test.size(), decrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], decrypted[i]);
	}
}

TEST(vcryptpp, encrypt) {
	auto encrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/weapon_pistol.enc");
	auto decrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/weapon_pistol.dec");

	auto test = VICE::encrypt(decrypted);
	ASSERT_EQ(test.size(), encrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], encrypted[i]);
	}
}
