package kr.ac.skku.nyx.movinggateway;

//import android.view.SurfaceView;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;

import java.util.ArrayList;
import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import static android.R.attr.x;
import static android.R.attr.y;
import static android.R.transition.move;

/**
 * Created by redcarrottt on 2016. 11. 18..
 */

interface OnBulbTouchedListener {
    public void onBulbTouched(int bulb_num);
}

public class RVCSurfaceView extends View {
    private final int kBulbMarginX = 55;
    private final int kBulbMarginY = 55;
    private final int kRVCMarginX = 90;
    private final int kRVCMarginY = 51;
//    private final int kBulbMarginX = 24;
//    private final int kBulbMarginY = 24;
//    private final int kRVCMarginX = 32;
//    private final int kRVCMarginY = 27;

    class Bulb {
        public float roboticX;
        public float roboticY;
        public int canvasX;
        public int canvasY;
        public boolean isFound;
        public boolean isOn;

        public Bulb(float roboticX, float roboticY) {
            set(roboticX, roboticY, false);
            this.isOn = false;
        }

        public void set(float roboticX, float roboticY, boolean isFound) {
            this.roboticX = roboticX;
            this.roboticY = roboticY;
            this.canvasX = toCanvasX(roboticX) - kBulbMarginX;
            this.canvasY = toCanvasY(roboticY) - kBulbMarginY;
            this.isFound = isFound;
        }

        public void turnOn() {
            this.isOn = true;
        }

        public void turnOff() {
            this.isOn = false;
        }
    }

    protected int toCanvasX(float roboticX) {
        return (int) (((roboticX - kRoboticMinX) / (float) (kRoboticMaxX - kRoboticMinX)) * (getWidth() - kCanvasMarginX * 2) + kCanvasMarginX);
    }

    protected int toCanvasY(float roboticY) {
        return (int) ((((kRoboticMaxY - kRoboticMinY) - (roboticY - kRoboticMinY)) / (float) (kRoboticMaxY - kRoboticMinY)) * (getHeight() - kCanvasMarginY * 2) + kCanvasMarginY);
    }

    class RVC {
        public float roboticX;
        public float roboticY;
        public int canvasX;
        public int canvasY;

        public RVC(float roboticX, float roboticY) {
            set(roboticX, roboticY);
        }

        public void set(float roboticX, float roboticY) {
            this.roboticX = roboticX;
            this.roboticY = roboticY;
            this.canvasX = toCanvasX(roboticX) - kRVCMarginX;
            this.canvasY = toCanvasY(roboticY) - kRVCMarginY;
        }
    }

    class RVCMove {
        public float roboticFromX;
        public float roboticFromY;
        public float roboticToX;
        public float roboticToY;

        public int canvasFromX;
        public int canvasFromY;
        public int canvasToX;
        public int canvasToY;

        public RVCMove(float fromX, float fromY, float toX, float toY) {
            this.roboticFromX = fromX;
            this.roboticFromY = fromY;
            this.roboticToX = toX;
            this.roboticToY = toY;

            this.canvasFromX = toCanvasX(this.roboticFromX);
            this.canvasToX = toCanvasX(this.roboticToX);
            this.canvasFromY = toCanvasY(this.roboticFromY);
            this.canvasToY = toCanvasY(this.roboticToY);
        }
    }

    private final String TAG = "RVCSurfaceView";

    private final float kRoboticMaxX = 500.0f;
    private final float kRoboticMinX = -500.0f;
    private final float kRoboticMaxY = 1000.0f;
    private final float kRoboticMinY = 0.0f;

    private final int kCanvasMarginX = 150;
    private final int kCanvasMarginY = 150;

    private final int kMaxBulbs = 3;

    private boolean mIsInitialized = false;

    private OnBulbTouchedListener mOnBulbTouchedListener = null;

    //    private float mPrevRvcX = 0.0f;
//    private float mPrevRvcY = 0.0f;
    private RVC mPrevRvc;
    private RVC mHereRvc;
    //    private float mRvcX = 0.0f;
//    private float mRvcY = 0.0f;
    private ArrayList<Bulb> mBulbs = new ArrayList<Bulb>();
    private BlockingQueue<RVCMove> mMoveQueue = new ArrayBlockingQueue<RVCMove>(1000);

    private Canvas mMovingPathCanvas;
    private Bitmap mMovingPathBitmap;
    private Bitmap mRVCIconBitmap;
    private Bitmap mBlubOnIconBitmap;
    private Bitmap mBlubOffIconBitmap;

    private void initialize() {
        // Log.d(TAG, "width: " + this.getWidth() + " height: " + this.getHeight());
        // Initialize OnTouchListener
        this.setOnTouchListener(this.mOnTouchListener);

        // Initialize RVCs
        this.mPrevRvc = new RVC(0.0f, 0.0f);
        this.mHereRvc = new RVC(0.0f, 0.0f);

        // Initialize MovingPath Bitmap & Canvas
        Bitmap.Config conf = Bitmap.Config.ARGB_8888;
        this.mMovingPathBitmap = Bitmap.createBitmap(this.getWidth(), this.getHeight(), conf);
        this.mMovingPathCanvas = new Canvas(this.mMovingPathBitmap);

        // Fill MovingPath Bitmap with WHITE
        Paint paint = new Paint();
        paint.setColor(Color.WHITE);
        this.mMovingPathCanvas.drawRect(0, 0, this.getWidth(), this.getHeight(), paint);

        // Initialize RVCIcon Bitmap
        BitmapDrawable bd = (BitmapDrawable) getResources().getDrawable(R.drawable.rvcicon);
        this.mRVCIconBitmap = bd.getBitmap();

        // Initialize BulbIcon Bitmap
        BitmapDrawable bd2 = (BitmapDrawable) getResources().getDrawable(R.drawable.bulbonicon);
        this.mBlubOnIconBitmap = bd2.getBitmap();

        BitmapDrawable bd3 = (BitmapDrawable) getResources().getDrawable(R.drawable.bulbofficon);
        this.mBlubOffIconBitmap = bd3.getBitmap();

        // Initalize Bulb list
        for (int i = 0; i < kMaxBulbs; i++) {
            Bulb newBulb = new Bulb(0.0f, 0.0f);
            this.mBulbs.add(newBulb);
        }

//        try {
//            this.mMoveQueue.put(new RVCMove(0.0f, 0.0f, 250.0f, 250.0f));
//            this.mMoveQueue.put(new RVCMove(250.0f, 250.0f, 500.0f, 0.0f));
//            this.mMoveQueue.put(new RVCMove(500.0f, 0.0f, -500.0f, 750.0f));
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }

        this.mIsInitialized = true;
    }

    public RVCSurfaceView(Context context) {
        super(context);
    }

    public RVCSurfaceView(Context context, AttributeSet attributeSet) {
        super(context, attributeSet);
    }

    // API
    public void updateRVCPosition(float roboticX, float roboticY) {
        // Push to MoveList for drawing
        try {
            this.mMoveQueue.put(new RVCMove(this.mPrevRvc.roboticX, this.mPrevRvc.roboticY, roboticX, roboticY));
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        // Update RVC Position
        this.mPrevRvc.set(this.mHereRvc.roboticX, this.mHereRvc.roboticY);
        this.mHereRvc.set(x, y);
    }

    // API
    public void updateBulbPosition(int bulb_num, float roboticX, float roboticY) {
        if (bulb_num < 0 || bulb_num >= kMaxBulbs) {
            Log.d(TAG, "invalid bulb_num: " + bulb_num);
            return;
        }
        Bulb bulb = this.mBulbs.get(bulb_num);
        bulb.set(roboticX, roboticY, true);
    }

    // In order to redraw, please use invalidate() function!
    @Override
    protected void onDraw(Canvas mainCanvas) {
        super.onDraw(mainCanvas);

        if (this.mIsInitialized == false)
            this.initialize();

        setBackgroundColor(Color.WHITE);

        int width = this.getWidth();
        int height = this.getHeight();

        // Draw additional moving path on MovingPathCanvas
        Paint paint = new Paint();
        paint.setColor(Color.RED);
        paint.setStrokeWidth(10.0f);
        while (!this.mMoveQueue.isEmpty()) {
            try {
                RVCMove move = this.mMoveQueue.take();
                this.mMovingPathCanvas.drawLine(move.canvasFromX, move.canvasFromY, move.canvasToX, move.canvasToY, paint);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        // Draw MovingPathCanvas on MainCanvas
        mainCanvas.drawBitmap(this.mMovingPathBitmap, 0.0f, 0.0f, null);

        // Draw BulbIcon on MainCanvas
        for (Bulb bulb : this.mBulbs) {
            if (bulb.isFound == true) {
                if (bulb.isOn == true) {
                    mainCanvas.drawBitmap(this.mBlubOnIconBitmap,
                            bulb.canvasX, bulb.canvasY,
                            null);
                } else {
                    mainCanvas.drawBitmap(this.mBlubOffIconBitmap,
                            bulb.canvasX, bulb.canvasY,
                            null);
                }
            }
        }

        // Draw RVCIcon on MainCanvas
        mainCanvas.drawBitmap(this.mRVCIconBitmap, this.mHereRvc.canvasX, this.mHereRvc.canvasY, null);
    }

    private OnTouchListener mOnTouchListener = new OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            int x = (int) event.getX();
            int y = (int) event.getY();
            int i = 0;
            Log.d(TAG, x + ", " + y);
            for (Bulb bulb : mBulbs) {
                if(bulb.isFound == true) {
                    int criteriaXMin = bulb.canvasX;
                    int criteriaXMax = bulb.canvasX + kBulbMarginX * 2;
                    int criteriaYMin = bulb.canvasY;
                    int criteriaYMax = bulb.canvasY + kBulbMarginY * 2;
                    Log.d(TAG, "Bulb " + i + " (" + criteriaXMin + ", " + criteriaYMin + ") ~ ("
                            + criteriaXMax + ", " + criteriaYMax + ")");
                    if (x >= criteriaXMin && x <= criteriaXMax
                            && y >= citeriaYMin && y <= criteriaYMax) {
                        Log.d(TAG, "Bulb " + i + " is touched!");
                        if (mOnBulbTouchedListener != null) {
                            mOnBulbTouchedListener.onBulbTouched(i);
                            return false;
                        }
                    }
                }
                i++;
            }
            return false;
        }
    };

    public void setListener(OnBulbTouchedListener listener) {
        this.mOnBulbTouchedListener = listener;
    }
}