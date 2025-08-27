#include <gtest/gtest.h>

#include <sourcepp/FS.h>
#include <vcryptpp/vcryptpp.h>

using namespace sourcepp;
using namespace vcryptpp;

TEST(vcryptpp, vice_encrypt) {
	const auto encrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/weapon_pistol.enc");
	const auto decrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/weapon_pistol.dec");

	const auto test = VICE::encrypt(decrypted);
	ASSERT_EQ(test.size(), encrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], encrypted[i]);
	}
}

TEST(vcryptpp, vice_decrypt) {
	const auto encrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/weapon_pistol.enc");
	const auto decrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/weapon_pistol.dec");

	const auto test = VICE::decrypt(encrypted);
	ASSERT_EQ(test.size(), decrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], decrypted[i]);
	}
}

TEST(vcryptpp, vfont_encrypt) {
	const auto encrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/test.vfont");
	const auto decrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/test.ttf");

	const auto test = VFONT::decrypt(VFONT::encrypt(decrypted));
	fs::writeFileBuffer(ASSET_ROOT "vcryptpp/test_wringer.ttf", test);
	ASSERT_EQ(test.size(), decrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], decrypted[i]);
	}
}

TEST(vcryptpp, vfont_decrypt) {
	const auto encrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/test.vfont");
	const auto decrypted = fs::readFileBuffer(ASSET_ROOT "vcryptpp/test.ttf");

	const auto test = VFONT::decrypt(encrypted);
	ASSERT_EQ(test.size(), decrypted.size());

	for (int i = 0; i < test.size(); i++) {
		EXPECT_EQ(test[i], decrypted[i]);
	}
}
