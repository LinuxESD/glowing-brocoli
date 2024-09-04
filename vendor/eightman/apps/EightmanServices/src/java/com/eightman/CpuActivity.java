    package com.eightman;
     
    import android.app.Activity;
    import android.os.Bundle;
    import android.util.Log;
    import android.view.View;
    import android.widget.Button;
    import android.widget.TextView;
     
    public class CpuActivity extends Activity {
        private TextView valueTV;
     
        @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_cpu);
            valueTV = findViewById(R.id.valueTV);
        }
     
        @Override
        protected void onResume() {
            super.onResume();
            refresh();
        }
     
        private void refresh() {
            try {
                valueTV.setText(EightmanServices.getCpu().getScalingGovernor().trim());
                valueTV.setTextColor(getColor(R.color.success));
            } catch (android.os.RemoteException e) {
                Log.e("CpuActivity", "getCpu().getScalingGovernor() returned error", e);
                valueTV.setText("Error: " + e.getMessage());
                valueTV.setTextColor(getColor(R.color.error));
            }
        }
     
        public void onFetchValue(View view) {
            refresh();
        }
     
        public void onClick(View view) {
            String setValue = ((Button)view).getText().toString();
     
            // [---------------- com.eightman ----------------------------]
            // [ android_app: /system_ext/app/EightmanServices/           ]
            // [                     |/\/\|                               ]
            // [ aidl_interface: apk:classes.dex:vendor.eightman.cpu.ICpu ]
            // [----------------------------------------------------------]
            //                         ||
            //                    /dev/vndbinder
            //                         ||
            // [------------- vendor.eightman.cpu-service --------------------------]
            // [ aidl_interface: /vendor/lib/vendor.eightman.cpu-V1-ndk_platform.so ]
            // [                     |/\/\|                                         ]
            // [ cc_binary: /vendor/bin/hw/vendor.eightman.cpu-service              ]
            // [--------------------------------------------------------------------]
            try {
                updateTV(valueTV, EightmanServices.getCpu().setScalingGovernor(setValue));
            } catch (android.os.RemoteException e) {
                Log.e("CpuActivity", "cpu AIDL Java proxy returned error", e);
            }
        }
     
        private void updateTV(TextView successTV, boolean success){
            if (success) {
                successTV.setText("Success");
                successTV.setTextColor(getColor(R.color.success));
            } else {
                successTV.setText("Error");
                successTV.setTextColor(getColor(R.color.error));
            }
        }
    }
