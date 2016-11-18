package kr.ac.skku.nyx.movinggateway;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

/**
 * Created by redcarrottt on 2016. 11. 18..
 */

interface BulbDialogFragmentListener {
    public void onTurnOnButtonTouched();

    public void onTurnOffButtonTouched();

    public void onChangeColorWhiteButtonTouched();

    public void onChangeColorRedButtonTouched();

    public void onChangeColorGreenButtonTouched();

    public void onChangeColorBlueButtonTouched();

    public void onBulbDialogFragmentDestroyed();

}

public class BulbDialogFragment extends DialogFragment {
    private BulbDialogFragmentListener mListener = null;
    private int mBulbNum = -1;

    public void setListener(BulbDialogFragmentListener listener) {
        this.mListener = listener;
    }

    public void setBulbNum(int bulbNum) {
        this.mBulbNum = bulbNum;
    }

    private View mSelfView = null;

    private View.OnTouchListener mOnTouchListener = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            Button turnOnButton = (Button) mSelfView.findViewById(R.id.turn_on_btn);
            Button turnOffButton = (Button) mSelfView.findViewById(R.id.turn_off_btn);
            Button changeColorWhiteButton = (Button) mSelfView.findViewById(R.id.change_color_white_btn);
            Button changeColorRedButton = (Button) mSelfView.findViewById(R.id.change_color_red_btn);
            Button changeColorGreenButton = (Button) mSelfView.findViewById(R.id.change_color_green_btn);
            Button changeColorBlueButton = (Button) mSelfView.findViewById(R.id.change_color_blue_btn);

            if (mListener != null) {
                if (v == turnOnButton) {
                    mListener.onTurnOnButtonTouched();
                } else if (v == turnOffButton) {
                    mListener.onTurnOffButtonTouched();
                } else if (v == changeColorWhiteButton) {
                    mListener.onChangeColorWhiteButtonTouched();
                } else if (v == changeColorRedButton) {
                    mListener.onChangeColorRedButtonTouched();
                } else if (v == changeColorGreenButton) {
                    mListener.onChangeColorGreenButtonTouched();
                } else if (v == changeColorBlueButton) {
                    mListener.onChangeColorBlueButtonTouched();
                }
            }
            return false;
        }
    };

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        // Get the layout inflater
        LayoutInflater inflater = getActivity().getLayoutInflater();

        // Inflate and set the layout for the dialog
        // Pass null as the parent view because its going in the dialog layout
        this.mSelfView = inflater.inflate(R.layout.dialog_bulb, null);
        builder.setView(this.mSelfView).setPositiveButton("Close", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int id) {
            }
        });
        Dialog dialog = builder.create();

        TextView bulbTextView = (TextView) mSelfView.findViewById(R.id.bulb_text_view);
        Button turnOnButton = (Button) this.mSelfView.findViewById(R.id.turn_on_btn);
        Button turnOffButton = (Button) this.mSelfView.findViewById(R.id.turn_off_btn);
        Button changeColorWhiteButton = (Button) this.mSelfView.findViewById(R.id.change_color_white_btn);
        Button changeColorRedButton = (Button) this.mSelfView.findViewById(R.id.change_color_red_btn);
        Button changeColorGreenButton = (Button) this.mSelfView.findViewById(R.id.change_color_green_btn);
        Button changeColorBlueButton = (Button) this.mSelfView.findViewById(R.id.change_color_blue_btn);

        bulbTextView.setText("Bulb #" + this.mBulbNum);
        turnOnButton.setOnTouchListener(this.mOnTouchListener);
        turnOffButton.setOnTouchListener(this.mOnTouchListener);
        changeColorWhiteButton.setOnTouchListener(this.mOnTouchListener);
        changeColorRedButton.setOnTouchListener(this.mOnTouchListener);
        changeColorGreenButton.setOnTouchListener(this.mOnTouchListener);
        changeColorBlueButton.setOnTouchListener(this.mOnTouchListener);

        return dialog;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        this.mListener.onBulbDialogFragmentDestroyed();
    }
}
