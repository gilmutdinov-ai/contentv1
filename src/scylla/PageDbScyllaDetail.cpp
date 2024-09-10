#include "PageDbScylla.h"
#include "misc/Log.h"

namespace contentv1 {

CassError PageDbScylla::executeQuery(CassSession *_session, const char *_query,
                                     std::string &_err_str) {
  CassError rc = CASS_OK;
  CassFuture *future = NULL;
  CassStatement *statement = cass_statement_new(_query, 0);

  future = cass_session_execute(_session, statement);
  cass_future_wait(future);

  rc = cass_future_error_code(future);
  if (rc != CASS_OK)
    _err_str = PageDbScylla::getError(future);

  cass_future_free(future);
  cass_statement_free(statement);

  return rc;
}

void PageDbScylla::deleteAll() {

  std::string err_str;
  executeQuery(m_session, s_truncate_table_query, err_str);
  if (!err_str.empty())
    throw std::runtime_error("PageDbScylla::deleteAll truncate: " + err_str);
}

std::string PageDbScylla::getError(CassFuture *_future) {
  const char *message;
  size_t message_length;
  cass_future_error_message(_future, &message, &message_length);
  return std::string{message, message_length};
}

CassError PageDbScylla::upsertWebPage(CassSession *_session, const char *_url,
                                      const char *_content) {
  CassError rc = CASS_OK;
  CassStatement *statement = NULL;
  CassFuture *future = NULL;
  const char *query = "UPDATE contentv1.web_pages SET content=? WHERE url=?";

  statement = cass_statement_new(query, 2);
  cass_statement_bind_string(statement, 0, _content);
  cass_statement_bind_string(statement, 1, _url);
  future = cass_session_execute(_session, statement);
  cass_future_wait(future);

  rc = cass_future_error_code(future);

  std::string err_str;
  if (rc != CASS_OK) {
    err_str = PageDbScylla::getError(future);
  }

  cass_future_free(future);
  cass_statement_free(statement);

  if (!err_str.empty())
    throw std::runtime_error(err_str);

  return rc;
}

void PageDbScylla::selectWebPage(CassSession *_session, const char *_url,
                                 std::string &_content) {
  CassError rc = CASS_OK;
  CassStatement *statement = NULL;
  CassFuture *future = NULL;
  const char *query = "SELECT content FROM contentv1.web_pages WHERE url = ?";

  statement = cass_statement_new(query, 1);
  cass_statement_bind_string(statement, 0, _url);
  future = cass_session_execute(_session, statement);
  cass_future_wait(future);
  rc = cass_future_error_code(future);
  std::string err_str;
  if (rc != CASS_OK) {
    err_str = "PageDbScylla::selectWebPage query exec error " +
              PageDbScylla::getError(future);
  } else {
    const CassResult *result = cass_future_get_result(future);
    CassIterator *iterator = cass_iterator_from_result(result);

    // cass_value_get_string(const CassValue* value, const char** output,
    // size_t* output_length)

    if (cass_iterator_next(iterator)) {

      char *content;
      size_t content_length;
      const CassRow *row = cass_iterator_get_row(iterator);
      if (cass_value_get_string(cass_row_get_column(row, 0),
                                (const char **)&content,
                                &content_length) != CASS_OK) {
        err_str = "PageDbScylla::selectWebPage "
                  "cass_value_get_string !CASS_OK";
      } else {
        _content = std::string{content, content_length};
      }
    } else {
      err_str = "PageDbScylla::selectWebPage "
                "iterate no rows";
    }

    cass_result_free(result);
    cass_iterator_free(iterator);
  }

  cass_future_free(future);
  cass_statement_free(statement);

  if (err_str.empty())
    throw std::runtime_error(err_str);
}

std::string PageDbScylla::getColumnValue(CassIterator *_iterator, int _col_idx,
                                         std::string &_err_str) {
  char *content;
  size_t content_length;
  const CassRow *row = cass_iterator_get_row(_iterator);
  if (cass_value_get_string(cass_row_get_column(row, _col_idx),
                            (const char **)&content,
                            &content_length) != CASS_OK) {
    _err_str = "PageDbScylla::getColumnValue "
               "cass_value_get_string !CASS_OK";
    return std::string{};
  }
  return std::string{content, content_length};
}

void PageDbScylla::fullScanWebPages(CassSession *_session, ScanPageCb _cb) {
  CassError rc = CASS_OK;
  CassStatement *statement = NULL;
  CassFuture *future = NULL;

  static constexpr int URL_COL_ID{0};
  static constexpr int CONTENT_COL_ID{1};
  const char *query = "SELECT url, content FROM contentv1.web_pages";

  static constexpr int QUERY_PARAMS_COUNT{0};
  statement = cass_statement_new(query, QUERY_PARAMS_COUNT);
  future = cass_session_execute(_session, statement);
  cass_future_wait(future);
  rc = cass_future_error_code(future);
  std::string err_str;
  if (rc != CASS_OK) {
    err_str = "PageDbScylla::fullScanWebPage query exec error " +
              PageDbScylla::getError(future);
  } else {
    const CassResult *result = cass_future_get_result(future);
    CassIterator *iterator = cass_iterator_from_result(result);

    // cass_value_get_string(const CassValue* value, const char** output,
    // size_t* output_length)

    while (cass_iterator_next(iterator)) {

      std::string url = getColumnValue(iterator, URL_COL_ID, err_str);
      if (!err_str.empty())
        break;

      std::string content = getColumnValue(iterator, CONTENT_COL_ID, err_str);
      if (!err_str.empty())
        break;

      _cb(url, content);
    }

    cass_result_free(result);
    cass_iterator_free(iterator);
  }

  cass_future_free(future);
  cass_statement_free(statement);

  if (!err_str.empty())
    throw std::runtime_error(err_str);
}

} // namespace contentv1
