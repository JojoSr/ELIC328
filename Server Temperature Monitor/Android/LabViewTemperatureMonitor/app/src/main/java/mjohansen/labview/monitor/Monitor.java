package mjohansen.labview.monitor;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.MediaPlayer;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.messaging.FirebaseMessaging;

public class Monitor extends AppCompatActivity {

    // controls
    com.warkiz.tickseekbar.TickSeekBar z1;
    com.warkiz.tickseekbar.TickSeekBar z2;
    com.warkiz.tickseekbar.TickSeekBar z3;
    com.warkiz.tickseekbar.TickSeekBar z4;
    com.warkiz.tickseekbar.TickSeekBar z5;

    // data update listener
    private BroadcastReceiver broadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Bundle bundle = intent.getBundleExtra("msg");
            if(bundle.containsKey("update")) {
                z1.setProgress(bundle.getFloat("z1"));
                z2.setProgress(bundle.getFloat("z2"));
                z3.setProgress(bundle.getFloat("z3"));
                z4.setProgress(bundle.getFloat("z4"));
                z5.setProgress(bundle.getFloat("z5"));
            }
        }
    };

    // alert notification
    private  BroadcastReceiver alarmBroadcast = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final AlertDialog.Builder builder;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                builder = new AlertDialog.Builder(context, android.R.style.Theme_Material_Dialog_Alert);
            } else {
                builder = new AlertDialog.Builder(context);
            }
            Bundle bundle = intent.getBundleExtra("msg");
            builder.setTitle("Alarm Triggered!")
                    .setMessage(bundle.getString("message"))
                    .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                        }
                    })
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .show();
        }
    };


    // Create UI
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_monitor);
        // set controls
        z1 = findViewById(R.id.listener1);
        z2 = findViewById(R.id.listener2);
        z3 = findViewById(R.id.listener3);
        z4 = findViewById(R.id.listener4);
        z5 = findViewById(R.id.listener5);

        FirebaseMessaging.getInstance().subscribeToTopic("data")
                .addOnCompleteListener(new OnCompleteListener<Void>() {
                    @Override
                    public void onComplete(@NonNull Task<Void> task) {
                        if(!task.isSuccessful()) {
                            Log.e("labview", "Failed to subscribe to Topic ....");
                        }
                    }
                });

        FirebaseMessaging.getInstance().subscribeToTopic("alarm")
                .addOnCompleteListener(new OnCompleteListener<Void>() {
                    @Override
                    public void onComplete(@NonNull Task<Void> task) {
                        if(!task.isSuccessful()) {
                            Log.e("labview", "Failed to subscribe to Topic ....");
                        }
                    }
                });


        // register data updates
        if(broadcastReceiver != null){
            IntentFilter intentFilter = new IntentFilter("SERVER_TEMP_UPDATE");
            registerReceiver(broadcastReceiver, intentFilter);
        }
        // register alarm notifications
        if(alarmBroadcast != null){
            IntentFilter alarmFilter = new IntentFilter("SERVER_TEMP_ALARM");
            registerReceiver(alarmBroadcast, alarmFilter);
        }
    }
}
