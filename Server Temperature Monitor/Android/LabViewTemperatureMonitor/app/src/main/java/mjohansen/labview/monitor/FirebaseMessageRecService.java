package mjohansen.labview.monitor;

import android.app.AlertDialog;
import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

public class FirebaseMessageRecService extends FirebaseMessagingService {
    private static final String TAG = "labview";

    public FirebaseMessageRecService() {
    }

    @Override
    public void onMessageReceived(RemoteMessage remoteMessage) {
        Log.d(TAG, "From: " + remoteMessage.getFrom());

        // Check if message contains a data payload.
        if (remoteMessage.getData().size() > 0) {
            Log.d(TAG, "Message data payload: " + remoteMessage.getData());

            // if the payload has a size then we are updating the UI

            if(remoteMessage.getData().containsKey("Update"))
            {
                // update request
                Bundle bundle = new Bundle();
                bundle.putString("update", remoteMessage.getData().get("Update"));
                bundle.putFloat("z1", Float.parseFloat(remoteMessage.getData().get("z1")));
                bundle.putFloat("z2", Float.parseFloat(remoteMessage.getData().get("z2")));
                bundle.putFloat("z3", Float.parseFloat(remoteMessage.getData().get("z3")));
                bundle.putFloat("z4", Float.parseFloat(remoteMessage.getData().get("z4")));
                bundle.putFloat("z5", Float.parseFloat(remoteMessage.getData().get("z5")));

                Intent intent = new Intent();
                intent.setAction("SERVER_TEMP_UPDATE");
                intent.putExtra("msg", bundle);
                sendBroadcast(intent);
            }


        }

        // Check if message contains a notification payload.
        if (remoteMessage.getNotification() != null) {
            Log.d(TAG, "Message Notification Body: " + remoteMessage.getNotification().getBody());

            Bundle bundle = new Bundle();
            bundle.putString("message", remoteMessage.getNotification().getBody());

            Intent intent = new Intent();
            intent.setAction("SERVER_TEMP_ALARM");
            intent.putExtra("msg", bundle);
            sendBroadcast(intent);

        }
    }
}
