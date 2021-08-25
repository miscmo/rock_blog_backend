#include "article_category_rel_info.h"
#include "sylar/log.h"

namespace blog {
namespace data {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("orm");

ArticleCategoryRelInfo::ArticleCategoryRelInfo()
    :m_isDeleted()
    ,m_id()
    ,m_articleId()
    ,m_categoryId()
    ,m_createTime()
    ,m_updateTime() {
}

std::string ArticleCategoryRelInfo::toJsonString() const {
    Json::Value jvalue;
    jvalue["id"] = std::to_string(m_id);
    jvalue["article_id"] = std::to_string(m_articleId);
    jvalue["category_id"] = std::to_string(m_categoryId);
    jvalue["is_deleted"] = m_isDeleted;
    jvalue["create_time"] = sylar::Time2Str(m_createTime);
    jvalue["update_time"] = sylar::Time2Str(m_updateTime);
    return sylar::JsonUtil::ToString(jvalue);
}

void ArticleCategoryRelInfo::setId(const int64_t& v) {
    m_id = v;
}

void ArticleCategoryRelInfo::setArticleId(const int64_t& v) {
    m_articleId = v;
}

void ArticleCategoryRelInfo::setCategoryId(const int64_t& v) {
    m_categoryId = v;
}

void ArticleCategoryRelInfo::setIsDeleted(const int32_t& v) {
    m_isDeleted = v;
}

void ArticleCategoryRelInfo::setCreateTime(const int64_t& v) {
    m_createTime = v;
}

void ArticleCategoryRelInfo::setUpdateTime(const int64_t& v) {
    m_updateTime = v;
}


int ArticleCategoryRelInfoDao::Update(ArticleCategoryRelInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "update article_category_rel set article_id = ?, category_id = ?, is_deleted = ?, create_time = ?, update_time = ? where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_articleId);
    stmt->bindInt64(2, info->m_categoryId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    stmt->bindInt64(6, info->m_id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::Insert(ArticleCategoryRelInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "insert into article_category_rel (article_id, category_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_articleId);
    stmt->bindInt64(2, info->m_categoryId);
    stmt->bindInt32(3, info->m_isDeleted);
    stmt->bindTime(4, info->m_createTime);
    stmt->bindTime(5, info->m_updateTime);
    int rt = stmt->execute();
    if(rt == 0) {
        info->m_id = conn->getLastInsertId();
    }
    return rt;
}

int ArticleCategoryRelInfoDao::InsertOrUpdate(ArticleCategoryRelInfo::ptr info, sylar::IDB::ptr conn) {
    if(info->m_id == 0) {
        return Insert(info, conn);
    }
    std::string sql = "replace into article_category_rel (id, article_id, category_id, is_deleted, create_time, update_time) values (?, ?, ?, ?, ?, ?)";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    stmt->bindInt64(2, info->m_articleId);
    stmt->bindInt64(3, info->m_categoryId);
    stmt->bindInt32(4, info->m_isDeleted);
    stmt->bindTime(5, info->m_createTime);
    stmt->bindTime(6, info->m_updateTime);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::Delete(ArticleCategoryRelInfo::ptr info, sylar::IDB::ptr conn) {
    std::string sql = "delete from article_category_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, info->m_id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::DeleteById( const int64_t& id, sylar::IDB::ptr conn) {
    std::string sql = "delete from article_category_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::DeleteByArticleId( const int64_t& article_id, sylar::IDB::ptr conn) {
    std::string sql = "delete from article_category_rel where article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::DeleteByArticleIdCategoryId( const int64_t& article_id,  const int64_t& category_id, sylar::IDB::ptr conn) {
    std::string sql = "delete from article_category_rel where article_id = ? and category_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    stmt->bindInt64(1, category_id);
    return stmt->execute();
}

int ArticleCategoryRelInfoDao::QueryAll(std::vector<ArticleCategoryRelInfo::ptr>& results, sylar::IDB::ptr conn) {
    std::string sql = "select id, article_id, category_id, is_deleted, create_time, update_time from article_category_rel";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    auto rt = stmt->query();
    if(!rt) {
        return stmt->getErrno();
    }
    while (rt->next()) {
        ArticleCategoryRelInfo::ptr v(new ArticleCategoryRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_categoryId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    }
    return 0;
}

ArticleCategoryRelInfo::ptr ArticleCategoryRelInfoDao::Query( const int64_t& id, sylar::IDB::ptr conn) {
    std::string sql = "select id, article_id, category_id, is_deleted, create_time, update_time from article_category_rel where id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    ArticleCategoryRelInfo::ptr v(new ArticleCategoryRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_articleId = rt->getInt64(1);
    v->m_categoryId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

int ArticleCategoryRelInfoDao::QueryByArticleId(std::vector<ArticleCategoryRelInfo::ptr>& results,  const int64_t& article_id, sylar::IDB::ptr conn) {
    std::string sql = "select id, article_id, category_id, is_deleted, create_time, update_time from article_category_rel where article_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return conn->getErrno();
    }
    stmt->bindInt64(1, article_id);
    auto rt = stmt->query();
    if(!rt) {
        return 0;
    }
    while (rt->next()) {
        ArticleCategoryRelInfo::ptr v(new ArticleCategoryRelInfo);
        v->m_id = rt->getInt64(0);
        v->m_articleId = rt->getInt64(1);
        v->m_categoryId = rt->getInt64(2);
        v->m_isDeleted = rt->getInt32(3);
        v->m_createTime = rt->getTime(4);
        v->m_updateTime = rt->getTime(5);
        results.push_back(v);
    };
    return 0;
}

ArticleCategoryRelInfo::ptr ArticleCategoryRelInfoDao::QueryByArticleIdCategoryId( const int64_t& article_id,  const int64_t& category_id, sylar::IDB::ptr conn) {
    std::string sql = "select id, article_id, category_id, is_deleted, create_time, update_time from article_category_rel where article_id = ? and category_id = ?";
    auto stmt = conn->prepare(sql);
    if(!stmt) {
        SYLAR_LOG_ERROR(g_logger) << "stmt=" << sql
                 << " errno=" << conn->getErrno() << " errstr=" << conn->getErrStr();
        return nullptr;
    }
    stmt->bindInt64(1, article_id);
    stmt->bindInt64(2, category_id);
    auto rt = stmt->query();
    if(!rt) {
        return nullptr;
    }
    if(!rt->next()) {
        return nullptr;
    }
    ArticleCategoryRelInfo::ptr v(new ArticleCategoryRelInfo);
    v->m_id = rt->getInt64(0);
    v->m_articleId = rt->getInt64(1);
    v->m_categoryId = rt->getInt64(2);
    v->m_isDeleted = rt->getInt32(3);
    v->m_createTime = rt->getTime(4);
    v->m_updateTime = rt->getTime(5);
    return v;
}

int ArticleCategoryRelInfoDao::CreateTableSQLite3(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE article_category_rel("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "article_id INTEGER NOT NULL DEFAULT 0,"
            "category_id INTEGER NOT NULL DEFAULT 0,"
            "is_deleted INTEGER NOT NULL DEFAULT 0,"
            "create_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00',"
            "update_time TIMESTAMP NOT NULL DEFAULT '1980-01-01 00:00:00');"
            "CREATE INDEX article_category_rel_article_id ON article_category_rel(article_id);"
            "CREATE UNIQUE INDEX article_category_rel_article_id_category_id ON article_category_rel(article_id,category_id);"
            );
}

int ArticleCategoryRelInfoDao::CreateTableMySQL(sylar::IDB::ptr conn) {
    return conn->execute("CREATE TABLE article_category_rel("
            "`id` bigint AUTO_INCREMENT COMMENT '自增长id',"
            "`article_id` bigint NOT NULL DEFAULT 0 COMMENT '文章id',"
            "`category_id` bigint NOT NULL DEFAULT 0 COMMENT '类目id',"
            "`is_deleted` int NOT NULL DEFAULT 0 COMMENT '是否删除',"
            "`create_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' COMMENT '创建时间',"
            "`update_time` timestamp NOT NULL DEFAULT '1980-01-01 00:00:00' ON UPDATE current_timestamp  COMMENT '更新时间',"
            "PRIMARY KEY(`id`),"
            "KEY `article_category_rel_article_id` (`article_id`),"
            "UNIQUE KEY `article_category_rel_article_id_category_id` (`article_id`,`category_id`)) COMMENT='文章类目关系表'");
}
} //namespace data
} //namespace blog
