#pragma once
#include "core/Types.h"
#include "httplib.h"
#include "misc/Config.h"
#include "worker/QueryLoop.h"
#include <memory>

/*
Will no read
    https://www.ietf.org/archive/id/draft-ietf-oauth-v2-bearer-23.txt

Impl like there
    https://developer.x.com/en/docs/authentication/oauth-2-0/bearer-tokens

methods:

/version
/check_token
/get_query_groups_list
/set_query_group (group-id, [queries])
/get_query_group (group-id)
/get_hits (group-id) -> [hits]

*/

namespace contentv1 {

class APIv1Config : virtual public misc::Config {
public:
  inline static const std::string STR_LISTEN_ADDR{"apiv1_listen_addr"};
  inline static const std::string INT_LISTEN_PORT{"apiv1_listen_port"};

  APIv1Config();
  virtual ~APIv1Config() = default;
};

class APIv1 {
  inline static const std::string s_version_json_str{"{\"version\":\"1.0\"}"};
  inline static const std::string s_internal_server_error_json{
      "{\"status\":\"internal server error\"}"};
  inline static const std::string s_content_type_json{"application/json"};

  inline static const std::string s_auth_basic_header{"Authorization"};
  inline static constexpr size_t s_header_basic_min_size =
      std::string_view{"Basic XXX"}.size();
  inline static const std::string s_header_basic_prefix{"Basic "};

  // TOKEN
  //  base64(_api_key)[:-5][6 random chars][hash(prev + pepper):-5]
  inline static constexpr size_t s_token_size{16};

  inline static constexpr std::string_view s_header_bearer_prefix{"Bearer "};
  inline static constexpr size_t s_header_bearer_exact_size =
      s_header_bearer_prefix.size() + s_token_size;

  inline static const std::string s_pepper{"crypto_pepper_string_06.08.24"};
  inline static long s_token_expires_in{3600 * 24 * 365};

  inline static constexpr std::array<std::string_view, 2> s_api_keys_hardcode{
      {"_API_KEY_", "_API_SECRET_KEY_"}};

public:
  using Ptr = std::shared_ptr<APIv1>;
  APIv1(QueryDb::Ptr _query_db, QueryLoop::Ptr _query_loop,
        const APIv1Config &_config);
  ~APIv1();
  void join();

private:
  void _setupServer();

  struct APIRequest {
    const httplib::Headers &headers;
    const std::string &body;
    const httplib::Params &params;
  };

  struct APIResponse {
    std::string body;
    std::string content_type = s_content_type_json;
    int status = 200;

    void set(const std::string &_body,
             const std::string &_ct = s_content_type_json);
    void set(int _status, const std::string &_body,
             const std::string &_ct = s_content_type_json);

    void setAccessDenied(const std::string &_mess);
    void setBadRequest(const std::string &_mess);
    void setInternalError(const std::string &_mess);
  };

  using Handler =
      std::function<void(const APIRequest &_api_req, APIResponse &_api_res)>;

  enum Method { GET = 0, POST = 1, DELETE = 2 };

  struct Route {

    Method method;
    std::string path;
    bool need_auth;
    Handler handler;
  };

  // CRYPTO
  bool _genToken(const std::string &_api_key,
                 const std::string &_api_secret_key, std::string &_token,
                 long &_expires_in);

  static bool _checkToken(const std::string &_token);
  static bool _isToken(const std::string &_token);
  static std::string hash(const std::string &_str);
  static bool _auth(const httplib::Headers &_headers, std::string &_error_str);

  // PARSING
  static bool _parseApiKeys(const httplib::Headers &_headers,
                            std::string &_api_key, std::string &_api_secret_key,
                            std::string &_error_str);
  static bool _parseBearer(const httplib::Headers &_headers,
                           std::string &_token, std::string &_error_str);

  // HANDLERS
  void hdlVersion(const APIRequest &_api_req, APIResponse &_api_res);
  void hdlToken(const APIRequest &_api_req, APIResponse &_api_res);
  void hdlCheckToken(const APIRequest &_api_req, APIResponse &_api_res);
  void hdlGetQueryGroupsList(const APIRequest &_api_req, APIResponse &_api_res);
  void hdlSetQueryGroup(const APIRequest &_api_req, APIResponse &_api_res);
  void hdlGetQueryGroup(const APIRequest &_api_req, APIResponse &_api_res);
  void hdlDeleteQueryGroup(const APIRequest &_api_req, APIResponse &_api_res);
  void hdlGetHits(const APIRequest &_api_req, APIResponse &_api_res);

  friend class APIv1Tester;

private:
  std::map<std::string, std::string> m_api_keys;
  std::mutex m_mtx;

  QueryLoop::Ptr m_query_loop;
  QueryDb::Ptr m_query_db;

  std::unique_ptr<httplib::Server> m_srv;
  const std::string m_listen_addr;
  const int m_listen_port;

  std::thread m_th;

  std::vector<Route> m_routes;
};

} // namespace contentv1
