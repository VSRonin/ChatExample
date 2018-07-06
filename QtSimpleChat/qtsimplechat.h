#ifndef QTSIMPLECHAT_H
#define QTSIMPLECHAT_H

#include <QtGlobal>

#if defined(QTSIMPLECHAT_LIBRARY)
#  define QTSIMPLECHAT_EXPORT Q_DECL_EXPORT
#else
#  define QTSIMPLECHAT_EXPORT Q_DECL_IMPORT
#endif

template <class T>
class QSharedDataPointer;

class ChatMessage;
typedef QSharedDataPointer<ChatMessage> ChatMessagePointer;

#endif // QTSIMPLECHAT_H
