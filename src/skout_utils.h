#define ZAP(x) \
    if (x != nullptr) { \
        delete x; \
        x = nullptr; \
    }
