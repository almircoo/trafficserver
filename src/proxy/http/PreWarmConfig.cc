/** @file

  Configs for PreWarming Tunnel

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

#include "proxy/http/PreWarmConfig.h"
#include "proxy/http/PreWarmManager.h"

////
// PreWarmConfigParams
//
PreWarmConfigParams::PreWarmConfigParams()
{
  // RECU_RESTART_TS
  RecEstablishStaticConfigByte(enabled, "proxy.config.tunnel.prewarm.enabled");

  // RECU_DYNAMIC
  event_period = RecGetRecordInt("proxy.config.tunnel.prewarm.event_period").value_or(0);
  algorithm    = RecGetRecordInt("proxy.config.tunnel.prewarm.algorithm").value_or(0);
}

////
// PreWarmConfig
//
void
PreWarmConfig::startup()
{
  _config_update_handler = std::make_unique<ConfigUpdateHandler<PreWarmConfig>>();

  // dynamic configs
  _config_update_handler->attach("proxy.config.tunnel.prewarm.event_period");
  _config_update_handler->attach("proxy.config.tunnel.prewarm.algorithm");

  reconfigure();
}

void
PreWarmConfig::reconfigure()
{
  PreWarmConfigParams *params = new PreWarmConfigParams();
  _config_id                  = configProcessor.set(_config_id, params);

  prewarmManager.reconfigure();
}

PreWarmConfigParams *
PreWarmConfig::acquire()
{
  return static_cast<PreWarmConfigParams *>(configProcessor.get(_config_id));
}

void
PreWarmConfig::release(PreWarmConfigParams *params)
{
  configProcessor.release(_config_id, params);
}
