#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/reboot.h>

#include "edify/expr.h"
#include "mincrypt/sha.h"
#include "minzip/Zip.h"
#include "mtdutils/mounts.h"
#include "updater/updater.h"
#include "bootloader.h"

bool copy_file_to_device(const char* filename, const char* devicename, int offset)
{
    bool success;
    printf("----- start to write %s to %s -----\n", filename, devicename);

    FILE* fr = fopen(filename, "rb");
    if (fr == NULL) {
        fprintf(stderr, "E: can't open %s: %d\n", filename, (int)errno);
        return false;
    }

    FILE* fw = fopen(devicename, "r+b");
    if (fw == NULL) {
        fprintf(stderr, "E: can't open %s: %d\n", devicename, (int)errno);
        fclose(fr);
        return false;
    }
    fseek(fw, offset, SEEK_SET);

    success = true;
    char* buffer = malloc(BUFSIZ);
    char* pbuf = NULL;
    int read, wrote, count;
    while (success && (read = fread(buffer, 1, BUFSIZ, fr)) > 0) {
        wrote = 0;
        pbuf = buffer;
        while (wrote < read){
            count = fwrite(pbuf, 1, read - wrote, fw);
            wrote += count;
            pbuf += count;
        }
        success &= (wrote == read);
        if (!success){
            fprintf(stderr, "E: emmc write data to %s failed: %d\n", devicename, (int)errno);
        }
    }
    free(buffer);
    fclose(fr);
    fclose(fw);

    printf("----- %s to write %s to %s -----\n", success ? "successful" : "failed", filename, devicename);
    return success;
}

Value* WriteRawImageEmmcFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;
    char* devicename;
    char* filename;
    char* offset_str;

    if (ReadArgs(state, argv, 3, &devicename, &filename, &offset_str) < 0) {
        return NULL;
    }

    if (!(devicename && devicename[0])) {
        ErrorAbort(state, "device argument to %s can't be empty", name);
        goto done;
    }
    if (!(filename && filename[0])) {
        ErrorAbort(state, "file argument to %s can't be empty", name);
        goto done;
    }
    if (!(offset_str && offset_str[0])) {
        ErrorAbort(state, "offset argument to %s can't be empty", name);
        goto done;
    }

    int offset = strtol(offset_str, NULL, 10);
    bool success = copy_file_to_device(filename, devicename, offset);
    result = success ? devicename : strdup("");

done:
    if (result != devicename) free(devicename);
    free(filename);
    free(offset_str);
    return StringValue(result);
}

Value* UpdateFirmwareFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;
    char* devicename;
    char* commandstr;
    struct bootloader_message boot;

    if (ReadArgs(state, argv, 2, &devicename, &commandstr) < 0) {
        return NULL;
    }

    if (!(devicename && devicename[0])) {
        ErrorAbort(state, "device argument to %s can't be empty", name);
        goto done;
    }
    if (!(commandstr && commandstr[0])) {
        ErrorAbort(state, "command argument to %s can't be empty", name);
        goto done;
    }
    memset(&boot, 0, sizeof(boot));
    strlcpy(boot.command, commandstr, sizeof(boot.command));

    printf("----- start to write bootloader message to misc -----\n");
    FILE* f = fopen(devicename, "r+b");
    if (f == NULL) {
        result = strdup("");
        goto done;
    }
    int count = fwrite(&boot, sizeof(boot), 1, f);
    fclose(f);
    if (count != 1) {
        result = strdup("");
        goto done;
    }
    result = devicename;
    printf("----- successful to write bootloader message to misc -----\n");

done:
    if (result != devicename) free(devicename);
    free(commandstr);
    return StringValue(result);
}

Value* RebootFn(const char* name, State* state, int argc, Expr* argv[]) {
    printf("----- reboot now -----\n");
    reboot(RB_AUTOBOOT);
    return StringValue(strdup(""));
}

void Register_librecovery_updater_mrvl() {
    fprintf(stderr, "installing Marvell updater extensions\n");
    RegisterFunction("write_raw_image_emmc", WriteRawImageEmmcFn);
    RegisterFunction("mrvl_update_firmware", UpdateFirmwareFn);
    RegisterFunction("reboot", RebootFn);
}

