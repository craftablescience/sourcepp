#pragma once

#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <iostream>
#include <cstring>

namespace sourcepp::detail {

bool contains(std::string_view s, char c);

void ltrim(std::string& s);

void rtrim(std::string& s);

void trim(std::string& s);

void ltrim(std::string& s, std::string_view c);

void rtrim(std::string& s, std::string_view c);

void trim(std::string& s, std::string_view c);

std::vector<std::string> split(std::string_view s, char delim);

class UtilStringView
{
    std::string owningString;
    std::string_view string;
    bool selfOwning = false;
public:

    UtilStringView(const char* str)
    {
        owningString = str;
        string = std::string_view(owningString);
        selfOwning = true;
    }

    UtilStringView(std::string& str)
    {
        owningString = std::move(str);
        string = std::string_view(owningString);
        selfOwning = true;
    }

    UtilStringView(std::string_view str)
    {
        string = str;
    }

    UtilStringView() = default;

    ~UtilStringView() = default;

    template<typename T>
    explicit UtilStringView(const UtilStringView& t){
        if(t.selfOwning) {
            owningString = t.owningString;
            string = std::string_view(owningString);
        } else
            string = t.string;
    }

    template<typename T>
    explicit UtilStringView(UtilStringView&& t)

    {
        if(t.selfOwning) {
            owningString = std::move(t.owningString);
            string = std::string_view(owningString);
            this->selfOwning = true;
        } else
            string = t.string;
    }

    std::string_view operator ->() const
    {
        return this->string;
    }

    operator std::string_view() const {
        return this->string;
    }

    explicit operator std::string() const {
        {
            if(this->selfOwning)
                return this->owningString;
            return std::string(string);
        }
    }

    bool operator==( const std::string& str ) const
    {
        return string == str;
    }

    bool operator==( const std::string_view& str ) const
    {
        return string == str;
    }

    bool operator==( const char* str) const
    {
        return string == str;
    }

    UtilStringView& operator=(const char* str)
    {
        owningString = str;
        string = owningString;
        selfOwning = true;
        return *this;
    }

    UtilStringView& operator=(std::string str)
    {
        owningString = std::move(str);
        string = owningString;
        selfOwning = true;
        return *this;
    }

    UtilStringView& operator=(std::string_view str)
    {
        string = str;
        return *this;
    }

    UtilStringView& operator<<(const char* str)
    {
        this->append(str);
        return *this;
    }

    UtilStringView& operator<<(std::string_view str)
    {
        this->append(str);
        return *this;
    }

    UtilStringView& operator<<(const std::string& str)
    {
        this->append(str);
        return *this;
    }

    static UtilStringView make_self_owning(std::string_view str)
    {
        UtilStringView selfOwningView = {};
        selfOwningView.owningString = str;
        selfOwningView.string = selfOwningView.owningString;
        selfOwningView.selfOwning = true;
        return selfOwningView;
    }

    static UtilStringView make_view(std::string_view str)
    {
        UtilStringView string_view = {};
        string_view.string = str;
        return string_view;
    }

    static UtilStringView make_view(const std::string& str)
    {
        UtilStringView string_view = {};
        string_view.string = str;
        return string_view;
    }

    static UtilStringView make_view(const char* str)
    {
        UtilStringView string_view = {};
        string_view.string = str;
        return string_view;
    }

    [[nodiscard]] size_t length() const
    {
        return this->string.length();
    }

    [[nodiscard]] const char* data() const
    {
        return this->string.data();
    }

    [[nodiscard]] std::string get_string() const
    {
        return std::string(string);
    }

    [[nodiscard]] bool ends_with(std::string_view str) const
    {
        return this->string.ends_with(str);
    }

    std::string_view& append(const std::string_view & str)
    {
        if(!this->selfOwning) {
            this->owningString = string;
            this->selfOwning = true;
        }
        this->owningString.append(str);
        this->string = std::string_view(owningString);
        return this->string;
    }

    [[nodiscard]] bool isSelfOwning() const
    {
        return selfOwning;
    }

};

} // namespace sourcepp::detail
