#include <iostream>
#include <charconv>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <regex>
#include <vector>
#include <unordered_map>
#include <cassert>
#include "print.h"

struct JSONObject;
using JSONList = std::vector<JSONObject>;
using JSONDict = std::unordered_map<std::string, JSONObject>;

struct JSONObject {
  std::variant
  < std::nullptr_t // null
  , bool   // true or false
  , int   // 42
  , double  // 3.1415
  , std::string // "hello world"
  , JSONList // [42, "hello world"]
  , JSONDict  // {"hello": 985, "world": 211}
  > inner;

  void do_print() const {
    print(inner);
  }
  template <typename T>
  auto get() const-> const T& {
    return std::get<T>(inner);
  }

  template <typename T>
  auto is(T& t) const -> bool {
    return std::holds_alternative<T>(inner);
  }
};


// 解析数值类型, int, double
template <class T>
auto try_parse_num(std::string_view str) -> std::optional<T> {
  T value;
  auto res = std::from_chars(str.data(), str.data() + str.size(), value);
  if (res.ec == std::errc() && res.ptr == str.data() + str.size()) {
    return value;
  }
  return std::nullopt;
}

char unescaped_char(char c) {
  switch (c) {
    case 'a': return '\a';
    case 'b': return '\b';
    case 'f': return '\f';
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    case 'v': return '\v';
    case '0': return '\0';
    default: return c;
  }
}

// 解析string类型

std::pair<JSONObject, size_t> parse(std::string_view json) {
  if (json.empty()) {
    return {JSONObject{std::nullptr_t{}}, 0};
  } else if (json[0] == 'n' || json[0] == 't' || json[0] == 'f') {  // parse `null`, boolean
    std::regex alpha_regex("[a-z]+");
    std::cmatch match;
    std::regex_search(json.begin(), json.end(), match, alpha_regex); 
    if (match.str() == "true") {
      return {JSONObject{true}, match.size()};
    } else if (match.str() == "false") {
      return {JSONObject{false}, match.size()};
    } else if (match.str() == "null") {
      return {JSONObject{std::nullptr_t{}}, match.size()};
    } 
    // if parse failed, we can throw exeception...
  } else if(size_t offset = json.find_first_not_of(" \t\r\v\n\f\0"); offset != 0 && offset != json.npos) {
    auto [obj, eaten] = parse(json.substr(offset));
    return {std::move(obj), eaten + offset};
  }  else if ((json[0] >= '0' && json[0] <= '9') || json[0] == '+' || json[0] == '-') {  // 解析数值
    // number regex
    std::regex num_regex{"[+-]?[0-9]+(\\.[0-9]*)?([eE][+-]?[0-9]+)?"};
    std::cmatch match;
    if (std::regex_search(json.begin(), json.end(), match, num_regex)) {
      std::string str = match.str(); 
      if (auto num = try_parse_num<int>(str)) {
        return {JSONObject{*num}, str.size()};
      }
      if (auto num = try_parse_num<double>(str)) {
        return {JSONObject{*num}, str.size()};
      }
    }
   } else if(json[0] == '"') {     // 解析字符串 
    std::string str;
    enum {Raw, Escaped} phase = Raw;
    size_t i;
    for (i = 1; i < json.size(); ++i) {
      char ch = json[i];
      if (phase == Raw) {
        if (ch == '\\') {
          phase = Escaped;
        } else if (ch == '"') {
          i += 1;
          break;
        } else {
          str += ch;
        }
      } else if (phase == Escaped) {
        str += unescaped_char(ch);
        phase = Raw;
      }
    }
    return {JSONObject{std::move(str)}, i};
  } else if (json[0] == '[') {  // 解析列表
    std::vector<JSONObject> res;
    size_t i;
    for (i = 1; i < json.size();) {
        if (json[i] == ']') {
            i += 1;
            break;
        }
        auto [obj, eaten] = parse(json.substr(i));
        if (eaten == 0) {
            i = 0;
            break;
        }
        res.push_back(std::move(obj));
        i += eaten;
        if (json[i] == ',') {
            i += 1;
        }
    }
    return {JSONObject{std::move(res)}, i};
  } else if (json[0] == '{') { // 解析字典 {"hello":985, "world":211}
    std::unordered_map<std::string, JSONObject> res;
    size_t i = 1;
    while (i < json.size()) {
      if (json[i] == '}') {
        i += 1;
        break;
      }
      auto [key_obj, key_eaten] = parse(json.substr(i));
      if (key_eaten == 0) {
        i = 0;
        break;
      }
      i += key_eaten;
      if (!std::holds_alternative<std::string>(key_obj.inner)) {
        i = 0;
        break;
      }
      if (json[i] == ':') {
        i += 1;
      }
      std::string key = key_obj.get<std::string>();
      auto [val_obj, val_eaten] = parse(json.substr(i));
      if (val_eaten == 0) {
        i = 0;
        break;
      }
      i += val_eaten;
      res.insert_or_assign(std::move(key), std::move(val_obj));
      if (json[i] == ',') {
        i += 1;
      }
    }
    return {JSONObject{std::move(res)}, i};
  }
  // std::cout << "should not reach this branch: " << json << '\n'; 
  return {JSONObject{std::nullptr_t{}}, 0};
}


template <typename... Ts>
struct overloaded: Ts... {
  using Ts::operator()...;
};

template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

int main()
{
  std::string_view str = R"JSON({
    "work": 996,
    "school": [985, 211],
    "id": null,
    "male": false
    })JSON";
  auto [obj, size] = parse(str);
  print(obj);
  // const auto &dict = obj.get<JSONDict>();
  // const auto &school = dict.at("school");
  // overloaded overload = {
  //   [&](int val) {
  //     print("int is ", val);
  //   },
  //   [&](double val) {
  //     print("double is ", val);
  //   },
  //   [&](std::string val) {
  //     print("string is ", val);
  //   },
  //   [&](auto val) {
  //     print("unknown");
  //   }
  // };
  // std::visit(overload, obj.inner);
  return 0;
}
