package com.example.modulethree_mason;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.Button;
import android.widget.Toast;
import android.view.ViewGroup;
import android.graphics.Color;
import android.content.res.ColorStateList;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

public class SMSActivity extends AppCompatActivity {

    private ActivityResultLauncher<String> requestPermissionLauncher;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sms);

        Button btnRequestSms = findViewById(R.id.btnRequestSms);

        // Move button down slightly
        ViewGroup.MarginLayoutParams params =
                (ViewGroup.MarginLayoutParams) btnRequestSms.getLayoutParams();
        params.topMargin = 60;
        btnRequestSms.setLayoutParams(params);

        // Set button color to black
        btnRequestSms.setBackgroundTintList(
                ColorStateList.valueOf(Color.BLACK)
        );

        // Initialize permission launcher
        requestPermissionLauncher = registerForActivityResult(
                new ActivityResultContracts.RequestPermission(),
                isGranted -> {
                    if (isGranted) {
                        Toast.makeText(this, "SMS notifications enabled!", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(this, "SMS notifications denied.", Toast.LENGTH_SHORT).show();
                    }
                }
        );

        // Button click
        btnRequestSms.setOnClickListener(v -> {
            if (ContextCompat.checkSelfPermission(
                    this,
                    Manifest.permission.SEND_SMS
            ) == PackageManager.PERMISSION_GRANTED) {

                Toast.makeText(this, "SMS notifications already enabled!", Toast.LENGTH_SHORT).show();

            } else {
                requestPermissionLauncher.launch(Manifest.permission.SEND_SMS);
            }
        });
    }
}
