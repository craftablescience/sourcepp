#pragma once

#include <optional>

#include "structs/Node.h"

namespace vmfpp {

namespace DefaultSections {

constexpr std::string_view VERSIONINFO = "versioninfo";
constexpr std::string_view VISGROUPS = "visgroups";
constexpr std::string_view VIEWSETTINGS = "viewsettings";
constexpr std::string_view WORLD = "world";
constexpr std::string_view ENTITY = "entity";
constexpr std::string_view CAMERAS = "cameras";
constexpr std::string_view CORDON = "cordon"; // Pre-L4D
constexpr std::string_view CORDONS = "cordons"; // Post-L4D
constexpr std::string_view HIDDEN = "hidden";

} // namespace DefaultSections

// A KeyValue object with no key, only children
class VMF : protected Node {
public:
	static std::optional<VMF> openMem(std::string_view data);

	static std::optional<VMF> openFile(std::string_view filePath);

	[[nodiscard]] const std::unordered_map<std::string, std::vector<Node>>& getSections() const;

	[[nodiscard]] std::unordered_map<std::string, std::vector<Node>>& getSections();

	[[nodiscard]] bool hasSection(std::string_view key) const;

	[[nodiscard]] const std::vector<vmfpp::Node>& getSection(std::string_view key) const;

	[[nodiscard]] std::vector<vmfpp::Node>& getSection(std::string_view key);

	void addSection(std::string key, Node value);

	void addSection(std::string_view key, Node value);

	void removeSection(std::string_view key);

protected:
	VMF() = default;
};

} // namespace vmfpp
