/**
 * Implementation for RabbitMQ integration, handling message consumption and
 * connection management.
 *
 * Copyright (C) Sapura Secured Technologies, 2025. All Rights Reserved.
 *
 * @file
 * @version $Id: RabbitMqInt.cpp 1913 2025-03-20 02:09:23Z zulzaidi $
 * @author Zulzaidi Atan
 */
#include <amqp_tcp_socket.h>
#if defined(_WIN32) || defined(WIN32)
#include <WinSock2.h> //struct timeval
#else
#include <sys/time.h> //struct timeval
#endif

#include "assert.h"
#include "Locker.h"
#include "RabbitMqInt.h"

using namespace std;

static const string LOGPREFIX("RabbitMqInt:: ");

RabbitMqInt    *RabbitMqInt::sInstance(0);
Logger         *RabbitMqInt::sLogger(0);
PalLock::LockT  RabbitMqInt::sSingletonLock;

static void *startConsumeThread(void *obj)
{
    static_cast<RabbitMqInt *>(obj)->consumeThread();
    return 0;
}

void RabbitMqInt::consumeThread()
{
    LOGGER_DEBUG(sLogger, LOGPREFIX << "consumeThread started.");
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    mState = STATE_STARTED;
    string tag;
    amqp_envelope_t env;
    while (mState != STATE_STOPPED)
    {
        PalLock::take(&mRegLock);
        amqp_maybe_release_buffers(mConn);
        if (amqp_consume_message(mConn, &env, &timeout, 0).reply_type ==
            AMQP_RESPONSE_NORMAL)
        {
            tag.assign(static_cast<char *>(env.consumer_tag.bytes),
                       env.consumer_tag.len);
            if (mCbData.count(tag) != 0)
                mCbData[tag].callback(mCbData[tag].obj,
                                      string(static_cast<char *>
                                                 (env.message.body.bytes),
                                             env.message.body.len));
            amqp_destroy_envelope(&env);
        }
        PalLock::release(&mRegLock);
    }
    LOGGER_DEBUG(sLogger, LOGPREFIX << "consumeThread stopped.");
    mState = STATE_END;
}

bool RabbitMqInt::init(Logger       *logger,
                       const string &host,
                       int           port,
                       const string &user,
                       const string &pwd)
{
    if (logger == 0)
    {
        assert("Bad param in RabbitMqInt::init" == 0);
        return false;
    }
    sLogger = logger;
    return instance().start(host, port, user, pwd);
}

RabbitMqInt &RabbitMqInt::instance()
{
    static bool isCreated = false;
    //a modified Double Checked Locking Pattern for thread-safe Singleton,
    //lock obtained only before/during creation
    if (!isCreated)
    {
        PalLock::take(&sSingletonLock);
        if (!isCreated) //thread-safety double check
        {
            sInstance = new RabbitMqInt();
            //ensure the compiler cannot reorder the statements and that the
            //flag is set true only after creation
            if (sInstance != 0)
                isCreated = true;
        }
        PalLock::release(&sSingletonLock);
    }
    return *sInstance;
}

bool RabbitMqInt::registerConsumer(const string &src,
                                   const string &key,
                                   void         *obj,
                                   CallbackFn    fn)
{
    if (sInstance == 0)
    {
        assert("Bad param in RabbitMqInt::registerConsumer" == 0);
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "registerConsumer: Class not initialized.");
        return false;
    }
    return sInstance->regConsumer(src, key, obj, fn);
}

void RabbitMqInt::destroy()
{
    delete sInstance;
    sInstance = 0;
}

RabbitMqInt::RabbitMqInt() :
mValid(false), mState(STATE_INVALID), mConn(0), mConsumeThread(0)
{
    PalLock::init(&mRegLock);
    if (sLogger == 0)
        assert("Bad param in RabbitMqInt::RabbitMqInt" == 0);
}

RabbitMqInt::~RabbitMqInt()
{
    mState = STATE_STOPPED;
    if (mConsumeThread != 0)
    {
        PalThread::stop(mConsumeThread);
        while (mState != STATE_END) //wait for thread to end
        {
            PalThread::msleep(10);
        }
    }
    if (mConn != 0)
    {
        for (const auto &it : mCbData)
        {
            amqp_channel_close(mConn, it.second.channel, AMQP_REPLY_SUCCESS);
        }
        amqp_connection_close(mConn, AMQP_REPLY_SUCCESS);
        amqp_destroy_connection(mConn);
    }
    PalLock::destroy(&mRegLock);
}

bool RabbitMqInt::start(const string &host,
                        int           port,
                        const string &user,
                        const string &pwd)
{
    if (mState == STATE_STARTED)
        return true;
    mConn = amqp_new_connection();
    if (mConn == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "start: Failed to create AMQP connection object.");
        return false;
    }
    //connect and login to the broker with a 128kB (131072 bytes) max frame size
    //as recommended by the lib
    auto *socket = amqp_tcp_socket_new(mConn);
    if (socket != 0 &&
        amqp_socket_open(socket, host.c_str(), port) == AMQP_STATUS_OK &&
        amqp_login(mConn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                   user.c_str(), pwd.c_str()).reply_type == AMQP_RESPONSE_NORMAL)
    {
        mValid = true;
        PalThread::start(&mConsumeThread, startConsumeThread, this);
        return true;
    }
    LOGGER_ERROR(sLogger, LOGPREFIX << "start: Failed to start interface.");
    if (socket != 0)
        amqp_connection_close(mConn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(mConn);
    mConn = 0;
    return false;
}

bool RabbitMqInt::regConsumer(const string &src,
                              const string &key,
                              void         *obj,
                              CallbackFn    fn)
{
    if (!mValid)
        return false;
    Locker lock(&mRegLock);
    for (const auto &it : mCbData)
    {
        if (it.second.src == src && it.second.key == key)
            return true; //already registered - do nothing
    }
    static int channel = 0;
    amqp_channel_open(mConn, ++channel);
    if (amqp_get_rpc_reply(mConn).reply_type != AMQP_RESPONSE_NORMAL)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX
                     << "regConsumer: Failed to open AMQP channel " << channel);
        return false;
    }
    amqp_bytes_t queue;
    if (key.empty())
    {
        queue = amqp_cstring_bytes(src.c_str());
    }
    else
    {
        amqp_exchange_declare(mConn, channel, amqp_cstring_bytes(src.c_str()),
                              amqp_cstring_bytes("topic"), 0, 0, 0, 0,
                              amqp_empty_table);
        if (amqp_get_rpc_reply(mConn).reply_type != AMQP_RESPONSE_NORMAL)
        {
            LOGGER_ERROR(sLogger, LOGPREFIX
                         << "regConsumer: Failed to declare AMQP exchange for '"
                         << src << ":" << key << "'.");
            amqp_channel_close(mConn, channel, AMQP_REPLY_SUCCESS);
            return false;
        }
        amqp_queue_declare_ok_t *q = amqp_queue_declare(mConn, channel,
                                                        amqp_empty_bytes, 0, 0,
                                                        1, 1, amqp_empty_table);
        if (q == 0)
        {
            LOGGER_ERROR(sLogger, LOGPREFIX << "regConsumer: Failed to declare "
                         << "queue for '" << src << ":" << key << "'.");
            amqp_channel_close(mConn, channel, AMQP_REPLY_SUCCESS);
            return false;
        }
        queue = q->queue;
        amqp_queue_bind(mConn, channel, queue, amqp_cstring_bytes(src.c_str()),
                        amqp_cstring_bytes(key.c_str()), amqp_empty_table);
        if (amqp_get_rpc_reply(mConn).reply_type != AMQP_RESPONSE_NORMAL)
        {
            LOGGER_ERROR(sLogger, LOGPREFIX
                         << "regConsumer: Failed to bind queue.");
            amqp_channel_close(mConn, channel, AMQP_REPLY_SUCCESS);
            return false;
        }
    }
    amqp_basic_consume_ok_t *ok = amqp_basic_consume(mConn, channel, queue,
                                                     amqp_empty_bytes, 0, 1, 0,
                                                     amqp_empty_table);
    if (ok == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX << "regConsumer: Failed to consume for "
                     "'" << src << ((key.empty())? "": ":") << key << "'.");
        amqp_channel_close(mConn, channel, AMQP_REPLY_SUCCESS);
        return false;
    }
    string tag(static_cast<char *>(ok->consumer_tag.bytes),
               ok->consumer_tag.len);
    mCbData[tag] = CbData(obj, fn, channel, src, key);
    LOGGER_DEBUG(sLogger, LOGPREFIX << "regConsumer: Registered tag " << tag
                 << " for '" << src << ((key.empty())? "": ":") << key
                 << "'.");
    return true;
}
