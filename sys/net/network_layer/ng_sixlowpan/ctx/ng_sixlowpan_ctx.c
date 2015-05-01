/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <stdbool.h>
#include <inttypes.h>

#include "mutex.h"
#include "net/ng_sixlowpan/ctx.h"
#include "vtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static ng_sixlowpan_ctx_t _ctxs[NG_SIXLOWPAN_CTX_SIZE];
static uint32_t _ctx_inval_times[NG_SIXLOWPAN_CTX_SIZE];
static mutex_t _ctx_mutex = MUTEX_INIT;

static uint32_t _current_minute(void);
static void _update_lifetime(unsigned int id);

#if ENABLE_DEBUG
static char ipv6str[NG_IPV6_ADDR_MAX_STR_LEN];
#endif

static inline bool _still_valid(unsigned int id)
{
    _update_lifetime(id);
    return (_ctxs[id].ltime > 0);
}

ng_sixlowpan_ctx_t *ng_sixlowpan_ctx_lookup_addr(const ng_ipv6_addr_t *addr)
{
    uint8_t best = 0;
    ng_sixlowpan_ctx_t *res = NULL;

    mutex_lock(&_ctx_mutex);

    for (unsigned int id = 0; id < NG_SIXLOWPAN_CTX_SIZE; id++) {
        if (_still_valid(id)) {
            uint8_t match = ng_ipv6_addr_match_prefix(&_ctxs[id].prefix, addr);

            if ((_ctxs[id].prefix_len <= match) && (match > best)) {
                best = match;
                res = &(_ctxs[id]);
            }
        }
    }

    mutex_unlock(&_ctx_mutex);

#if ENABLE_DEBUG
    if (res != NULL) {
        DEBUG("6lo ctx: found context (%u, %s/%" PRIu8 ") ", res->id,
              ng_ipv6_addr_to_str(ipv6str, &res->prefix, sizeof(ipv6str)),
              res->prefix_len);
        DEBUG("for address %s\n", ng_ipv6_addr_to_str(ipv6str, addr, sizeof(ipv6str)));
    }
#endif

    return res;
}

ng_sixlowpan_ctx_t *ng_sixlowpan_ctx_lookup_id(uint8_t id)
{
    if (id >= NG_SIXLOWPAN_CTX_SIZE) {
        return NULL;
    }

    mutex_lock(&_ctx_mutex);

    if (_still_valid((unsigned int)id)) {
        DEBUG("6lo ctx: found context (%u, %s/%" PRIu8 ")\n", id,
              ng_ipv6_addr_to_str(ipv6str, &_ctxs[id].prefix, sizeof(ipv6str)),
              _ctxs[id].prefix_len);
        mutex_unlock(&_ctx_mutex);
        return &(_ctxs[id]);
    }

    mutex_unlock(&_ctx_mutex);

    return NULL;
}

ng_sixlowpan_ctx_t *ng_sixlowpan_ctx_update(uint8_t id, const ng_ipv6_addr_t *prefix,
                                            uint8_t prefix_len, uint16_t ltime)
{
    if ((id >= NG_SIXLOWPAN_CTX_SIZE)) {
        return NULL;
    }

    mutex_lock(&_ctx_mutex);

    _ctxs[id].ltime = ltime;

    if (ltime == 0) {
        mutex_unlock(&_ctx_mutex);
        DEBUG("6lo ctx: remove context (%u, %s/%" PRIu8 ")\n", id,
              ng_ipv6_addr_to_str(ipv6str, &_ctxs[id].prefix, sizeof(ipv6str)),
              _ctxs[id].prefix_len);
        return NULL;
    }

    /* test prefix_len now so that invalidation is possible regardless of the
     * value. */
    if (prefix_len == 0) {
        mutex_unlock(&_ctx_mutex);
        _ctxs[id].ltime = 0;
        return NULL;
    }

    if (prefix_len > NG_IPV6_ADDR_BIT_LEN) {
        _ctxs[id].prefix_len = NG_IPV6_ADDR_BIT_LEN;
    }
    else {
        _ctxs[id].prefix_len = prefix_len;
    }

    _ctxs[id].id = id;

    if (!ng_ipv6_addr_equal(&(_ctxs[id].prefix), prefix)) {
        ng_ipv6_addr_set_unspecified(&(_ctxs[id].prefix));
        ng_ipv6_addr_init_prefix(&(_ctxs[id].prefix), prefix,
                                 _ctxs[id].prefix_len);
    }
    DEBUG("6lo ctx: update context (%u, %s/%" PRIu8 "), lifetime: %" PRIu16 " min\n",
          id, ng_ipv6_addr_to_str(ipv6str, &_ctxs[id].prefix, sizeof(ipv6str)),
          _ctxs[id].prefix_len, _ctxs[id].ltime);
    _ctx_inval_times[id] = ltime + _current_minute();

    mutex_unlock(&_ctx_mutex);

    return &(_ctxs[id]);
}

static uint32_t _current_minute(void)
{
    timex_t now;
    vtimer_now(&now);
    return now.seconds / 60;
}

static void _update_lifetime(unsigned int id)
{
    uint32_t now;

    if (_ctxs[id].ltime == 0) {
        return;
    }

    now = _current_minute();

    if (now >= _ctx_inval_times[id]) {
        DEBUG("6lo ctx: context %u was invalidated\n", id);
        _ctxs[id].ltime = 0;
    }
    else {
        _ctxs[id].ltime = (uint16_t)(_ctx_inval_times[id] - now);
    }
}

#ifdef TEST_SUITES
#include <string.h>

void ng_sixlowpan_ctx_reset(void)
{
    memset(_ctxs, 0, sizeof(_ctxs));
}
#endif

/** @} */
