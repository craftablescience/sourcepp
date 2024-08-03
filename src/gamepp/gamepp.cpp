#include <gamepp/gamepp.h>

#include <string>
#include <thread>

using namespace gamepp;
using namespace sourcepp;

// All this stuff is very Windows-dependent, sorry!
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::optional<GameInstance> GameInstance::find(std::string_view windowNameOverride) {
	GameInstance instance;

	if (!windowNameOverride.empty()) {
		instance.hwnd = FindWindowA(windowNameOverride.data(), nullptr);
	} else {
		instance.hwnd = FindWindowA("Valve001", nullptr);
		if (!instance.hwnd) {
			instance.hwnd = FindWindowA("Strata001", nullptr);
		}
		// are any other names used in other branches known?
	}

	if (!instance.hwnd) {
		return std::nullopt;
	}
	return instance;
}

std::string GameInstance::getWindowTitle() const {
	// This should be large enough
	std::string title(512, '\0');
	if (auto size = GetWindowTextA(reinterpret_cast<HWND>(this->hwnd), title.data(), title.length())) {
		title.resize(size);
		return title;
	}
	return "";
}

math::Vec2i GameInstance::getWindowPos() const {
	RECT rect;
	GetWindowRect(reinterpret_cast<HWND>(this->hwnd), &rect);
	return {rect.left, rect.top};
}

sourcepp::math::Vec2i GameInstance::getWindowSize() const {
	RECT rect;
	GetWindowRect(reinterpret_cast<HWND>(this->hwnd), &rect);
	return {rect.right - rect.left, rect.bottom - rect.top};
}

const GameInstance& GameInstance::command(std::string_view command) const {
	COPYDATASTRUCT data;
	data.dwData = 0;
	data.cbData = command.length() + 1;
	data.lpData = reinterpret_cast<void*>(const_cast<char*>(command.data()));
	SendMessageTimeoutA(reinterpret_cast<HWND>(this->hwnd), WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&data), SMTO_ABORTIFHUNG, 0, nullptr);
	return *this;
}

#else

std::optional<GameInstance> GameInstance::find(std::string_view) {
	return std::nullopt;
}

std::string GameInstance::getWindowTitle() const {
	return "";
}

math::Vec2i GameInstance::getWindowPos() const {
	return {};
}

math::Vec2i GameInstance::getWindowSize() const {
	return {};
}

const GameInstance& GameInstance::command(std::string_view) const {
	return *this;
}

#endif

const GameInstance& GameInstance::inputBegin(std::string_view input) const {
	this->command("+" + std::string{input});
	return *this;
}

const GameInstance& GameInstance::inputEnd(std::string_view input) const {
	this->command("-" + std::string{input});
	return *this;
}

const GameInstance& GameInstance::inputOnce(std::string_view input) const {
	this->inputBegin(input).inputEnd(input);
	return *this;
}

const GameInstance& GameInstance::inputHold(std::string_view input, double sec) const {
	this->inputBegin(input).wait(sec).inputEnd(input);
	return *this;
}

const GameInstance& GameInstance::wait(double sec) const {
	std::this_thread::sleep_for(std::chrono::duration<double>(sec));
	return *this;
}
