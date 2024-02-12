#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vmfpp {

// A KeyValue object with one key, multiple keyvalues
class Node {
public:
    Node() = default;

    [[nodiscard]] const std::unordered_map<std::string, std::vector<std::string>>& getValues() const;

    [[nodiscard]] std::unordered_map<std::string, std::vector<std::string>>& getValues();

    [[nodiscard]] bool hasValue(const std::string& key) const;

    [[nodiscard]] const std::vector<std::string>& getValue(const std::string& key) const;

    [[nodiscard]] std::vector<std::string>& getValue(const std::string& key);

    void addValue(std::string key, std::string value);

    void removeValue(const std::string& key);

    [[nodiscard]] const std::unordered_map<std::string, std::vector<Node>>& getChildren() const;

    [[nodiscard]] std::unordered_map<std::string, std::vector<Node>>& getChildren();

    [[nodiscard]] bool hasChild(const std::string& key) const;

    [[nodiscard]] const std::vector<vmfpp::Node>& getChild(const std::string& key) const;

    [[nodiscard]] std::vector<vmfpp::Node>& getChild(const std::string& key);

    void addChild(std::string key, Node value);

    void removeChild(const std::string& key);

    [[nodiscard]] std::size_t getNumItems() const;

protected:
    std::unordered_map<std::string, std::vector<std::string>> values;
    std::unordered_map<std::string, std::vector<Node>> children;
};

} // namespace vmfpp
