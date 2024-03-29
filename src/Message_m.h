//
// Generated file, do not edit! Created by nedtool 5.7 from Message.msg.
//

#ifndef __MESSAGE_M_H
#define __MESSAGE_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0507
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
    #include <ARQ.h>
// }}

/**
 * Struct generated from Message.msg:8 by nedtool.
 */
struct Event
{
    Event();
    uint8_t error;
    uint8_t message_id;
    ::omnetpp::opp_string content;
};

// helpers for local use
void __doPacking(omnetpp::cCommBuffer *b, const Event& a);
void __doUnpacking(omnetpp::cCommBuffer *b, Event& a);

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Event& obj) { __doPacking(b, obj); }
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Event& obj) { __doUnpacking(b, obj); }

/**
 * Enum generated from <tt>Message.msg:15</tt> by nedtool.
 * <pre>
 * enum Message_Type
 * {
 *     DATA = 0;
 *     ACK = 1;
 *     NACK = 2;
 * }
 * </pre>
 */
enum Message_Type {
    DATA = 0,
    ACK = 1,
    NACK = 2
};

/**
 * Struct generated from Message.msg:22 by nedtool.
 */
struct Header
{
    Header();
    uint8_t message_id;
    double sending_time;
    uint8_t message_type;
};

// helpers for local use
void __doPacking(omnetpp::cCommBuffer *b, const Header& a);
void __doUnpacking(omnetpp::cCommBuffer *b, Header& a);

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Header& obj) { __doPacking(b, obj); }
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Header& obj) { __doUnpacking(b, obj); }

/**
 * Struct generated from Message.msg:29 by nedtool.
 */
struct Frame
{
    Frame();
    Header header;
    Payload payload;
    uint8_t trailer;
};

// helpers for local use
void __doPacking(omnetpp::cCommBuffer *b, const Frame& a);
void __doUnpacking(omnetpp::cCommBuffer *b, Frame& a);

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Frame& obj) { __doPacking(b, obj); }
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Frame& obj) { __doUnpacking(b, obj); }

/**
 * Class generated from <tt>Message.msg:36</tt> by nedtool.
 * <pre>
 * packet Message_Event
 * {
 *     Event event;
 * }
 * </pre>
 */
class Message_Event : public ::omnetpp::cPacket
{
  protected:
    Event event;

  private:
    void copy(const Message_Event& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Message_Event&);

  public:
    Message_Event(const char *name=nullptr, short kind=0);
    Message_Event(const Message_Event& other);
    virtual ~Message_Event();
    Message_Event& operator=(const Message_Event& other);
    virtual Message_Event *dup() const override {return new Message_Event(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual Event& getEvent();
    virtual const Event& getEvent() const {return const_cast<Message_Event*>(this)->getEvent();}
    virtual void setEvent(const Event& event);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Message_Event& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Message_Event& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>Message.msg:41</tt> by nedtool.
 * <pre>
 * packet Message_Frame
 * {
 *     Frame frame;
 * }
 * </pre>
 */
class Message_Frame : public ::omnetpp::cPacket
{
  protected:
    Frame frame;

  private:
    void copy(const Message_Frame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Message_Frame&);

  public:
    Message_Frame(const char *name=nullptr, short kind=0);
    Message_Frame(const Message_Frame& other);
    virtual ~Message_Frame();
    Message_Frame& operator=(const Message_Frame& other);
    virtual Message_Frame *dup() const override {return new Message_Frame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual Frame& getFrame();
    virtual const Frame& getFrame() const {return const_cast<Message_Frame*>(this)->getFrame();}
    virtual void setFrame(const Frame& frame);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Message_Frame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Message_Frame& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>Message.msg:46</tt> by nedtool.
 * <pre>
 * packet Message_Duplicate_Frame
 * {
 *     Frame frame;
 *     bool lost;
 * }
 * </pre>
 */
class Message_Duplicate_Frame : public ::omnetpp::cPacket
{
  protected:
    Frame frame;
    bool lost;

  private:
    void copy(const Message_Duplicate_Frame& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Message_Duplicate_Frame&);

  public:
    Message_Duplicate_Frame(const char *name=nullptr, short kind=0);
    Message_Duplicate_Frame(const Message_Duplicate_Frame& other);
    virtual ~Message_Duplicate_Frame();
    Message_Duplicate_Frame& operator=(const Message_Duplicate_Frame& other);
    virtual Message_Duplicate_Frame *dup() const override {return new Message_Duplicate_Frame(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual Frame& getFrame();
    virtual const Frame& getFrame() const {return const_cast<Message_Duplicate_Frame*>(this)->getFrame();}
    virtual void setFrame(const Frame& frame);
    virtual bool getLost() const;
    virtual void setLost(bool lost);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Message_Duplicate_Frame& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Message_Duplicate_Frame& obj) {obj.parsimUnpack(b);}


#endif // ifndef __MESSAGE_M_H

