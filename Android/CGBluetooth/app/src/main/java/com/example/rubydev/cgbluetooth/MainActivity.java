package com.example.rubydev.cgbluetooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothClass;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.Switch;
import android.widget.TextView;

import org.w3c.dom.Text;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.text.DecimalFormat;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity implements SensorEventListener {

    private SensorManager senSensorManager;
    private Sensor senAccelerometer;

    private BluetoothAdapter bAdapter = null;
    private UUID uuid = null;
    private BluetoothDevice device = null;
    private BluetoothSocket socket = null;
    private OutputStream output = null;

    private Button buttonPlay = null;
    private boolean sending = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        buttonPlay = (Button) findViewById(R.id.playPause);

        bAdapter = BluetoothAdapter.getDefaultAdapter();
        device = bAdapter.getRemoteDevice("2C:D0:5A:22:BC:F7");
        uuid = UUID.fromString("4CC4A570-3212-11E3-BBA6-AC1945E34500");
        try {
            socket = device.createRfcommSocketToServiceRecord(uuid);
            socket.connect();
            output = socket.getOutputStream();
        } catch (IOException e) {
            e.printStackTrace();
        }

        senSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        senAccelerometer = senSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        senSensorManager.registerListener(this, senAccelerometer, SensorManager.SENSOR_DELAY_UI);

        buttonPlay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (sending) {
                    sending = false;
                    buttonPlay.setText("Começar");
                    senSensorManager.unregisterListener(MainActivity.this);
                } else {
                    sending = true;
                    buttonPlay.setText("Parar");
                    senSensorManager.registerListener(MainActivity.this, senAccelerometer, SensorManager.SENSOR_DELAY_UI);
                }
            }
        });
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        int mySensor = event.sensor.getType();

        if (mySensor == Sensor.TYPE_ACCELEROMETER && sending){
            try {
                float y = event.values[1];
                String y_str = String.valueOf(y);
                if(y_str.length() >= 5){
                    y_str = y_str.substring(0, 5);
                } else if (y_str.length() < 5 && y_str.contains(".")) {
                    for(int i = 0;i < 5 - y_str.length();i++)
                        y_str = y_str + "0";
                } else if (y_str.length() < 5 && !y_str.contains(".")){
                    for(int i = 0;i < 5 - y_str.length();i++)
                        y_str = "0" + y_str;
                }

                output.flush();
                output.write(y_str.getBytes());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
}