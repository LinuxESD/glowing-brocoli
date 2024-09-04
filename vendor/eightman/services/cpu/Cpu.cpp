    #include "Cpu.h"
    #include <utils/Log.h>
     
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>
     
    namespace aidl::vendor::eightman::cpu {
     
    // conservative|powersave|performance|schedutil
    static const char SCALING_GOVERNOR[] = \
            "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor";
     
    ndk::ScopedAStatus Cpu::getScalingGovernor(std::string* _aidl_return) {
        char str[20];
        int fd;
        ssize_t ret = 0;
        struct stat info;
        void *data = NULL;
        size_t size;
     
        // If open returns error code EINTR, retry again until error code
        // is not a TEMP_FAILURE
        fd = TEMP_FAILURE_RETRY(open(SCALING_GOVERNOR, O_RDONLY));
        if (fd < 0) {
            return ndk::ScopedAStatus::fromServiceSpecificError(-1);
        }
     
        fstat(fd, &info);
        size = info.st_size;
        data = malloc(size);
        if (data == NULL) {
            *_aidl_return = "error: can't malloc";
            goto exit;
        }
     
        ret = read(fd, data, size);
        if (ret < 0) {
            *_aidl_return = "error: reading fd";
            goto exit;
        }
     
        snprintf(str, sizeof(str), "%s", (const unsigned char*)data);
        ALOGI("Cpu::getScalingGovernor data=%s", str);
        *_aidl_return = str;
     
    exit:
        close(fd);
        free(data);
        return ndk::ScopedAStatus::ok();
    }
     
    ndk::ScopedAStatus Cpu::setScalingGovernor(const std::string& in_governor, bool* _aidl_return) {
        int fd, to_write, written;
        const char *value = in_governor.c_str();
     
        ALOGI("Cpu::setScalingGovernor data=%s", value);
     
        fd = TEMP_FAILURE_RETRY(open(SCALING_GOVERNOR, O_WRONLY));
        if (fd < 0) {
            ALOGI("Cpu::setScalingGovernor failed to open for write %s", SCALING_GOVERNOR);
            *_aidl_return = false;
            return ndk::ScopedAStatus::fromServiceSpecificError(-2);
        } else {
            to_write = strlen(value) + 1;
            written = TEMP_FAILURE_RETRY(write(fd, value, to_write));
            close(fd);
     
            if (written != to_write) {
               ALOGI("Cpu::setScalingGovernor failed to write %s", value);
               *_aidl_return = false;
               return ndk::ScopedAStatus::ok();
            }
     
            *_aidl_return = true;
            errno = 0;
        }
        return ndk::ScopedAStatus::ok();
    }
    }  // namespace aidl::vendor::eightman::cpu
