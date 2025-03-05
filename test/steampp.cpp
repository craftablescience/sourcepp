#ifdef SOURCEPP_BUILD_TESTS_EXTRA

#include <gtest/gtest.h>

#include <steampp/steampp.h>

using namespace sourcepp;
using namespace steampp;

TEST(steampp, list_installed_apps) {
	Steam steam;
	ASSERT_TRUE(steam);

	std::cout << "Steam install directory: " << steam.getInstallDir();

	for (auto appID : steam.getInstalledApps()) {
		std::cout << '\n' << steam.getAppName(appID) << " (" << appID << "): " << steam.getAppInstallDir(appID);
	}
	std::cout << std::endl;
}

TEST(steampp, search_for_apps_using_engine) {
	Steam steam;
	ASSERT_TRUE(steam);

	for (auto appID : steam.getInstalledApps()) {
		if (steam.isAppUsingGoldSrcEngine(appID) || steam.isAppUsingSourceEngine(appID) || steam.isAppUsingSource2Engine(appID)) {
			std::cout << steam.getAppName(appID) << " (" << appID << "): " << steam.getAppInstallDir(appID) << '\n';
		}
	}
	std::cout << std::endl;
}

#endif
