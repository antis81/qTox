/*
    Copyright © 2014-2015 by The qTox Project Contributors

    This file is part of qTox, a Qt-based graphical interface for Tox.

    qTox is libre software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    qTox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with qTox.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "friend.h"
#include "friendlist.h"
#include "core/core.h"
#include "persistence/settings.h"
#include "persistence/profile.h"
#include "nexus.h"
#include "grouplist.h"
#include "group.h"

Friend::Friend(uint32_t FriendId, const ToxId &UserId)
    : userName{Core::getInstance()->getPeerName(UserId)}
    , userID(UserId), friendId(FriendId)
    , hasNewEvents(0), friendStatus(Status::Offline)

{
    if (userName.size() == 0)
        userName = UserId.publicKey;

    userAlias = Settings::getInstance().getFriendAlias(UserId);
}

Friend::~Friend()
{
}

/**
 * @brief Loads the friend's chat history if enabled
 */
void Friend::loadHistory()
{
    if (Nexus::getProfile()->isHistoryEnabled())
    {
    }
}

void Friend::setName(QString name)
{
   if (name.isEmpty())
       name = userID.publicKey;

    userName = name;
    if (userAlias.size() == 0)
    {
        emit displayedNameChanged(friendId, getStatus(), hasNewEvents);
    }
}

void Friend::setAlias(QString name)
{
    userAlias = name;
    QString dispName = userAlias.size() == 0 ? userName : userAlias;

    emit displayedNameChanged(friendId, getStatus(), hasNewEvents);

    for (Group *g : GroupList::getAllGroups())
    {
        g->regeneratePeerList();
    }
}

void Friend::setStatusMessage(QString message)
{
    statusMessage = message;
}

QString Friend::getStatusMessage()
{
    return statusMessage;
}

QString Friend::getDisplayedName() const
{
    if (userAlias.size() == 0)
        return userName;

    return userAlias;
}

bool Friend::hasAlias() const
{
    return !userAlias.isEmpty();
}

const ToxId &Friend::getToxId() const
{
    return userID;
}

uint32_t Friend::getFriendID() const
{
    return friendId;
}

void Friend::setEventFlag(int f)
{
    hasNewEvents = f;
}

int Friend::getEventFlag() const
{
    return hasNewEvents;
}

void Friend::setStatus(Status s)
{
    friendStatus = s;
}

Status Friend::getStatus() const
{
    return friendStatus;
}
