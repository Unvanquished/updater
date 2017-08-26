#ifndef RET_H
#define RET_H

#include <string>

#include <QString>

class Ret {
public:
    Ret(const QString& msg) : ok_(false), msg_(msg) {}
    Ret(bool ok) : ok_(ok) {}

    bool ok() const { return ok_; }
    const QString& msg() const { return msg_; }

    const bool ok_;
    const QString msg_;
};

#endif // RET_H
