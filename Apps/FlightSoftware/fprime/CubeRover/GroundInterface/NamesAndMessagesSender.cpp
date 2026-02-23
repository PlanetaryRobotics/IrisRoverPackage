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


void NamesAndMessageSender::downlinkNameOrMessage(){
  switch(m_downlinkingNamesOrMessages){
    case DownlinkingNamesOrMessages::NAMES:
      this->downlinkName(m_pName);
      break;
    case DownlinkingNamesOrMessages::MESSAGES:
      this->downlinkMessage(m_pMessage);
      break;
  }
}


void NamesAndMessageSender::advanceStringCollectionHead(const char ** pHead, const char * pStringsCollection, unsigned int lenStringsCollection){
  // Advance to the next name:
  *pHead += strlen(*pHead) + 1; // advance past null terminator for this string
  // If that puts us past the end of the collection of string, reset:
  if(*pHead >= (pStringsCollection + lenStringsCollection)){
    *pHead = pStringsCollection;
  }
}


void NamesAndMessageSender::advanceNameOrMessageHead(){
  switch(m_downlinkingNamesOrMessages){
    case DownlinkingNamesOrMessages::NAMES:
      this->advanceStringCollectionHead(&m_pName, CREDITS, LEN_CREDITS);
      m_downlinkingNamesOrMessages = DownlinkingNamesOrMessages::MESSAGES;
      break;
    case DownlinkingNamesOrMessages::MESSAGES:
      this->advanceStringCollectionHead(&m_pMessage, MESSAGES, LEN_MESSAGES);
      m_downlinkingNamesOrMessages = DownlinkingNamesOrMessages::NAMES;
      break;
  }
}


void NamesAndMessageSender::downlinkNameOrMessageIfAllowed(uint32_t now_ms){
  // Only emit a name/message if enough time has elapsed since the last one AND we're not
  // currently waiting on one to be downlinked (don't want to flood the packet with these):
  if(
    m_namesAndMessagesAllowed &&
    !m_awaitingNameOrMessageDownlink &&
    (now_ms - m_prevNameOrMessageSendTime_ms) > m_nameOrMessageDownlinkPeriod_ms
  ){
    // Flag that we're now awaiting a downlink (do this BEFORE calling downlink since the call stack of that function may clear it):
    m_awaitingNameOrMessageDownlink = true;
    // Put a log into the downlink packet queue:
    downlinkNameOrMessage();
    // Flag when this emission happened so we won't emit another one until the period is exceeded:
    m_prevNameOrMessageSendTime_ms = now_ms;
  }
}


void NamesAndMessageSender::advanceNameOrMessageHeadIfNeeded(){
  if(m_awaitingNameOrMessageDownlink){
    m_awaitingNameOrMessageDownlink = false;
    advanceNameOrMessageHead();
  }
}

}
