#include "wgnx/i18n.hpp"
#include <cassert>
#include <cstring>

int main() {
    using namespace wgnx::i18n;
    initialize();
    assert(std::strcmp(tr("Connection Info"), "Connection Info") == 0);
    assert(setLanguage("zh-CN"));
    assert(std::strcmp(tr("Connection Info"), "连接信息") == 0);
    assert(std::strcmp(tr("Untranslated diagnostic"), "Untranslated diagnostic") == 0);
    assert(!setLanguage("invalid"));
    assert(language == Language::ChineseSimplified);
    assert(setLanguage("en"));
    assert(std::strcmp(tr("Connection Info"), "Connection Info") == 0);
}
