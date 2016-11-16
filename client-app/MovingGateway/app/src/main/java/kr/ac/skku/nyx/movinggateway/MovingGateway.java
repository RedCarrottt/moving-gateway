package kr.ac.skku.nyx.movinggateway;

/**
 * Created by eslab on 2016-11-16.
 */

public class MovingGateway {
    private OnResourceEventListener onResourceEventListener = null;
    public class Resource {
    }
    public interface OnResourceEventListener {
        void onFoundResource(Resource res);
    }
    public void setOnResourceEventListener(OnResourceEventListener onResourceEventListener) {
    }
    public void scan () {
    }
}
