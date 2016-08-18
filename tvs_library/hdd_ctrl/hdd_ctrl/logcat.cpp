// Copyright 2006-2014 The Android Open Source Project
#define LOG_TAG "hdd_ctrl_logcat"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <cutils/sockets.h>
#include <log/log.h>
#include <log/log_read.h>
#include <log/logger.h>
#include <log/logd.h>
#include <log/logprint.h>
#include <log/event_tag_map.h>
#include "LogHelperThread.h"

#define DEFAULT_LOG_ROTATE_SIZE_KBYTES 16
#define DEFAULT_MAX_ROTATED_LOGS 4

static AndroidLogFormat * g_logformat;

/* logd prefixes records with a length field */
#define RECORD_LENGTH_FIELD_SIZE_BYTES sizeof(uint32_t)

struct log_device_t {
    const char* device;
    bool binary;
    struct logger *logger;
    struct logger_list *logger_list;
    bool printed;
    char label;

    log_device_t* next;

    log_device_t(const char* d, bool b, char l) {
        device = d;
        binary = b;
        label = l;
        next = NULL;
        printed = false;
    }
};

namespace android {

/* Global Variables */

static const char * g_outputFileName = NULL;
static int g_logRotateSizeKBytes = 0;                   // 0 means "no log rotation"
static int g_maxRotatedLogs = DEFAULT_MAX_ROTATED_LOGS; // 0 means "unbounded"
static int g_outFD = -1;
static off_t g_outByteCount = 0;
static int g_printBinary = 0;
static int g_devCount = 0;

static EventTagMap* g_eventTagMap = NULL;

static int openLogFile (const char *pathname)
{
    return open(pathname, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
}

static void rotateLogs()
{
    int err;

    // Can't rotate logs if we're not outputting to a file
    if (g_outputFileName == NULL) {
        return;
    }

    close(g_outFD);

    for (int i = g_maxRotatedLogs ; i > 0 ; i--) {
        char *file0, *file1;

        asprintf(&file1, "%s.%d", g_outputFileName, i);

        if (i - 1 == 0) {
            asprintf(&file0, "%s", g_outputFileName);
        } else {
            asprintf(&file0, "%s.%d", g_outputFileName, i - 1);
        }

        err = rename (file0, file1);

        if (err < 0 && errno != ENOENT) {
            perror("while rotating log files");
        }

        free(file1);
        free(file0);
    }

    g_outFD = openLogFile (g_outputFileName);

    if (g_outFD < 0) {
        ALOGE("couldn't open output file");
        return ;
    }

    g_outByteCount = 0;

}

void printBinary(struct log_msg *buf)
{
    size_t size = buf->len();

    TEMP_FAILURE_RETRY(write(g_outFD, buf, size));
}

static void processBuffer(log_device_t* dev, struct log_msg *buf)
{
    int bytesWritten = 0;
    int err;
    AndroidLogEntry entry;
    char binaryMsgBuf[1024];

    if (dev->binary) {
        err = android_log_processBinaryLogBuffer(&buf->entry_v1, &entry,
                                                 g_eventTagMap,
                                                 binaryMsgBuf,
                                                 sizeof(binaryMsgBuf));
        //printf(">>> pri=%d len=%d msg='%s'\n",
        //    entry.priority, entry.messageLen, entry.message);
    } else {
        err = android_log_processLogBuffer(&buf->entry_v1, &entry);
    }
    if (err < 0) {
        goto error;
    }

    if (android_log_shouldPrintLine(g_logformat, entry.tag, entry.priority)) {
        if (false && g_devCount > 1) {
            binaryMsgBuf[0] = dev->label;
            binaryMsgBuf[1] = ' ';
            bytesWritten = write(g_outFD, binaryMsgBuf, 2);
            if (bytesWritten < 0) {
                ALOGE("output error(%d)", __LINE__);
                return ;
            }
        }

        bytesWritten = android_log_printLogLine(g_logformat, g_outFD, &entry);

        if (bytesWritten < 0) {
            ALOGE("output error(%d)", __LINE__);
            return ;
        }
    }

    g_outByteCount += bytesWritten;

    if (g_logRotateSizeKBytes > 0
        && (g_outByteCount / 1024) >= g_logRotateSizeKBytes
    ) {
        rotateLogs();
    }

error:
    //fprintf (stderr, "Error processing record\n");
    return;
}

static void maybePrintStart(log_device_t* dev) {
    if (!dev->printed) {
        dev->printed = true;
        if (g_devCount > 1 && !g_printBinary) {
            char buf[1024];
            snprintf(buf, sizeof(buf), "--------- beginning of %s\n",
                     dev->device);
            if (write(g_outFD, buf, strlen(buf)) < 0) {
                ALOGE("output error(%d)", __LINE__);
                return ;
            }
        }
    }
}

static int setupOutput()
{

    if (g_outputFileName == NULL) {
        g_outFD = STDOUT_FILENO;

    } else {
        struct stat statbuf;

        g_outFD = openLogFile (g_outputFileName);

        if (g_outFD < 0) {
            ALOGE("couldn't open output file %s", g_outputFileName);
            return -1;
        }

        fstat(g_outFD, &statbuf);

        g_outByteCount = statbuf.st_size;
    }

    return 0;
}

static void show_help(const char *cmd)
{
    fprintf(stderr,"Usage: %s [options] [filterspecs]\n", cmd);

    fprintf(stderr, "options include:\n"
                    "  -s              Set default filter to silent.\n"
                    "                  Like specifying filterspec '*:s'\n"
                    "  -f <filename>   Log to file. Default to stdout\n"
                    "  -r [<kbytes>]   Rotate log every kbytes. (16 if unspecified). Requires -f\n"
                    "  -n <count>      Sets max number of rotated logs to <count>, default 4\n"
                    "  -v <format>     Sets the log print format, where <format> is one of:\n\n"
                    "                  brief process tag thread raw time threadtime long\n\n"
                    "  -c              clear (flush) the entire log and exit\n"
                    "  -d              dump the log and then exit (don't block)\n"
                    "  -t <count>      print only the most recent <count> lines (implies -d)\n"
                    "  -t '<time>'     print most recent lines since specified time (implies -d)\n"
                    "  -T <count>      print only the most recent <count> lines (does not imply -d)\n"
                    "  -T '<time>'     print most recent lines since specified time (not imply -d)\n"
                    "                  count is pure numerical, time is 'MM-DD hh:mm:ss.mmm'\n"
                    "  -g              get the size of the log's ring buffer and exit\n"
                    "  -b <buffer>     Request alternate ring buffer, 'main', 'system', 'radio',\n"
                    "                  'events', 'crash' or 'all'. Multiple -b parameters are\n"
                    "                  allowed and results are interleaved. The default is\n"
                    "                  -b main -b system -b crash.\n"
                    "  -B              output the log in binary.\n"
                    "  -S              output statistics.\n"
                    "  -G <size>       set size of log ring buffer, may suffix with K or M.\n"
                    "  -p              print prune white and ~black list. Service is specified as\n"
                    "                  UID, UID/PID or /PID. Weighed for quicker pruning if prefix\n"
                    "                  with ~, otherwise weighed for longevity if unadorned. All\n"
                    "                  other pruning activity is oldest first. Special case ~!\n"
                    "                  represents an automatic quicker pruning for the noisiest\n"
                    "                  UID as determined by the current statistics.\n"
                    "  -P '<list> ...' set prune white and ~black list, using same format as\n"
                    "                  printed above. Must be quoted.\n");

    fprintf(stderr,"\nfilterspecs are a series of \n"
                   "  <tag>[:priority]\n\n"
                   "where <tag> is a log component tag (or * for all) and priority is:\n"
                   "  V    Verbose\n"
                   "  D    Debug\n"
                   "  I    Info\n"
                   "  W    Warn\n"
                   "  E    Error\n"
                   "  F    Fatal\n"
                   "  S    Silent (supress all output)\n"
                   "\n'*' means '*:d' and <tag> by itself means <tag>:v\n"
                   "\nIf not specified on the commandline, filterspec is set from ANDROID_LOG_TAGS.\n"
                   "If no filterspec is found, filter defaults to '*:I'\n"
                   "\nIf not specified with -v, format is set from ANDROID_PRINTF_LOG\n"
                   "or defaults to \"brief\"\n\n");



}


} /* namespace android */

static int setLogFormat(const char * formatString)
{
    static AndroidLogPrintFormat format;

    format = android_log_formatFromString(formatString);

    if (format == FORMAT_OFF) {
        // FORMAT_OFF means invalid string
        return -1;
    }

    android_log_setPrintFormat(g_logformat, format);

    return 0;
}

static const char multipliers[][2] = {
    { "" },
    { "K" },
    { "M" },
    { "G" }
};

static unsigned long value_of_size(unsigned long value)
{
    for (unsigned i = 0;
            (i < sizeof(multipliers)/sizeof(multipliers[0])) && (value >= 1024);
            value /= 1024, ++i) ;
    return value;
}

static const char *multiplier_of_size(unsigned long value)
{
    unsigned i;
    for (i = 0;
            (i < sizeof(multipliers)/sizeof(multipliers[0])) && (value >= 1024);
            value /= 1024, ++i) ;
    return multipliers[i];
}

#if 0
int main(int argc, char **argv)
#else
int logcat_main(int argc, char **argv)
#endif
{
    int err;
    int hasSetLogFormat = 0;
    int clearLog = 0;
    int getLogSize = 0;
    unsigned long setLogSize = 0;
    int getPruneList = 0;
    char *setPruneList = NULL;
    int printStatistics = 0;
    int mode = O_RDONLY;
    const char *forceFilters = NULL;
    log_device_t* devices = NULL;
    log_device_t* dev;
    bool needBinary = false;
    struct logger_list *logger_list;
    unsigned int tail_lines = 0;
    log_time tail_time(log_time::EPOCH);

    //signal(SIGPIPE, exit);

    g_logformat = android_log_format_new();

    if (argc == 2 && 0 == strcmp(argv[1], "--help")) {
        return -1;
    }

    for (;;) {
        int ret;

        ret = getopt(argc, argv, "cdt:T:gG:sQf:r:n:v:b:BSpP:");

        if (ret < 0) {
            break;
        }

        switch(ret) {
            case 's':
                // default to all silent
                android_log_addFilterRule(g_logformat, "*:s");
            break;

            case 'c':
                clearLog = 1;
                mode = O_WRONLY;
            break;

            case 'd':
                mode = O_RDONLY | O_NDELAY;
            break;

            case 't':
                mode = O_RDONLY | O_NDELAY;
                /* FALLTHRU */

            case 'g':
                getLogSize = 1;
            break;

            case 'G': {
                // would use atol if not for the multiplier
                char *cp = optarg;
                setLogSize = 0;
                while (('0' <= *cp) && (*cp <= '9')) {
                    setLogSize *= 10;
                    setLogSize += *cp - '0';
                    ++cp;
                }

                switch(*cp) {
                case 'g':
                case 'G':
                    setLogSize *= 1024;
                /* FALLTHRU */
                case 'm':
                case 'M':
                    setLogSize *= 1024;
                /* FALLTHRU */
                case 'k':
                case 'K':
                    setLogSize *= 1024;
                /* FALLTHRU */
                case '\0':
                break;

                default:
                    setLogSize = 0;
                }

                if (!setLogSize) {
                    ALOGE("ERROR: -G <num><multiplier>\n");
                }
            }
            break;

            case 'p':
                getPruneList = 1;
            break;

            case 'P':
                setPruneList = optarg;
            break;

            case 'b': {
                if (strcmp(optarg, "all") == 0) {
                    while (devices) {
                        dev = devices;
                        devices = dev->next;
                        delete dev;
                    }

                    dev = devices = new log_device_t("main", false, 'm');
                    android::g_devCount = 1;
                    if (android_name_to_log_id("system") == LOG_ID_SYSTEM) {
                        dev->next = new log_device_t("system", false, 's');
                        if (dev->next) {
                            dev = dev->next;
                            android::g_devCount++;
                        }
                    }
                    if (android_name_to_log_id("radio") == LOG_ID_RADIO) {
                        dev->next = new log_device_t("radio", false, 'r');
                        if (dev->next) {
                            dev = dev->next;
                            android::g_devCount++;
                        }
                    }
                    if (android_name_to_log_id("events") == LOG_ID_EVENTS) {
                        dev->next = new log_device_t("events", true, 'e');
                        if (dev->next) {
                            dev = dev->next;
                            android::g_devCount++;
                            needBinary = true;
                        }
                    }
                    if (android_name_to_log_id("crash") == LOG_ID_CRASH) {
                        dev->next = new log_device_t("crash", false, 'c');
                        if (dev->next) {
                            android::g_devCount++;
                        }
                    }
                    break;
                }

                bool binary = strcmp(optarg, "events") == 0;
                if (binary) {
                    needBinary = true;
                }

                if (devices) {
                    dev = devices;
                    while (dev->next) {
                        dev = dev->next;
                    }
                    dev->next = new log_device_t(optarg, binary, optarg[0]);
                } else {
                    devices = new log_device_t(optarg, binary, optarg[0]);
                }
                android::g_devCount++;
            }
            break;

            case 'B':
                android::g_printBinary = 1;
            break;

            case 'f':
                // redirect output to a file

                android::g_outputFileName = optarg;

            break;

            case 'r':
                if (optarg == NULL) {
                    android::g_logRotateSizeKBytes
                                = DEFAULT_LOG_ROTATE_SIZE_KBYTES;
                } else {
                    if (!isdigit(optarg[0])) {
                        ALOGE("Invalid parameter to -r\n");
                        return -2;
                    }
                    android::g_logRotateSizeKBytes = atoi(optarg);
                }
            break;

            case 'n':
                if (!isdigit(optarg[0])) {
                    ALOGE("Invalid parameter to -r\n");
                    return -3;
                }

                android::g_maxRotatedLogs = atoi(optarg);
            break;

            case 'v':
                err = setLogFormat (optarg);
                if (err < 0) {
                    ALOGE("Invalid parameter to -v\n");
                    return -4;
                }

                hasSetLogFormat = 1;
            break;

            case 'S':
                printStatistics = 1;
                break;

            default:
                fprintf(stderr,"Unrecognized Option\n");
                android::show_help(argv[0]);
                return -4;
            break;
        }
    }

    if (!devices) {
        dev = devices = new log_device_t("main", false, 'm');
        android::g_devCount = 1;
        if (android_name_to_log_id("system") == LOG_ID_SYSTEM) {
            dev = dev->next = new log_device_t("system", false, 's');
            android::g_devCount++;
        }
        if (android_name_to_log_id("crash") == LOG_ID_CRASH) {
            dev = dev->next = new log_device_t("crash", false, 'c');
            android::g_devCount++;
        }
    }

    if (android::g_logRotateSizeKBytes != 0
        && android::g_outputFileName == NULL
    ) {
        ALOGE("-r requires -f as well\n");
        return -5;
    }

    if(android::setupOutput())
    {
        return -6;
    }

    if (hasSetLogFormat == 0) {
        const char* logFormat = getenv("ANDROID_PRINTF_LOG");

        if (logFormat != NULL) {
            err = setLogFormat(logFormat);

            if (err < 0) {
                fprintf(stderr, "invalid format in ANDROID_PRINTF_LOG '%s'\n",
                                    logFormat);
            }
        }
    }

    if (forceFilters) {
        err = android_log_addFilterString(g_logformat, forceFilters);
        if (err < 0) {
            ALOGE("Invalid filter expression in -logcat option\n");
            return -7;
        }
    } else if (argc == optind) {
        // Add from environment variable
        char *env_tags_orig = getenv("ANDROID_LOG_TAGS");

        if (env_tags_orig != NULL) {
            err = android_log_addFilterString(g_logformat, env_tags_orig);

            if (err < 0) {
                ALOGE("Invalid filter expression in"
                                    " ANDROID_LOG_TAGS\n");
                return -8;
            }
        }
    } else {
        // Add from commandline
        for (int i = optind ; i < argc ; i++) {
            err = android_log_addFilterString(g_logformat, argv[i]);

            if (err < 0) {
                ALOGE("Invalid filter expression '%s'\n", argv[i]);
                return -9;
            }
        }
    }

    dev = devices;
    if (tail_time != log_time::EPOCH) {
        logger_list = android_logger_list_alloc_time(mode, tail_time, 0);
    } else {
        logger_list = android_logger_list_alloc(mode, tail_lines, 0);
    }
    while (dev) {
        dev->logger_list = logger_list;
        dev->logger = android_logger_open(logger_list,
                                          android_name_to_log_id(dev->device));
        if (!dev->logger) {
            ALOGE("Unable to open log device '%s'\n", dev->device);
            break;
        }

        if (clearLog) {
            int ret;
            ret = android_logger_clear(dev->logger);
            if (ret) {
                ALOGE("failed to clear the log");
                break;
            }
        }

        if (setLogSize && android_logger_set_log_size(dev->logger, setLogSize)) {
            ALOGE("failed to set the log size");
            break;
        }

        if (getLogSize) {
            long size, readable;

            size = android_logger_get_log_size(dev->logger);
            if (size < 0) {
                ALOGE("failed to get the log size");
                break;
            }

            readable = android_logger_get_log_readable_size(dev->logger);
            if (readable < 0) {
                ALOGE("failed to get the readable log size");
                break;
            }

            ALOGE("%s: ring buffer is %ld%sb (%ld%sb consumed), "
                   "max entry is %db, max payload is %db\n", dev->device,
                   value_of_size(size), multiplier_of_size(size),
                   value_of_size(readable), multiplier_of_size(readable),
                   (int) LOGGER_ENTRY_MAX_LEN, (int) LOGGER_ENTRY_MAX_PAYLOAD);
        }

        dev = dev->next;
    }

    if (setPruneList) {
        size_t len = strlen(setPruneList) + 32; // margin to allow rc
        char *buf = (char *) malloc(len);

        strcpy(buf, setPruneList);
        int ret = android_logger_set_prune_list(logger_list, buf, len);
        free(buf);

        if (ret) {
            ALOGE("failed to set the prune list");
        }
    }

    if (printStatistics || getPruneList) {
        size_t len = 8192;
        char *buf;

        for(int retry = 32;
                (retry >= 0) && ((buf = new char [len]));
                delete [] buf, --retry) {
            if (getPruneList) {
                android_logger_get_prune_list(logger_list, buf, len);
            } else {
                android_logger_get_statistics(logger_list, buf, len);
            }
            buf[len-1] = '\0';
            size_t ret = atol(buf) + 1;
            if (ret < 4) {
                delete [] buf;
                buf = NULL;
                break;
            }
            bool check = ret <= len;
            len = ret;
            if (check) {
                break;
            }
        }

        if (!buf) {
            ALOGE("failed to read data");
            return -1;
        }

        // remove trailing FF
        char *cp = buf + len - 1;
        *cp = '\0';
        bool truncated = *--cp != '\f';
        if (!truncated) {
            *cp = '\0';
        }

        // squash out the byte count
        cp = buf;
        if (!truncated) {
            while (isdigit(*cp)) {
                ++cp;
            }
            if (*cp == '\n') {
                ++cp;
            }
        }

        ALOGE("%s", cp);
        delete [] buf;
        return -1;
    }

#if 0
    if (getLogSize) {
        exit(0);
    }
    if (setLogSize || setPruneList) {
        exit(0);
    }
    if (clearLog) {
        exit(0);
    }
#endif

    //LOG_EVENT_INT(10, 12345);
    //LOG_EVENT_LONG(11, 0x1122334455667788LL);
    //LOG_EVENT_STRING(0, "whassup, doc?");

    if (needBinary)
        android::g_eventTagMap = android_openEventTagMap(EVENT_TAG_MAP_FILE);

    int ret = 0;

    while (LogHelperThread::isRunning()) {
        struct log_msg log_msg;
        int ret = android_logger_list_read(logger_list, &log_msg);

        if (ret == 0) {
            ALOGE("read: Unexpected EOF!\n");
            ret = -1;
            break;
        }

        if (ret < 0) {
            if (ret == -EAGAIN) {
                ALOGE("read: EAGAIN!\n");
                ret = -2;
            }

            if (ret == -EIO) {
                ALOGE("read: Unexpected EOF!\n");
                ret = -3;
            }
            if (ret == -EINVAL) {
                ALOGE("read: unexpected length.\n");
                ret = -4;
            }
            ALOGE("logcat read failure");
            break;
        }

        for(dev = devices; dev; dev = dev->next) {
            if (android_name_to_log_id(dev->device) == log_msg.id()) {
                break;
            }
        }
        if (!dev) {
            ALOGE("read: Unexpected log ID!\n");
            ret = -5;
            break;
        }

        android::maybePrintStart(dev);
        if (android::g_printBinary) {
            android::printBinary(&log_msg);
        } else {
            android::processBuffer(dev, &log_msg);
        }
    }

    android_logger_list_free(logger_list);

    close(g_outFD);
    return ret;
}
