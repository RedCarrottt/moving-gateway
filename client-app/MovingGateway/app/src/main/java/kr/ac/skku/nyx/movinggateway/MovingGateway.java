package kr.ac.skku.nyx.movinggateway;

import android.os.Handler;
import android.util.Log;
import android.widget.EditText;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Map;

/**
 * Created by eslab on 2016-11-16.
 */

public class MovingGateway {
    final int UPDATE_SCREEN = 1;
    final int SERVER_PORT = 5000;
    private OnResourceEventListener onResourceEventListener = null;
    private Socket mSocket;
    private Handler mHandler;
    private boolean mIsConnect;
    private PrintWriter mPrintWriter;
    private JSONObject jsonObject;
    public Position pos;

    MovingGateway(Handler handler) {
        mIsConnect = false;
        mHandler = handler;
        mSocket = null;
        pos = new Position();
    }

    public class Position {
        public float x;
        public float y;

        Position (float x, float y) {
            this.x = x;
            this.y = y;
        }
        Position () {
            x = 0;
            y = 0;
        }
    }
    public class Resource {
        private int model_id;

        private Map<String, String> attr;

        public int getResourceId() {
            return model_id;
        }
        public void setResourceId(int res_id) {
            this.model_id = res_id;
        }

        public Map<String, String> getAttribute() {
            return attr;
        }
        public void setAttribute(Map<String, String> attr) {
            this.attr = attr;
        }
    }

    public boolean isConnected() {
        return mIsConnect;
    }
    public interface OnResourceEventListener {
        void onFoundResource(Resource res);
        void onGetResource(String json_rep);
        void onPutResource(String json_rep);
    }
    public void setOnResourceEventListener(OnResourceEventListener onResourceEventListener) {

    }
    public void scan () {
    }

    public void get(Resource res, String request) {

    }

    public void put(Resource res, String request) {

    }

    public void connectServer(String ip){
        final String mIp = ip;

        new Thread() {
            public void run(){
                Log.d("Connection", "Thread generated");
                InetAddress serverAddr = null;
                int bytesRead = 0;
                byte[] buffer = new byte[512];

                try {
                    serverAddr = InetAddress.getByName(mIp);
                } catch (UnknownHostException e1) {
                    e1.printStackTrace();
                    disconnectServer();
                    return;
                }

                try {
                    mSocket = new Socket(serverAddr, SERVER_PORT);
                    mSocket.setSoTimeout(5000);
                } catch (IOException e) {
                    e.printStackTrace();
                    disconnectServer();
                    return;
                }

                try {
                    mPrintWriter = new PrintWriter(new BufferedWriter(new OutputStreamWriter(mSocket.getOutputStream())), true);
                } catch (IOException e1) {
                    disconnectServer();
                    return;
                }

                mIsConnect = true;
                mHandler.sendEmptyMessage(UPDATE_SCREEN);

                try {
                    while ((bytesRead = mSocket.getInputStream().read(buffer)) != -1) {
                        if(bytesRead>0){
                            parsingJson(buffer);
                            mHandler.sendEmptyMessage(UPDATE_SCREEN);
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }

                disconnectServer();
            }
        }.start();
    }

    public void disconnectServer(){
        if(mSocket!=null){
            try {
                mSocket.close();
                mSocket = null;
            } catch (IOException e) {
            }
        }

        mIsConnect = false;
        mHandler.sendEmptyMessage(UPDATE_SCREEN);
    }

    synchronized public void sendCommand(final String data) {
        //Log.d("cmd", data);
        new Thread() {
            public void run() {
                if (mPrintWriter!= null && !mPrintWriter.checkError()) {
                    mPrintWriter.println(data);
                    mPrintWriter.flush();
                }
            }
        }.start();
    }

    private void parsingJson(byte[] buffer) {
        JSONObject reader = null;

        String str = new String(buffer);
        try {
            reader = new JSONObject(str);

            JSONObject posVal = reader.getJSONObject("position");
            double x = posVal.getDouble("x");
            double y = posVal.getDouble("y");

            pos.x = (float) x;
            pos.y = (float) y;

            mHandler.sendEmptyMessage(UPDATE_SCREEN);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

}
