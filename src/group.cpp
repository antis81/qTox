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

#include "group.h"
#include "friendlist.h"
#include "friend.h"
#include "core/core.h"
#include <QDebug>
#include <QTimer>

Group::Group(int GroupId, QString Name, bool IsAvGroupchat)
    : groupId(GroupId), nPeers{0}, avGroupchat{IsAvGroupchat}
{
    //in groupchats, we only notify on messages containing your name <-- dumb
    // sound notifications should be on all messages, but system popup notification
    // on naming is appropriate
    hasNewMessages = 0;
    userWasMentioned = 0;
}

Group::~Group()
{
}

void Group::updatePeer(int peerId, QString name)
{
    ToxId id = Core::getInstance()->getGroupPeerToxId(groupId, peerId);
    QString toxid = id.publicKey;
    peers[peerId] = name;
    toxids[toxid] = name;

    Friend *f = FriendList::findFriend(id);
    if (f != nullptr && f->hasAlias())
    {
        peers[peerId] = f->getDisplayedName();
        toxids[toxid] = f->getDisplayedName();
    }
    else
    {
        emit userListChanged(groupId);
    }
}

void Group::setName(const QString& name)
{
    emit titleChanged(groupId);
}

QString Group::getName() const
{
    // TODO: the group name has to be returned here!
    return tr("TODO: GROUPNAME MUST BE SET!");
}

void Group::regeneratePeerList()
{
    const Core* core = Core::getInstance();
    peers = core->getGroupPeerNames(groupId);
    toxids.clear();
    nPeers = peers.size();
    for (int i = 0; i < nPeers; ++i)
    {
        ToxId id = core->getGroupPeerToxId(groupId, i);
        ToxId self = core->getSelfId();
        if (id == self)
            selfPeerNum = i;

        QString toxid = id.publicKey;
        toxids[toxid] = peers[i];
        if (toxids[toxid].isEmpty())
            toxids[toxid] = tr("<Empty>", "Placeholder when someone's name in a group chat is empty");

        Friend *f = FriendList::findFriend(id);
        if (f != nullptr && f->hasAlias())
        {
            peers[i] = f->getDisplayedName();
            toxids[toxid] = f->getDisplayedName();
        }
    }

    emit userListChanged(groupId);
}

bool Group::isAvGroupchat() const
{
    return avGroupchat;
}

int Group::getGroupId() const
{
    return groupId;
}

int Group::getPeersCount() const
{
    return nPeers;
}

GroupChatForm *Group::getChatForm()
{
    return chatForm;
}

GroupWidget *Group::getGroupWidget()
{
    return widget;
}

QStringList Group::getPeerList() const
{
    return peers;
}

bool Group::isSelfPeerNumber(int num) const
{
    return num == selfPeerNum;
}

void Group::setEventFlag(int f)
{
    hasNewMessages = f;
}

int Group::getEventFlag() const
{
    return hasNewMessages;
}

void Group::setMentionedFlag(int f)
{
    userWasMentioned = f;
}

int Group::getMentionedFlag() const
{
    return userWasMentioned;
}

QString Group::resolveToxId(const ToxId &id) const
{
    QString key = id.publicKey;
    auto it = toxids.find(key);

    if (it != toxids.end())
        return *it;

    return QString();
}
