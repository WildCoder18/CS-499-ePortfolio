package com.example.modulethree_mason;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Get references to UI elements
        EditText nameText = findViewById(R.id.nameText);
        Button buttonSayHello = findViewById(R.id.buttonSayHello);

        // Add TextChangedListener to EditText
        nameText.addTextChangedListener(new android.text.TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
                // Not used
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                // Enable button if there is text, disable if empty
                buttonSayHello.setEnabled(s.toString().trim().length() > 0);
            }

            @Override
            public void afterTextChanged(android.text.Editable s) {
                // Not used
            }
        });
    }

    // SayHello function
    public void SayHello(View view) {
        EditText nameText = findViewById(R.id.nameText);
        TextView textGreeting = findViewById(R.id.textGreeting);

        String name = nameText.getText().toString();
        if (name.isEmpty()) {
            return;
        }

        textGreeting.setText("Hello " + name);
    }
}