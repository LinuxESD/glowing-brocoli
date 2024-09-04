    #include <aidl/vendor/eightman/cpu/ICpu.h>
    #include <utils/misc.h>
    #include <iostream>
    #include <cstdlib>
    #include <string>
    #include <android/binder_manager.h>
     
    using aidl::vendor::eightman::cpu::ICpu;
     
    int main(int argc, char *argv[]) {
        if (argc != 2) {
            std::cout << "USAGE ./cpu_client <conservative|powersave|performance|schedutil>\n";
            exit(0);
        }
     
        const std::string cpu_interface_name = std::string(ICpu::descriptor) + "/default";
        std::shared_ptr<ICpu> cpuService = ICpu::fromBinder(ndk::SpAIBinder(
            AServiceManager_waitForService(cpu_interface_name.c_str())));
     
        if (cpuService == nullptr) {
            std::cout << "Failed to get CPU service\n";
            exit(-1);
        }
     
        std::string result;
        cpuService->getScalingGovernor(&result);
        std::cout << "getScalingGovernor:" << result << std::endl;
     
        bool success;
        ::ndk::ScopedAStatus status = cpuService->setScalingGovernor(argv[1], &success);
        if (!status.isOk()) {
            if (status.getServiceSpecificError() != 0) {
                std::cout << "CPU Error: " << status.getServiceSpecificError() << std::endl;
            } else {
                std::cout << "Exception Code: " << status.getExceptionCode() << std::endl;
            }
            exit(-1);
        }
        std::cout << "setScalingGovernor:" << success << std::endl;
     
        cpuService->getScalingGovernor(&result);
        std::cout << "getScalingGovernor:" << result << std::endl;
     
        return 0;
    }
