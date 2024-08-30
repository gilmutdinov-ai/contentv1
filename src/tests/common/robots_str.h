#pragma once

#include <string>

namespace contentv1 {

static std::string g_robots_test_str = {
    "User-agent: *\n"
    "Disallow: /banners/\n"
    "Disallow: /link/\n"
    "Disallow: /scripts/\n"
    "Disallow: /profile/\n"
    "Disallow: /go/\n"
    "Disallow: /print/\n"
    "Disallow: /cgi-bin/\n"
    "Disallow: /frames/\n"
    "Disallow: /daily/article_cover/\n"
    "Disallow: /best/fbs/\n"
    "Disallow: /search/\n"
    "Disallow: /*/wp-admin/*\n"
    "Disallow: /*/wp-json/*\n"
    "Disallow: /*/wp-login.php\n"
    "Disallow: /*/wp-register.php\n"
    "Disallow: /*/24-hours-rss/\n"
    "Disallow: /comments/*\n"
    "Disallow: /content/api/*\n"
    "Disallow: /daily/26945/3996748/\n"
    "Disallow: *couponRedirect\n"
    "User-agent: Yandex\n"
    "Disallow: /afisha/*/amp/$\n"
    "Disallow: *couponRedirect\n"
    "Clean-param: erid\n"
    "Clean-param: from \n"
    "Clean-param: fromrss\n"
    "Clean-param: utm_campaign \n"
    "Clean-param: utm_medium \n"
    "Clean-param: utm_term \n"
    "Clean-param: utm_source\n"
    "Clean-param: disableGlobalInfoCollect\n"
    "Clean-param: clckid&__twitter_impression&__timestamp__&calltouch_tm\n"
    "User-agent: Googlebot\n"
    "Disallow: /*?\n"
    "Host: https://www.kp.ru\n"
    "Sitemap: https://www.kp.ru/sitemap.xml.gz\n"};

}