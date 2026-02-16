package com.example.modulethree_mason;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.content.ContentValues;
import android.database.Cursor;

public class DatabaseHelper extends SQLiteOpenHelper {

    private static final String DATABASE_NAME = "EventTracking.db";
    private static final int DATABASE_VERSION = 2;

    // Table names
    public static final String TABLE_USERS = "users";
    public static final String TABLE_EVENTS = "events";

    // Users columns
    public static final String COLUMN_USER_ID = "id";
    public static final String COLUMN_USERNAME = "username";
    public static final String COLUMN_PASSWORD = "password";

    // Events columns
    public static final String COLUMN_EVENT_ID = "id";
    public static final String COLUMN_EVENT_NAME = "event_name";
    public static final String COLUMN_EVENT_DATE = "event_date";
    public static final String COLUMN_EVENT_DESC = "event_desc";

    public static final String COLUMN_EVENT_CATEGORY = "event_category";

    public DatabaseHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    @Override
public void onCreate(SQLiteDatabase db) {
    // Create users table
    String CREATE_USERS_TABLE = "CREATE TABLE " + TABLE_USERS + " ("
            + COLUMN_USER_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, "
            + COLUMN_USERNAME + " TEXT NOT NULL UNIQUE, "
            + COLUMN_PASSWORD + " TEXT NOT NULL"
            + ")";
    db.execSQL(CREATE_USERS_TABLE);

    // Create events table
    String CREATE_EVENTS_TABLE = "CREATE TABLE " + TABLE_EVENTS + " ("
            + COLUMN_EVENT_ID + " INTEGER PRIMARY KEY AUTOINCREMENT, "
            + COLUMN_EVENT_NAME + " TEXT NOT NULL, "
            + COLUMN_EVENT_DATE + " TEXT NOT NULL, "
            + COLUMN_EVENT_DESC + " TEXT, "
            + COLUMN_EVENT_CATEGORY + " TEXT DEFAULT 'General', "
            + COLUMN_USER_ID + " INTEGER NOT NULL, "
            + "FOREIGN KEY (" + COLUMN_USER_ID + ") REFERENCES "
            + TABLE_USERS + "(" + COLUMN_USER_ID + ")"
            + ")";
    db.execSQL(CREATE_EVENTS_TABLE);
}

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_USERS);
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_EVENTS);
        onCreate(db);
    }

    // Check if user exists
    public boolean checkUser(String username, String password) {
        SQLiteDatabase db = this.getReadableDatabase();
        String[] columns = {COLUMN_USER_ID};
        String selection = COLUMN_USERNAME + "=? AND " + COLUMN_PASSWORD + "=?";
        String[] selectionArgs = {username, password};
        Cursor cursor = db.query(TABLE_USERS, columns, selection, selectionArgs, null, null, null);
        boolean exists = cursor.getCount() > 0;
        cursor.close();
        return exists;
    }

    // Add new user
    public boolean addUser(String username, String password) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues values = new ContentValues();
        values.put(COLUMN_USERNAME, username);
        values.put(COLUMN_PASSWORD, password);
        long result = db.insert(TABLE_USERS, null, values);
        return result != -1;
    }

    // Add new event with user association
public boolean addEvent(int userId, String name, String date, String desc, String category) {
    SQLiteDatabase db = this.getWritableDatabase();
    ContentValues values = new ContentValues();
    values.put(COLUMN_USER_ID, userId);             // <-- associate event with user
    values.put(COLUMN_EVENT_NAME, name);
    values.put(COLUMN_EVENT_DATE, date);
    values.put(COLUMN_EVENT_DESC, desc);
    values.put(COLUMN_EVENT_CATEGORY, category);   // <-- store category
    long result = db.insert(TABLE_EVENTS, null, values);
    return result != -1;
}

    // Get all events for a specific user
public Cursor getAllEvents(int userId) {
    SQLiteDatabase db = this.getReadableDatabase();
    String selection = COLUMN_USER_ID + "=?";
    String[] selectionArgs = { String.valueOf(userId) };
    return db.query(TABLE_EVENTS, null, selection, selectionArgs, null, null, COLUMN_EVENT_DATE + " ASC");
}

    // Delete event by ID
    public boolean deleteEvent(int id) {
        SQLiteDatabase db = this.getWritableDatabase();
        int result = db.delete(TABLE_EVENTS, COLUMN_EVENT_ID + "=?", new String[]{String.valueOf(id)});
        return result > 0;
    }

    // Update event
    public boolean updateEvent(int id, String name, String date, String desc) {
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues values = new ContentValues();
        values.put(COLUMN_EVENT_NAME, name);
        values.put(COLUMN_EVENT_DATE, date);
        values.put(COLUMN_EVENT_DESC, desc);
        int result = db.update(TABLE_EVENTS, values, COLUMN_EVENT_ID + "=?", new String[]{String.valueOf(id)});
        return result > 0;
    }

}
