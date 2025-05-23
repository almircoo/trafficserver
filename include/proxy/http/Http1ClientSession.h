/** @file

  A brief file description

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

/****************************************************************************

   Http1ClientSession.h

   Description:

 ****************************************************************************/

#pragma once

#include "api/InkAPIInternal.h"
#include "proxy/hdrs/HTTP.h"
#include "proxy/http/HttpConfig.h"
#include "proxy/IPAllow.h"
#include "proxy/ProxySession.h"
#include "proxy/http/Http1ClientTransaction.h"

#ifdef USE_HTTP_DEBUG_LISTS
extern ink_mutex debug_cs_list_mutex;
#endif

class HttpSM;
class Http1ClientSession : public ProxySession
{
public:
  using super = ProxySession; ///< Parent type.
  Http1ClientSession();
  ~Http1ClientSession() = default;

  // Implement ProxySession interface.
  void new_connection(NetVConnection *new_vc, MIOBuffer *iobuf, IOBufferReader *reader) override;
  void start() override;
  void release(ProxyTransaction *trans) override; // Indicate we are done with a transaction
  void release_transaction();
  void destroy() override;
  void free() override;

  bool attach_server_session(PoolableSession *ssession, bool transaction_done = true) override;

  // Implement VConnection interface.
  void do_io_close(int lerrno = -1) override;

  // Accessor Methods
  bool         allow_half_open() const;
  void         set_half_close_flag(bool flag) override;
  bool         get_half_close_flag() const override;
  int          get_transact_count() const override;
  bool         is_chunked_encoding_supported() const override;
  virtual bool is_outbound_transparent() const;

  PoolableSession *get_server_session() const override;
  const char      *get_protocol_string() const override;

  void increment_current_active_connections_stat() override;
  void decrement_current_active_connections_stat() override;

private:
  Http1ClientSession(Http1ClientSession &);

  ProxyTransaction *new_transaction() override;

  int state_keep_alive(int event, void *data);
  int state_slave_keep_alive(int event, void *data);
  int state_wait_for_close(int event, void *data);

  enum class C_Read_State {
    INIT,
    ACTIVE_READER,
    KEEP_ALIVE,
    HALF_CLOSED,
    CLOSED,
  };

  enum class Magic : uint32_t {
    ALIVE = 0x0123FEED,
    DEAD  = 0xDEADFEED,
  };

  Magic magic = Magic::DEAD;

  /// A monotonically increasing count of all transactions ever handled by the session.
  int  transact_count = 0;
  bool half_close     = false;
  bool conn_decrease  = false;

  MIOBuffer      *read_buffer = nullptr;
  IOBufferReader *_reader     = nullptr;

  C_Read_State read_state = C_Read_State::INIT;

  VIO *ka_vio       = nullptr;
  VIO *slave_ka_vio = nullptr;

  PoolableSession *bound_ss = nullptr;

  int released_transactions = 0;

  int64_t read_from_early_data = 0;

public:
  // Link<Http1ClientSession> debug_link;
  LINK(Http1ClientSession, debug_link);

  /// Set outbound connection to transparent.
  bool f_outbound_transparent = false;

  Http1ClientTransaction trans;
};

extern ClassAllocator<Http1ClientSession, true> http1ClientSessionAllocator;
