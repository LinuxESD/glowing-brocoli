    package vendor.eightman.cpu;
     
    @VintfStability
    interface ICpu {
        // returns the scaling governor 
        String getScalingGovernor();
     
        // set the scaling governor
        boolean setScalingGovernor(in String governor);
    }
