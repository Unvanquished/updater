/* Unvanquished Updater
 * Copyright (C) Unvanquished Developers
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef GAMELAUNCHER_H_
#define GAMELAUNCHER_H_

#include <QObject>
#include <QString>
#include "settings.h"

class GameLauncher : public QObject
{
    Q_OBJECT

private:
    QString connectUrl_;
    const Settings& settings_;

public:
    GameLauncher(const QString& connectUrl, const Settings& settings);

    Q_INVOKABLE void startGame(bool useConnectUrl, bool failIfWindowsAdmin);
};

#endif // GAMELAUNCHER_H_
