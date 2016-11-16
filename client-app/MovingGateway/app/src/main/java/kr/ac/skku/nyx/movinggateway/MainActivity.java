package kr.ac.skku.nyx.movinggateway;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        MovingGateway movingGateway = new MovingGateway();
        movingGateway.setOnResourceEventListener(new MovingGateway.OnResourceEventListener() {
            @Override
            public void onFoundResource(MovingGateway.Resource res) {

            }
        });

        Button scan_btn = (Button)findViewById(R.id.scan_btn);
        scan_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });
    }
}
