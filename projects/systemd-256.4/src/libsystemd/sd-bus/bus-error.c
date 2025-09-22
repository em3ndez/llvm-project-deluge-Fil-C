/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sd-bus.h"

#include "alloc-util.h"
#include "bus-common-errors.h"
#include "bus-error.h"
#include "errno-list.h"
#include "errno-util.h"
#include "string-util.h"
#include "strv.h"

const sd_bus_error_map bus_standard_errors[] = {
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_FAILED,                             EACCES),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_NO_MEMORY,                          ENOMEM),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_SERVICE_UNKNOWN,                    EHOSTUNREACH),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_NAME_HAS_NO_OWNER,                  ENXIO),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_NO_REPLY,                           ETIMEDOUT),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_IO_ERROR,                           EIO),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_BAD_ADDRESS,                        EADDRNOTAVAIL),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_NOT_SUPPORTED,                      EOPNOTSUPP),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_LIMITS_EXCEEDED,                    ENOBUFS),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_ACCESS_DENIED,                      EACCES),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_AUTH_FAILED,                        EACCES),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_NO_SERVER,                          EHOSTDOWN),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_TIMEOUT,                            ETIMEDOUT),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_NO_NETWORK,                         ENONET),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_ADDRESS_IN_USE,                     EADDRINUSE),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_DISCONNECTED,                       ECONNRESET),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_INVALID_ARGS,                       EINVAL),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_FILE_NOT_FOUND,                     ENOENT),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_FILE_EXISTS,                        EEXIST),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_UNKNOWN_METHOD,                     EBADR),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_UNKNOWN_OBJECT,                     EBADR),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_UNKNOWN_INTERFACE,                  EBADR),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_UNKNOWN_PROPERTY,                   EBADR),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_PROPERTY_READ_ONLY,                 EROFS),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_UNIX_PROCESS_ID_UNKNOWN,            ESRCH),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_INVALID_SIGNATURE,                  EINVAL),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_INCONSISTENT_MESSAGE,               EBADMSG),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_TIMED_OUT,                          ETIMEDOUT),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_MATCH_RULE_NOT_FOUND,               ENOENT),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_MATCH_RULE_INVALID,                 EINVAL),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_INTERACTIVE_AUTHORIZATION_REQUIRED, EACCES),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_INVALID_FILE_CONTENT,               EINVAL),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_SELINUX_SECURITY_CONTEXT_UNKNOWN,   ESRCH),
        SD_BUS_ERROR_MAP(SD_BUS_ERROR_OBJECT_PATH_IN_USE,                 EBUSY),

        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_UNIT,                 ENOENT),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_UNIT_FOR_PID,              ESRCH),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_UNIT_FOR_INVOCATION_ID,    ENOENT),
        SD_BUS_ERROR_MAP(BUS_ERROR_UNIT_EXISTS,                  EEXIST),
        SD_BUS_ERROR_MAP(BUS_ERROR_LOAD_FAILED,                  EIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_BAD_UNIT_SETTING,             ENOEXEC),
        SD_BUS_ERROR_MAP(BUS_ERROR_JOB_FAILED,                   EREMOTEIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_JOB,                  ENOENT),
        SD_BUS_ERROR_MAP(BUS_ERROR_NOT_SUBSCRIBED,               EINVAL),
        SD_BUS_ERROR_MAP(BUS_ERROR_ALREADY_SUBSCRIBED,           EINVAL),
        SD_BUS_ERROR_MAP(BUS_ERROR_ONLY_BY_DEPENDENCY,           EINVAL),
        SD_BUS_ERROR_MAP(BUS_ERROR_TRANSACTION_JOBS_CONFLICTING, EDEADLK),
        SD_BUS_ERROR_MAP(BUS_ERROR_TRANSACTION_ORDER_IS_CYCLIC,  EDEADLK),
        SD_BUS_ERROR_MAP(BUS_ERROR_TRANSACTION_IS_DESTRUCTIVE,   EDEADLK),
        SD_BUS_ERROR_MAP(BUS_ERROR_UNIT_MASKED,                  ERFKILL),
        SD_BUS_ERROR_MAP(BUS_ERROR_UNIT_GENERATED,               EADDRNOTAVAIL),
        SD_BUS_ERROR_MAP(BUS_ERROR_UNIT_LINKED,                  ELOOP),
        SD_BUS_ERROR_MAP(BUS_ERROR_JOB_TYPE_NOT_APPLICABLE,      EBADR),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_ISOLATION,                 EPERM),
        SD_BUS_ERROR_MAP(BUS_ERROR_SHUTTING_DOWN,                ECANCELED),
        SD_BUS_ERROR_MAP(BUS_ERROR_SCOPE_NOT_RUNNING,            EHOSTDOWN),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_DYNAMIC_USER,         ESRCH),
        SD_BUS_ERROR_MAP(BUS_ERROR_NOT_REFERENCED,               EUNATCH),
        SD_BUS_ERROR_MAP(BUS_ERROR_DISK_FULL,                    ENOSPC),
        SD_BUS_ERROR_MAP(BUS_ERROR_FILE_DESCRIPTOR_STORE_DISABLED,
                                                                 EHOSTDOWN),

        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_MACHINE,              ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_IMAGE,                ENOENT),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_MACHINE_FOR_PID,           ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_MACHINE_EXISTS,               EEXIST),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_PRIVATE_NETWORKING,        ENOSYS),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_USER_MAPPING,         ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_GROUP_MAPPING,        ENXIO),

        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_PORTABLE_IMAGE,       ENOENT),
        SD_BUS_ERROR_MAP(BUS_ERROR_BAD_PORTABLE_IMAGE_TYPE,      EMEDIUMTYPE),

        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_SESSION,              ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SESSION_FOR_PID,           ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_USER,                 ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_USER_FOR_PID,              ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_SEAT,                 ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_SESSION_NOT_ON_SEAT,          EINVAL),
        SD_BUS_ERROR_MAP(BUS_ERROR_NOT_IN_CONTROL,               EINVAL),
        SD_BUS_ERROR_MAP(BUS_ERROR_DEVICE_IS_TAKEN,              EINVAL),
        SD_BUS_ERROR_MAP(BUS_ERROR_DEVICE_NOT_TAKEN,             EINVAL),
        SD_BUS_ERROR_MAP(BUS_ERROR_OPERATION_IN_PROGRESS,        EINPROGRESS),
        SD_BUS_ERROR_MAP(BUS_ERROR_SLEEP_VERB_NOT_SUPPORTED,     EOPNOTSUPP),
        SD_BUS_ERROR_MAP(BUS_ERROR_SESSION_BUSY,                 EBUSY),
        SD_BUS_ERROR_MAP(BUS_ERROR_NOT_YOUR_DEVICE,              EPERM),

        SD_BUS_ERROR_MAP(BUS_ERROR_AUTOMATIC_TIME_SYNC_ENABLED,  EALREADY),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_NTP_SUPPORT,               EOPNOTSUPP),

        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_PROCESS,              ESRCH),

        SD_BUS_ERROR_MAP(BUS_ERROR_NO_NAME_SERVERS,              ESRCH),
        SD_BUS_ERROR_MAP(BUS_ERROR_INVALID_REPLY,                EINVAL),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_RR,                   ENOENT),
        SD_BUS_ERROR_MAP(BUS_ERROR_CNAME_LOOP,                   EDEADLK),
        SD_BUS_ERROR_MAP(BUS_ERROR_ABORTED,                      ECANCELED),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_SERVICE,              EUNATCH),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNSSEC_FAILED,                EHOSTUNREACH),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_TRUST_ANCHOR,              EHOSTUNREACH),
        SD_BUS_ERROR_MAP(BUS_ERROR_RR_TYPE_UNSUPPORTED,          EOPNOTSUPP),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_LINK,                 ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_LINK_BUSY,                    EBUSY),
        SD_BUS_ERROR_MAP(BUS_ERROR_NETWORK_DOWN,                 ENETDOWN),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SOURCE,                    ESRCH),
        SD_BUS_ERROR_MAP(BUS_ERROR_STUB_LOOP,                    ELOOP),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_DNSSD_SERVICE,        ENOENT),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNSSD_SERVICE_EXISTS,         EEXIST),

        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_FORMERR,                  EBADMSG),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_SERVFAIL,                 EHOSTDOWN),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_NXDOMAIN,                 ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_NOTIMP,                   ENOSYS),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_REFUSED,                  EACCES),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_YXDOMAIN,                 EEXIST),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_YRRSET,                   EEXIST),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_NXRRSET,                  ENOENT),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_NOTAUTH,                  EACCES),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_NOTZONE,                  EREMOTE),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_BADVERS,                  EBADMSG),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_BADKEY,                   EKEYREJECTED),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_BADTIME,                  EBADMSG),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_BADMODE,                  EBADMSG),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_BADNAME,                  EBADMSG),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_BADALG,                   EBADMSG),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_BADTRUNC,                 EBADMSG),
        SD_BUS_ERROR_MAP(BUS_ERROR_DNS_BADCOOKIE,                EBADR),

        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_TRANSFER,             ENXIO),
        SD_BUS_ERROR_MAP(BUS_ERROR_TRANSFER_IN_PROGRESS,         EBUSY),

        SD_BUS_ERROR_MAP(BUS_ERROR_NO_PRODUCT_UUID,              EOPNOTSUPP),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_HARDWARE_SERIAL,           EOPNOTSUPP),
        SD_BUS_ERROR_MAP(BUS_ERROR_FILE_IS_PROTECTED,            EACCES),
        SD_BUS_ERROR_MAP(BUS_ERROR_READ_ONLY_FILESYSTEM,         EROFS),

        SD_BUS_ERROR_MAP(BUS_ERROR_SPEED_METER_INACTIVE,         EOPNOTSUPP),
        SD_BUS_ERROR_MAP(BUS_ERROR_UNMANAGED_INTERFACE,          EOPNOTSUPP),
        SD_BUS_ERROR_MAP(BUS_ERROR_NETWORK_ALREADY_RELOADING,    EBUSY),

        SD_BUS_ERROR_MAP(BUS_ERROR_NO_SUCH_HOME,                 EEXIST),
        SD_BUS_ERROR_MAP(BUS_ERROR_UID_IN_USE,                   EEXIST),
        SD_BUS_ERROR_MAP(BUS_ERROR_USER_NAME_EXISTS,             EEXIST),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_EXISTS,                  EEXIST),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_ALREADY_ACTIVE,          EALREADY),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_ALREADY_FIXATED,         EALREADY),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_UNFIXATED,               EADDRNOTAVAIL),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_NOT_ACTIVE,              EALREADY),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_ABSENT,                  EREMOTE),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_BUSY,                    EBUSY),
        SD_BUS_ERROR_MAP(BUS_ERROR_BAD_PASSWORD,                 ENOKEY),
        SD_BUS_ERROR_MAP(BUS_ERROR_LOW_PASSWORD_QUALITY,         EUCLEAN),
        SD_BUS_ERROR_MAP(BUS_ERROR_BAD_PASSWORD_AND_NO_TOKEN,    EBADSLT),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOKEN_PIN_NEEDED,             ENOANO),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOKEN_PROTECTED_AUTHENTICATION_PATH_NEEDED, ERFKILL),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOKEN_USER_PRESENCE_NEEDED,   EMEDIUMTYPE),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOKEN_USER_VERIFICATION_NEEDED, ENOCSI),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOKEN_ACTION_TIMEOUT,         ENOSTR),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOKEN_PIN_LOCKED,             EOWNERDEAD),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOKEN_BAD_PIN,                ENOLCK),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOKEN_BAD_PIN_FEW_TRIES_LEFT, ETOOMANYREFS),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOKEN_BAD_PIN_ONE_TRY_LEFT,   EUCLEAN),
        SD_BUS_ERROR_MAP(BUS_ERROR_BAD_SIGNATURE,                EKEYREJECTED),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_RECORD_MISMATCH,         EUCLEAN),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_RECORD_DOWNGRADE,        ESTALE),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_RECORD_SIGNED,           EROFS),
        SD_BUS_ERROR_MAP(BUS_ERROR_BAD_HOME_SIZE,                ERANGE),
        SD_BUS_ERROR_MAP(BUS_ERROR_NO_PRIVATE_KEY,               ENOPKG),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_LOCKED,                  ENOEXEC),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_NOT_LOCKED,              ENOEXEC),
        SD_BUS_ERROR_MAP(BUS_ERROR_TOO_MANY_OPERATIONS,          ENOBUFS),
        SD_BUS_ERROR_MAP(BUS_ERROR_AUTHENTICATION_LIMIT_HIT,     ETOOMANYREFS),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_CANT_AUTHENTICATE,       EKEYREVOKED),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_IN_USE,                  EADDRINUSE),
        SD_BUS_ERROR_MAP(BUS_ERROR_REBALANCE_NOT_NEEDED,         EALREADY),
        SD_BUS_ERROR_MAP(BUS_ERROR_HOME_NOT_REFERENCED,          EBADR),

        SD_BUS_ERROR_MAP_END
};

/* Additional maps registered with sd_bus_error_add_map() are in this
 * NULL terminated array */
static const sd_bus_error_map **additional_error_maps = NULL;

static int bus_error_name_to_errno(const char *name) {
        const sd_bus_error_map **map, *m;
        const char *p;
        int r;

        if (!name)
                return EINVAL;

        p = startswith(name, "System.Error.");
        if (p) {
                r = errno_from_name(p);
                if (r < 0)
                        return EIO;

                return r;
        }

        if (additional_error_maps)
                for (map = additional_error_maps; *map; map++)
                        for (m = *map;; m++) {
                                /* For additional error maps the end marker is actually the end marker */
                                if (m->code == BUS_ERROR_MAP_END_MARKER)
                                        break;

                                if (streq(m->name, name)) {
                                        assert(m->code > 0);
                                        return m->code;
                                }
                        }

        m = bus_standard_errors;
        while (m->code != BUS_ERROR_MAP_END_MARKER) {
                if (streq(m->name, name)) {
                        assert(m->code > 0);
                        return m->code;
                }

                m++;
        }

        return EIO;
}

static sd_bus_error errno_to_bus_error_const(int error) {

        if (error < 0)
                error = -error;

        switch (error) {

        case ENOMEM:
                return BUS_ERROR_OOM;

        case EPERM:
        case EACCES:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_ACCESS_DENIED, "Access denied");

        case EINVAL:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_INVALID_ARGS, "Invalid argument");

        case ESRCH:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_UNIX_PROCESS_ID_UNKNOWN, "No such process");

        case ENOENT:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_FILE_NOT_FOUND, "File not found");

        case EEXIST:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_FILE_EXISTS, "File exists");

        case ETIMEDOUT:
        case ETIME:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_TIMEOUT, "Timed out");

        case EIO:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_IO_ERROR, "Input/output error");

        case ENETRESET:
        case ECONNABORTED:
        case ECONNRESET:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_DISCONNECTED, "Disconnected");

        case EOPNOTSUPP:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_NOT_SUPPORTED, "Not supported");

        case EADDRNOTAVAIL:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_BAD_ADDRESS, "Address not available");

        case ENOBUFS:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_LIMITS_EXCEEDED, "Limits exceeded");

        case EADDRINUSE:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_ADDRESS_IN_USE, "Address in use");

        case EBADMSG:
                return SD_BUS_ERROR_MAKE_CONST(SD_BUS_ERROR_INCONSISTENT_MESSAGE, "Inconsistent message");
        }

        return SD_BUS_ERROR_NULL;
}

static int errno_to_bus_error_name_new(int error, char **ret) {
        const char *name;
        char *n;

        if (error < 0)
                error = -error;

        name = errno_to_name(error);
        if (!name)
                return 0;

        n = strjoin("System.Error.", name);
        if (!n)
                return -ENOMEM;

        *ret = n;
        return 1;
}

bool bus_error_is_dirty(sd_bus_error *e) {
        if (!e)
                return false;

        return e->name || e->message || e->_need_free != 0;
}

_public_ void sd_bus_error_free(sd_bus_error *e) {
        if (!e)
                return;

        if (e->_need_free > 0) {
                free((void*) e->name);
                free((void*) e->message);
        }

        *e = SD_BUS_ERROR_NULL;
}

_public_ int sd_bus_error_set(sd_bus_error *e, const char *name, const char *message) {
        int r;

        if (!name)
                return 0;

        if (e) {
                assert_return(!bus_error_is_dirty(e), -EINVAL);

                e->name = strdup(name);
                if (!e->name) {
                        *e = BUS_ERROR_OOM;
                        return -ENOMEM;
                }

                if (message)
                        e->message = strdup(message);

                e->_need_free = 1;
        }

        r = bus_error_name_to_errno(name);
        assert(r > 0);
        return -r;
}

_public_ int sd_bus_error_setfv(sd_bus_error *e, const char *name, const char *format, va_list ap) {
        int r;

        if (!name)
                return 0;

        if (e) {
                assert_return(!bus_error_is_dirty(e), -EINVAL);

                e->name = strdup(name);
                if (!e->name) {
                        *e = BUS_ERROR_OOM;
                        return -ENOMEM;
                }

                if (format) {
                        _cleanup_free_ char *mesg = NULL;

                        /* If we hit OOM on formatting the pretty message, we ignore
                         * this, since we at least managed to write the error name */

                        if (vasprintf(&mesg, format, ap) >= 0)
                                e->message = TAKE_PTR(mesg);
                }

                e->_need_free = 1;
        }

        r = bus_error_name_to_errno(name);
        assert(r > 0);
        return -r;
}

_public_ int sd_bus_error_setf(sd_bus_error *e, const char *name, const char *format, ...) {
        int r;

        if (format) {
                va_list ap;

                va_start(ap, format);
                r = sd_bus_error_setfv(e, name, format, ap);
                if (name)
                        assert(r < 0);
                va_end(ap);

                return r;
        }

        r = sd_bus_error_set(e, name, NULL);
        if (name)
                assert(r < 0);
        return r;
}

_public_ int sd_bus_error_copy(sd_bus_error *dest, const sd_bus_error *e) {

        if (!sd_bus_error_is_set(e))
                return 0;
        if (!dest)
                goto finish;

        assert_return(!bus_error_is_dirty(dest), -EINVAL);

        /*
         * _need_free  < 0 indicates that the error is temporarily const, needs deep copying
         * _need_free == 0 indicates that the error is perpetually const, needs no deep copying
         * _need_free  > 0 indicates that the error is fully dynamic, needs deep copying
         */

        if (e->_need_free == 0)
                *dest = *e;
        else {
                dest->name = strdup(e->name);
                if (!dest->name) {
                        *dest = BUS_ERROR_OOM;
                        return -ENOMEM;
                }

                if (e->message)
                        dest->message = strdup(e->message);

                dest->_need_free = 1;
        }

finish:
        return -bus_error_name_to_errno(e->name);
}

_public_ int sd_bus_error_move(sd_bus_error *dest, sd_bus_error *e) {
        int r;

        if (!sd_bus_error_is_set(e)) {

                if (dest)
                        *dest = SD_BUS_ERROR_NULL;

                return 0;
        }

        r = -bus_error_name_to_errno(e->name);

        if (dest) {
                *dest = *e;
                *e = SD_BUS_ERROR_NULL;
        } else
                sd_bus_error_free(e);

        return r;
}

_public_ int sd_bus_error_set_const(sd_bus_error *e, const char *name, const char *message) {
        if (!name)
                return 0;
        if (!e)
                goto finish;

        assert_return(!bus_error_is_dirty(e), -EINVAL);

        *e = SD_BUS_ERROR_MAKE_CONST(name, message);

finish:
        return -bus_error_name_to_errno(name);
}

_public_ int sd_bus_error_is_set(const sd_bus_error *e) {
        if (!e)
                return 0;

        return !!e->name;
}

_public_ int sd_bus_error_has_name(const sd_bus_error *e, const char *name) {
        if (!e)
                return 0;

        return streq_ptr(e->name, name);
}

_public_ int sd_bus_error_has_names_sentinel(const sd_bus_error *e, ...) {
        if (!e || !e->name)
                return 0;

        va_list ap;
        const char *p;

        va_start(ap, e);
        while ((p = va_arg(ap, const char *)))
                if (streq(p, e->name))
                        break;
        va_end(ap);
        return !!p;
}

_public_ int sd_bus_error_get_errno(const sd_bus_error* e) {
        if (!e || !e->name)
                return 0;

        return bus_error_name_to_errno(e->name);
}

static void bus_error_strerror(sd_bus_error *e, int error) {
        size_t k = 64;
        char *m;

        assert(e);

        for (;;) {
                char *x;

                m = new(char, k);
                if (!m)
                        return;

                errno = 0;
                x = strerror_r(error, m, k);
                if (errno == ERANGE || strlen(x) >= k - 1) {
                        free(m);
                        k *= 2;
                        continue;
                }

                if (errno) {
                        free(m);
                        return;
                }

                if (x == m) {
                        if (e->_need_free > 0) {
                                /* Error is already dynamic, let's just update the message */
                                free((char*) e->message);
                                e->message = x;

                        } else {
                                char *t;
                                /* Error was const so far, let's make it dynamic, if we can */

                                t = strdup(e->name);
                                if (!t) {
                                        free(m);
                                        return;
                                }

                                e->_need_free = 1;
                                e->name = t;
                                e->message = x;
                        }
                } else {
                        free(m);

                        if (e->_need_free > 0) {
                                char *t;

                                /* Error is dynamic, let's hence make the message also dynamic */
                                t = strdup(x);
                                if (!t)
                                        return;

                                free((char*) e->message);
                                e->message = t;
                        } else {
                                /* Error is const, hence we can just override */
                                e->message = x;
                        }
                }

                return;
        }
}

_public_ int sd_bus_error_set_errno(sd_bus_error *e, int error) {

        if (error < 0)
                error = -error;

        if (!e)
                return -error;
        if (error == 0)
                return 0;

        assert_return(!bus_error_is_dirty(e), -EINVAL);

        /* First, try a const translation */
        *e = errno_to_bus_error_const(error);

        if (!sd_bus_error_is_set(e)) {
                int k;

                /* If that didn't work, try a dynamic one. */

                k = errno_to_bus_error_name_new(error, (char**) &e->name);
                if (k > 0)
                        e->_need_free = 1;
                else if (k < 0) {
                        *e = BUS_ERROR_OOM;
                        return -error;
                } else
                        *e = BUS_ERROR_FAILED;
        }

        /* Now, fill in the message from strerror_r() if we can */
        bus_error_strerror(e, error);
        return -error;
}

_public_ int sd_bus_error_set_errnofv(sd_bus_error *e, int error, const char *format, va_list ap) {
        PROTECT_ERRNO;

        if (error < 0)
                error = -error;

        if (!e)
                return -error;
        if (error == 0)
                return 0;

        assert_return(!bus_error_is_dirty(e), -EINVAL);

        /* First, try a const translation */
        *e = errno_to_bus_error_const(error);

        if (!sd_bus_error_is_set(e)) {
                int k;

                /* If that didn't work, try a dynamic one */

                k = errno_to_bus_error_name_new(error, (char**) &e->name);
                if (k > 0)
                        e->_need_free = 1;
                else if (k < 0) {
                        *e = BUS_ERROR_OOM;
                        return -ENOMEM;
                } else
                        *e = BUS_ERROR_FAILED;
        }

        if (format) {
                _cleanup_free_ char *m = NULL;

                /* Then, let's try to fill in the supplied message */

                errno = error; /* Make sure that %m resolves to the specified error */
                if (vasprintf(&m, format, ap) < 0)
                        goto fail;

                if (e->_need_free <= 0) {
                        char *t;

                        t = strdup(e->name);
                        if (!t)
                                goto fail;

                        e->_need_free = 1;
                        e->name = t;
                }

                e->message = TAKE_PTR(m);
                return -error;
        }

fail:
        /* If that didn't work, use strerror_r() for the message */
        bus_error_strerror(e, error);
        return -error;
}

_public_ int sd_bus_error_set_errnof(sd_bus_error *e, int error, const char *format, ...) {
        int r;

        if (error < 0)
                error = -error;

        if (!e)
                return -error;
        if (error == 0)
                return 0;

        assert_return(!bus_error_is_dirty(e), -EINVAL);

        if (format) {
                va_list ap;

                va_start(ap, format);
                r = sd_bus_error_set_errnofv(e, error, format, ap);
                va_end(ap);

                return r;
        }

        return sd_bus_error_set_errno(e, error);
}

const char* _bus_error_message(const sd_bus_error *e, int error, char buf[static ERRNO_BUF_LEN]) {
        /* Sometimes, the D-Bus server is a little bit too verbose with
         * its error messages, so let's override them here */
        if (sd_bus_error_has_name(e, SD_BUS_ERROR_ACCESS_DENIED))
                return "Access denied";

        if (e && e->message)
                return e->message;

        return strerror_r(abs(error), buf, ERRNO_BUF_LEN);
}

static bool map_ok(const sd_bus_error_map *map) {
        for (; map->code != BUS_ERROR_MAP_END_MARKER; map++)
                if (!map->name || map->code <= 0)
                        return false;
        return true;
}

_public_ int sd_bus_error_add_map(const sd_bus_error_map *map) {
        const sd_bus_error_map **maps = NULL;
        unsigned n = 0;

        assert_return(map, -EINVAL);
        assert_return(map_ok(map), -EINVAL);

        if (additional_error_maps)
                for (; additional_error_maps[n] != NULL; n++)
                        if (additional_error_maps[n] == map)
                                return 0;

        maps = reallocarray(additional_error_maps, n + 2, sizeof(struct sd_bus_error_map*));
        if (!maps)
                return -ENOMEM;

        maps[n] = map;
        maps[n+1] = NULL;

        additional_error_maps = maps;
        return 1;
}
