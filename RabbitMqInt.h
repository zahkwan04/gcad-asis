/**
 * A singleton class for consuming RabbitMQ messages.
 *
 * Copyright (C) Sapura Secured Technologies, 2025. All Rights Reserved.
 *
 * @file
 * @version $Id: RabbitMqInt.h 1913 2025-03-20 02:09:23Z zulzaidi $
 * @author Zulzaidi Atan
 */
#ifndef RABBITMQINT_H
#define RABBITMQINT_H

#include <amqp.h>

#include "Logger.h"
#include "PalLock.h"
#include "PalThread.h"

class RabbitMqInt
{
public:
    //callback signature for received message
    typedef void (*CallbackFn)(void *obj, const std::string &msg);

    /**
     * Continuously consumes and processes messages.
     */
    void consumeThread();

    /**
     * Initializes members and starts interface.
     *
     * @param[in] logger App Logger.
     * @param[in] host   Server hostname.
     * @param[in] port   Server port.
     * @param[in] user   Username.
     * @param[in] pwd    Password.
     * @return true if successful.
     */
    static bool init(Logger            *logger,
                     const std::string &host,
                     int                port,
                     const std::string &user,
                     const std::string &pwd);

    /**
     * Instantiates the singleton if it has not been created.
     *
     * @return The instance.
     */
    static RabbitMqInt &instance();

    /**
     * Registers a consumer for message consumption from an exchange or direct
     * queue.
     *
     * @param[in] src Exchange or direct queue name.
     * @param[in] key Exchange routing key, or empty for direct queue.
     * @param[in] obj Callback function owner.
     * @param[in] fn  Callback function for received messages.
     * @return true if successful.
     */
    static bool registerConsumer(const std::string &src,
                                 const std::string &key,
                                 void              *obj,
                                 CallbackFn         fn);

    /**
     * Deletes the single instance.
     */
    static void destroy();

private:
    //thread state
    enum eState
    {
        STATE_INVALID,
        STATE_STARTED,
        STATE_STOPPED,
        STATE_END
    };

    struct CbData
    {
        CbData() : obj(0), callback(0) {}
        CbData(void              *o,
               CallbackFn         f,
               int                c,
               const std::string &s,
               const std::string &k) :
        obj(o), callback(f), channel(c), src(s), key(k) {}

        void        *obj;
        CallbackFn   callback;
        int          channel;
        std::string  src;
        std::string  key;
    };
    typedef std::map<std::string, CbData> CbDataMapT; //key is consumer tag

    bool                    mValid;
    int                     mState;
    amqp_connection_state_t mConn;
    PalLock::LockT          mRegLock;       //guards registration process
    PalThread::ThreadT      mConsumeThread;
    CbDataMapT              mCbData;

    static RabbitMqInt    *sInstance;       //single class instance
    static Logger         *sLogger;
    static PalLock::LockT  sSingletonLock;  //guards instance creation

    /**
     * Constructor is private to prevent direct instantiation.
     */
    RabbitMqInt();

    ~RabbitMqInt();

    /**
     * Prevents copy construction.
     */
    RabbitMqInt(const RabbitMqInt &);

    /**
     * Prevents assignment operation.
     */
    RabbitMqInt &operator=(const RabbitMqInt &);

    /**
     * Starts interface.
     * Does nothing if already started.
     *
     * @param[in] host Server hostname.
     * @param[in] port Server port.
     * @param[in] user Authentication username.
     * @param[in] pwd  Authentication password.
     * @return true if successful.
     */
    bool start(const std::string &host,
               int                port,
               const std::string &user,
               const std::string &pwd);

    /**
     * Registers an exchange or direct queue.
     *
     * @param[in] src Exchange or direct queue name.
     * @param[in] key Exchange routing key, or empty for direct queue.
     * @param[in] obj Callback function owner.
     * @param[in] fn  Callback function for received messages.
     * @return true if successful.
     */
    bool regConsumer(const std::string &src,
                     const std::string &key,
                     void              *obj,
                     CallbackFn         fn);
};
#endif //RABBITMQINT_H
