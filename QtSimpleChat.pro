TEMPLATE = subdirs

SUBDIRS = QtSimpleChat QtSimpleChatClient QtSimpleChatServer QtSimpleChatServerThreaded

QtSimpleChatClient.depends = QtSimpleChat
QtSimpleChatServer.depends = QtSimpleChat
QtSimpleChatServerThreaded.depends = QtSimpleChat
