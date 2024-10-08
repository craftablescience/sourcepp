#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <sourcepp/math/Vector.h>

namespace gamepp {

class GameInstance {
public:
	/**
	 * Find a running instance of a Source engine game. Note that this function will only ever
	 * succeed on Windows, because Valve never bothered to expose this functionality on Linux.
	 * @param windowNameOverride The classname of the window to search for, e.g. "Valve001".
	 *                           If unspecified it will check for "Valve001" and "Strata001".
	 * @return A GameInstance if a game is located.
	 */
	[[nodiscard]] static std::optional<GameInstance> find(std::string_view windowNameOverride = "");

	/**
	 * Get the human-readable window title.
	 * @return The window title.
	 */
	[[nodiscard]] std::string getWindowTitle() const;

	/**
	 * Get the window position on-screen.
	 * @return The window position.
	 */
	[[nodiscard]] sourcepp::math::Vec2i getWindowPos() const;

	/**
	 * Get the window size on-screen.
	 * @return The window size.
	 */
	[[nodiscard]] sourcepp::math::Vec2i getWindowSize() const;

	/**
	 * Send a command to the engine (ran as if it was entered into the console by the user).
	 * @param command The command to run.
	 * @return This GameInstance.
	 */
	const GameInstance& command(std::string_view command) const; // NOLINT(*-use-nodiscard)

	/**
	 * Begin "pressing" an input such as forward or left.
	 * @param input The input to run (without the plus sign prefix).
	 * @return This GameInstance.
	 */
	const GameInstance& inputBegin(std::string_view input) const; // NOLINT(*-use-nodiscard)

	/**
	 * End "pressing" an input such as forward or left.
	 * @param input The input to run (without the minus sign prefix).
	 * @return This GameInstance.
	 */
	const GameInstance& inputEnd(std::string_view input) const; // NOLINT(*-use-nodiscard)

	/**
	 * Begin and end "pressing" an input in one tick, like tapping the use key.
	 * @param input The input to run (without any prefix).
	 * @return This GameInstance.
	 */
	const GameInstance& inputOnce(std::string_view input) const; // NOLINT(*-use-nodiscard)

	/**
	 * Begin and end "pressing" an input in the given timespan, like holding the use key.
	 * @param input The input to run (without any prefix).
	 * @param sec The time to hold the input for.
	 * @return This GameInstance.
	 */
	const GameInstance& inputHold(std::string_view input, double sec) const; // NOLINT(*-use-nodiscard)

	/**
	 * Sleep on the current thread for the given number of seconds.
	 * @param sec The number of seconds.
	 * @return This GameInstance.
	 */
	const GameInstance& wait(double sec) const; // NOLINT(*-use-nodiscard)

protected:
	GameInstance() = default;

	void* hwnd;
};

} // namespace gamepp
