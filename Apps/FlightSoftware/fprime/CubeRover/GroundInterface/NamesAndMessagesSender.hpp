#ifndef IRIS_NAMES_AND_MESSAGES_SENDER_HPP
#define IRIS_NAMES_AND_MESSAGES_SENDER_HPP

#include <stdint.h>
#include <Fw/Cfg/Config.hpp>
#include "NamesAndMessages.hpp"

namespace IrisNames {

class NamesAndMessageSender {
public:
  const char * m_pCreditsHead = CREDITS;
  const char * m_pMessagesHead = MESSAGES;

  const char * m_pName = CREDITS;
  const char * m_pMessage = MESSAGES;

  // What should be downlinked next: a name or a message?
  enum class DownlinkingNamesOrMessages {
    NAMES = 1,
    MESSAGES = 2
  } m_downlinkingNamesOrMessages;

  // Min period (ms) between name / message downlinks:
  uint32_t m_nameOrMessageDownlinkPeriod_ms = 5000;
  // Time the last name / message was sent:
  uint32_t m_prevNameOrMessageSendTime_ms = 0;
  // Whether or not a name/message log has been put into the packet buffer and is awaiting downlink:
  bool m_awaitingNameOrMessageDownlink = false;
  // Whether or not downlinking names and messages is allowed:
  bool m_namesAndMessagesAllowed = true;

  NamesAndMessageSender();
  virtual ~NamesAndMessageSender();

  // Downlinks the currently queued up name or message IF ALLOWED (by target period) based on the current time stamp:
  void downlinkNameOrMessageIfAllowed(uint32_t now_ms);
  
  // Advances the name or messeage head pointer if necessary (i.e. if a message is awaiting completion of a downlink).
  // Also resets the `m_awaitingNameOrMessageDownlink` flag.
  void advanceNameOrMessageHeadIfNeeded();

private:
  // Downlinks the currently queued up name or message.
  void downlinkNameOrMessage();
  /* Advances the name or message head pointer to the next
   * null-terminated name or message (or loops back around to the start).
   * Toggles what should be downlinked next: name or message.
   * Should only be called once we're sure the log packet made it to the
   * downlink interface (i.e. when higher priority data is being sent,
   * this will be tossed out).
   */
  void advanceNameOrMessageHead();

  /* Downlink a name.
   * `pHead` must be the head of a null-terminated cstring.
  **/
  void downlinkName(const char * pHead);
  /* Downlink a message.
   * `pHead` must be the head of a null-terminated cstring with
   * the message and messager (person sending the message) separated by a '\t'.
  **/
  void downlinkMessage(const char * pHead);
  /*
   * Advances to the next string in a collection of strings (or loops back to the start).
   * A collection of strings should be a series of consecutive c-style strings with
   * null-terminations in memory. See `CREDITS` and `MESSAGES` for examples.
  **/
  static void advanceStringCollectionHead(const char ** pHead, const char * pStringsCollection, unsigned int lenStringsCollection);

protected:
  /* Abstract internal impl. for downlinking a name(GI on rover, std::cout in testing).
   * `pNameString` must be the head of a null-terminated cstring.
  **/
  virtual void downlinkNameCoreImpl(const char * pNameString) = 0;
  /* Abstract internal impl. for downlinking a name(GI on rover, std::cout in testing).
   * `pMessagerString` must be the head of a null-terminated cstring for the person sending the message.
   * `pMessageString` must be the head of a null-terminated cstring for the string.
  **/
  virtual void downlinkMessageCoreImpl(const char * pMessagerString, const char * pMessageString) = 0;
};

} // namespace IrisNames
#endif // IRIS_NAMES_AND_MESSAGES_SENDER_HPP
