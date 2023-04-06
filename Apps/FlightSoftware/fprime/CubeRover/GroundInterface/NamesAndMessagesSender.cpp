#include "NamesAndMessagesSender.hpp"
#include "NamesAndMessages.hpp"

#include <string.h>

namespace IrisNames {

static char g_messagerWorkingBuffer[LEN_MAX_MESSAGER_NAME+1] = {0};

NamesAndMessageSender::NamesAndMessageSender() :
  m_downlinkingNamesOrMessages{DownlinkingNamesOrMessages::NAMES}
{};

NamesAndMessageSender::~NamesAndMessageSender(){};

void NamesAndMessageSender::downlinkName(const char * pHead){
    this->downlinkNameCoreImpl(pHead);
}

void NamesAndMessageSender::downlinkMessage(const char * pHead){
    // Find the boundary between the name of the messager and the message:
    const char * pNameStart = pHead;
    const char * pMsgStart = pHead;
    unsigned int lenName = 0;

    bool found = false;
    while(!found){
      if(*pMsgStart == '\t'){
        // found name message boundary
        // ptr will be advanced to start of message at loop end
        found = true;
      }

      if(pMsgStart == MESSAGES + LEN_MESSAGES - 1){
        // We're about to overrun the messages.
        // Message boundary must have been corrupted and missed. Abort.
        break;
      }

      if(pMsgStart == pHead + LEN_MAX_MESSAGER_NAME + 1){
        // We're about to pass beyond the farthest possible spot for the messager name.
        // Message boundary must have been corrupted and missed. Abort.
        break;
      }

      pMsgStart++;
    }

    if(found){
      // Copy name into terminated working buffer:
      lenName = pMsgStart-pHead-1;
      if(lenName > LEN_MAX_MESSAGER_NAME){
        lenName = LEN_MAX_MESSAGER_NAME;
      }
      memcpy(g_messagerWorkingBuffer, pNameStart, lenName);
      g_messagerWorkingBuffer[lenName] = '\0';  // terminate
    } else {
      // Couldn't find the name|message boundary. Must have been corrupted.
      // Treat the whole thing as a message (so we'll still get whatever's left of the name and the message):
      strcpy(g_messagerWorkingBuffer, "UNKNOWN");
      pMsgStart = pHead;
    }

    this->downlinkMessageCoreImpl(g_messagerWorkingBuffer, pMsgStart);
}

}
