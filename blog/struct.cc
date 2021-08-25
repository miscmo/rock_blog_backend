#include "struct.h"
#include "blog/manager/user_manager.h"
#include "blog/util.h"
#include "sylar/db/sqlite3.h"

namespace blog {

static sylar::Logger::ptr g_logger_access = SYLAR_LOG_NAME("access");

const std::string CookieKey::SESSION_KEY = "SSESSIONID";
const std::string CookieKey::USER_ID = "S_UID";
const std::string CookieKey::TOKEN = "S_TOKEN";
const std::string CookieKey::TOKEN_TIME = "S_TOKEN_TIME";
const std::string CookieKey::IS_AUTH= "IS_AUTH";
const std::string CookieKey::COMMENT_LAST_TIME = "COMMENT_LAST_TIME";
const std::string CookieKey::ARTICLE_LAST_TIME = "ARTICLE_LAST_TIME";
const std::string CookieKey::EMAIL_LAST_TIME = "EMAIL_LAST_TIME";

std::string GetRemoteIP(sylar::http::HttpRequest::ptr request
                        ,sylar::http::HttpSession::ptr session) {
    auto rt = request->getHeader("X-Real-IP");
    if(!rt.empty()) {
        return rt;
    }
    rt = session->getRemoteAddressString();
    auto pos = rt.find(':');
    return rt.substr(0, pos);
}

Result::Result(int32_t c, const std::string& m)
    :code(c)
    ,used(sylar::GetCurrentUS())
    ,msg(m){
}

std::string Result::toJsonString() const {
    Json::Value v;
    v["code"] = std::to_string(code);
    v["msg"] = msg;
    v["used"] = ((sylar::GetCurrentUS() - used) / 1000.0);
    if(!jsondata.isNull()) {
        v["data"] = jsondata;
    } else {
        //if(!datas.empty()) {
        //    auto& d = v["data"];
        //    for(auto& i : datas) {
        //        d[i.first] = i.second;
        //    }
        //}
    }
    return sylar::JsonUtil::ToString(v);
}

void Result::setResult(int32_t c, const std::string& m) {
    code = c;
    msg = m;
}

BlogServlet::BlogServlet(const std::string& name)
    :sylar::http::Servlet(name) {
}

int32_t BlogServlet::handle(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session) {
    uint64_t ts = sylar::GetCurrentUS();
    Result::ptr result = std::make_shared<Result>();
    response->setHeader("Access-Control-Allow-Origin", "*");
    response->setHeader("Access-Control-Allow-Credentials", "true");
    if(handlePre(request, response, session, result)) {
        handle(request, response, session, result);
    } else {
        response->setBody(result->toJsonString());
    }
    uint64_t used = sylar::GetCurrentUS() - ts;
    handlePost(request, response, session, result);
    response->setHeader("used", std::to_string((used * 1.0 / 1000)) + "ms");
    return 0;
}

bool BlogServlet::handlePre(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result) {
    if(request->getPath() != "/user/login"
            && request->getPath() != "/user/logout") {
        initLogin(request, response, session);
    }
    if(request->getMethod() != sylar::http::HttpMethod::GET
            && request->getMethod() != sylar::http::HttpMethod::POST) {
        result->setResult(300, "invalid method");
        return false;
    }
    return true;
}

bool BlogServlet::handlePost(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result) {
    SYLAR_LOG_INFO(g_logger_access)
        << GetRemoteIP(request, session) << "\t"
        << request->getCookie(CookieKey::SESSION_KEY, "-") << "\t"
        << getUserId(request) << "\t"
        << result->code << "\t"
        << result->msg << "\t" << request->getPath()
        << "\t" << (!request->getQuery().empty() ? request->getQuery() : "-");
    return true;
}

sylar::http::SessionData::ptr BlogServlet::getSessionData(sylar::http::HttpRequest::ptr request
                                                          ,sylar::http::HttpResponse::ptr response) {
    std::string sid = request->getCookie(CookieKey::SESSION_KEY);
    if(!sid.empty()) {
        auto data = sylar::http::SessionDataMgr::GetInstance()->get(sid);
        if(data) {
            return data;
        }
    }
    sylar::http::SessionData::ptr data(new sylar::http::SessionData(true));
    sylar::http::SessionDataMgr::GetInstance()->add(data);
    response->setCookie(CookieKey::SESSION_KEY, data->getId(), 0, "/");
    request->setCookie(CookieKey::SESSION_KEY, data->getId());
    return data;
}

bool BlogServlet::initLogin(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session) {
    auto data = getSessionData(request, response);
    int64_t uid = data->getData<int64_t>(CookieKey::USER_ID);
    if(uid) {
        return true;
    }
    int32_t is_auth = data->getData<int32_t>(CookieKey::IS_AUTH);
    if(is_auth) {
        return false;
    }
    bool is_login = false;
    do {
        int64_t uid = request->getCookieAs<int64_t>(CookieKey::USER_ID);
        if(!uid) {
            break;
        }
        auto token = request->getCookie(CookieKey::TOKEN);
        if(token.empty()) {
            break;
        }
        int64_t token_time = request->getCookieAs<int64_t>(CookieKey::TOKEN_TIME);
        if(token_time <= time(0)) {
            break;
        }

        data::UserInfo::ptr uinfo = UserMgr::GetInstance()->get(uid);
        if(!uinfo) {
            break;
        }
        if(uinfo->getState() != 2) {
            break;
        }
        auto md5 = UserManager::GetToken(uinfo, token_time);
        if(md5 != token) {
            SYLAR_LOG_INFO(g_logger_access)
                << GetRemoteIP(request, session) << "\t"
                << request->getCookie(CookieKey::SESSION_KEY, "-") << "\t"
                << uid << "\t"
                << 310 << "\t"
                << "invalid_token" << "\tauto_login" << request->getPath()
                << "\t" << (!request->getQuery().empty() ? request->getQuery() : "-");
            break;
        }
        data->setData(CookieKey::USER_ID, uid);
        is_login = true;
        SYLAR_LOG_INFO(g_logger_access)
            << GetRemoteIP(request, session) << "\t"
            << request->getCookie(CookieKey::SESSION_KEY, "-") << "\t"
            << uid << "\t"
            << 200 << "\t"
            << "ok" << "\tauto_login" << request->getPath()
            << "\t" << (!request->getQuery().empty() ? request->getQuery() : "-");

        uinfo->setLoginTime(time(0));
        auto db = getDB();
        if(db) {
            data::UserInfoDao::Update(uinfo, db);
        }
        is_login = true;
    } while(0);
    data->setData(CookieKey::IS_AUTH, (int32_t)1);
    return is_login;
}

sylar::IDB::ptr BlogServlet::getDB() {
    return GetDB();
}

BlogLoginedServlet::BlogLoginedServlet(const std::string& name)
    :BlogServlet(name) {
}

bool BlogLoginedServlet::handlePre(sylar::http::HttpRequest::ptr request
                                   ,sylar::http::HttpResponse::ptr response
                                   ,sylar::http::HttpSession::ptr session
                                   ,Result::ptr result) {
    if(!initLogin(request, response, session)) {
        result->setResult(410, "not login");
        return false;
    }
    if(request->getMethod() != sylar::http::HttpMethod::GET
            && request->getMethod() != sylar::http::HttpMethod::POST) {
        result->setResult(300, "invalid method");
        return false;
    }
    return true;
}

int64_t BlogServlet::getUserId(sylar::http::HttpRequest::ptr request) {
    std::string sid = request->getCookie(CookieKey::SESSION_KEY);
    if(!sid.empty()) {
        auto data = sylar::http::SessionDataMgr::GetInstance()->get(sid);
        if(data) {
            return data->getData<int64_t>(CookieKey::USER_ID);
        }
    }
    return 0;
}

std::string BlogServlet::getCookieId(sylar::http::HttpRequest::ptr request) {
    return request->getCookie(CookieKey::SESSION_KEY);
}

}
