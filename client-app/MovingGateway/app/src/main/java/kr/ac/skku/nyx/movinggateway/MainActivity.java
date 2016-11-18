package kr.ac.skku.nyx.movinggateway;

import android.content.Intent;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.appindexing.Action;
import com.google.android.gms.appindexing.AppIndex;
import com.google.android.gms.appindexing.Thing;
import com.google.android.gms.common.api.GoogleApiClient;

public class MainActivity extends AppCompatActivity implements OnClickListener,
        OnBulbTouchedListener, BulbDialogFragmentListener {
    final int UPDATE_SCREEN = 1;
    MovingGateway mMovingGateway = null;
    Handler mHandler = null;

    final private String id[] = {"00:17:88:01:10:3c:0f:c5-0b",
            "00:17:88:01:10:3c:0a:c1-0b",
            "00:17:88:01:10:3c:0d:3a-0b"};
    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    private GoogleApiClient client;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initialize();
        updateScreen();
        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client = new GoogleApiClient.Builder(this).addApi(AppIndex.API).build();
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateScreen();
    }

    public void initialize() {
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case UPDATE_SCREEN:
                        updateScreen();
                        break;
                    default:
                        break;
                }
            }
        };

        mMovingGateway = new MovingGateway(mHandler);
        mMovingGateway.setOnResourceEventListener(new MovingGateway.OnResourceEventListener() {
            @Override
            public void onFoundResource(MovingGateway.Resource res) {

            }

            @Override
            public void onGetResource(String json_rep) {

            }

            @Override
            public void onPutResource(String json_rep) {

            }
        });

        Button scan_btn = (Button) findViewById(R.id.scan_btn);
        scan_btn.setOnClickListener(this);

        Button left_btn = (Button) findViewById(R.id.left_btn);
        left_btn.setOnClickListener(this);

        Button right_btn = (Button) findViewById(R.id.right_btn);
        right_btn.setOnClickListener(this);

        Button mv_btn = (Button) findViewById(R.id.mv_btn);
        mv_btn.setOnClickListener(this);

        Button stop_btn = (Button) findViewById(R.id.stop_btn);
        stop_btn.setOnClickListener(this);

        Button connect_btn = (Button) findViewById(R.id.connect_btn);
        connect_btn.setOnClickListener(this);

        /* RedCarrottt */
        RVCSurfaceView rvcSurfaceView = (RVCSurfaceView) findViewById(R.id.rvc_surface_view);
        rvcSurfaceView.setListener(this);
        /***************/
    }

    public void updateScreen() {
        EditText et_ip = (EditText) findViewById(R.id.edit_text_ip);
        et_ip.setEnabled(!mMovingGateway.isConnected());

        EditText et_x = (EditText) findViewById(R.id.edit_text_x_val);
        et_x.setEnabled(mMovingGateway.isConnected());

        EditText et_y = (EditText) findViewById(R.id.edit_text_y_val);
        et_y.setEnabled(mMovingGateway.isConnected());

        Button mv_btn = (Button) findViewById(R.id.mv_btn);
        mv_btn.setEnabled(mMovingGateway.isConnected());

        Button scan_btn = (Button) findViewById(R.id.scan_btn);
        scan_btn.setEnabled(mMovingGateway.isConnected());

        Button stop_btn = (Button) findViewById(R.id.stop_btn);
        stop_btn.setEnabled(mMovingGateway.isConnected());

        Button connect_btn = (Button) findViewById(R.id.connect_btn);
        if (mMovingGateway.isConnected())
            connect_btn.setText("Disconnect");
        else
            connect_btn.setText("Connect");

        TextView tv_position = (TextView) findViewById(R.id.text_view_pos_val);
        String pos = "( " + Integer.toString((int) mMovingGateway.pos.x) + ", " + Integer.toString((int) mMovingGateway.pos.y) + ", " + Integer.toString((int) mMovingGateway.pos.q) + " )";
        tv_position.setText(pos);

        RVCSurfaceView sv = (RVCSurfaceView) findViewById(R.id.rvc_surface_view);
        sv.updateRVCPosition(mMovingGateway.pos.x, mMovingGateway.pos.y);
        MovingGateway.Resource res = mMovingGateway.getResource(id[0]);
        if (res != null)
            sv.updateBulbPosition(0, res.x, res.y);
        res = mMovingGateway.getResource(id[1]);
        if (res != null)
            sv.updateBulbPosition(1, res.x, res.y);
        res = mMovingGateway.getResource(id[2]);
        if (res != null)
            sv.updateBulbPosition(2, res.x, res.y);
    }

    @Override
    public void onClick(View v) {
        if (mMovingGateway == null)
            return;

        String cmd = (String) v.getTag();
        switch (v.getId()) {
            case R.id.left_btn:
                startActivity(new Intent(MainActivity.this, MapView.class));
                overridePendingTransition(R.anim.anim_slide_in_left, R.anim.anim_slide_out_right);
                finish();
                break;
            case R.id.right_btn:
                startActivity(new Intent(MainActivity.this, MapView.class));
                overridePendingTransition(R.anim.anim_slide_in_right, R.anim.anim_slide_out_left);
                finish();
                break;
            case R.id.scan_btn:
                cmd = String.format(cmd);
                mMovingGateway.sendCommand(cmd);

                break;
            case R.id.mv_btn:
                cmd = String.format(cmd,
                        ((EditText) findViewById(R.id.edit_text_x_val)).getText().toString(),
                        ((EditText) findViewById(R.id.edit_text_y_val)).getText().toString());
                mMovingGateway.sendCommand(cmd);

                break;
            case R.id.stop_btn:
                cmd = String.format(cmd);
                mMovingGateway.sendCommand(cmd);
                break;
            case R.id.connect_btn:
                EditText et_ip = (EditText) findViewById(R.id.edit_text_ip);
                if (mMovingGateway.isConnected() == false) {
                    if (et_ip.getText().toString().isEmpty()) {
                        Toast.makeText(getApplicationContext(), "Fill out IP", Toast.LENGTH_SHORT).show();
                    } else {
                        et_ip.setEnabled(false);
                        mMovingGateway.connectServer(et_ip.getText().toString());
                    }
                } else {
                    mMovingGateway.disconnectServer();
                }
        }

    }

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    public Action getIndexApiAction() {
        Thing object = new Thing.Builder()
                .setName("Main Page") // TODO: Define a title for the content shown.
                // TODO: Make sure this auto-generated URL is correct.
                .setUrl(Uri.parse("http://[ENTER-YOUR-URL-HERE]"))
                .build();
        return new Action.Builder(Action.TYPE_VIEW)
                .setObject(object)
                .setActionStatus(Action.STATUS_TYPE_COMPLETED)
                .build();
    }

    @Override
    public void onStart() {
        super.onStart();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client.connect();
        AppIndex.AppIndexApi.start(client, getIndexApiAction());
    }

    @Override
    public void onStop() {
        super.onStop();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        AppIndex.AppIndexApi.end(client, getIndexApiAction());
        client.disconnect();
    }

    private int mPresentBulbNum = 0;
    private boolean mDialogEnabled = false;


    @Override
    public void onBulbTouched(int bulb_num) {
        if(this.mDialogEnabled == true)
            return;

        this.mPresentBulbNum = bulb_num;
        BulbDialogFragment dialogFragment = new BulbDialogFragment();
        dialogFragment.setListener(this);
        dialogFragment.setBulbNum(bulb_num);
        dialogFragment.show(getFragmentManager(), "Bulb");
        this.mDialogEnabled = true;
    }

    @Override
    public void onBulbDialogFragmentDestroyed() {
        this.mDialogEnabled = false;
    }

    @Override
    public void onTurnOnButtonTouched() {
        // TODO: connect it
        String cmd = ((View)findViewById(R.id.rvc_surface_view)).getTag().toString();
        MovingGateway.Resource res = mMovingGateway.getResource(id[mPresentBulbNum]);
        cmd = String.format(cmd, res.unique_id, res.rgb, 1);
        mMovingGateway.sendCommand(cmd);
    }

    @Override
    public void onTurnOffButtonTouched() {
        // TODO: connect it
        String cmd = ((View)findViewById(R.id.rvc_surface_view)).getTag().toString();
        MovingGateway.Resource res = mMovingGateway.getResource(id[mPresentBulbNum]);
        cmd = String.format(cmd, res.unique_id, res.rgb, 0);
        mMovingGateway.sendCommand(cmd);
    }

    @Override
    public void onChangeColorWhiteButtonTouched() {
        // TODO: connect it
        String cmd = ((View)findViewById(R.id.rvc_surface_view)).getTag().toString();
        MovingGateway.Resource res = mMovingGateway.getResource(id[mPresentBulbNum]);
        cmd = String.format(cmd, res.unique_id, 0xFFFFFF, res.power);
        mMovingGateway.sendCommand(cmd);
    }

    @Override
    public void onChangeColorRedButtonTouched() {
        // TODO: connect it
        String cmd = ((View)findViewById(R.id.rvc_surface_view)).getTag().toString();
        MovingGateway.Resource res = mMovingGateway.getResource(id[mPresentBulbNum]);
        cmd = String.format(cmd, res.unique_id, 0xFF0000, res.power);
        mMovingGateway.sendCommand(cmd);
    }

    @Override
    public void onChangeColorGreenButtonTouched() {
        // TODO: connect it
        String cmd = ((View)findViewById(R.id.rvc_surface_view)).getTag().toString();
        MovingGateway.Resource res = mMovingGateway.getResource(id[mPresentBulbNum]);
        cmd = String.format(cmd, res.unique_id, 0x00FF00, res.power);
        mMovingGateway.sendCommand(cmd);
    }

    @Override
    public void onChangeColorBlueButtonTouched() {
        // TODO: connect it
        String cmd = ((View)findViewById(R.id.rvc_surface_view)).getTag().toString();
        MovingGateway.Resource res = mMovingGateway.getResource(id[mPresentBulbNum]);
        cmd = String.format(cmd, res.unique_id, 0x0000FF, res.power);
        mMovingGateway.sendCommand(cmd);
    }
    /***************/
}
