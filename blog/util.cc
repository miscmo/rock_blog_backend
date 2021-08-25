#include "util.h"
#include <regex>
#include "sylar/sylar.h"
#include "sylar/config.h"

namespace blog {

static sylar::ConfigVar<int32_t>::ptr g_db_type =
    sylar::Config::Lookup("db.type", (int32_t)2, "db type 1:sqlite3, 2:mysql");

bool is_email(const std::string& str) {
    static const std::regex s_email_regex("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,8}(\\.[a-z]{2,8})?)");
    return std::regex_match(str, s_email_regex);
}

bool is_valid_account(const std::string& str) {
    static const std::regex s_account_regex("[A-Za-z][0-9A-Za-z\\-_\\.]{4,15}");
    return std::regex_match(str, s_account_regex);
}

sylar::IDB::ptr GetDB() {
    if(g_db_type->getValue() == 2) {
        return sylar::MySQLMgr::GetInstance()->get("blog");
    } else {
        return sylar::SQLite3Mgr::GetInstance()->get("blog");
    }
}

std::string get_max_length_string(const std::string& str, size_t len) {
    auto wstr = sylar::StringUtil::StringToWString(str);
    wstr.resize(len);
    return sylar::StringUtil::WStringToString(wstr);
}

void SendWX(const std::string& group, const std::string& msg) {
    sylar::IOManager::GetThis()->schedule([group, msg](){
        std::string url = "http://127.0.0.1:8072/send_wx?group=" + group
            + "&msg=" + sylar::StringUtil::UrlEncode(msg);
        sylar::http::HttpConnection::DoGet(url, 5000);
    });
}

}
