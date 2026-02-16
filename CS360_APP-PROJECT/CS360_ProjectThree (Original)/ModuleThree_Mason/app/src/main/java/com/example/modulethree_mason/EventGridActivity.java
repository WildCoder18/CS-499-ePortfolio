package com.example.modulethree_mason;

import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.widget.Button;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import java.util.ArrayList;
import java.util.List;
import android.telephony.SmsManager;
import android.Manifest;
import android.content.pm.PackageManager;
import androidx.core.content.ContextCompat;

public class EventGridActivity extends AppCompatActivity {

    private RecyclerView recyclerView;
    private EventAdapter adapter;
    private List<Event> eventList;
    private DatabaseHelper dbHelper;
    private ActivityResultLauncher<Intent> addEventLauncher;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_event_grid);

        dbHelper = new DatabaseHelper(this);
        recyclerView = findViewById(R.id.recyclerEvents);
        eventList = new ArrayList<>();

        // Load events from database
        loadEventsFromDatabase();

        adapter = new EventAdapter(eventList);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        recyclerView.setAdapter(adapter);

        // Add Event Launcher
        addEventLauncher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result -> {
                    if (result.getResultCode() == RESULT_OK && result.getData() != null) {
                        String name = result.getData().getStringExtra("name");
                        String date = result.getData().getStringExtra("date");
                        String category = result.getData().getStringExtra("category");

                        // Add to database
                        dbHelper.addEvent(name, date, category);

                        // Send SMS notification if permission is granted
                        if (ContextCompat.checkSelfPermission(
                                EventGridActivity.this,
                                Manifest.permission.SEND_SMS
                        ) == PackageManager.PERMISSION_GRANTED) {

                            SmsManager smsManager = SmsManager.getDefault();
                            smsManager.sendTextMessage(
                                    "5551234567", // emulator test number
                                    null,
                                    "New event added: " + name + " on " + date,
                                    null,
                                    null
                            );
                        }

                        // Refresh list
                        loadEventsFromDatabase();
                        adapter.notifyDataSetChanged();
                    }
                }
        );

        // Add Event button
        Button btnAddEvent = findViewById(R.id.btnAddEvent);
        btnAddEvent.setOnClickListener(v -> {
            Intent intent = new Intent(EventGridActivity.this, EventEntryActivity.class);
            addEventLauncher.launch(intent);
        });

        // SMS Settings button
        Button btnSmsSettings = findViewById(R.id.btnSmsSettings);
        btnSmsSettings.setOnClickListener(v -> {
            Intent intent = new Intent(EventGridActivity.this, SMSActivity.class);
            startActivity(intent);
        });
    }

    private void loadEventsFromDatabase() {
        eventList.clear();
        Cursor cursor = dbHelper.getAllEvents();
        if (cursor != null && cursor.moveToFirst()) {
            do {
                int id = cursor.getInt(cursor.getColumnIndexOrThrow(DatabaseHelper.COLUMN_EVENT_ID));
                String name = cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COLUMN_EVENT_NAME));
                String date = cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COLUMN_EVENT_DATE));
                String category = cursor.getString(cursor.getColumnIndexOrThrow(DatabaseHelper.COLUMN_EVENT_DESC)); // using desc for category
                eventList.add(new Event(name, date, category));
            } while (cursor.moveToNext());
            cursor.close();
        }
    }
}
