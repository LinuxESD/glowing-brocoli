    package com.eightman;
     
    import android.app.Application;
    import android.util.Log;
    import android.content.IntentFilter;
    import android.os.ServiceManager;
    import android.os.IBinder;
    import vendor.eightman.cpu.ICpu;
     
    public class EightmanServices extends Application {
        private static final String TAG = "EightmanServices";
     
        private static final String ICPU_AIDL_INTERFACE = ICpu.DESCRIPTOR + "/default";
     
        private static ICpu cpu;
     
        private EightmanBroadcastReceiver eightmanBroadcastReceiver;
     
        public void onCreate() {
            super.onCreate();
            Log.d(TAG, "onCreate()");
     
            IBinder binder = ServiceManager.getService(ICPU_AIDL_INTERFACE);
            if (binder == null) {
                Log.e(TAG, "Getting " + ICPU_AIDL_INTERFACE + " service daemon binder failed");
            } else {
                cpu = ICpu.Stub.asInterface(binder);
                if (cpu == null) {
                    Log.e(TAG, "Getting ICpu AIDL daemon interface failed");
                } else {
                    try {
                        Log.d(TAG, "getScalingGovernor=" + cpu.getScalingGovernor());
                        Log.d(TAG, "setScalingGovernor(conservative)=" + cpu.setScalingGovernor("conservative"));
                    } catch (android.os.RemoteException e) {
                        Log.e(TAG, "ICpu AIDL error", e);
                    }
                }
            }
     
            eightmanBroadcastReceiver = new EightmanBroadcastReceiver();
            IntentFilter filter = new IntentFilter("com.eightman.CPU");
            registerReceiver(eightmanBroadcastReceiver, filter, RECEIVER_EXPORTED);
        }
     
        public void onTerminate() {
            super.onTerminate();
            Log.d(TAG, "Terminated");
        }
     
        public static ICpu getCpu() {
            return cpu;
        }
    }
