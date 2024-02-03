#pragma once

#include <string>

namespace SmallBasic {

class Obj {
public:
    class Substr {
    public:
        size_t index = std::string::npos;
        size_t length = 0;
        std::string str;

        Substr(size_t index, size_t length, std::string const& str):
            index(index), length(length), str(str) {}
        Substr() {}
    };

private:
    static Substr substr_until(size_t start, std::string const& str, char c) {
        std::string search_list = "\\" + std::string(&c, 1);
        size_t idx = start;
        while (true) {
            idx = str.find_first_of(search_list, idx);
            if (idx == std::string::npos) return Substr();
            else if (str[idx] == '\\') {
                idx += 2;
                continue;
            }
            else break;
        }
        Substr substr(start, idx-start, str.substr(start, idx-start));
        return substr;
    }

    static std::string escape(std::string const& in) {
        std::string out;
        size_t last_idx = 0, idx = 0;
        while ((idx = in.find_first_of(";=\\", idx)) != std::string::npos) {
            out += in.substr(last_idx, idx-last_idx);
            out += "\\" + std::string(&in[idx], 1);
            idx += 1;
            last_idx = idx;
        }
        out += in.substr(last_idx);
        return out;
    }
    static std::string unescape(std::string const& in) {
        std::string out;
        size_t last_idx = 0, idx = 0;
        while ((idx = in.find('\\', idx)) != std::string::npos) {
            out += in.substr(last_idx, idx-last_idx);
            idx += 2;
            last_idx = idx;
            if ((idx-1) < in.size()) {
                out += in[idx-1];
            }
            else {
                break;
            }
        }
        out += in.substr(last_idx);
        return out;
    }

    std::vector<Obj> associated_objects;
    Obj *owner = nullptr;
    std::string owner_key;
    std::string str;
public:
    bool is_number(double *res = nullptr) const {
        try {
            size_t idx;
            double num = std::stod(this->str, &idx);
            if (idx == this->str.size()) {
                if (res != nullptr) {
                    *res = num;
                }
                return true;
            }
            return false;
        }
        catch (std::invalid_argument err) {
            return false;
        }
        catch (std::out_of_range err) {
            //FIXME: correct response?
            return false;
        }
    }
    bool array_iter(size_t &idx, Substr &key, Substr &value) const {
        auto arr_key = substr_until(idx, this->str, '=');
        if (arr_key.index == std::string::npos) return false;
        idx += arr_key.length + 1;
        arr_key.str = unescape(arr_key.str);

        auto arr_value = substr_until(idx, this->str, ';');
        if (arr_value.index == std::string::npos) return false;
        idx += arr_value.length + 1;
        arr_value.str = unescape(arr_value.str);

        key = arr_key;
        value = arr_value;
        
        return true;
    }
    bool array_has(std::string const& key, Substr *value_pt = nullptr) const {
        size_t idx = 0;
        Substr arr_key, value;
        while (array_iter(idx, arr_key, value)) {
            if (arr_key.str == key) {
                if (value_pt != nullptr) {
                    *value_pt = value;
                }
                return true;
            }
        }
        return false;
    }
    Obj array_get(Obj const& key) const {
        Substr value;
        if (!array_has(key, &value)) {
            throw std::out_of_range("no element with key: " +
                (std::string)key);
        }
        return value.str;
    }
    void array_set(Obj const& key, Obj const& value) {
        size_t idx = 0;
        Substr arr_key, arr_value;
        while (array_iter(idx, arr_key, arr_value)) {
            if (arr_key.str == key) {
                size_t value_start = arr_value.index;
                size_t value_end = arr_value.index + arr_value.length;
                *this = this->str.substr(0, value_start) +
                    escape((std::string)value) +
                    this->str.substr(value_end);
                return;
            }
        }
        *this = this->str.substr(0, idx) + escape((std::string)key) +
            "=" + escape((std::string)value) + ";";
    }
private:
    void update_owner() {
        if (this->owner != nullptr) {
            this->owner->array_set(this->owner_key, *this);
        }
    }
public:
    operator double() const {
        double num;
        if (this->is_number(&num)) {
            return num;
        }
        return 0;
    }
    operator std::string const&() const {
        return this->str;
    }
    Obj(): Obj("") {}
    Obj(std::string str) {
        this->str = str;
    }
    Obj(const char *str) {
        this->str = std::string(str);
    }
    Obj(long num) {
        this->str = std::to_string(num);
    }
    Obj(int num) {
        this->str = std::to_string(num);
    }
    Obj(double num) {
        char buf[33];
        snprintf(buf, sizeof(buf), "%lg", num);
        this->str = std::string(buf);
    }
    Obj(Obj const& obj) {
        *this = obj;
    }
    Obj& operator=(Obj const& obj) {
        this->str = obj.str;
        update_owner();
        return *this;
    }
    Obj& operator+=(Obj const& rhs) {
        if (this->is_number() && rhs.is_number()) {
            *this = (double)*this + (double)rhs;
        }
        else {
            *this = this->str + rhs.str;
        }
        return *this;
    }
    Obj& operator-=(Obj const& rhs) {
        *this = (double)*this - (double)rhs;
        return *this;
    }
    Obj& operator*=(Obj const& rhs) {
        *this = (double)*this * (double)rhs;
        return *this;
    }
    Obj& operator/=(Obj const& rhs) {
        *this = (double)*this / (double)rhs;
        return *this;
    }
    friend Obj operator+(Obj lhs, Obj const& rhs) {
        lhs += rhs;
        return lhs;
    }
    friend Obj operator-(Obj lhs, Obj const& rhs) {
        lhs -= rhs;
        return lhs;
    }
    friend Obj operator*(Obj lhs, Obj const& rhs) {
        lhs *= rhs;
        return lhs;
    }
    friend Obj operator/(Obj lhs, Obj const& rhs) {
        lhs /= rhs;
        return lhs;
    }
    friend bool operator==(Obj const& lhs, Obj const& rhs) {
        if (lhs.is_number() && rhs.is_number()) {
            return (double)lhs == (double)rhs;
        }
        else {
            return (std::string)lhs == (std::string)rhs;
        }
    }
    friend bool operator==(Obj const& lhs, int rhs) {
        return lhs == Obj(rhs);
    }
    friend bool operator!=(Obj const& lhs, Obj const& rhs) {
        return !(lhs == rhs);
    }
    friend bool operator>(Obj const& lhs, Obj const& rhs) {
        return (double)lhs > (double)rhs;
    }
    friend bool operator<(Obj const& lhs, Obj const &rhs) {
        return (double)lhs < (double)rhs;
    }
    friend bool operator>=(Obj const& lhs, Obj const& rhs) {
        return (lhs > rhs) || (lhs == rhs);
    }
    friend bool operator<=(Obj const& lhs, Obj const& rhs) {
        return (lhs < rhs) || (lhs == rhs);
    }
    Obj operator+() const {
        return (double)*this;
    }
    Obj operator-() const {
        return -(double)*this;
    }
    Obj operator[](Obj const& key) {
        Obj proxy;
        Substr value;
        if (this->array_has(key, &value)) {
            proxy = value.str;
        }

        proxy.owner = this;
        proxy.owner_key = (std::string)key;
        return proxy;
    }
    Obj operator[](std::vector<Obj> const& keys) {
        if (keys.size() == 0) {
            throw std::runtime_error("keys.size() == 0");
        }
        else if (keys.size() == 1) {
            return (*this)[keys[0]];
        }
        Obj obj;
        obj = (*this)[keys[0]];
        std::vector<Obj> incomplete_objects;
        for (int i=1; i<keys.size(); i++) {
            incomplete_objects.push_back(obj);
            obj = obj[keys[i]];
        }
        obj.associated_objects = incomplete_objects;

        auto &objects = obj.associated_objects;
        objects[0].owner = this;
        objects[0].owner_key = (std::string)keys[0];
        for (int i=1; i<objects.size(); i++) {
            objects[i].owner = &objects[i-1];
            objects[i].owner_key = (std::string)keys[i];
        }
        obj.owner = &objects[objects.size()-1];
        obj.owner_key = (std::string)keys[objects.size()];
        return obj;
    }
    void array_delete(Obj const& key) {
        size_t idx = 0;
        Substr arr_key, arr_value;
        while (array_iter(idx, arr_key, arr_value)) {
            if (arr_key.str == key) {
                size_t entry_start = arr_key.index;
                size_t entry_end = idx;
                *this = this->str.substr(0, entry_start)
                    + this->str.substr(entry_end);
                return;
            }
        }
    }
};

}