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

/*****************************************************************************
 *
 *  CacheControl.h - Interface to Cache Control system
 *
 *
 ****************************************************************************/

#pragma once

#include "iocore/eventsystem/EventSystem.h"
#include "proxy/ControlBase.h"
#include "tscore/Result.h"

struct RequestData;

const int CC_UNSET_TIME = -1;

#define CACHE_CONTROL_TIMEOUT (HRTIME_HOUR * 1)

//   Use 10 second time for purify testing under low
//     load to verify memory allocation
// #define CACHE_CONTROL_TIMEOUT            (HRTIME_SECOND*10)

enum class CacheControlType {
  INVALID = 0,
  REVALIDATE_AFTER,
  NEVER_CACHE,
  STANDARD_CACHE,
  IGNORE_NO_CACHE,
  IGNORE_CLIENT_NO_CACHE,
  IGNORE_SERVER_NO_CACHE,
  PIN_IN_CACHE,
  TTL_IN_CACHE,
  NUM_TYPES
};

struct matcher_line;

class CacheControlResult
{
public:
  CacheControlResult();
  void Print() const;

  // Data for external use
  //
  //   Describes the cache-control for a specific URL
  //
  int  revalidate_after;
  int  pin_in_cache_for;
  int  ttl_in_cache;
  bool never_cache                = false;
  bool ignore_client_no_cache     = false;
  bool ignore_server_no_cache     = false;
  bool ignore_client_cc_max_age   = true;
  int  cache_responses_to_cookies = -1; ///< Override for caching cookied responses.

  // Data for internal use only
  //
  //   Keeps track of the last line number
  //    on which a parameter was set
  //   Used to tell if a parameter needs to
  //    be overridden by something that appeared
  //    earlier in the config file
  //
  int reval_line         = -1;
  int never_line         = -1;
  int pin_line           = -1;
  int ttl_line           = -1;
  int ignore_client_line = -1;
  int ignore_server_line = -1;
};

inline CacheControlResult::CacheControlResult()
  : revalidate_after(CC_UNSET_TIME), pin_in_cache_for(CC_UNSET_TIME), ttl_in_cache(CC_UNSET_TIME)

{
}

class CacheControlRecord : public ControlBase
{
public:
  CacheControlRecord();
  CacheControlType directive                  = CacheControlType::INVALID;
  int              time_arg                   = 0;
  int              cache_responses_to_cookies = -1;
  Result           Init(matcher_line *line_info);
  void             UpdateMatch(CacheControlResult *result, RequestData *rdata);
  void             Print() const;
};

inline CacheControlRecord::CacheControlRecord() : ControlBase() {}

//
// API to outside world
//
class URL;
struct HttpConfigParams;
struct OverridableHttpConfigParams;

void getCacheControl(CacheControlResult *result, HttpRequestData *rdata, const OverridableHttpConfigParams *h_txn_conf,
                     char *tag = nullptr);
bool host_rule_in_CacheControlTable();
bool ip_rule_in_CacheControlTable();

void initCacheControl();
void reloadCacheControl();
