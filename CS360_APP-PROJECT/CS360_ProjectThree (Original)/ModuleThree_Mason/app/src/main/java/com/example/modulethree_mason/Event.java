package com.example.modulethree_mason;

public class Event {
    private String name;
    private String date;
    private String category;

    public Event(String name, String date, String category) {
        this.name = name;
        this.date = date;
        this.category = category;
    }

    public String getName() {
        return name;
    }

    public String getDate() {
        return date;
    }

    public String getCategory() {
        return category;
    }
}
