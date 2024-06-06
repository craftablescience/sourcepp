#include <vmfpp/vmfpp.h>

#include <algorithm>
#include <sstream>
#include <fstream>

#include <sourcepp/string/String.h>

using namespace sourcepp;
using namespace vmfpp;

constexpr char CHAR_ENTER_SCOPE = '{';
constexpr char CHAR_EXIT_SCOPE = '}';
constexpr char CHAR_QUOTE = '\"';
constexpr std::string_view CHAR_DISCARD_CUTSET = "{} \r\n";
constexpr std::string_view NODE_ROOT_KEY = "$root";

namespace {

// NOLINTNEXTLINE(*-no-recursion)
bool readNode(std::istream& stream, Node* scope, std::string_view key = NODE_ROOT_KEY) {
	for (std::string line; std::getline(stream, line); ) {
		if (line.empty()) {
			continue;
		}
		// New scope
		else if (string::contains(line, CHAR_ENTER_SCOPE)) {
			// Scope is opened after the key is read
			// There may be situations where there is no key, so we must account for that
			if ((key.empty() && (scope->getChildren().size() + scope->getValues().size()) > 0) || key == NODE_ROOT_KEY) {
				Node newScope;
				if (!readNode(stream, &newScope)) {
					return false;
				}
				scope->addChild(key.data(), newScope);
			}
		}
		// Exit scope
		else if (string::contains(line, CHAR_EXIT_SCOPE)) {
			break;
		}
		// Value
		else if (string::contains(line, CHAR_QUOTE)) {
			auto p = string::split(line, CHAR_QUOTE);
			if (std::count(line.begin(), line.end(), CHAR_QUOTE) == 3) {
				// Multi-line property value, because value quotes aren't closed
				// Read lines until we encounter a closing quote
				while (std::getline(stream, line)) {
					if (string::contains(line, CHAR_QUOTE)) {
						p[3] += '\n' + string::split(line, CHAR_QUOTE)[0];
						break;
					}
					p[3] += '\n' + line;
				}
			}
			scope->addValue(p[1], p[3]);
		}
		// Child
		else {
			Node newScope;
			std::string newKey = line;
			string::trim(newKey, CHAR_DISCARD_CUTSET);
			string::trim(newKey);
			if (!readNode(stream, &newScope, newKey)) {
				return false;
			}
			scope->addChild(newKey, newScope);
		}
	}
	return true;
}

} // namespace

std::optional<VMF> VMF::openMem(std::string_view data) {
	std::istringstream file{data.data()};
	VMF vmf;
	if (!readNode(file, reinterpret_cast<Node*>(&vmf), NODE_ROOT_KEY)) {
		return std::nullopt;
	}
	return vmf;
}

std::optional<VMF> VMF::openFile(std::string_view filePath) {
	std::ifstream file{filePath.data(), std::ios_base::in};
	if (!file.is_open()) {
		return std::nullopt;
	}
	VMF vmf;
	if (!readNode(file, reinterpret_cast<Node*>(&vmf), NODE_ROOT_KEY)) {
		return std::nullopt;
	}
	return vmf;
}

const std::unordered_map<std::string, std::vector<Node>>& VMF::getSections() const {
	return this->children;
}

std::unordered_map<std::string, std::vector<Node>>& VMF::getSections() {
	return this->children;
}

bool VMF::hasSection(std::string_view key) const {
	return this->hasChild(key.data());
}

const std::vector<Node>& VMF::getSection(std::string_view key) const {
	return this->getChild(key.data());
}

std::vector<Node>& VMF::getSection(std::string_view key) {
	return this->getChild(key.data());
}

void VMF::addSection(std::string key, Node value) {
	this->addChild(std::move(key), std::move(value));
}

void VMF::addSection(std::string_view key, Node value) {
	this->addChild(key.data(), std::move(value));
}

void VMF::removeSection(std::string_view key) {
	this->removeChild(key.data());
}
