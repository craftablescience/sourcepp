#include <gtest/gtest.h>

import steampp;

using namespace steampp;

#if 0

TEST(steampp, list_installed_apps) {
	Steam steam;
	ASSERT_TRUE(steam);

	std::cout << "Steam install directory: " << steam.getInstallDir() << std::endl;

	for (auto appID : steam.getInstalledApps()) {
		std::cout << steam.getAppName(appID) << " (" << appID << "): " << steam.getAppInstallDir(appID) << std::endl;
	}
}

TEST(steampp, search_for_apps_using_engine) {
	Steam steam;
	ASSERT_TRUE(steam);

	for (auto appID : steam.getInstalledApps()) {
		if (steam.isAppUsingSourceEngine(appID) || steam.isAppUsingSource2Engine(appID)) {
			std::cout << steam.getAppName(appID) << " (" << appID << "): " << steam.getAppInstallDir(appID) << std::endl;
		}
	}
}

#endif
