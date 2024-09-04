    #define LOG_TAG "vendor.eightman.cpu-service"

    #include <android-base/logging.h>
    #include <android/binder_process.h>
    #include <android/binder_manager.h>
    #include <binder/ProcessState.h>
    #include <binder/IServiceManager.h>

    #include "Cpu.h"
     
    using aidl::vendor::eightman::cpu::Cpu;
    using std::string_literals::operator""s;

     
    int main() {
        android::ProcessState::initWithDriver("/dev/vndbinder");
     
        ABinderProcess_startThreadPool();
     
        LOG(INFO) << "Cpu aidl service for eightman is starting.,,";
     
        std::shared_ptr<Cpu> cpu = ndk::SharedRefBase::make<Cpu>();
        const std::string name = Cpu::descriptor + "/default"s;
        CHECK_EQ(STATUS_OK, AServiceManager_addService(cpu->asBinder().get(), name.c_str()));
     
        LOG(INFO) << "Initialized " << name << " service";
     
        ABinderProcess_joinThreadPool();
     
        return EXIT_FAILURE;  // should not reach
    }
