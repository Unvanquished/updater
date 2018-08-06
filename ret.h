#ifndef RET_H
#define RET_H

#include <string>

#include <QString>

class Ret {
public:
    Ret(const QString& msg) : ok_(false), msg_(msg) {}
    Ret(bool ok) : ok_(ok) {}

    Ret& operator<<(const char* str) { msg_.append(str); return *this; }
    Ret& operator<<(const std::string& str) { msg_.append(str.c_str()); return *this; }
    Ret& operator<<(const QString& str) { msg_.append(str); return *this; }

    bool ok() const { return ok_; }
    const QString& msg() const { return msg_; }

    bool ok_;
    QString msg_;
};

#endif // RET_H
