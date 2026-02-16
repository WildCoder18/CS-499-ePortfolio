package com.example.modulethree_mason;

import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;


import androidx.appcompat.app.AppCompatActivity;

public class EventEntryActivity extends AppCompatActivity {

    private EditText edtName, edtDate, edtCategory;
    private Button btnSave;
    private DatabaseHelper dbHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_event_entry);

        // Link UI elements
        edtName = findViewById(R.id.edtEventName);
        edtDate = findViewById(R.id.edtEventDate);
        edtCategory = findViewById(R.id.edtEventCategory);
        btnSave = findViewById(R.id.btnSaveEvent);

        // Initialize database helper
        dbHelper = new DatabaseHelper(this);

        btnSave.setOnClickListener(v -> {
            String name = edtName.getText().toString().trim();
            String date = edtDate.getText().toString().trim();
            String category = edtCategory.getText().toString().trim();

            if (name.isEmpty() || date.isEmpty() || category.isEmpty()) {
                Toast.makeText(this, "Please fill all fields", Toast.LENGTH_SHORT).show();
            } else {
                // Save event to database
                boolean success = dbHelper.addEvent(name, date, category); // addEvent uses 3 fields now
                if (success) {
                    Toast.makeText(this, "Event saved!", Toast.LENGTH_SHORT).show();
                    finish(); // close activity and return to grid
                } else {
                    Toast.makeText(this, "Failed to save event", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }
}