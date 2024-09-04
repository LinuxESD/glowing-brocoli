    #pragma once
     
    #include <aidl/vendor/eightman/cpu/BnCpu.h>
     
    namespace aidl::vendor::eightman::cpu {
     
    class Cpu : public BnCpu {
      public:
        ndk::ScopedAStatus getScalingGovernor(std::string* _aidl_return) override;
        ndk::ScopedAStatus setScalingGovernor(const std::string& in_governor, bool* _aidl_return) override;
    };
     
    }  // namespace aidl::vendor::eightman::cpu
