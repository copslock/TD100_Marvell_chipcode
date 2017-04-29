LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
	external/icu4c/common \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../android \
	external/zlib \
	external/libxml2/include \
        external/glib                   \
        external/glib/android           \
        external/glib/glib              \
        external/glib/gmodule           \
        external/glib/gobject           \
        external/glib/gthread           \
        $(TARGET_OUT_HEADERS)/glib

LOCAL_CFLAGS += -DHAVE_CONFIG_H -DG_LOG_DOMAIN=\"libsoup\" -DG_DISABLE_SINGLE_INCLUDES

LOCAL_SRC_FILES:= \
soup-address.c             soup-auth-ntlm.c        soup-enum-types.c         soup-message-headers.c    soup-nossl.c                  soup-session.c\
soup-auth-basic.c          soup-auth.c             soup-form.c               soup-message-io.c         soup-path-map.c               soup-session-sync.c\
soup-auth-digest.c         soup-connection.c       soup-gnutls.c             soup-message.c            soup-proxy-resolver.c         soup-socket.c\
soup-auth-domain-basic.c   soup-cookie-jar.c       soup-headers.c            soup-message-queue.c      soup-proxy-resolver-static.c  soup-status.c\
soup-auth-domain-digest.c  soup-cookie-jar-text.c  soup-logger.c             soup-message-server-io.c  soup-proxy-uri-resolver.c     soup-uri.c\
soup-auth-domain.c         soup-cookie.c           soup-marshal.c            soup-method.c             soup-server.c                 soup-value-utils.c\
soup-auth-manager-ntlm.c   soup-date.c             soup-message-body.c       soup-misc.c               soup-session-async.c          soup-xmlrpc.c\
soup-auth-manager.c        soup-dns.c              soup-message-client-io.c  soup-multipart.c          soup-session-feature.c


LOCAL_SHARED_LIBRARIES += \
	libz \
        libglib-2.0             \
        libgthread-2.0          \
        libgmodule-2.0          \
        libgobject-2.0		\
	libgio-2.0

LOCAL_SHARED_LIBRARIES += libxml2

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE:= libsoup
LOCAL_MODULE_TAGS:= optional

include $(BUILD_SHARED_LIBRARY)

